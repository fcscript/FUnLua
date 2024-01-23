// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"


/**
 * Utility functions for standard GizmoComponent rendering
 */
namespace AxisGizmoRenderingUtil
{
	/**
	 * This is a hack. When we are inside the ::GetDynamicMeshElements() of a UGizmoBaseComponent RenderProxy, we need 
	 * to draw to a set of FSceneView and we do not know which one is focused. UGizmoBaseComponent implementations generally
	 * try to adapt their rendering to the current view, and some of that (currently) can only happen at the
	 * level of the RenderProxy. So we need to tell the Component which FSceneView has focus, and we do that
	 * by calling this function from the EdMode.
	 */
    UGC_API void SetGlobalFocusedEditorSceneView(const FSceneView* View);

	/**
	 * This function searches the input Views list for the global currently-focused FSceneView that was last set in
	 * SetGlobalFocusedEditorSceneView(). Note that the Views pointers are different every frame, and 
	 * we are calling SetGlobalFocusedEditorSceneView() on the Game thread but this function from the Render Thread.
	 * So it is entirely possible that we will "miss" our FSceneView and this function will return NULL.
	 * Implementations need to handle this case.
	 */
    UGC_API const FSceneView* FindFocusedEditorSceneView(
		const TArray<const FSceneView*>& Views,
		const FSceneViewFamily& ViewFamily,
		uint32 VisibilityMap);


	/**
	 * Turn on/off the above global focused scene view tracking. When disabled, FindFocusedEditorSceneView()
	 * will return the first visible FSceneView, or the first element of the Views array otherwise.
	 * Enabled by default in Editor builds, disabled by default in Runtime builds.
	 */
    UGC_API void SetGlobalFocusedSceneViewTrackingEnabled(bool bEnabled);


	/**
	 * @return Conversion factor between pixel and world-space coordinates at 3D point Location in View.
	 * @warning This is a local estimate and is increasingly incorrect as the 3D point gets further from Location
	 */
    UGC_API float CalculateLocalPixelToWorldScale(
		const FSceneView* View,
		const FVector& Location);

    UGC_API void RayPlaneIntersectionPoint(
        const FVector& PlaneOrigin, const FVector& PlaneNormal,
        const FVector& RayOrigin, const FVector& RayDirection,
        bool& bIntersectsOut, FVector& PlaneIntersectionPointOut);

    /**
     * Find the closest point to QueryPoint that is on the circle
     * defined by CircleOrigin, CircleNormal, and CircleRadius.
     */
    UGC_API void ClosetPointOnCircle(
        const FVector& QueryPoint,
        const FVector& CircleOrigin, const FVector& CircleNormal, float CircleRadius,
        FVector& ClosestPointOut);

    /**
     * Construct two mutually-perpendicular unit vectors BasisAxis1Out and BasisAxis2Out
     * which are orthogonal to PlaneNormal
     */
    UGC_API void MakeNormalPlaneBasis(
        const FVector& PlaneNormal,
        FVector& BasisAxis1Out, FVector& BasisAxis2Out);

    /**
     * Find the intersection of the ray defined by RayOrigin and RayDirection
     * with the sphere defined by SphereOrigin and SphereRadius.
     * Returns intersection success/failure in bIntersectsOut and the intersection point in SphereIntersectionPointOut
     */
    UGC_API void RaySphereIntersection(
        const FVector& SphereOrigin, const float SphereRadius,
        const FVector& RayOrigin, const FVector& RayDirection,
        bool& bIntersectsOut, FVector& SphereIntersectionPointOut);


    UGC_API void MakeNormalPlaneBasis(
        const FVector& PlaneNormal,
        FVector& BasisAxis1Out, FVector& BasisAxis2Out);
}
