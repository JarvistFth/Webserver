//
// Created by jarvist on 2020/10/3.
//

#include "AsyncWaker.h"
#include <../Epoll/Channel.h>
#include <Logger.h>
#include <unistd.h>

AsyncWaker::AsyncWaker(EventLoop* loop) : myloop(loop),
wakeupFd(::eventfd(0,EFD_NONBLOCK | EFD_CLOEXEC)),wakeupChannel(new Channel(loop,wakeupFd))
{
    if(wakeupFd < 0){
        LOG_ERROR << "wake up fd < 0";
    }
    wakeupChannel->setReadCallback(std::bind(&AsyncWaker::handleRead,this));
    wakeupChannel->enableRead();
}

AsyncWaker::~AsyncWaker() {
    wakeupChannel->disableAll();
    ::close(wakeupFd);
}

void AsyncWaker::handleRead() {
    uint64_t one = 1;
    ssize_t n = ::read(wakeupFd,&one, sizeof(one));
    if(n != sizeof(one)){
        LOG_ERROR<< n << " bytes received instead of 8 ";
    }
}

void AsyncWaker::wakeup() {
    uint64_t one = 1;
    ssize_t n = ::write(wakeupFd,&one, sizeof(one));
    if(n != sizeof(one)){
        LOG_ERROR<< n << " bytes received instead of 8 ";
    }
}
