//
// Created by jarvist on 5/11/20.
//

#ifndef WEBSERVER_HTTPRESPONSE_H
#define WEBSERVER_HTTPRESPONSE_H


#include <map>
#include "../Base/Copyable.h"
#include "../Base/Buffer.h"

using std::map;
using std::string;

class HttpResponse : Copyable{
public:
    enum StatusCode{
        UNKNOWN,
        OK=200,
        MOVE_PERMANENTLY = 301,
        BAD_REQUEST = 400,
        UNAUTHORIZED = 401,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        INTERNAL_SERVER_ERROR = 500,
        SERVICE_UNVAILABLE = 503

    };
    explicit HttpResponse(bool close):closeConnection(close),statusCode(UNKNOWN)
    {};
    ~HttpResponse() = default;


public:
    StatusCode getStatusCode() const {
        return statusCode;
    }

    void setStatusCode(StatusCode statusCode) {
        HttpResponse::statusCode = statusCode;
    }

    const string &getStatusMsg() const {
        return statusMsg;
    }

    void setStatusMsg(const string &statusMsg) {
        HttpResponse::statusMsg = statusMsg;
    }

    const string &getBody() const {
        return body;
    }

    void setBody(const string &body) {
        HttpResponse::body = body;
    }
    bool isCloseConnection() const {
        return closeConnection;
    }
    void setCloseConnection(bool closeConnection) {
        HttpResponse::closeConnection = closeConnection;
    }
    void setContentType(const string &contentType)
    {
        addHeader("Content-Type", contentType);
    }

    void addHeader(const string &key, const string &value)
    {
        headers[key] = value;
    }


    void appendToBuffer(Buffer *output) const;

private:
    map<string,string> headers;
    StatusCode statusCode;
    string statusMsg;
    bool closeConnection;
    string body;

};


#endif //WEBSERVER_HTTPRESPONSE_H
