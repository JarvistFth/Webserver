//
// Created by jarvist on 2020/10/3.
//

#include "EventLoop.h"
#include <../Epoll/Epoller.h>
#include <Logger.h>
#include "AsyncWaker.h"
#include "../Timer/Timer.h"
#include <../Epoll/Channel.h>
#include <sstream>

__thread EventLoop *t_loopInThisThread = 0;

EventLoop::EventLoop() : looping(false),quited(false),threadID(std::this_thread::get_id()),doingPendingFunctors(false),
epoller(new Epoller(this)),asyncWaker(new AsyncWaker(this)),timerManager(new TimerManager(this))
{
    if (t_loopInThisThread) {
        LOG_ERROR<< "another eventloop in this thread";
    } else {
        t_loopInThisThread = this;
    }
}

EventLoop::~EventLoop() {
    assert(!looping);
    t_loopInThisThread = nullptr;
}

void EventLoop::loop() {
    assert(!looping);
    looping = true;
    quited = false;

    while(!quited){
//        LOG_INFO<<"eventloop in "<< getThreadIDString(threadID) << " start looping...";
        activeChannels.clear();
        auto retTime = epoller->poll(&activeChannels);
        for(const auto& channel:activeChannels){
            channel->handleEvent(retTime);
        }
        doPendingFunctors();

    }
    looping = false;
}

void EventLoop::doPendingFunctors() {
    std::vector<Functor> funcList;
    doingPendingFunctors = true;
    {
        std::lock_guard<std::mutex> lockGuard(mMutex);
        funcList.swap(pendingFunctors);
    }
    for(const auto &func : funcList){
        func();
    }
    doingPendingFunctors = false;
}

void EventLoop::quit() {
    quited = true;
    if(!isInLoopThread()){
        asyncWaker->wakeup();
    }
}

void EventLoop::runInLoop(const Functor& cb) {
    if(isInLoopThread()){
        cb();
    }else{
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(const EventLoop::Functor &cb) {
    {
        std::lock_guard<std::mutex> lockGuard(mMutex);
        pendingFunctors.push_back(cb);
    }

    if(!isInLoopThread() || doingPendingFunctors){
        asyncWaker->wakeup();
    }
}


TimerId EventLoop::runAt(TimeStamp time, const TimerCallback& cb) {
    return timerManager->addTimer(cb,time,0.0);
}

TimerId EventLoop::runEvery(double interval, const TimerCallback& cb) {
    TimeStamp time(addTime(TimeStamp::now(),interval));
    return timerManager->addTimer(cb,time,interval);

}

TimerId EventLoop::runAfter(double delay, const TimerCallback& cb) {
    TimeStamp time(addTime(TimeStamp::now(), delay));
    return runAt(time, cb);
}

void EventLoop::cancel(const TimerId& timerid) {
    return timerManager->cancel(timerid);
}

std::string EventLoop::getThreadIDString(std::thread::id id) {
    std::stringstream ss;
    ss << id;
    return ss.str();
}

void EventLoop::updateChannel(Channel *channel) {
    epoller->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel) {
    epoller->removeChannel(channel);
}

void EventLoop::assertInLoopThread() {
    if (!isInLoopThread()) {
        LOG_FATAL<<"this loop is create in "<<getThreadIDString(threadID)<<
        " ,but current thread id is :" << getThreadIDString(std::this_thread::get_id());
        std::abort();
    }
}

