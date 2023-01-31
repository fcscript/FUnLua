#pragma once

#include "Containers/Set.h"
#include "UObject/GCObject.h"

class FCObjectReferencer : public FGCObject
{
public:
    static FCObjectReferencer& Instance()
    {
        static FCObjectReferencer Referencer;
        return Referencer;
    }

    void AddObjectRef(UObject *Object)
    {
        ReferencedObjects.Add(Object);
    }

    void RemoveObjectRef(UObject *Object)
    {
        ReferencedObjects.Remove(Object);
    }

    void Cleanup()
    {
        return ReferencedObjects.Empty();
    }

    virtual void AddReferencedObjects(FReferenceCollector& Collector) override
    {
        Collector.AddReferencedObjects(ReferencedObjects);
    }

private:
    FCObjectReferencer() {}

    TSet<UObject*> ReferencedObjects;
};

