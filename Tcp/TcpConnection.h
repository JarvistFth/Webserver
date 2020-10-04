//
// Created by jarvist on 2020/10/3.
//

#ifndef WEBSERVER_TCPCONNECTION_H
#define WEBSERVER_TCPCONNECTION_H


#include <NonCopyable.h>
#include <memory>
#include <TimeStamp.h>
#include <Buffer.h>
#include <HttpContext.h>
#include "../EventLoop/EventLoop.h"
#include "Socket.h"
#include "INetAddress.h"
#include "Callbacks.h"


class TcpConnection  : NonCopyable, public std::enable_shared_from_this<TcpConnection>{

private:
    enum TcpConnState{
        connecting,
        connected,
        disconnecting,
        disconnected,
    };

    TcpConnState state;
    bool faultError;

    void handleRead(TimeStamp recvTime);
    void handleWrite();
    void handleError();
    void handleClose();

    void shutdownInLoop();

    EventLoop* myloop;
    std::string name;
    std::unique_ptr<Socket> socket;
    std::unique_ptr<Channel> channel;

    INetAddress localAddr;
    INetAddress peerAddr;

    ConnectionCallback connectionCallback;
    OnMessageCallback onMessageCallback;
    WriteCompleteCallback writeCompleteCallback;
    CloseCallback closeCallback;

    Buffer inputBuffer;
    Buffer outputBuffer;

    HttpContext httpContext;

public:
    TcpConnection(EventLoop* loop, const std::string& name,int sockfd,
            const INetAddress& localAddr, const INetAddress& peerAddr);

    ~TcpConnection();

    void setNewTcpConnection(EventLoop* loop, const std::string& name,int sockfd,
                             const INetAddress& localAddr, const INetAddress& peerAddr);
    EventLoop* getloop(){return myloop;};

    const string &getName() const {
        return name;
    }

    const INetAddress &getLocalAddr() const {
        return localAddr;
    }

    const INetAddress &getPeerAddr() const {
        return peerAddr;
    }

    void send(const std::string& msg);
    void sendInLoop(const std::string& msg);
    void shutdown();

    void setTcpNoDelay(bool on);
    void setTcpKeepAlive(bool on);
    bool isConnected(){return state == connected;}

    HttpContext* getHttpContext()  {
        return &httpContext;
    }

    void setHttpContext(const HttpContext &httpContext) {
        TcpConnection::httpContext = httpContext;
    }

    void setConnectionCallback(const ConnectionCallback &connectionCallback) {
        TcpConnection::connectionCallback = connectionCallback;
    }

    void setOnMessageCallback(const OnMessageCallback &onMessageCallback) {
        TcpConnection::onMessageCallback = onMessageCallback;
    }

    void setWriteCompleteCallback(const WriteCompleteCallback &writeCompleteCallback) {
        TcpConnection::writeCompleteCallback = writeCompleteCallback;
    }

    void setCloseCallback(const CloseCallback &closeCallback) {
        TcpConnection::closeCallback = closeCallback;
    }

    void setState(TcpConnState state);

    void connEstablished();
    void connDestroyed();

};


#endif //WEBSERVER_TCPCONNECTION_H
