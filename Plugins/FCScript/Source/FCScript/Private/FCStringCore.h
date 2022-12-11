#pragma once

#include <vector>
#include <string>
#include <hash_map>
//#include <unordered_map>

#include "../StringCore/FDList.h"

struct fc_hash_string_key
{
    static size_t  Hash(const char* Key)
    {
        if (!Key)
        {
            return 0;
        }
        size_t  nHash = 0;
        while (*Key)
        {
            nHash = (nHash << 5) + nHash + *Key++;
        }
        return nHash;
    }
    static bool Less(const char* Key1, const char* Key2)
    {
        if (!Key1)
            Key1 = "";
        if (!Key2)
            Key2 = "";
        return strcmp(Key1, Key2) < 0;
    }
};

template<> struct stdext::hash_compare<const char *>
{
    enum
    {	// parameters for hash table
        bucket_size = 1		// 0 < bucket_size
    };
    size_t operator()(const char * Key) const
    {	// hash _Keyval to size_t value by pseudorandomizing transform
        return fc_hash_string_key::Hash(Key);
    }

    bool operator()(const char * key1, const char * key2) const
    {	// test if _Keyval1 ordered before _Keyval2
        return fc_hash_string_key::Less(key1, key2);
    }
};

struct FCDoubleKey
{
    const char* KeyType;
    const char* ValueType;
    FCDoubleKey() :KeyType(nullptr), ValueType(nullptr) {}
    FCDoubleKey(const char* InKeyType, const char* InValueType) :KeyType(InKeyType), ValueType(InValueType) {}
};

template<> struct stdext::hash_compare<FCDoubleKey>
{
    enum
    {	// parameters for hash table
        bucket_size = 1		// 0 < bucket_size
    };
    size_t operator()(const FCDoubleKey& Key) const
    {	// hash _Keyval to size_t value by pseudorandomizing transform
        return fc_hash_string_key::Hash(Key.KeyType) + fc_hash_string_key::Hash(Key.ValueType);
    }

    bool operator()(const FCDoubleKey& key1, const FCDoubleKey& key2) const
    {	// test if _Keyval1 ordered before _Keyval2
        return fc_hash_string_key::Less(key1.KeyType, key2.KeyType) && fc_hash_string_key::Less(key1.ValueType, key2.ValueType);
    }
};

struct ObjRefKey
{
	const unsigned char* ParentAddr;    // 
    const unsigned char* OffsetPtr;    // 对象自己的地址
	ObjRefKey() :ParentAddr(nullptr), OffsetPtr(nullptr) {}
    ObjRefKey(const void* InParentAddr, const void* InOffsetPtr) : ParentAddr((const unsigned char* )InParentAddr), OffsetPtr((const unsigned char* )InOffsetPtr) {}
};

template<> struct stdext::hash_compare<ObjRefKey>
{
	enum
	{	// parameters for hash table
		bucket_size = 1		// 0 < bucket_size
	};
	size_t operator()(const ObjRefKey& Key) const
	{	// hash _Keyval to size_t value by pseudorandomizing transform
        //return (size_t)(Key.ParentAddr + Key.OffsetPtr);
        return (size_t)Key.ParentAddr + (size_t)Key.OffsetPtr;
	}

	bool operator()(const ObjRefKey& key1, const ObjRefKey& key2) const
	{	// test if _Keyval1 ordered before _Keyval2
        if(key1.OffsetPtr == key2.OffsetPtr)
            return key1.ParentAddr < key2.ParentAddr;
        else
            return key1.OffsetPtr < key2.OffsetPtr;
	}
};


template <class _TyPtrMap>
void  ReleasePtrMap(_TyPtrMap &PtrMap)
{
	while (PtrMap.size() > 0)
	{
		_TyPtrMap::iterator itPtr = PtrMap.begin();
		auto Ptr = itPtr->second;
		PtrMap.erase(itPtr);
        if(Ptr)
        {
		    delete Ptr;
        }
	}
}