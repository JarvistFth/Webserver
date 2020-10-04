//
// Created by jarvist on 2020/10/3.
//

#ifndef WEBSERVER_ACCEPTOR_H
#define WEBSERVER_ACCEPTOR_H


#include <NonCopyable.h>
#include <functional>
#include "Socket.h"
#include "../Epoll/Channel.h"

class EventLoop;
class INetAddress;
class Acceptor : NonCopyable {
public:
    typedef std::function<void(int sockfd, const INetAddress&)> NewConnectionCallback;

    Acceptor(EventLoop* loop, const INetAddress& listenAddr, bool reusePort);
    Acceptor(EventLoop* loop, const INetAddress& listenAddr);

    ~Acceptor();

    bool isListening(){
        return listening;
    };

    void listen();


private:
    void handleRead();
    EventLoop* acceptorLoop;
    Socket acceptSocket;
    Channel acceptChannel;
    NewConnectionCallback newConnectionCallback;

public:
    void setNewConnectionCallback(const NewConnectionCallback &newConnectionCallback) {
        Acceptor::newConnectionCallback = newConnectionCallback;
    }

private:
    bool listening;
};


#endif //WEBSERVER_ACCEPTOR_H
