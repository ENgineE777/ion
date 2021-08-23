#pragma once

#include "Common/Common.h"

#define BIN2DWORD(ptr, num) { num = 0; num |= (((dword)(ptr)[0]) << 24); num |= (((dword)(ptr)[1]) << 16); num |= (((dword)(ptr)[2]) << 8); num |= (((dword)(ptr)[3]) << 0); }
#define BIN2QWORD(ptr, num) { num = 0; num |= (((uint64)(ptr)[0]) << 56); num |= (((uint64)(ptr)[1]) << 48); num |= (((uint64)(ptr)[2]) << 40); num |= (((uint64)(ptr)[3]) << 32); num |= (((uint64)(ptr)[4]) << 24); num |= (((uint64)(ptr)[5]) << 16); num |= (((uint64)(ptr)[6]) << 8); num |= (((uint64)(ptr)[7]) << 0); }

#ifdef SendMessage
#undef SendMessage
#endif

class NetStaticBuffer
{
public:
    NetStaticBuffer(int size = 8192) : buffer_(new byte[size]), max_size_(size), used_size_(0)
    {
        Reset();
    }

    virtual ~NetStaticBuffer()
    {
        delete buffer_;
    }

    virtual byte& operator[](int i)
    {
        return buffer_[i];
    }

    virtual const byte& operator[](int i) const
    {
        return buffer_[i];
    }

    virtual void Reset()
    {
        pos_ = buffer_;
        ::memset(buffer_, 0, max_size_);
    }

    virtual int GetSize() const
    {
        return used_size_ > 0 ? used_size_ : ((int)(pos_ - buffer_));
    }

    virtual void CopyTo(byte* dst);


    virtual void Send(int sockfd);
    virtual void SendDATA(int sockfd);
    virtual void RecvDATA(int sockfd);

    virtual void PackBYTE(byte num);
    virtual void PackSHORT(word num);
    virtual void PackDWORD(dword num);
    virtual void PackQWORD(uint64 num);

    virtual void PackDATA(const byte* data, int data_len);
    virtual void PackDATA(const NetStaticBuffer& data);

    virtual void PackRAW(const byte* data, int data_len);
    virtual void PackRAW(const NetStaticBuffer& data);

    virtual void PackBase64(const byte* data, int data_len);

    virtual byte UnpackBYTE();
    virtual word UnpackSHORT();
    virtual dword UnpackDWORD();
    virtual uint64 UnpackQWORD();

    virtual void UnpackDATA(NetStaticBuffer& buffer);
    virtual void UnpackBase64(NetStaticBuffer& buffer) const;

    virtual void SetUsedSize(int val) { used_size_ = val; }

    virtual bool IsEqual(const char* str);

protected:
    virtual bool ProcessOverflow(int overflow_size)
    {
        return false;
    }

    int max_size_;
    int used_size_;

    byte* buffer_;
    byte* pos_;

private:
    NetStaticBuffer(const NetStaticBuffer&);
    void operator=(const NetStaticBuffer&);
};

class NetBlockBuffer : public NetStaticBuffer
{
public:
    NetBlockBuffer(int block_size, int size = 8192);

    void ResetToSize(int size);

protected:
    virtual bool ProcessOverflow(int overflow_size);

private:
    int block_size_;
};