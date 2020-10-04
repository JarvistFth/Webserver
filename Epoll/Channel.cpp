//
// Created by jarvist on 2020/10/3.
//

#include "Channel.h"
#include "../EventLoop/EventLoop.h"

const int Channel::noneEvent = 0;
const int Channel::readEvent = EPOLLIN | EPOLLPRI;
const int Channel::writeEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop, int fd) : loop(loop), fd(fd),event(0),revent(0),status(0),eventHandling(false) {

}

Channel::~Channel() {

}

void Channel::setReadCallback(const Channel::ReadCallback &readCallback) {
    Channel::readCallback = readCallback;
}

void Channel::setWriteCallback(const Channel::EventCallback &writeCallback) {
    Channel::writeCallback = writeCallback;
}

void Channel::setErrorCallback(const Channel::EventCallback &errorCallback) {
    Channel::errorCallback = errorCallback;
}

void Channel::setCloseCallback(const Channel::EventCallback &closeCallback) {
    Channel::closeCallback = closeCallback;
}

int Channel::getFd() const {
    return fd;
}

void Channel::setFd(int fd) {
    Channel::fd = fd;
}

int Channel::getEvent() const {
    return event;
}

void Channel::setEvent(int event) {
    Channel::event = event;
}

int Channel::getRevent() const {
    return revent;
}

void Channel::setRevent(int revent) {
    Channel::revent = revent;
}

int Channel::getStatus() const {
    return status;
}

void Channel::setStatus(int status) {
    Channel::status = status;
}

EventLoop *Channel::getLoop() const {
    return loop;
}

void Channel::handleEvent(TimeStamp receivedTime) {
    eventHandling = true;
    if((revent & EPOLLHUP) && !(revent & EPOLLIN)){
        return ;
    }

    if(revent & EPOLLERR){
        errorCallback();
    }

    if(revent & (EPOLLIN | EPOLLPRI | EPOLLRDHUP) ){
        readCallback(receivedTime);
    }

    if(revent & EPOLLOUT){
        writeCallback();
    }
    eventHandling = false;
}

void Channel::update() {
    loop->updateChannel(this);
}

void Channel::remove() {
    loop->removeChannel(this);
}
