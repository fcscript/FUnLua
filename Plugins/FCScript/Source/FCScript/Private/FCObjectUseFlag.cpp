#include "FCObjectUseFlag.h"

FCObjectUseFlag& FCObjectUseFlag::GetIns()
{
    static FCObjectUseFlag s_Ins;
    return s_Ins;
}

void  FCObjectUseFlag::Ref(const UObjectBase* Object)
{
    if(Object)
    {
        m_FlagsCS.Lock();
        m_UseFlags[Object] = true;
        m_FlagsCS.Unlock();
    }
}

void  FCObjectUseFlag::UnRef(const UObjectBase* Object)
{
    m_FlagsCS.Lock();
    m_UseFlags.erase(Object);
    m_FlagsCS.Unlock();
}
