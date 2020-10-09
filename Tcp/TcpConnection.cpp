//
// Created by jarvist on 2020/10/3.
//

#include <Types.h>

#include <memory>
#include <unistd.h>
#include "TcpConnection.h"
#include "SocketsOps.h"
#include "TcpServer.h"

using namespace std::placeholders;

TcpConnection::TcpConnection(EventLoop *loop, const std::string &name,
        int sockfd, const INetAddress &localAddr,
        const INetAddress &peerAddr)
        : myloop(CheckNotNull<EventLoop>(loop)),name(name),state(connecting),
        socket(new Socket(sockfd)),channel(new Channel(loop,sockfd)),
        localAddr(localAddr),peerAddr(peerAddr),keepAlive(false)
{
    channel->setReadCallback(std::bind(&TcpConnection::handleRead,this,_1));
    channel->setWriteCallback(std::bind(&TcpConnection::handleWrite,this));
    channel->setCloseCallback(std::bind((&TcpConnection::handleClose),this));
    channel->setErrorCallback(std::bind((&TcpConnection::handleError),this));
}

TcpConnection::~TcpConnection() {
//    LOG_INFO << "remove tcp conn from" << name << "fd = " <<channel->getFd();
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
    keepAlive = false;
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
            void (TcpConnection::*fp)(const std::string& msg) = &TcpConnection::sendInLoop;
            myloop->runInLoop(std::bind(fp,this,msg));
        }
    }
}

void TcpConnection::send(Buffer *msg) {
    if(state == connected){
        if(myloop->isInLoopThread()){
            sendInLoop(msg->peek(),msg->readableBytes());
            msg->retrieveAll();
        }else{
            void (TcpConnection::*fp)(const std::string& msg) = &TcpConnection::sendInLoop;
            myloop->runInLoop(std::bind(fp,this,msg->retrieveAsString()));
        }
    }
}

//尝试直接发送发送数据，如果没发送完，将待发送的放入outputBuffer缓存，并关注writable事件；
//后续如果wriable，就会调用writecallback。

void TcpConnection::sendInLoop(const std::string &msg) {
//    myloop->assertInLoopThread();
//    ssize_t nwrote = 0;
//    ssize_t allwrote = 0;
//    size_t remaining = msg.size();
//    while (remaining > 0) {
//#ifdef USE_EPOLL_LT
//        nwrote = ::write(channel->getFd(), msg.data() + allwrote, remaining);
//#else
//        nwrote = writeET(channel->getFd(), msg.data() + allwrote, remaining);
//#endif
//        if (nwrote >= 0) {
//            remaining -= nwrote;
//            allwrote += nwrote;
//        } else {
//            if (errno != EWOULDBLOCK) {
//                LOG_ERROR <<" send in loop error"<<errno;
//            }
//        }
//    }
    sendInLoop(msg.data(),msg.size());
}

void TcpConnection::sendInLoop(const void *msg, size_t len) {
    myloop->assertInLoopThread();
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool faultError = false;
    if(state == disconnected){
        LOG_WARN <<" conn disconnected, give up send msg";
    }
    if(!channel->isWriting() && outputBuffer.readableBytes() == 0){
#ifdef USE_EPOLL_LT
        nwrote = ::write(channel->getFd(), msg, len);
#else
        nwrote = writeET(channel->getFd(), static_cast<const char *>(msg), len);
#endif
        if(nwrote >= 0){
            remaining = len - nwrote;
            if(remaining == 0 && writeCompleteCallback){
                myloop->queueInLoop(std::bind(writeCompleteCallback,shared_from_this()));
            }
        }else{
            nwrote = 0;
            if(errno != EWOULDBLOCK){
                LOG_ERROR << "send Error";
                if(errno == EPIPE || errno == ECONNRESET){
                    faultError = true;
                }
            }
        }
    }

    if(!faultError && remaining > 0){
        outputBuffer.append(static_cast<const char*>(msg)+nwrote,remaining);
        if(!channel->isWriting()){
            channel->enableWrite();
        }
    }
}

