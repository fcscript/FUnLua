#include "FCInitWrap.h"
#include "FCUObjectWrap.h"
#include "FCUEUtilWrap.h"
#include "FCDelegateWrap.h"
#include "FCMulticastDelegateWrap.h"
#include "FCTArrayWrap.h"
#include "FCTMapWrap.h"
#include "TWeakObjectPtrWrap.h"
#include "TLazyObjectPtrWrap.h"
#include "TSoftObjectPtrWrap.h"
#include "FCTMapIteratorWrap.h"
#include "FCTSetWrap.h"
#include "FCVector2Wrap.h"
#include "FCVector3Wrap.h"
#include "FCVector4Wrap.h"
#include "FCColorWrap.h"
#include "FCIntPointWrap.h"
#include "FCIntVectorWrap.h"
#include "FCLinearColorWrap.h"
#include "FCQuatWrap.h"
#include "FCRotatorWrap.h"
#include "FCTransformWrap.h"
#include "FCClassWrap.h"
#include "FCDataTableWrap.h"
#include "FCUnLuaLib.h"
#include "FCUELibWrap.h"
#include "FCWorldWrap.h"
#include "FCActorSpawnParametersWrap.h"


void FCInitWrap::Register(lua_State* L)
{
	FCUObjectWrap::Register(L);
    FCClassWrap::Register(L);
	FCUEUtilWrap::Register(L);
	FCDelegateWrap::Register(L);
	FCMulticastDelegateWrap::Register(L);
	FCTArrayWrap::Register(L);
	FCTMapWrap::Register(L);
	TWeakObjectPtrWrap::Register(L);
	TLazyObjectPtrWrap::Register(L);
	TSoftObjectPtrWrap::Register(L);
    TSoftClassPtrWrap::Register(L);
	FCTMapIteratorWrap::Register(L);
	FCTSetWrap::Register(L);
	FCVector2Wrap::Register(L);
	FCVector3Wrap::Register(L);
	FCVector4Wrap::Register(L);
	FCColorWrap::Register(L);
	FCIntPointWrap::Register(L);
	FCIntVectorWrap::Register(L);
	FCLinearColorWrap::Register(L);
	FCQuatWrap::Register(L);
	FCRotatorWrap::Register(L);
	FCTransformWrap::Register(L);
    FCDataTableWrap::Register(L);

    FCUnLuaWrap::Register(L);
    FCUELibWrap::Register(L);
    FCActorSpawnParametersWrap::Register(L);
    FCWorldWrap::Register(L);
}

