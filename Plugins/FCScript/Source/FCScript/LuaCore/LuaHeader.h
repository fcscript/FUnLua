#pragma once

// TODO: 在此处引用程序需要的其他头文件
extern "C"
{
#include "../src/lua.h"
#include "../src/lualib.h"
#include "../src/lauxlib.h"
}

struct FLuaValue
{
    int  ValueIdx;
    bool bValid;
    FLuaValue() :ValueIdx(0), bValid(false) {}
    FLuaValue(int nValueIdx) :ValueIdx(nValueIdx), bValid(true) {}
};

struct FLuaRetValues
{
    int TopIdx;
    int RetCount;  // 变量数
    FLuaRetValues() :TopIdx(0), RetCount(0)
    {
    }
    FLuaRetValues(int nTopIdx, int nRetCount) :TopIdx(nTopIdx), RetCount(nRetCount)
    {

    }
    bool IsValid() const
    {
        return RetCount > 0;
    }
    int size() const
    {
        return RetCount;
    }
    int operator [](int nIndex) const
    {
        return TopIdx + nIndex;
    }
};

