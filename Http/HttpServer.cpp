//
// Created by jarvist on 2020/10/4.
//

#include "HttpServer.h"
#include "HttpContext.h"
//#include "TcpConnection.h"

void defaultHttpCallback(const HttpRequest& req, HttpResponse *resp){
    if(req.getPath() == "/good"){
        resp->setStatusCode(HttpResponse::OK);
        resp->setStatusMsg("OK");
        resp->setContentType("text/plain");
        resp->addHeader("Server", "WebServer");
        string ret("ret");
        ret+="\n";
        resp->setBody(ret);
    }else if (req.getPath() == "/")
    {
        resp->setStatusCode(HttpResponse::OK);
        resp->setStatusMsg("OK");
        resp->setContentType("text/html");
        resp->addHeader("Server", "Webserver");
        string now = TimeStamp::now().toFormattedString(false);
        resp->setBody("<html><head><title>This is title</title></head>"
                      "<body><h1>Hello</h1>Now is " +
                      now +
                      "</body></html>");
    }
}

void HttpServer::setHttpCallback(const HttpServer::HttpCallback &httpCallback) {
    HttpServer::httpCallback = httpCallback;
}

HttpServer::HttpServer(EventLoop *loop, const INetAddress &listenAddr, const string &name, int tNums) :
server(loop,listenAddr,name,tNums),httpCallback(defaultHttpCallback)
{
    server.setConnectionCallback(std::bind(&HttpServer::onHttpConnection,this,_1));
    server.setOnMessageCallback(std::bind(&HttpServer::onHttpMessage,this,_1,_2,_3));
}

HttpServer::~HttpServer() {

}

void HttpServer::start() {
    server.start();
}

void HttpServer::onHttpConnection(const TcpConnectionPtr &conn) {
    if(conn->isConnected()){
        conn->setHttpContext(HttpContext());
    }
}

void HttpServer::onHttpMessage(const TcpConnectionPtr &conn, Buffer *buf, TimeStamp recvTime) {
    HttpContext* ctx = conn->getHttpContext();
    if(!ctx->parseRequest(buf,recvTime)){
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }
    if(ctx->getAll()){
        onHttpRequest(conn,ctx->getRequest());
        ctx->reset();
    }
}

void HttpServer::onHttpRequest(const TcpConnectionPtr &conn, const HttpRequest &req) {
    const string& connection = req.getHeader("Connection");
    bool close = connection == "close" ||
                 (req.getVersion() == HttpRequest::HTTP_10 && connection!= "Keep-Alive");
    HttpResponse response(close);
    defaultHttpCallback(req,&response);
    Buffer buf;
    response.appendToBuffer(&buf);
    string tmp = buf.retrieveAsString();
    conn->send(tmp);
    if(response.isCloseConnection()){
        conn->shutdown();
    }
}

