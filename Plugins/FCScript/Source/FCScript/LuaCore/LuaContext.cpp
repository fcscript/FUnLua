#include "LuaContext.h"
#include "FCScriptDelegates.h"

//extern "C"
//{
//#include "lfunc.h"
//#include "lstate.h"
//#include "lobject.h"
//}

//static TValue* GetTValue(lua_State* L, int32 Index)
//{
//    CallInfo* ci = L->ci;
//    if (Index > 0)
//    {
//        TValue* V = ci->func + Index;
//        checkf(Index <= ci->top - (ci->func + 1), TEXT("LUA STACK OVERFLOW!!! Maybe there are bugs in your codes, or use lua_checkstack() to make sure there is enough space on the lua stack!"));
//        return V >= L->top ? (TValue*)NULL : V;
//    }
//    else if (Index > LUA_REGISTRYINDEX)             // negative
//    {
//        check(Index != 0 && -Index <= L->top - (ci->func + 1));
//        return L->top + Index;
//    }
//    else if (Index == LUA_REGISTRYINDEX)
//    {
//        return &G(L)->l_registry;
//    }
//    else                                            // upvalues
//    {
//        Index = LUA_REGISTRYINDEX - Index;
//        check(Index <= MAXUPVAL + 1);
//        if (ttislcf(ci->func))
//        {
//            return (TValue*)NULL;                   // light C function has no upvalues
//        }
//        else
//        {
//            CClosure* Closure = clCvalue(ci->func);
//            return (Index <= Closure->nupvalues) ? &Closure->upvalue[Index - 1] : (TValue*)NULL;
//        }
//    }
//}

uint8 CalcUserdataPadding(int32 Alignment)
{
    //return (uint8)(Align(sizeof(UUdata), Alignment) - sizeof(UUdata));      // sizeof(UUdata) == 40
    return (uint8)(Align(40, Alignment) - 40);      // sizeof(UUdata) == 40
}

bool TryToSetMetatable(lua_State* L, const char* MetatableName)
{
    int32 Type = luaL_getmetatable(L, MetatableName);
    if (Type == LUA_TTABLE)
    {
        lua_setmetatable(L, -2);                                    // set the metatable directly
        return true;
    }
    else
    {
        lua_pop(L, 1);
        //if (RegisterClass(L, MetatableName))                        // register class first
        //{
        //    luaL_getmetatable(L, MetatableName);                    // get the metatable and push it on the top
        //    lua_setmetatable(L, -2);                                // set the metatable
        //    return true;
        //}
    }
    return false;
}

#define BIT_VARIANT_TAG            (1 << 7)         // variant tag for userdata
#define BIT_TWOLEVEL_PTR        (1 << 5)            // two level pointer flag
#define BIT_SCRIPT_CONTAINER    (1 << 4)            // script container (TArray, TSet, TMap) flag

/**
 * Set padding size info for userdata
 */
void SetUserdataPadding(lua_State* L, int32 Index, uint8 PaddingBytes)
{
    //TValue* Value = GetTValue(L, Index);
    //if ((Value->tt_ & 0x0F) == LUA_TUSERDATA && PaddingBytes < 128)
    //{
    //    Udata* U = uvalue(Value);
    //    U->ttuv_ |= BIT_VARIANT_TAG;
    //    U->user_.b = (int32)PaddingBytes;
    //}
}

void* NewUserdataWithPadding(lua_State* L, int32 Size, const char* MetatableName, uint8 PaddingSize)
{
    if (Size < 1)
    {
        // userdata size must be greater than 0
        //UNLUA_LOGERROR(L, LogUnLua, Warning, TEXT("%s, Invalid size!"), ANSI_TO_TCHAR(__FUNCTION__));
        return nullptr;
    }
    if ((PaddingSize & 0x07) != 0)          // 8 bytes padding at least..., 8, 24, 88
    {
        // padding size must be greater or equal to 8 bytes
        //UNLUA_LOGERROR(L, LogUnLua, Warning, TEXT("%s, Invalid padding size!"), ANSI_TO_TCHAR(__FUNCTION__));
        return nullptr;
    }

    void* Userdata = lua_newuserdata(L, Size + PaddingSize);        // userdata size must add padding size
    if (PaddingSize > 0)
    {
        SetUserdataPadding(L, -1, PaddingSize);                     // store padding size in internal userdata structure
    }
    if (MetatableName)
    {
        bool bSuccess = TryToSetMetatable(L, MetatableName);        // set metatable
        if (!bSuccess)
        {
            //UNLUA_LOGERROR(L, LogUnLua, Warning, TEXT("%s, Invalid metatable, metatable name: !"), ANSI_TO_TCHAR(__FUNCTION__), ANSI_TO_TCHAR(MetatableName));
            return nullptr;
        }
    }
    return (uint8*)Userdata + PaddingSize;                          // return 'valid' address (userdata memory address + padding size)
}

