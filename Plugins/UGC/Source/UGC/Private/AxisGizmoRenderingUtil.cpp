// Copyright Epic Games, Inc. All Rights Reserved.

#include "AxisGizmoRenderingUtil.h"
#include "RHI.h"


// yuck global value set by Editor
static const FSceneView* GlobalCurrentSceneView = nullptr;

static FCriticalSection GlobalCurrentSceneViewLock;

#if WITH_EDITOR
static bool bGlobalUseCurrentSceneViewTracking = true;
#else
static bool bGlobalUseCurrentSceneViewTracking = false;
#endif


void AxisGizmoRenderingUtil::SetGlobalFocusedEditorSceneView(const FSceneView* View)
{
	GlobalCurrentSceneViewLock.Lock();
	GlobalCurrentSceneView = View;
	GlobalCurrentSceneViewLock.Unlock();
}

void AxisGizmoRenderingUtil::SetGlobalFocusedSceneViewTrackingEnabled(bool bEnabled)
{
	bGlobalUseCurrentSceneViewTracking = bEnabled;
}


const FSceneView* AxisGizmoRenderingUtil::FindFocusedEditorSceneView(
	const TArray<const FSceneView*>& Views,
	const FSceneViewFamily& ViewFamily,
	uint32 VisibilityMap)
{
	// we are likely being called from a rendering thread GetDynamicMeshElements() function
	const FSceneView* GlobalEditorView = nullptr;
	GlobalCurrentSceneViewLock.Lock();
	GlobalEditorView = GlobalCurrentSceneView;
	GlobalCurrentSceneViewLock.Unlock();

	// if we only have one view, and we are not tracking active view, just use that one
	if (bGlobalUseCurrentSceneViewTracking == false && Views.Num() == 1)
	{
		return Views[0];
	}

	// otherwise try to find the view that the game thread set for us
	int32 VisibleViews = 0;
	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
	{
		if (VisibilityMap & (1 << ViewIndex))
		{
			VisibleViews++;
			if (Views[ViewIndex] == GlobalEditorView)
			{
				return Views[ViewIndex];
			}
		}
	}

	// if we did not find our view, but only one view is visible, we can speculatively return that one
	if (bGlobalUseCurrentSceneViewTracking == false && VisibleViews == 1)
	{
		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (VisibilityMap & (1 << ViewIndex))
			{
				return Views[ViewIndex];
			}
		}
	}

	// ok give up
	if (bGlobalUseCurrentSceneViewTracking == false)
	{
		return Views[0];
	}
	else
	{
		return nullptr;
	}
}




static double VectorDifferenceSqr(const FVector2D& A, const FVector2D& B)
{
	double ax = A.X, ay = A.Y;
	double bx = B.X, by = B.Y;
	ax -= bx;
	ay -= by;
	return ax*ax + ay*ay;
}

static double VectorDifferenceSqr(const FVector& A, const FVector& B)
{
	double ax = A.X, ay = A.Y, az = A.Z;
	double bx = B.X, by = B.Y, bz = B.Z;
	ax -= bx; 
	ay -= by;
	az -= bz;
	return ax*ax + ay*ay + az*az;
}

// duplicates FSceneView::WorldToPixel but in double where possible (unfortunately WorldToScreen still in float)
static FVector2D WorldToPixelDouble(const FSceneView* View, const FVector& Location)
{
	FVector4 ScreenPoint = View->WorldToScreen(Location);

	double InvW = (ScreenPoint.W > 0.0 ? 1.0 : -1.0) / (double)ScreenPoint.W;
	double Y = (GProjectionSignY > 0.0) ? (double)ScreenPoint.Y : 1.0 - (double)ScreenPoint.Y;

	const FIntRect& UnscaledViewRect = View->UnscaledViewRect;
	double PosX = (double)UnscaledViewRect.Min.X + (0.5 + (double)ScreenPoint.X * 0.5 * InvW) * (double)UnscaledViewRect.Width();
	double PosY = (double)UnscaledViewRect.Min.Y + (0.5 - Y * 0.5 * InvW) * (double)UnscaledViewRect.Height();

	return FVector2D((float)PosX, (float)PosY);
}


float AxisGizmoRenderingUtil::CalculateLocalPixelToWorldScale(
	const FSceneView* View,
	const FVector& Location)
{
	// To calculate this scale at Location, we project Location to screen and also project a second
	// point at a small distance from Location in a camera-perpendicular plane, then measure 2D/3D distance ratio. 
	// However, because some of the computations are done in float, there will be enormous numerical error 
	// when the camera is very far from the location if the distance is relatively small. The "W" value
	// below gives us a sense of this distance, so we make the offset relative to that
	// (this does do one redundant WorldToScreen)
	FVector4 LocationScreenPoint = View->WorldToScreen(Location);
	float OffsetDelta = LocationScreenPoint.W * 0.01f;

	FVector2D PixelA = WorldToPixelDouble(View, Location);
	FVector OffsetPointWorld = Location + OffsetDelta*View->GetViewRight() + OffsetDelta*View->GetViewUp();
	FVector2D PixelB = WorldToPixelDouble(View, OffsetPointWorld);

	double PixelDeltaSqr = VectorDifferenceSqr(PixelA, PixelB);
	double WorldDeltaSqr = VectorDifferenceSqr(Location, OffsetPointWorld);
	return (float)(sqrt(WorldDeltaSqr / PixelDeltaSqr));
}

