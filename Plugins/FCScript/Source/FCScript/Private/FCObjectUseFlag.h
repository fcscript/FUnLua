#pragma once
#include "UObject/UnrealType.h"
#include "UObject/ObjectMacros.h"
#include "CoreUObject.h"
#include "FCStringCore.h"

class FCObjectUseFlag
{
    typedef  std::unordered_map<const UObjectBase*, bool>   CObjectUseFlagMap;
public:
    static FCObjectUseFlag &GetIns();
public:
    void  Ref(const UObjectBase* Object);
    void  UnRef(const UObjectBase* Object);
    bool  IsRef(const UObjectBase* Object)
    {
        m_FlagsCS.Lock();
        bool bRef = m_UseFlags.find(Object) != m_UseFlags.end();
        m_FlagsCS.Unlock();
        return bRef;
    }
private:
    CObjectUseFlagMap  m_UseFlags;
    FCriticalSection   m_FlagsCS;      // critical section for accessing 'Candidates'
};