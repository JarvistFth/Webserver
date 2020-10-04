//
// Created by jarvist on 5/8/20.
//

#ifndef WEBSERVER_HTTPCONTEXT_H
#define WEBSERVER_HTTPCONTEXT_H


#include "HttpRequest.h"
#include "../Base/Buffer.h"

class HttpContext : Copyable{
public:
    enum ParseState{
        EXPECT_METHOD,
        EXPECT_HEADERS,
        EXPECT_BODY,
        GETALL,
    };
    HttpContext():state(EXPECT_METHOD){};
    ~HttpContext() = default;

    bool parseRequest(Buffer* input,TimeStamp receivedTime);
    bool getAll()const{
        return state == GETALL;
    }
    void reset(){
        state = EXPECT_METHOD;
        HttpRequest reset;
        request.swap(reset);
    }
    const HttpRequest& getRequest()const{
        return request;
    }

    HttpRequest& getRequest(){
        return request;
    }

private:

    bool processLine(const char*begin ,const char* end);
    ParseState state;
    HttpRequest request;
    static const char kCRLF[];

};


#endif //WEBSERVER_HTTPCONTEXT_H
