#include "FCScriptDelegates.h"


FCScriptDelegates::FOnLuaContextCreateWrap FCScriptDelegates::OnLuaStateCreateWrap;  // LUA����������󣬿���ִ��һЩע����¼�wrap
FCScriptDelegates::FOnLuaContextStart   FCScriptDelegates::OnLuaStateStart; // ִ����wrapע��󣬿�ʼִ�д�����
FCScriptDelegates::FOnLuaContextCleanup FCScriptDelegates::OnLuaContextCleanup; // ������ͷ��߼�
FCScriptDelegates::FLuaFileLoader FCScriptDelegates::LoadLuaFile;  // �����ļ������ؽӿ�
FCScriptDelegates::FGenericLuaDelegate FCScriptDelegates::ReportLuaCallError;