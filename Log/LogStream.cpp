//
// Created by jarvist on 3/28/20.
//

#include "LogStream.h"
#include <algorithm>
#include <string.h>

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;

template<typename T>
size_t Convert2String(char buf[],T value){
    T i = value;
    char *p = buf;
    do{
        int lsd = static_cast<int>(i % 10);
        i /= 10;
        *p++ = zero[lsd];
    }while(i);

    if(value<0){
        *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf,p);
    return p - buf;
}

template<typename T>
void LogStream::itoc(T v) {
    //over MaxNumSize would discard
    if(buffer.getAvail() >= MaxNumSize){
        auto len = Convert2String(buffer.current(),v);
        buffer.add(len);
    }
}

LogStream &LogStream::operator<<(short v) {
    *this << static_cast<int>(v);
    return *this;
}

LogStream &LogStream::operator<<(unsigned short v) {
    *this<< static_cast<unsigned int>(v);
    return *this;
}

LogStream &LogStream::operator<<(int v) {
    itoc(v);
    return *this;
}

LogStream &LogStream::operator<<(unsigned int v) {
    itoc(v);
    return *this;
}

LogStream &LogStream::operator<<(long v) {
    itoc(v);
    return *this;
}

LogStream &LogStream::operator<<(unsigned long v) {
    itoc(v);
    return *this;
}

LogStream &LogStream::operator<<(long long v) {
    itoc(v);
    return *this;
}

LogStream &LogStream::operator<<(unsigned long long v) {
    itoc(v);
    return *this;
}

LogStream &LogStream::operator<<(double v) {
    if (buffer.getAvail() >= MaxNumSize) {
        int len = snprintf(buffer.current(), MaxNumSize, "%.12g", v);
        buffer.add(len);
    }
    return *this;

}

LogStream &LogStream::operator<<(long double v) {
    if (buffer.getAvail() >= MaxNumSize) {
        int len = snprintf(buffer.current(), MaxNumSize, "%.12Lg", v);
        buffer.add(len);
    }
    return *this;
}


