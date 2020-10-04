//
// Created by jarvist on 2020/10/4.
//

#ifndef WEBSERVER_HTTPSERVER_H
#define WEBSERVER_HTTPSERVER_H


#include <NonCopyable.h>
#include <TcpServer.h>
#include "HttpRequest.h"
#include "HttpResponse.h"

class HttpServer : NonCopyable{
    typedef std::function<void(const HttpRequest &, HttpResponse*)> HttpCallback;
private:
    void onHttpConnection(const TcpConnectionPtr& conn);
    void onHttpMessage(const TcpConnectionPtr& conn, Buffer* buf, TimeStamp recvTime);
    void onHttpRequest(const TcpConnectionPtr& conn, const HttpRequest &req);

    TcpServer server;
    HttpCallback httpCallback;

public:
    HttpServer(EventLoop* loop, const INetAddress& listenAddr, const string& name,int tNums);
    ~HttpServer();

    void start();
    void setThread(int tNums){
        server.setThreadNums(tNums);
    }
    void setHttpCallback(const HttpCallback &httpCallback);

};


#endif //WEBSERVER_HTTPSERVER_H
