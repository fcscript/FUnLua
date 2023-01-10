#pragma once
#include <string>

template <int BufferSize>
class FCStringBuffer
{
    char *mBuffer;
    int   mSize;
    int   mMaxSize;
    char  TempBuffer[BufferSize];
public:
    FCStringBuffer():mBuffer(nullptr), mSize(0), mMaxSize(0)
    {
        mBuffer = TempBuffer;
        mMaxSize = BufferSize;
        TempBuffer[0] = 0;
    }
    ~FCStringBuffer()
    {
        if(mBuffer != TempBuffer)
        {
            delete []mBuffer;
        }
    }
    FCStringBuffer(const FCStringBuffer &Other):mBuffer(nullptr), mSize(0), mMaxSize(0)
    {
        mBuffer = TempBuffer;
        mMaxSize = BufferSize;
        TempBuffer[0] = 0;
        Copy(Other);
    }
    FCStringBuffer &operator = (const FCStringBuffer &Other)
    {
        Copy(Other);
        return *this;
    }
    void  Copy(const FCStringBuffer &Other)
    {
        if(this != &Other)
        {
            Reserve(Other.mSize + 1);
            mSize = Other.mSize;
            memcpy(mBuffer, Other.mBuffer, Other.mSize);
            mBuffer[mSize] = 0;
        }
    }
public:
    operator const char *()
    {
        return mBuffer;
    }
    const char *GetString() const
    {
        return mBuffer;
    }
    int   Size()
    {
        return mSize;
    }
    void  Empty()
    {
        mSize = 0;
        mBuffer[0] = 0;
    }
    void  Reserve(int nNewSize)
    {
        if(nNewSize < mMaxSize)
        {
            return ;        
        }
        char  *NewBuffer = new char[nNewSize+1];
        memcpy(NewBuffer, mBuffer, mSize);
        if(mBuffer != TempBuffer)
        {
            delete[]mBuffer;
        }
        mBuffer = NewBuffer;
        mMaxSize = nNewSize;
        mBuffer[mSize] = 0;
    }
    void  PushChar(char ch)
    {
        if(mSize >= mMaxSize)
        {
            Reserve(mMaxSize * 2);
        }
        mBuffer[mSize++] = ch;
        mBuffer[mSize] = 0;
    }
    const char* ReadValue(const char *InStr, char chEnd)
    {        
        Empty();
        if(!InStr)
            return InStr;
        for(; *InStr; ++InStr)
        {
            if(*InStr == chEnd)
            {
                ++InStr;
                break;
            }
            PushChar(*InStr);
        }
        return InStr;
    }
    float ReadFloat(const char *&InStr, char chEnd = ',')
    {
        InStr = ReadValue(InStr, chEnd);
        return atof(mBuffer);
    }
    FCStringBuffer &operator << (const char *InStr)
    {
        if(!InStr)
        {
            return *this;
        }
        int Len = strlen(InStr);
        Reserve(mSize + Len + 1);
        for(; *InStr; ++InStr)
        {
            mBuffer[mSize++] = *InStr;
        }
        mBuffer[mSize] = 0;
        return *this;
    }
};

typedef FCStringBuffer<128>  FCStringBuffer128;