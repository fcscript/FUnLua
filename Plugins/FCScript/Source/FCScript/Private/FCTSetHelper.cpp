#include "FCTSetHelper.h"

void  FCTSetHelper::Copy(const FScriptSet* Other)
{
    SetProperty->CopyValuesInternal(ScriptSet, Other, SetProperty->ArrayDim);
}