struct FTableFlagList
{
    typedef std::unordered_map<const void *, bool>   CVoidPtrMap;
    CVoidPtrMap  Flags;
public:
    bool IsExit(const void* TableAddr) const
    {
        CVoidPtrMap::const_iterator itFlag = Flags.find(TableAddr);
        return itFlag != Flags.end();
    }
    bool Push(const void* TableAddr)
    {
        if (IsExit(TableAddr))
        {
            return false;
        }
        Flags[TableAddr] = true;
        return true;
    }
};

//CallGlobalVoidFunction(L, "UnLua");
//FLuaRetValues cloneReturn = CallGlobalLua(L, "clone", SrcValue);
//FLuaValue cloneTable = cloneReturn[0];
FLuaValue   DepthCloneTable(lua_State* L, int SrcIdx, FTableFlagList& TableFlags)
{
    int nTopIdx = lua_gettop(L);
    // 如果使用栈顶，就转换成栈底
    if (SrcIdx < 0)
    {
        SrcIdx += nTopIdx + 1;
    }
    int nValueType = lua_type(L, SrcIdx);
    if (nValueType != LUA_TTABLE)
    {
        return FLuaValue();
    }
    const void* TableAddr = lua_topointer(L, SrcIdx);
    TableFlags.Push(TableAddr);

    // 创建一个新的Table
    lua_newtable(L);
    FLuaValue  RetValue(lua_gettop(L));

    bool ontop = SrcIdx == -1 || SrcIdx == (lua_gettop(L));
    // 如果不在栈顶上，就压入此表的一份引用到栈顶
    if (!ontop)
        lua_pushvalue(L, SrcIdx);

    lua_pushnil(L);
    // 现在的栈：-2 => table; -1 => nil
    while (lua_next(L, -2))
    {
        lua_pushvalue(L, -2);
        //int nMemberType1 = lua_type(L, -1);
        int nMemberType2 = lua_type(L, -2);
        lua_pushvalue(L, -2);
        if (LUA_TTABLE == nMemberType2)
        {
            const char* key = lua_tostring(L, -2); // key, 由于置换了，所以这个是-2，不再是-1
            if (strcmp(key, "__index") == 0)
            {
                lua_rawset(L, RetValue.ValueIdx);
            }
            else
            {
                TableAddr = lua_topointer(L, -1);
                if (TableFlags.IsExit(TableAddr))
                {
                    lua_rawset(L, RetValue.ValueIdx);
                }
                else
                {
                    FLuaValue  childTable = DepthCloneTable(L, -1, TableFlags);
                    if (childTable.bValid)
                    {
                        lua_replace(L, -2);
                        lua_rawset(L, RetValue.ValueIdx);
                    }
                    else
                    {
                        lua_pop(L, 2);
                    }
                }
            }
        }
        else
        {
            lua_rawset(L, RetValue.ValueIdx);
        }
        lua_pop(L, 1);
    }
    int nTopIdx2 = lua_gettop(L);
    lua_setmetatable(L, RetValue.ValueIdx);
    int nCurTopIdx = lua_gettop(L);
    int nCount = nCurTopIdx - nTopIdx - 1;
    if (nCount > 0)
    {
        lua_pop(L, nCount);
    }
    return RetValue;
}

FLuaValue   CloneLuaTable(lua_State* L, int SrcIdx)
{
    FTableFlagList  Flags;
    lua_getglobal(L, "clone");
    if (lua_isfunction(L, -1) == false)
    {
        lua_pop(L, 1);
        return DepthCloneTable(L, SrcIdx, Flags);
    }
    int32 MessageHandlerIdx = lua_gettop(L) - 1;
    lua_pushvalue(L, SrcIdx);
    int32 Code = lua_pcall(L, 1, LUA_MULTRET, MessageHandlerIdx);
    int32 TopIdx = lua_gettop(L);
    if (Code == LUA_OK)
    {
        int32 NumResults = TopIdx - MessageHandlerIdx;
        if (NumResults > 0)
        {
            return FLuaValue(TopIdx);
        }
    }
    return DepthCloneTable(L, SrcIdx, Flags);
}

int LoopTable(lua_State* L, int TableIdx, void (*Callback)(lua_State* L, int Index, void* UserData), void* UserData)
{
    int Count = 0;
    int TopIdx = lua_gettop(L);
    int ValueType = lua_type(L, TableIdx);
    if (ValueType != LUA_TTABLE)
    {
        return 0;
    }
    bool bOnTop = TableIdx == -1 || TableIdx == TopIdx;
    // 如果不在栈顶上，就压入此表的一份引用到栈顶
    if (!bOnTop)
        lua_pushvalue(L, TableIdx);

    lua_pushnil(L);
    // 现在的栈：-2 => table; -1 => nil
    while (lua_next(L, -2))
    {
        lua_pushvalue(L, -2);
        if (Callback)
        {
            Callback(L, Count, UserData);  // // key : -1, vlaue : -2
        }
        ++Count;
        lua_pop(L, 2);
    }
    int CurTopIdx = lua_gettop(L);
    lua_pop(L, CurTopIdx - TopIdx);

    return Count;
}

