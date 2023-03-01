#include "FCMulticastDelegateWrap.h"
#include "FCDelegateWrap.h"
#include "FCObjectManager.h"
#include "FCGetObj.h"


void FCMulticastDelegateWrap::Register(lua_State* L)
{
	luaL_requiref(L, "MulticastDelegateEvent", FCDelegateWrap::RegisterDelegate, 1);
    luaL_requiref(L, "MulticastSparseDelegateEvent", FCDelegateWrap::RegisterDelegate, 1);
}
