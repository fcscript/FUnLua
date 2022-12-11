#pragma once

#include "CoreMinimal.h"

#include "../LuaCore/LuaHeader.h"

class FCSCRIPT_API FCScriptDelegates
{
public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnLuaContextCreateWrap, lua_State*);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnLuaContextStart, lua_State*);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnLuaContextCleanup, lua_State*);
    DECLARE_DELEGATE_RetVal_TwoParams(bool, FLuaFileLoader, const char*, TArray<uint8>&);
    DECLARE_DELEGATE_RetVal_OneParam(int32, FGenericLuaDelegate, lua_State*);

    static FOnLuaContextCreateWrap OnLuaStateCreateWrap;  // LUA����������󣬿���ִ��һЩע����¼�wrap
    static FOnLuaContextStart   OnLuaStateStart; // ִ����wrapע��󣬿�ʼִ�д�����
    static FOnLuaContextCleanup OnLuaContextCleanup; // ������ͷ��߼�
    static FLuaFileLoader LoadLuaFile;  // �����ļ������ؽӿ�
    static FGenericLuaDelegate ReportLuaCallError;
};