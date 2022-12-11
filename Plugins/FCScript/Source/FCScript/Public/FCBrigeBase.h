#pragma once
#include "FCGetArg.h"
#include "FCSetArg.h"

struct FCFunctionReturnVoid
{
};
struct FCFunctionReturnValue
{
};

template <class T>
struct FCFunctionReturnTraits
{
	typedef FCFunctionReturnValue  RetType;
};
template<> struct FCFunctionReturnTraits<void> { typedef FCFunctionReturnVoid  RetType; };

template<typename T>
T  FCReadScriptArg(lua_State* L, int& Index)
{
	T  Value = T();
	FCScript::GetArgValue(L, Index++, Value);
	return Value;
}


template <typename RetType, typename... ArgType>
int  FCInvokeFunctionBase(lua_State* L, RetType(*InFunc)(ArgType...), FCFunctionReturnVoid)
{
	int nIndex = 1;
	InFunc(FCReadScriptArg<ArgType>(L, nIndex)...);
    return 0;
}

template <typename RetType, typename... ArgType>
int  FCInvokeFunctionBase(lua_State* L, RetType(*InFunc)(ArgType...), FCFunctionReturnValue)
{
	int nIndex = 1;
	RetType Ret = InFunc(FCReadScriptArg<ArgType>(L, nIndex)...);
	// 将结果写入到脚本
    FCScript::SetArgValue(L, Ret);
    return 1;
}

template <typename RetType, typename... ArgType>
int  FCInvokeStaticFunction(lua_State* L, RetType(*InFunc)(ArgType...))
{
	return FCInvokeFunctionBase<RetType, ArgType...>(L, InFunc, FCFunctionReturnTraits<RetType>::RetType());
}


template <typename RetType, typename ClassType, typename... ArgType>
int  FCInvokeClassFunctionBase(lua_State* L, ClassType* ThisPtr, RetType(ClassType::* InFunc)(ArgType...), FCFunctionReturnVoid)
{
	int nIndex = 2;
	(ThisPtr->InFunc)(FCReadScriptArg<ArgType>(L, nIndex)...);
    return 0;
}

template <typename RetType, typename ClassType, typename... ArgType>
int  FCInvokeClassFunctionBase(lua_State* L, ClassType* ThisPtr, RetType(ClassType::* InFunc)(ArgType...), FCFunctionReturnValue)
{
	int nIndex = 2;
	RetType Ret = (ThisPtr->*InFunc)(FCReadScriptArg<ArgType>(L, nIndex)...);
	// 将结果写入到脚本
    FCScript::SetArgValue(L, Ret);
    return 1;
}

template <typename RetType, typename ClassType, typename... ArgType>
int  FCInvokeClassFunction(lua_State* L, ClassType* ThisPtr, RetType(ClassType::* InFunc)(ArgType...))
{
	int Ret = FCInvokeClassFunctionBase<RetType, ClassType, ArgType...>(L, ThisPtr, InFunc, FCFunctionReturnTraits<RetType>::RetType());
    return Ret;
}
