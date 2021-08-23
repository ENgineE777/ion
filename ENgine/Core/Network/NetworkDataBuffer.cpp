#include "NetworkDataBuffer.h"

#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>

#ifndef PC
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include "Sockets.h"

void NetStaticBuffer::CopyTo(byte* dst)
{
    ::memcpy(dst, buffer_, GetSize());
}

void NetStaticBuffer::PackBYTE(byte num)
{
    if (GetSize() + sizeof(num) >= max_size_ && !ProcessOverflow(sizeof(num)))
    {
        return;
    }

    pos_[0] = ((num >> 0) & 0xFF);
    pos_ += sizeof(num);
}

void NetStaticBuffer::PackSHORT(word num)
{
    if (GetSize() + sizeof(num) >= max_size_ && !ProcessOverflow(sizeof(num)))
    {
        return;
    }

    pos_[0] = ((num >> 8) & 0xFF);
    pos_[1] = ((num >> 0) & 0xFF);
    pos_ += sizeof(num);
}

void NetStaticBuffer::PackDWORD(dword num)
{
    if (GetSize() + sizeof(num) >= max_size_ && !ProcessOverflow(sizeof(num)))
    {
        return;
    }

    pos_[0] = ((num >> 24) & 0xFF);
    pos_[1] = ((num >> 16) & 0xFF);
    pos_[2] = ((num >> 8) & 0xFF);
    pos_[3] = ((num >> 0) & 0xFF);
    pos_ += sizeof(num);
}

void NetStaticBuffer::PackQWORD(uint64 num)
{
    if (GetSize() + sizeof(num) >= max_size_ && !ProcessOverflow(sizeof(num)))
    {
        return;
    }

    pos_[0] = ((num >> 56) & 0xFF);
    pos_[1] = ((num >> 48) & 0xFF);
    pos_[2] = ((num >> 40) & 0xFF);
    pos_[3] = ((num >> 32) & 0xFF);
    pos_[4] = ((num >> 24) & 0xFF);
    pos_[5] = ((num >> 16) & 0xFF);
    pos_[6] = ((num >> 8) & 0xFF);
    pos_[7] = ((num >> 0) & 0xFF);
    pos_ += sizeof(num);
}

void NetStaticBuffer::PackDATA(const byte* data, int data_len)
{
    PackDWORD(data_len);
    PackRAW(data, data_len);
}

void NetStaticBuffer::PackDATA(const NetStaticBuffer& data)
{
    PackDATA(&data[0], data.GetSize());
}

void NetStaticBuffer::PackRAW(const byte* data, int data_len)
{
    if (GetSize() + data_len >= max_size_ && !ProcessOverflow(data_len))
    {
        return;
    }

    ::memcpy(pos_, data, data_len);
    pos_ += data_len;
}

void NetStaticBuffer::PackRAW(const NetStaticBuffer& data)
{
    PackRAW(&data[0], data.GetSize());
}

void NetStaticBuffer::PackBase64(const byte* data, int data_len)
{
    BIO *bmem, *b64;
    BUF_MEM *bptr;

    b64 = BIO_new(BIO_f_base64());
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, data, data_len);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);

    const int b64_len = bptr->length - 1;
    if (GetSize() + b64_len >= max_size_ && !ProcessOverflow(b64_len))
    {
        BIO_free_all(b64);
        return;
    }

    ::memcpy(pos_, bptr->data, b64_len);
    pos_ += b64_len;

    BIO_free_all(b64);
}

byte NetStaticBuffer::UnpackBYTE()
{
    byte num = pos_[0];

    pos_ += sizeof(num);

    return num;
}

word NetStaticBuffer::UnpackSHORT()
{
    word num = 0;

    num |= (((dword)pos_[0]) << 8);
    num |= (((dword)pos_[1]) << 0);

    pos_ += sizeof(num);

    return num;
}

dword NetStaticBuffer::UnpackDWORD()
{
    dword num = 0;

    BIN2DWORD(pos_, num);

    pos_ += sizeof(num);

    return num;
}

uint64 NetStaticBuffer::UnpackQWORD()
{
    uint64 num = 0;

    BIN2QWORD(pos_, num);

    pos_ += sizeof(num);

    return num;
}

void NetStaticBuffer::UnpackDATA(NetStaticBuffer& buffer)
{
    dword len = UnpackDWORD();

    buffer.used_size_ = 0;

    if (len >= buffer.max_size_ && !buffer.ProcessOverflow(len))
    {
        return;
    }

    ::memcpy(&buffer[0], pos_, len);

    pos_ += len;
    buffer.used_size_ = len;
}
void NetStaticBuffer::UnpackBase64(NetStaticBuffer& buffer) const
{
    BIO* b64 = BIO_new(BIO_f_base64());

    BIO* bio_in = BIO_new_mem_buf((void*)buffer_, GetSize());
    bio_in = BIO_push(b64, bio_in);

    BIO_set_flags(bio_in, BIO_FLAGS_BASE64_NO_NL);

    byte read_buffer[4096] = {'\0'};

    buffer.used_size_ = 0;

    int sz = -1;
    do
    {
        sz = BIO_read(bio_in, read_buffer, sizeof(read_buffer));
        if (sz > 0)
        {
            buffer.PackRAW(read_buffer, sz);
        }
    }
    while (sz > 0);

    buffer.used_size_ = buffer.GetSize();

    BIO_free_all(bio_in);
}

void NetStaticBuffer::Send(int sockfd)
{
  //  send(sockfd, (const char*)buffer_, GetSize(), 0);
    int size  =  GetSize();
    SendAll(sockfd, (const char*)buffer_,&size);
}

void NetStaticBuffer::SendDATA(int sockfd)
{
    static NetStaticBuffer buffer;

    buffer.Reset();
    buffer.PackDATA(*this);
    int size  =  buffer.GetSize();
    //send(sockfd, (const char*)&buffer[0], buffer.GetSize(), 0);
    SendAll(sockfd, (const char*)&buffer[0],&size);
}

void NetStaticBuffer::RecvDATA(int sockfd)
{
    Reset();

    byte data_len_buffer[4];

    recv(sockfd, (char*)&data_len_buffer[0], sizeof(dword), 0);

    dword data_len = 0;
    BIN2DWORD(data_len_buffer, data_len);

    if (data_len >= max_size_ && !ProcessOverflow(data_len))
    {
        return;
    }

    recv(sockfd, (char*)buffer_, data_len, 0);
}

bool NetStaticBuffer::IsEqual(const char* str)
{
    const int offset = ::strlen(str);

    if (GetSize() - offset < 0)
    {
        return false;
    }

    return ::strcmp((const char*)pos_ - offset, str) == 0;
}

NetBlockBuffer::NetBlockBuffer(int block_size, int size) : NetStaticBuffer(size), block_size_(block_size)
{
}

void NetBlockBuffer::ResetToSize(int size)
{
    Reset();

    if (max_size_ > size)
    {
        delete[] buffer_;

        buffer_ = new byte[size];
        max_size_ = size;
        pos_ = buffer_;
    }
}

bool NetBlockBuffer::ProcessOverflow(int overflow_size)
{
    const int offset = (int)(pos_ - buffer_);
    const int inc_size = overflow_size > block_size_ ? (overflow_size + block_size_) : block_size_;

    const int new_size = max_size_ + inc_size;
    byte* new_buffer = new byte[new_size];

    ::memcpy(new_buffer, buffer_, max_size_);
    delete[] buffer_;

    buffer_ = new_buffer;
    max_size_ = new_size;

    pos_ = buffer_ + offset;

    return true;
}