void TcpConnection::shutdown() {
    if(state == connected){
        setState(disconnecting);
//        LOG_INFO << "shut down tcp";
        myloop->runInLoop(std::bind(&TcpConnection::shutdownInLoop,this));
    }else{
//        LOG_INFO << "tcp state =:" <<state;
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
//    LOG_INFO << myloop->getThreadIDString(myloop->threadID) << " established tcp";
    if(keepAlive){
        myloop->runAfter(10,std::bind(&TcpConnection::shutdown,this));
    }
    setState(connected);
#ifdef USE_EPOLL_LT
#else
    channel->enableEpollET();
#endif
    channel->enableRead();
    connectionCallback(shared_from_this());
}

void TcpConnection::connDestroyed() {
    if(state == disconnected){
        return ;
    }
    myloop->assertInLoopThread();
    assert(state == connected || state == disconnecting);
    if(state == connected){
        setState(disconnected);
//        LOG_INFO << "conn destroy";
        channel->disableAll();
        connectionCallback(shared_from_this());
    }
    channel->remove();
}

void TcpConnection::setTcpKeepAlive(bool on) {
    socket->setKeepAlive(on);
    keepAlive = on;
    myloop->runAfter(10,std::bind(&TcpConnection::shutdown,this));
}

void TcpConnection::handleRead(TimeStamp recvTime) {
    int savedErrno = 0;
#ifdef USE_EPOLL_LT
    ssize_t n = inputBuffer.readFd(channel->getFd(), &savedErrno);
#else
    // ET模式读写，直到发生EAGAIN，才返回
  ssize_t n = inputBuffer.readFdET(channel->getFd(), &savedErrno);
#endif
    if (n > 0) {
        onMessageCallback(shared_from_this(), &inputBuffer, recvTime);
    } else if (n == 0) {
        handleClose();
    } else {
        errno = savedErrno;
        handleError();
    }
}

void TcpConnection::handleWrite() {
    myloop->assertInLoopThread();if (channel->isWriting()) {
        int savedErrno = 0;
#ifdef USE_EPOLL_LT
        ssize_t n = outputBuffer.writeFd(channel->getFd(), &savedErrno);
#else
        // ET模式读写，直到发生EAGAIN，才返回
    ssize_t n = outputBuffer.writeFdET(channel->getFd(), &savedErrno);
#endif
        if (n > 0) {
            // retrieve过程(readIndex的设置)改为在write读取时就完成
//       outputBuffer_.retrieve(n);
            if (outputBuffer.readableBytes() == 0) {
                channel->disableWrite();
                if (writeCompleteCallback) {
                    myloop->queueInLoop(
                            std::bind(writeCompleteCallback, shared_from_this()));
                }
                // 通过状态机转换，如果有关闭请求，则处理关闭
                if (state == disconnecting) {
                    shutdownInLoop();
                }
            }
        } else {

        }
    } else {

    }
}

void TcpConnection::handleClose() {
    myloop->assertInLoopThread();
    assert(state == connected || state == disconnecting);
    setState(disconnected);
//    LOG_INFO <<this->myloop->getThreadIDString(myloop->threadID) << " tcp handle close, fd= " << channel->getFd();
    channel->disableAll();
    closeCallback(shared_from_this());

}

void TcpConnection::handleError() {
    int err = sockets::getSocketError(channel->getFd());
    LOG_ERROR<< " TCP handle Error, errno: " << err;
}

const CloseCallback &TcpConnection::getCloseCallback() const {
    return closeCallback;
}



#ifdef USE_EPOLL_LT
#else
ssize_t TcpConnection::writeET(int fd, const char *begin, size_t len) {
    ssize_t writesum = 0;
    char *tbegin = (char *)begin;
    for (;;) {
        ssize_t n = ::write(fd, tbegin, len);
        if (n > 0) {
            writesum += n;
            tbegin += n;
            len -= n;
            if (len == 0) {
                return writesum;
            }
        } else if (n < 0) {
            if (errno == EAGAIN) //系统缓冲区满，非阻塞返回
            {
                break;
            }
                // 暂未考虑其他错误
            else {
                return -1;
            }
        } else {
            // 返回0的情况，查看write的man，可以发现，一般是不会返回0的
            return 0;
        }
    }
    return writesum;
}
#endif





