//
// Created by jarvist on 3/28/20.
//

#ifndef WEBSERVER_FIXEDBUFFER_H
#define WEBSERVER_FIXEDBUFFER_H

#include "NonCopyable.h"
#include <string>
#include <cstring>

const int smallSize = 4000;
const int largeSize = 4000*1000;

template <int SIZE>
class FixedBuffer:NonCopyable{
private:
    char data[SIZE];
    char* cur;
    const char* end() const {
        return data + sizeof(data);
    }

public:
    FixedBuffer():cur(data){};
    ~FixedBuffer(){}

    int getAvail ()const{
        return static_cast<int>(end()-cur);
    }
    int length ()const {
        return static_cast<int>(cur - data);
    }
    const char * getData() const {
        return data;
    }
    char* current(){
        return cur;
    }

    void add(size_t len){
        cur += len;
    }

    void reset(){
        cur = data;
    }

    void bzero(){
        memset(data,0, sizeof(data));
    }

    void append(const char* buf,size_t len){
        if(getAvail() > len){
            memcpy(cur,buf,len);
            cur += len;
        }
    }

};


#endif //WEBSERVER_FIXEDBUFFER_H
