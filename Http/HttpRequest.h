//
// Created by jarvist on 5/11/20.
//

#ifndef WEBSERVER_HTTPREQUEST_H
#define WEBSERVER_HTTPREQUEST_H


#include <string>
#include <map>
#include "../Utils/TimeStamp.h"
using std::string;
using std::map;

class HttpRequest : Copyable{

public:
    enum Method{
        INVALID,
        GET,
        POST,
        PUT,
        DELETE
    };

    enum Version{
        UNKNOWN,
        HTTP_10,
        HTTP_11
    };

    HttpRequest():method(INVALID),version(UNKNOWN){};
    ~HttpRequest()= default;

    void setVersion(Version v){
        version = v;
    }
    Version getVersion() const{
        return version;
    }

    bool setMethod(const char* start, const char* end){
        string m(start,end);
        if(m == "GET"){
            method = GET;
        }else if(m == "POST"){
            method = POST;
        }else if(m == "PUT"){
            method = PUT;
        }else if(m == "DELETE"){
            method = DELETE;
        }else{
            method = INVALID;
        }
        return method != INVALID;
    }

    Method getMethod()const {
        return method;
    }

    void setPath(const char* begin, const char* end){
        path.assign(begin,end);
    }

    const string& getPath() const {
        return path;
    }

    void setQuery(const char*begin, const char* end){
        query.assign(begin,end);
    }
    const string& getQuery()const{
        return query;
    }

    void setReceivedTime(TimeStamp timeStamp){
        receiveTime = timeStamp;
    }

    TimeStamp getReceivedTime()const {
        return receiveTime;
    }

    void addHeader(const char* begin, const char* colon, const char*end){
        string field(begin, colon);
        ++colon;
        while(colon < end && isspace(*colon)){
            colon++;
        }
        string value(colon,end);
        //process space
        while (!value.empty() && isspace(value[value.size() - 1]))
        {
            value.resize(value.size() - 1);
        }
        headers[field] = value;
    }

    string getHeader(const string& field) const {
        string result;
        //[] will insert key into map; so use std::find(begin(),end())
        auto it = headers.find(field);
        if(it != headers.end()){
            result = it->second;
        }
        return result;
    }

    const map<string,string>& getHeaders() const {
        return headers;
    }

    void swap(HttpRequest& that){
        std::swap(method,that.method);
        std::swap(version, that.version);
        path.swap(that.path);
        query.swap(that.query);
        receiveTime.swap(that.receiveTime);
        headers.swap(that.headers);
    }

private:
    Method method;
    Version version;
    string path;
    string query;
    TimeStamp receiveTime;
    map<string,string>headers;

};


#endif //WEBSERVER_HTTPREQUEST_H
