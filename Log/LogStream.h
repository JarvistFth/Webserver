//
// Created by jarvist on 3/28/20.
//

#ifndef WEBSERVER_LOGSTREAM_H
#define WEBSERVER_LOGSTREAM_H


#include "../Base/NonCopyable.h"
#include "../Base/FixedBuffer.h"

class LogStream : NonCopyable {




public:

    typedef FixedBuffer<smallSize> Buffer;
    LogStream& operator<<(bool v){
        buffer.append(v?"1":"0",1);
        return *this;
    }

    LogStream& operator<<(short v);
    LogStream& operator<<(unsigned short v);
    LogStream& operator<<(int v);
    LogStream& operator<<(unsigned int v);
    LogStream& operator<<(long v);
    LogStream& operator<<(unsigned long v);
    LogStream& operator<<(long long v);
    LogStream& operator<<(unsigned long long v);

    LogStream& operator<<(float v) {
        *this << static_cast<double>(v);
        return *this;
    }
    LogStream& operator<<(double v);
    LogStream& operator<<(long double v);

    LogStream& operator<<(char v) {
        buffer.append(&v, 1);
        return *this;
    }

    LogStream& operator<<(const char* str) {
        if (str)
            buffer.append(str, strlen(str));
        else
            buffer.append("(null)", 6);
        return *this;
    }

    LogStream& operator<<(const unsigned char* str) {
        return operator<<(reinterpret_cast<const char*>(str));
    }

    LogStream& operator<<(const std::string& v) {
        buffer.append(v.c_str(), v.size());
        return *this;
    }

    void append(const char* data,int len){
        buffer.append(data,len);
    }

    const Buffer& getBuffer() const {
        return buffer;
    }

    void resetBuffer(){
        buffer.reset();
    }

private:
    Buffer buffer;
    static const int MaxNumSize = 32;

    template <typename T>
    void itoc(T v);
};




#endif //WEBSERVER_LOGSTREAM_H
