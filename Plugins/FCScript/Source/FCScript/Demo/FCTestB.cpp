#include "FCTestB.h"

#include "FCObjectManager.h"
#include "FCGetObj.h"

void UFCTestB::SetBasePtr(UFCTestB* Obj, UObject* Ptr)
{
    if(Obj)
    {
        Obj->BasePtr = Ptr;
    }
}

void UFCTestB::SetPtr(UObject* Ptr)
{
    BasePtr = Ptr;
}
