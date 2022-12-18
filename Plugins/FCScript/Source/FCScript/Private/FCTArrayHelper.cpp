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

void  FCTArrayHelper::CopyArray(const FScriptArray* OtherArray)
{
    bool bBaseCopy = IsBaseTypeCopy(ArrayProperty->Inner);
    int Num = OtherArray->Num();
    Resize(Num);
    if(bBaseCopy)
    {
        if(Num > 0)
            FMemory::Memcpy(ScriptArray->GetData(), OtherArray->GetData(), ElementSize * Num);
    }
    else
    {
        uint8* DesAddr = (uint8*)ScriptArray->GetData();
        uint8* SrcAddr = (uint8*)OtherArray->GetData();
        for(int i = 0; i< Num; ++i)
        {
            ArrayProperty->CopyValuesInternal(DesAddr, SrcAddr, 1);
            DesAddr += ElementSize;
            SrcAddr += ElementSize;
        }
    }
}