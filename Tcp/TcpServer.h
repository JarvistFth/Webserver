//
// Created by jarvist on 2020/10/3.
//

#ifndef WEBSERVER_TCPSERVER_H
#define WEBSERVER_TCPSERVER_H


#include <NonCopyable.h>
#include <string>
#include <bits/unique_ptr.h>
#include <memory>
#include <functional>
#include <Buffer.h>
#include <TimeStamp.h>
#include <map>
#include "INetAddress.h"
#include "Callbacks.h"
#include "TcpConnection.h"

class Acceptor;
class EventLoop;
class EventLoopThreadPool;
//class TcpConnection;


typedef std::map<std::string,TcpConnectionPtr> ConnectionMap;

class TcpServer : NonCopyable{



private:
    EventLoop* loop_;
    const std::string ipPort;
    const std::string name;
    std::unique_ptr<Acceptor> acceptor;

    ConnectionCallback connectionCallback;
    OnMessageCallback onMessageCallback;
    WriteCompleteCallback writeCompleteCallback;

    bool started;
    ConnectionMap connections;
    int nextConnId;
    int threadNums;

    std::unique_ptr<EventLoopThreadPool> threadPool;

    void newConnection(int sockfd, const INetAddress& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);

public:
    TcpServer(EventLoop* loop, const INetAddress& listenAddr,int tNums);
    TcpServer(EventLoop* loop, const INetAddress& listenAddr, std::string  nameArgs,int tNums);

    ~TcpServer();

    const std::string &getIpPort() const;

    EventLoop *getLoop() const;

    const std::string &getName() const;

    bool isStarted() const;

    void setConnectionCallback(const ConnectionCallback &connectionCallback);

    void setOnMessageCallback(const OnMessageCallback &onMessageCallback);

    void setWriteCompleteCallback(const WriteCompleteCallback &writeCompleteCallback);

    void setThreadNums(int threadNums);

    void start();


};


#endif //WEBSERVER_TCPSERVER_H
