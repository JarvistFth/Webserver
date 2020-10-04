//
// Created by jarvist on 3/26/20.
//

#ifndef WEBSERVER_BUFFER_H
#define WEBSERVER_BUFFER_H


#include <vector>
#include <cassert>
#include <string.h>
#include <string>
#include <algorithm>
//#include "Endian.h"
#include "Copyable.h"

class Buffer : Copyable{
private:
    std::vector<char> buffer;
    size_t readIndex;
    size_t writeIndex;
    static const char kCRLF[];

protected:
    char* begin(){
        return &*buffer.begin();
    }

    const char* begin() const{
        return &*buffer.begin();
    }

    char* end(){
        return &*buffer.end();
    }

    const char* end() const{
        return &*buffer.end();
    }



    void makeSpace(int len){
        if(writeableBytes() + prependableBytes() < len + kCheapPrependable){
            buffer.resize(len + writeIndex);

        }else{
            assert(kCheapPrependable < readIndex);
            size_t readable = readableBytes();
            std::copy(begin() + readIndex,begin()+writeIndex,begin()+kCheapPrependable);
            readIndex = kCheapPrependable;
            writeIndex = readIndex + readable;
            assert(readable == readableBytes());
        }
    }


public:
    static const size_t kCheapPrependable = 8;
    static const size_t kInitSize = 1024;
    explicit Buffer(size_t initialized = kInitSize): buffer(kCheapPrependable + kInitSize),
                                                     readIndex(kCheapPrependable), writeIndex(kCheapPrependable){
        assert(writeableBytes() == kInitSize);
        assert(readableBytes() == 0);
        assert(prependableBytes() == kCheapPrependable);

    }

    size_t writeableBytes ()const{
        return buffer.size() - writeIndex;
    }

    size_t readableBytes() const{
        return writeIndex - readIndex;
    }

    size_t prependableBytes() const{
        return readIndex;
    }

    char* beginWrite(){
        return begin() + writeIndex;
    }

    const char* beginWrite() const{
        return begin() + writeIndex;
    }

    void hasWriten(size_t len){
        assert(len <= writeableBytes());
        writeIndex += len;
    }

    void unWrite(size_t len){
        assert(len < readableBytes());
        writeIndex -= len;
    }

    void retrieveAll(){
        readIndex = kCheapPrependable;
        writeIndex = kCheapPrependable;
    }

    void retrieve(size_t len){
        assert(len <= readableBytes());
        if(len < readableBytes()){
            readIndex += len;
        }else{
            retrieveAll();
        }
    }

    void retrieveUntil(const char* end)
    {
        assert(peek() <= end);
        assert(end <= beginWrite());
        retrieve(end - peek());
    }

    void retrieveInt64()
    {
        retrieve(sizeof(int64_t));
    }

    void retrieveInt32()
    {
        retrieve(sizeof(int32_t));
    }

    void retrieveInt16()
    {
        retrieve(sizeof(int16_t));
    }

    void retrieveInt8()
    {
        retrieve(sizeof(int8_t));
    }

    std::string retrieveAllAsString()
    {
        return retrieveAsString(readableBytes());
    }

    std::string retrieveAsString()
    {
        std::string str(peek(), readableBytes());
        retrieveAll();
        return str;
    }

