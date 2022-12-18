#include "FCTMapHelper.h"


void  FCTMapHelper::Copy(const FScriptMap* OtherArray)
{
    MapProperty->CopyValuesInternal(ScriptMap, OtherArray, MapProperty->ArrayDim);
}