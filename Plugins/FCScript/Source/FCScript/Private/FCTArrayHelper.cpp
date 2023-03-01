#include "FCTArrayHelper.h"


void  FCTArrayHelper::Resize(int NewNum)
{
    FProperty* Inner = ArrayProperty->Inner;
    int32 OldNum = ScriptArray->Num();
    uint8* ObjAddr = (uint8*)ScriptArray->GetData();
    uint8* ValueAddr = ObjAddr;
    if (OldNum < NewNum)
    {
        int32 Index = ScriptArray_Add(ScriptArray, NewNum - OldNum, ElementSize);
        for (; Index < NewNum; ++Index)
        {
            ValueAddr = ObjAddr + Index * ElementSize;
            Inner->InitializeValue(ValueAddr);
        }
    }
    else
    {
        for (int32 Index = OldNum; Index < NewNum; ++Index)
        {
            ValueAddr = ObjAddr + Index * ElementSize;
            Inner->DestroyValue(ValueAddr);
        }
        ScriptArray_Remove(ScriptArray, OldNum, NewNum - OldNum, ElementSize);
    }
}

void  FCTArrayHelper::Copy(const FScriptArray* OtherArray)
{
    ArrayProperty->CopyValuesInternal(ScriptArray, OtherArray, ArrayProperty->ArrayDim);
}

void  FCTArrayHelper::Shuffle()
{
    if(!ScriptArray)
        return ;

    int32 LastIndex = Num() - 1;
    for (int32 i = 0; i <= LastIndex; ++i)
    {
        int32 Index = FMath::RandRange(i, LastIndex);
        if (i != Index)
        {
            ScriptArray->SwapMemory(i, Index, ElementSize);
        }
    }
}