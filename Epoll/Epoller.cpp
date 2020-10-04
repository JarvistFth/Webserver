//
// Created by jarvist on 2020/10/3.
//

#include <Logger.h>
#include "Epoller.h"
#include <unistd.h>
#include "Channel.h"
#include "../EventLoop/EventLoop.h"

namespace {
    const int newFd = 0;
    const int inPoll = 1;
    const int delFd = 2;
}

Epoller::Epoller(EventLoop* loop) : epfd(::epoll_create1(EPOLL_CLOEXEC)),myLoop(loop),
events(initialEventListSize)
{
    if(epfd < 0){
        LOG_ERROR<< "EPFD < 0";
    }
}

Epoller::~Epoller() {
    ::close(epfd);
}

TimeStamp Epoller::poll(Epoller::ChannelList *channelList) {
    int nready = ::epoll_wait(epfd,&*events.begin(), static_cast<int>(events.size()),timeouMs);

    int savedErrno = errno;
    TimeStamp now(TimeStamp::now());
    if(nready > 0){
//        LOG_INFO << nready << " things happened";
        fillActiveChannels(nready,channelList);
        if (implicit_cast<size_t>(nready) == events.size()) {
            events.resize(events.size() * 2);
        }
    }
    else if(nready == 0){
        LOG_INFO<< " NOTHING HAPPENED";
    }else{
        if(savedErrno != EINTR){
            errno = savedErrno;
            LOG_FATAL << errno;
        }
    }
    return now;
}

void Epoller::fillActiveChannels(int nready, Epoller::ChannelList *activeChannel) const {
    for(int i=0;i<nready;i++){
        auto channel = static_cast<Channel*> (events[i].data.ptr);
        channel->setRevent(events[i].events);
        activeChannel->push_back(channel);
    }
}

void Epoller::update(int opt, Channel *channel) {
    struct epoll_event event;
    memZero(&event, sizeof(event));
    event.events = channel->getEvent();
    event.data.ptr = channel;
    int fd = channel->getFd();
    int ret = ::epoll_ctl(epfd,opt,fd,&event);
    if(ret < 0){
        LOG_ERROR<< "EPOLL MOD ERROR";
    }
}

void Epoller::updateChannel(Channel *channel) {
    const int status = channel->getStatus();
    if(status == newFd || status == delFd){
        int fd = channel->getFd();
        if(status == newFd){
            channels[fd] = channel;
        }else{
            LOG_INFO<<"channel fd: "<< fd << " is deleted!! ";
        }
        channel->setStatus(inPoll);
        update(EPOLL_CTL_ADD,channel);
    }else{
        int fd = channel->getFd();
        if(channel->isNoneEvent()){
            update(EPOLL_CTL_DEL,channel);
            channel->setStatus(delFd);
        }else{
            update(EPOLL_CTL_MOD,channel);
//            channel->setStatus(inPoll);
        }
    }
}

void Epoller::removeChannel(Channel *channel) {
    int fd = channel->getFd();
    int status = channel->getStatus();
    auto n = channels.erase(fd);

    if(status == inPoll){
        update(EPOLL_CTL_DEL,channel);
    }
    channel->setStatus(newFd);
}

void Epoller::assertInLoopThread() {
    myLoop->assertInLoopThread();
}

void Epoller::handleExpired() {
    timerM
}