int ReportLuaCallError(lua_State* L)
{
    if (FCScriptDelegates::ReportLuaCallError.IsBound())
    {
        return FCScriptDelegates::ReportLuaCallError.Execute(L);      // developers can provide error reporter themselves
    }
    int Type = lua_type(L, -1);
    if (Type == LUA_TSTRING)
    {
        const char* ErrorString = lua_tostring(L, -1);
        luaL_traceback(L, L, ErrorString, 1);
        ErrorString = lua_tostring(L, -1);
        UE_LOG(LogFCScript, Error, TEXT("Lua error message: %s"), UTF8_TO_TCHAR(ErrorString));
    }
    else if (Type == LUA_TTABLE)
    {
        // multiple errors is possible
        int MessageIndex = 0;
        lua_pushnil(L);
        while (lua_next(L, -2) != 0)
        {
            const char* ErrorString = lua_tostring(L, -1);
            UE_LOG(LogFCScript, Error, TEXT("Lua error message %d : %s"), MessageIndex++, UTF8_TO_TCHAR(ErrorString));
            lua_pop(L, 1);
        }
    }
    lua_pop(L, 1);
    return 0;
}

bool GlobDoRequireFileData(lua_State* L, const char* ModuleName, TArray<uint8> &Data)
{
    int FileLen = Data.Num();
    int32 SkipLen = (3 < Data.Num()) && (0xEF == Data[0]) && (0xBB == Data[1]) && (0xBF == Data[2]) ? 3 : 0;        // skip UTF-8 BOM mark
    const char* FileData = (const char*)Data.GetData();

    int32 Code = luaL_loadbufferx(L, FileData + SkipLen, FileLen - SkipLen, ModuleName, 0);        // loads the buffer as a Lua chunk
    if (Code != LUA_OK)
    {
        ReportLuaCallError(L);
    }
    return Code == LUA_OK;
}

bool GlbTryLoadLuaFile(lua_State* L, const char* ModuleName)
{
    if (FCScriptDelegates::LoadLuaFile.IsBound())
    {
        TArray<uint8> Data;
        if (FCScriptDelegates::LoadLuaFile.Execute(ModuleName, Data))
        {
            return GlobDoRequireFileData(L, ModuleName, Data);
        }
    }

    FString  FileName(UTF8_TO_TCHAR(ModuleName));
    FileName.ReplaceCharInline('.', '/');

    FString  ScriptPath = FPaths::ProjectContentDir();
    ScriptPath += TEXT("Script/");
    ScriptPath += FileName;
    ScriptPath += TEXT(".lua");

    TArray<uint8> Data;
    if (FFileHelper::LoadFileToArray(Data, *ScriptPath, 0))
    {
        return GlobDoRequireFileData(L, ModuleName, Data);
    }
    else
    {
        ScriptPath = FPaths::ProjectContentDir();
        ScriptPath += FileName;
        ScriptPath += TEXT(".lua");
        if (FFileHelper::LoadFileToArray(Data, *ScriptPath, 0))
        {
            return GlobDoRequireFileData(L, ModuleName, Data);
        }
    }
    return false;
}

int Global_Require(lua_State* L)
{
    const char* ModuleName = lua_tostring(L, 1);
    if (!ModuleName)
    {
        //UNLUA_LOGERROR(L, LogUnLua, Log, TEXT("%s: Invalid module name!"), ANSI_TO_TCHAR(__FUNCTION__));
        return 0;
    }
    lua_settop(L, 1);       /* LOADED table will be at index 2 */
    lua_getfield(L, LUA_REGISTRYINDEX, LUA_LOADED_TABLE);
    lua_getfield(L, 2, ModuleName);
    if (lua_toboolean(L, -1))
    {
        return 1;
    }
    if (!GlbTryLoadLuaFile(L, ModuleName))
    {
        return 0;
    }

    lua_pushvalue(L, 1);
    //lua_pushstring(L, ModuleName);
    //lua_call(L, 2, 1);
    lua_call(L, 1, 1);

    if (!lua_isnil(L, -1))
    {
        lua_setfield(L, 2, ModuleName);
    }
    if (lua_getfield(L, 2, ModuleName) == LUA_TNIL)
    {
        lua_pushboolean(L, 1);
        lua_pushvalue(L, -1);
        lua_setfield(L, 2, ModuleName);
    }
    return 1;
}

int Global_LoadFile(lua_State* L)
{
    const char* ModuleName = lua_tostring(L, 1);
    if (!ModuleName)
    {
        return 0;
    }
    if (GlbTryLoadLuaFile(L, ModuleName))
    {
        return 1;
    }
    lua_pushnil(L);
    return 1;
}

int Global_Print(lua_State* L)
{
    int ParamCount = lua_gettop(L);
    // print(arg1, arg2, arg3, ...)
    FString StrLog;
    for (int32 i = 1; i <= ParamCount; ++i)
    {
        const char* arg = luaL_tolstring(L, i, nullptr);
        if (!arg)
        {
            arg = "";
        }
        StrLog += UTF8_TO_TCHAR(arg);
        StrLog += TEXT("    ");
    }

    UE_LOG(LogFCScript, Log, TEXT("== FCScript : %s"), *StrLog);
    //if (IsInGameThread())
    //{
        //UKismetSystemLibrary::PrintString(GWorld, StrLog, false, false);
    //}
    return 0;
}