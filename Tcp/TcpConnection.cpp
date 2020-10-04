//
// Created by jarvist on 2020/10/3.
//

#include <Types.h>

#include <memory>
#include <unistd.h>
#include "TcpConnection.h"
#include "SocketsOps.h"

using namespace std::placeholders;

TcpConnection::TcpConnection(EventLoop *loop, const std::string &name,
        int sockfd, const INetAddress &localAddr,
        const INetAddress &peerAddr)
        : myloop(CheckNotNull<EventLoop>(loop)),name(name),state(connecting),
        socket(new Socket(sockfd)),channel(new Channel(loop,sockfd)),
        localAddr(localAddr),peerAddr(peerAddr),faultError(false)
{
    channel->setReadCallback(std::bind(&TcpConnection::handleRead,this,_1));
    channel->setWriteCallback(std::bind(&TcpConnection::handleWrite,this));
    channel->setCloseCallback(std::bind((&TcpConnection::handleClose),this));
    channel->setErrorCallback(std::bind((&TcpConnection::handleError),this));
}

TcpConnection::~TcpConnection() {
    LOG_INFO << "remove tcp conn from" << name << "fd = " <<channel->getFd();
}

void
TcpConnection::setNewTcpConnection(EventLoop *loop, const std::string &name, int sockfd,
        const INetAddress &localAddr,const INetAddress &peerAddr)
{
    myloop = CheckNotNull<EventLoop>(loop);
    this->name = name;
    this->state = connecting;
    socket = std::make_unique<Socket>(sockfd);
    channel = std::make_unique<Channel>(loop,sockfd);
    this->localAddr = localAddr;
    this->peerAddr = peerAddr;
    faultError = false;
    inputBuffer.retrieveAll();
    outputBuffer.retrieveAll();
    channel->setReadCallback(std::bind(&TcpConnection::handleRead,this,_1));
    channel->setWriteCallback(std::bind(&TcpConnection::handleWrite,this));
    channel->setCloseCallback(std::bind((&TcpConnection::handleClose),this));
    channel->setErrorCallback(std::bind((&TcpConnection::handleError),this));
}

void TcpConnection::send(const std::string &msg) {
    if(state == connected){
        if(myloop->isInLoopThread()){
            sendInLoop(msg);
        }else{
            myloop->runInLoop(std::bind(&TcpConnection::sendInLoop,this,msg));
        }
    }
}

void TcpConnection::sendInLoop(const std::string &msg) {
    myloop->assertInLoopThread();
    ssize_t nwrote = 0;
    ssize_t remain = msg.size();

    if(!channel->isWriting() && outputBuffer.readableBytes() == 0){
        nwrote = ::write(channel->getFd(),msg.data(),msg.size());
        if(nwrote >= 0){
            remain -= nwrote;
            if(remain == 0 && writeCompleteCallback){
                myloop->queueInLoop(std::bind(&TcpConnection::writeCompleteCallback,shared_from_this()));
            }
        }else{
            nwrote = 0;
            if(errno != EWOULDBLOCK){
                LOG_ERROR<<"tcp conn failed in sendInLoop\n";
                if(errno == EPIPE || errno == ECONNRESET){
                    faultError = true;
                }
            }
        }
    }
    if(!faultError && remain > 0){
        outputBuffer.append(msg.data()+nwrote,remain);
        if(!channel->isWriting()){
            channel->enableWrite();
        }
    }
}

void TcpConnection::shutdown() {
    if(state == connected){
        setState(disconnecting);
        myloop->runInLoop(std::bind(&TcpConnection::shutdownInLoop,this));
    }
}

void TcpConnection::shutdownInLoop() {
    myloop->assertInLoopThread();
    if(!channel->isWriting()){
        socket->shutdownWrite();
    }
}

void TcpConnection::setState(TcpConnection::TcpConnState state) {
    TcpConnection::state = state;
}

void TcpConnection::setTcpNoDelay(bool on) {
    socket->setTcpNoDelay(on);
}

void TcpConnection::connEstablished() {
    myloop->assertInLoopThread();
    setState(connected);
    channel->enableRead();
    connectionCallback(shared_from_this());
}

void TcpConnection::connDestroyed() {
    myloop->assertInLoopThread();
    assert(state == connected || state == disconnecting);
    if(state == connected){
        setState(disconnected);
        channel->disableAll();
        connectionCallback(shared_from_this());
    }
    channel->remove();
}

void TcpConnection::setTcpKeepAlive(bool on) {
    socket->setKeepAlive(on);
}

void TcpConnection::handleRead(TimeStamp recvTime) {
    int savedErrno = 0;
    ssize_t n = inputBuffer.readFd(channel->getFd(), &savedErrno);
    if(n > 0 ){
        onMessageCallback(shared_from_this(), &inputBuffer, recvTime);
    }else if(n == 0){
        handleClose();
    }else{
        errno = savedErrno;
        LOG_ERROR<<"error when handle read from tcp,errno is:"<<errno;
        handleError();
    }
}

void TcpConnection::handleWrite(){
    myloop->assertInLoopThread();
    ssize_t n = write(channel->getFd(), outputBuffer.peek(), outputBuffer.readableBytes());
    if(channel->isWriting()){
        if(n > 0){
            outputBuffer.retrieve(n);
            if(outputBuffer.readableBytes() == 0){
                channel->disableWrite();
                if(writeCompleteCallback){
                    myloop->queueInLoop(std::bind(writeCompleteCallback,shared_from_this()));
                }
                if(state == disconnecting){
                    shutdownInLoop();
                }
            }
        }else{
            LOG_ERROR << "tcp handle write error";
        }
    }else{
        LOG_ERROR <<"fd: " <<channel->getFd() << "tcp conn is down";
    }
}

void TcpConnection::handleClose() {
    myloop->assertInLoopThread();
    setState(disconnecting);
    LOG_INFO <<this->myloop->getThreadIDString(myloop->threadID) << " tcp handle close, fd= " << channel->getFd();
    channel->disableAll();
    closeCallback(shared_from_this());
}

void TcpConnection::handleError() {
    int err = sockets::getSocketError(channel->getFd());
    LOG_ERROR<< " TCP handle Error, errno: " << err;
}






