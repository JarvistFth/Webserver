//
// Created by jarvist on 2020/10/3.
//

#include <Types.h>

#include <utility>
#include "TcpServer.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "SocketsOps.h"

const std::string &TcpServer::getIpPort() const {
    return ipPort;
}

EventLoop *TcpServer::getLoop() const {
    return loop_;
}

const std::string &TcpServer::getName() const {
    return name;
}

bool TcpServer::isStarted() const {
    return started;
}

void TcpServer::setConnectionCallback(const ConnectionCallback &connectionCallback) {
    TcpServer::connectionCallback = connectionCallback;
}

void TcpServer::setOnMessageCallback(const OnMessageCallback &onMessageCallback) {
    TcpServer::onMessageCallback = onMessageCallback;
}

void TcpServer::setWriteCompleteCallback(const WriteCompleteCallback &writeCompleteCallback) {
    TcpServer::writeCompleteCallback = writeCompleteCallback;
}

void TcpServer::setThreadNums(int threadNums) {
    TcpServer::threadNums = threadNums;
    threadPool->setThreadNums(threadNums);
}

TcpServer::TcpServer(EventLoop *loop, const INetAddress &listenAddr, std::string nameArgs,int tNums) :
loop_(CheckNotNull<EventLoop>(loop)),name(std::move(nameArgs)),acceptor(new Acceptor(loop,listenAddr)),
started(false),nextConnId(0),threadPool(new EventLoopThreadPool(loop,tNums)),threadNums(tNums)
{
    acceptor->setNewConnectionCallback(std::bind(&TcpServer::newConnection,this,std::placeholders::_1,std::placeholders::_2));

}

TcpServer::TcpServer(EventLoop *loop, const INetAddress &listenAddr, int tNums):
loop_(CheckNotNull<EventLoop>(loop)),name(listenAddr.toIpPortString()),acceptor(new Acceptor(loop,listenAddr)),
started(false),nextConnId(0),threadPool(new EventLoopThreadPool(loop,tNums)),threadNums(tNums)
{
    acceptor->setNewConnectionCallback(std::bind(&TcpServer::newConnection,this,std::placeholders::_1,std::placeholders::_2));
}

TcpServer::~TcpServer() {

}

void TcpServer::start() {
    if(!started){
        started = true;
        LOG_INFO<< "SERVER START ...";
        threadPool->start();
    }
    if(!acceptor->isListening()){
        loop_->runInLoop(std::bind(&Acceptor::listen,acceptor.get()));
    }
}

void TcpServer::newConnection(int sockfd, const INetAddress &peerAddr) {
    loop_->assertInLoopThread();
    char buf[32];
    snprintf(buf, sizeof buf, "#%d", nextConnId);
    ++nextConnId;
    std::string connName = name + buf;

    INetAddress localAddress(sockets::getLocalAddr(sockfd));
    EventLoop* ioLoop = threadPool->getNextLoop();

    TcpConnectionPtr conn = std::make_shared<TcpConnection>(ioLoop,connName,sockfd,localAddress,peerAddr);
    connections[connName] = conn;
    conn->setConnectionCallback(connectionCallback);
    conn->setOnMessageCallback(onMessageCallback);
    conn->setCloseCallback(std::bind(&TcpServer::removeConnection,this,std::placeholders::_1));
    conn->setWriteCompleteCallback(writeCompleteCallback);
    ioLoop->runInLoop(std::bind(&TcpConnection::connEstablished,conn));

}

void TcpServer::removeConnection(const TcpConnectionPtr &conn) {
    LOG_INFO << loop_->getThreadIDString(loop_->threadID) <<"remove conn";
    loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop,this,conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn) {
    loop_->assertInLoopThread();
    auto n = connections.erase(conn->getName());
    LOG_INFO<< loop_->getThreadIDString(loop_->threadID) <<" map erase n:" <<n;
    assert(n == 1);

    EventLoop* ioLoop = conn->getloop();
    LOG_INFO<<loop_->getThreadIDString(loop_->threadID) << "remove conn from: "<< conn->getName() << ".  ConnMap Size:" << connections.size();
    ioLoop->queueInLoop(std::bind(&TcpConnection::connDestroyed,conn));
}