    std::string retrieveAsString(size_t len)
    {
        assert(len <= readableBytes());
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

//    void appendInt64(int64_t x)
//    {
//        int64_t be64 = sockets::hostToNetwork64(x);
//        append(&be64, sizeof be64);
//    }

    ///
    /// Append int32_t using network endian
    ///
//    void appendInt32(int32_t x)
//    {
//        int32_t be32 = sockets::hostToNetwork32(x);
//        append(&be32, sizeof be32);
//    }
//
//    void appendInt16(int16_t x)
//    {
//        int16_t be16 = sockets::hostToNetwork16(x);
//        append(&be16, sizeof be16);
//    }

    void appendInt8(int8_t x)
    {
        append(&x, sizeof x);
    }

    ///
    /// Read int64_t from network endian
    ///
    /// Require: buf->readableBytes() >= sizeof(int32_t)
//    int64_t readInt64()
//    {
//        int64_t result = peekInt64();
//        retrieveInt64();
//        return result;
//    }

    ///
    /// Read int32_t from network endian
    ///
    /// Require: buf->readableBytes() >= sizeof(int32_t)
//    int32_t readInt32()
//    {
//        int32_t result = peekInt32();
//        retrieveInt32();
//        return result;
//    }

//    int16_t readInt16()
//    {
//        int16_t result = peekInt16();
//        retrieveInt16();
//        return result;
//    }

    int8_t readInt8()
    {
        int8_t result = peekInt8();
        retrieveInt8();
        return result;
    }

    ///
    /// Peek int64_t from network endian
    ///
    /// Require: buf->readableBytes() >= sizeof(int64_t)



//    int64_t peekInt64() const
//    {
//        assert(readableBytes() >= sizeof(int64_t));
//        int64_t be64 = 0;
//        ::memcpy(&be64, peek(), sizeof be64);
//        return sockets::networkToHost64(be64);
//    }

    ///
    /// Peek int32_t from network endian
    ///
    /// Require: buf->readableBytes() >= sizeof(int32_t)
//    int32_t peekInt32() const
//    {
//        assert(readableBytes() >= sizeof(int32_t));
//        int32_t be32 = 0;
//        ::memcpy(&be32, peek(), sizeof be32);
//        return sockets::networkToHost32(be32);
//    }

//    int16_t peekInt16() const
//    {
//        assert(readableBytes() >= sizeof(int16_t));
//        int16_t be16 = 0;
//        ::memcpy(&be16, peek(), sizeof be16);
//        return sockets::networkToHost16(be16);
//    }

    int8_t peekInt8() const
    {
        assert(readableBytes() >= sizeof(int8_t));
        int8_t x = *peek();
        return x;
    }

    ///
    /// Prepend int64_t using network endian
    ///
//    void prependInt64(int64_t x)
//    {
//        int64_t be64 = sockets::hostToNetwork64(x);
//        prepend(&be64, sizeof be64);
//    }

    ///
    /// Prepend int32_t using network endian
    ///
//    void prependInt32(int32_t x)
//    {
//        int32_t be32 = sockets::hostToNetwork32(x);
//        prepend(&be32, sizeof be32);
//    }

//    void prependInt16(int16_t x)
//    {
//        int16_t be16 = sockets::hostToNetwork16(x);
//        prepend(&be16, sizeof be16);
//    }

    void prependInt8(int8_t x)
    {
        prepend(&x, sizeof x);
    }

    void prepend(const void* /*restrict*/ data, size_t len)
    {
        assert(len <= prependableBytes());
        readIndex -= len;
        const char* d = static_cast<const char*>(data);
        std::copy(d, d+len, begin()+readIndex);
    }

    void shrink(size_t reserve)
    {
        // FIXME: use vector::shrink_to_fit() in C++ 11 if possible.
        Buffer other;
        other.ensureWriteableBytes(readableBytes()+reserve);
//        buffer.shrink_to_fit();
        other.append(toString());
        swap(other);
    }

    void shrink(){
        buffer.shrink_to_fit();
    }


    size_t internalCapacity() const
    {
        return buffer.capacity();
    }

    /// Read data directly into buffer.
    ///
    /// It may implement with readv(2)
    /// @return result of read(2), @c errno is saved
    ssize_t readFd(int fd, int* savedErrno);



    void swap(Buffer& rhs){
        buffer.swap(rhs.buffer);
        std::swap(readIndex,rhs.readIndex);
        std::swap(writeIndex,rhs.writeIndex);
    }

//    char* peek(){
//        return begin() + readIndex;
//    }

    const char* peek() const{
        return begin() + readIndex;
    }



    void append(const std::string& str){
        append(str.data(),str.size());
    }

    void append(const void* data, size_t len)
    {
        append(static_cast<const char*>(data), len);
    }

    void append(const char* data,size_t len){
        ensureWriteableBytes(len);
        std::copy(data,data+len,beginWrite());
        hasWriten(len);
    }

    void ensureWriteableBytes(size_t len){
        if(writeableBytes() < len){
            makeSpace(len);
        }
        assert(writeableBytes() >= len);

    }



    std::string toString(){
        return std::string(peek(), static_cast<int>(readableBytes()));
    }

    const char* findCRLF() const
    {
        // FIXME: replace with memmem()?
        const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF+2);
        return crlf == beginWrite() ? NULL : crlf;
    }

    const char* findCRLF(const char* start) const
    {
        assert(peek() <= start);
        assert(start <= beginWrite());
        // FIXME: replace with memmem()?
        const char* crlf = std::search(start, beginWrite(), kCRLF, kCRLF+2);
        return crlf == beginWrite() ? NULL : crlf;
    }

    const char* findEOL() const
    {
        const void* eol = memchr(peek(), '\n', readableBytes());
        return static_cast<const char*>(eol);
    }

    const char* findEOL(const char* start) const
    {
        assert(peek() <= start);
        assert(start <= beginWrite());
        const void* eol = memchr(start, '\n', beginWrite() - start);
        return static_cast<const char*>(eol);
    }



};


#endif //WEBSERVER_BUFFER_H
