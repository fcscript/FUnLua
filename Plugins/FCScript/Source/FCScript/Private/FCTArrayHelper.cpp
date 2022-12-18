#include "FCTArrayHelper.h"


void  FCTArrayHelper::Resize(int NewNum)
{
    FProperty* Inner = ArrayProperty->Inner;
    int32 OldNum = ScriptArray->Num();
    uint8* ObjAddr = (uint8*)ScriptArray->GetData();
    uint8* ValueAddr = ObjAddr;
    if (OldNum < NewNum)
    {
        int32 Index = ScriptArray->Add(NewNum - OldNum, ElementSize);
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
        ScriptArray->Remove(OldNum, NewNum - OldNum, ElementSize);
    }
}

void  FCTArrayHelper::Copy(const FScriptArray* OtherArray)
{
    ArrayProperty->CopyValuesInternal(ScriptArray, OtherArray, ArrayProperty->ArrayDim);
}