#include "FCTMapIteratorWrap.h"
#include "Containers/Map.h"
#include "FCTemplateType.h"

#include "FCObjectManager.h"
#include "FCGetObj.h"

static FCExportedClass  GTMapIteratorWrapClass("TMapIterator");

void FCTMapIteratorWrap::Register(lua_State* L)
{
    GTMapIteratorWrapClass.SetCustomRegister();
    luaL_requiref(L, "TMapIterator", LibOpen_wrap, 1);
}

int FCTMapIteratorWrap::LibOpen_wrap(lua_State* L)
{
    const LuaRegFunc LibFuncs[] =
    {
        //{ "key", Key_wrap },
        { "IsValid", IsValid_wrap },
        { "ToNext", ToNext_wrap },
        { "Reset", Reset_wrap },
        { nullptr, nullptr }
    };
    const LuaRegAttrib LibAttrib[] =
    {
        { "key", Key_wrap, nullptr },
        { "value", GetValue_wrap, SetValue_wrap },
        { nullptr, nullptr, nullptr }
    };
    const LuaRegFunc LibTable[] =
    {
        { "__index", obj_Index},
        { "__newindex", obj_NewIndex},
        { "__gc", FCExportedClass::obj_Delete },
        { "__call", obj_new },
        { "__eq", FCExportedClass::obj_equal },
        { nullptr, nullptr }
    };

    const char* ClassName = lua_tostring(L, 1);
    GTMapIteratorWrapClass.RegisterLibClass(L, ClassName, LibFuncs, LibAttrib, LibTable);
    return 1;
}

int FCTMapIteratorWrap::obj_new(lua_State* L)
{
    TMapIterator* Iterator = new TMapIterator();
    int64 ObjID = FCGetObj::GetIns()->PushMapIterator(Iterator);
    FCScript::PushBindObjRef(L, ObjID, "TMapIterator");
	return 1;
}

int FCTMapIteratorWrap::obj_Index(lua_State* L)
{
    return GTMapIteratorWrapClass.DoLibIndex(L);  // 这里不用通用的是为了省一次类型查找
}

int FCTMapIteratorWrap::obj_NewIndex(lua_State* L)
{
    return GTMapIteratorWrapClass.DoLibNewIndex(L);
}

int FCTMapIteratorWrap::Key_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    if (ObjRef && ObjRef->RefType == EFCObjRefType::MapIterator)
    {
        TMapIterator* Iterator = ObjRef->GetMapIterator();
        if (Iterator)
        {
            FScriptMap* ScriptMap = FCTMapIteratorWrap::GetScriptMap(Iterator->MapInsID);
            if (ScriptMap && ScriptMap->IsValidIndex(Iterator->Index))
            {
                FCObjRef* MapRef = FCGetObj::GetIns()->FindValue(Iterator->MapInsID);

                FMapProperty* MapProperty = (FMapProperty*)MapRef->DynamicProperty->Property;
                FProperty* KeyProp = MapProperty->KeyProp;
                const FScriptMapLayout& MapLayout = MapProperty->MapLayout;

                FCDynamicProperty  KeyProperty;
                KeyProperty.InitProperty(KeyProp);

                uint8* PairPtr = (uint8*)ScriptMap->GetData(Iterator->Index, MapLayout);
                KeyProperty.m_WriteScriptFunc(L, &KeyProperty, PairPtr, nullptr, nullptr);
                return 1;
            }
        }
    }
	return 0;
}