void AxisGizmoRenderingUtil::RayPlaneIntersectionPoint(
    const FVector& PlaneOrigin, const FVector& PlaneNormal,
    const FVector& RayOrigin, const FVector& RayDirection,
    bool& bIntersectsOut, FVector& PlaneIntersectionPointOut)
{
    bIntersectsOut = false;
    PlaneIntersectionPointOut = PlaneOrigin;

    double PlaneEquationD = -FVector::DotProduct(PlaneOrigin, PlaneNormal);
    double NormalDot = FVector::DotProduct(RayDirection, PlaneNormal);

    if (FMath::Abs(NormalDot) < SMALL_NUMBER)
    {
        return;
    }

    double RayParam = -(FVector::DotProduct(RayOrigin, PlaneNormal) + PlaneEquationD) / NormalDot;
    if (RayParam < 0)
    {
        return;
    }

    PlaneIntersectionPointOut = RayOrigin + RayParam * RayDirection;
    bIntersectsOut = true;
}

void AxisGizmoRenderingUtil::MakeNormalPlaneBasis(
    const FVector& PlaneNormal,
    FVector& BasisAxis1Out, FVector& BasisAxis2Out)
{
    // Duff et al method, from https://graphics.pixar.com/library/OrthonormalB/paper.pdf
    if (PlaneNormal.Z < 0)
    {
        double A = 1.0f / (1.0f - PlaneNormal.Z);
        double B = PlaneNormal.X * PlaneNormal.Y * A;
        BasisAxis1Out.X = 1.0f - PlaneNormal.X * PlaneNormal.X * A;
        BasisAxis1Out.Y = -B;
        BasisAxis1Out.Z = PlaneNormal.X;
        BasisAxis2Out.X = B;
        BasisAxis2Out.Y = PlaneNormal.Y * PlaneNormal.Y * A - 1.0f;
        BasisAxis2Out.Z = -PlaneNormal.Y;
    }
    else
    {
        double A = 1.0f / (1.0f + PlaneNormal.Z);
        double B = -PlaneNormal.X * PlaneNormal.Y * A;
        BasisAxis1Out.X = 1.0f - PlaneNormal.X * PlaneNormal.X * A;
        BasisAxis1Out.Y = B;
        BasisAxis1Out.Z = -PlaneNormal.X;
        BasisAxis2Out.X = B;
        BasisAxis2Out.Y = 1.0f - PlaneNormal.Y * PlaneNormal.Y * A;
        BasisAxis2Out.Z = -PlaneNormal.Y;
    }
}

void AxisGizmoRenderingUtil::RaySphereIntersection(
    const FVector& SphereOrigin, const float SphereRadius,
    const FVector& RayOrigin, const FVector& RayDirection,
    bool& bIntersectsOut, FVector& SphereIntersectionPointOut)
{
    bIntersectsOut = false;
    SphereIntersectionPointOut = RayOrigin;

    FVector DeltaPos = RayOrigin - SphereOrigin;
    double a0 = DeltaPos.SizeSquared() - (double)SphereRadius * (double)SphereRadius;
    double a1 = FVector::DotProduct(RayDirection, DeltaPos);
    double discr = a1 * a1 - a0;
    if (discr > 0)   // intersection only when roots are real
    {
        bIntersectsOut = true;
        double root = FMath::Sqrt(discr);
        double NearRayParam = -a1 + root;		// isn't it always this one?
        double NearRayParam2 = -a1 - root;
        double UseRayParam = FMath::Min(NearRayParam, NearRayParam2);
        SphereIntersectionPointOut = RayOrigin + UseRayParam * RayDirection;
    }
}


void AxisGizmoRenderingUtil::ClosetPointOnCircle(
    const FVector& QueryPoint,
    const FVector& CircleOrigin, const FVector& CircleNormal, float CircleRadius,
    FVector& ClosestPointOut)
{
    FVector PointDelta = QueryPoint - CircleOrigin;
    FVector DeltaInPlane = PointDelta - FVector::DotProduct(CircleNormal, PointDelta) * CircleNormal;
    double OriginDist = DeltaInPlane.Size();
    if (OriginDist > 0.0f)
    {
        ClosestPointOut = CircleOrigin + ((double)CircleRadius / OriginDist) * DeltaInPlane;
    }
    else    // all points equidistant, use any one
    {
        FVector PlaneX, PlaneY;
        MakeNormalPlaneBasis(CircleNormal, PlaneX, PlaneY);
        ClosestPointOut = CircleOrigin + (double)CircleRadius * PlaneX;
    }
}
