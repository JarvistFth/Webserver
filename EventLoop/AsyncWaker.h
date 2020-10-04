//
// Created by jarvist on 2020/10/3.
//

#ifndef WEBSERVER_ASYNCWAKER_H
#define WEBSERVER_ASYNCWAKER_H

#include <sys/eventfd.h>
#include <NonCopyable.h>

//#include "EventLoop.h"
class EventLoop;
class Channel;
class AsyncWaker : NonCopyable{
private:
    int wakeupFd;

    EventLoop* myloop;
    Channel* wakeupChannel;

    void handleRead();

public:
    AsyncWaker(EventLoop* loop);
    virtual ~AsyncWaker();

    void wakeup();
};


#endif //WEBSERVER_ASYNCWAKER_H