int FCTMapIteratorWrap::GetValue_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    if (ObjRef && ObjRef->RefType == EFCObjRefType::MapIterator)
    {
		TMapIterator  *Iterator = ObjRef->GetMapIterator();
		if(Iterator)
        {
            FScriptMap* ScriptMap = FCTMapIteratorWrap::GetScriptMap(Iterator->MapInsID);
			if(ScriptMap && ScriptMap->IsValidIndex(Iterator->Index))
            {
                FCObjRef* MapRef = FCGetObj::GetIns()->FindValue(Iterator->MapInsID);

                FMapProperty* MapProperty = (FMapProperty*)MapRef->DynamicProperty->Property;
                FProperty* ValueProp = MapProperty->ValueProp;
                const FScriptMapLayout& MapLayout = MapProperty->MapLayout;

                FCDynamicProperty  ValueProperty;
                ValueProperty.InitProperty(ValueProp);

                uint8* PairPtr = (uint8*)ScriptMap->GetData(Iterator->Index, MapLayout);
                uint8* Result = PairPtr + MapLayout.ValueOffset;
                ValueProperty.m_WriteScriptFunc(L, &ValueProperty, Result, nullptr, nullptr);
                return 1;
			}
		}
    }
	return 0;
}
int FCTMapIteratorWrap::SetValue_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    if (ObjRef && ObjRef->RefType == EFCObjRefType::MapIterator)
    {
        TMapIterator* Iterator = ObjRef->GetMapIterator();
        if (Iterator)
        {
            FScriptMap* ScriptMap = FCTMapIteratorWrap::GetScriptMap(Iterator->MapInsID);
            if (ScriptMap && ScriptMap->IsValidIndex(Iterator->Index))
            {
                FCObjRef* MapRef = FCGetObj::GetIns()->FindValue(Iterator->MapInsID);

                FMapProperty* MapProperty = (FMapProperty*)MapRef->DynamicProperty->Property;
                FProperty* ValueProp = MapProperty->ValueProp;
                const FScriptMapLayout& MapLayout = MapProperty->MapLayout;

                FCDynamicProperty  ValueProperty;
                ValueProperty.InitProperty(ValueProp);

                uint8* PairPtr = (uint8*)ScriptMap->GetData(Iterator->Index, MapLayout);
                uint8* Result = PairPtr + MapLayout.ValueOffset;
                ValueProperty.m_ReadScriptFunc(L, 2, &ValueProperty, Result, nullptr, nullptr);
            }
        }
    }
	return 0;
}

int FCTMapIteratorWrap::IsValid_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    bool bValid = false;
    if (ObjRef && ObjRef->RefType == EFCObjRefType::MapIterator)
    {
        TMapIterator* Iterator = ObjRef->GetMapIterator();
        if (Iterator)
        {
            FScriptMap* ScriptMap = FCTMapIteratorWrap::GetScriptMap(Iterator->MapInsID);
            if(ScriptMap && ScriptMap->IsValidIndex(Iterator->Index))
            {
                bValid = true;
            }
        }        
    }
    lua_pushboolean(L, bValid);
	return 1;
}

int FCTMapIteratorWrap::ToNext_wrap(lua_State* L)
{
    FCObjRef* ObjRef = (FCObjRef*)FCScript::GetObjRefPtr(L, 1);
    bool bValid = false;
    if (ObjRef && ObjRef->RefType == EFCObjRefType::MapIterator)
    {
        TMapIterator* Iterator = ObjRef->GetMapIterator();
        if (Iterator)
        {
            FScriptMap* ScriptMap = FCTMapIteratorWrap::GetScriptMap(Iterator->MapInsID);
            if (ScriptMap)
            {
                ++(Iterator->Index);
                Iterator->Index = FCTMapIteratorWrap::ToNextValidIterator(ScriptMap, Iterator->Index);
                bValid = ScriptMap->IsValidIndex(Iterator->Index);
            }
        }
    }
    lua_pushboolean(L, bValid);
	return 0;
}

int FCTMapIteratorWrap::Reset_wrap(lua_State* L)
{
	return 0;
}

FScriptMap* FCTMapIteratorWrap::GetScriptMap(int64 ObjID)
{
    FCObjRef* ObjRef = FCGetObj::GetIns()->FindValue(ObjID);
    if (ObjRef && ObjRef->DynamicProperty)
    {
        if (ObjRef->DynamicProperty->Type == FCPropertyType::FCPROPERTY_Map)
        {
            FScriptMap* ScriptMap = (FScriptMap*)ObjRef->GetThisAddr();
			return ScriptMap;
		}
	}
	return nullptr;
}

int32 FCTMapIteratorWrap::ToNextValidIterator(FScriptMap* ScriptMap, int32 NextIndex)
{
	int32 nMaxIndex = ScriptMap->GetMaxIndex();
	for(; NextIndex < nMaxIndex; ++NextIndex)
	{
		if(ScriptMap->IsValidIndex(NextIndex))
		{
			return NextIndex;
		}
	}
	return NextIndex;
}
