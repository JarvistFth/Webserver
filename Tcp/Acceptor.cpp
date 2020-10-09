//
// Created by jarvist on 2020/10/3.
//

#include "Acceptor.h"
#include "../EventLoop/EventLoop.h"
#include "SocketsOps.h"
#include "INetAddress.h"
#include <unistd.h>

Acceptor::Acceptor(EventLoop *loop, const INetAddress &listenAddr, bool reusePort) : acceptorLoop(loop),
acceptSocket(sockets::createNonblockingOrDie()),acceptChannel(loop,acceptSocket.fd()),listening(false)
{
    acceptSocket.setReuseAddr(reusePort);
    acceptSocket.bindAddress(listenAddr);
    acceptChannel.setReadCallback(std::bind(&Acceptor::handleRead,this));
}

Acceptor::Acceptor(EventLoop *loop, const INetAddress &listenAddr): acceptorLoop(loop),
acceptSocket(sockets::createNonblockingOrDie()),acceptChannel(loop,acceptSocket.fd()),listening(false)
{
    acceptSocket.setReuseAddr(true);
    acceptSocket.setReusePort(true);
    acceptSocket.bindAddress(listenAddr);
    acceptChannel.setReadCallback(std::bind(&Acceptor::handleRead,this));
}

Acceptor::~Acceptor() {
    acceptChannel.disableAll();
    acceptChannel.remove();
//    ::close(acceptSocket.fd());
}

void Acceptor::listen() {
    acceptorLoop->assertInLoopThread();
    listening = true;
    acceptSocket.listen();
    acceptChannel.enableRead();
}

void Acceptor::handleRead() {
    acceptorLoop->assertInLoopThread();

    for(;;){
        INetAddress peerAddr(0);
        int connfd = acceptSocket.accept(&peerAddr);
        if(connfd >= 0){
            if(newConnectionCallback){
                newConnectionCallback(connfd,peerAddr);
            }else{
                sockets::close(connfd);
            }
        }else{
            break;
        }
#ifdef USE_EPOLL_LT
        break;
#endif
    }
}


