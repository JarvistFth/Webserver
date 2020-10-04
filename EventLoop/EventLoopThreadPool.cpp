//
// Created by jarvist on 2020/10/3.
//

#include <Logger.h>
#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseloop) : baseLoop(baseloop),
started(false),threadNums(0),next(0)
{

}

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseloop,int tNums) : baseLoop(baseloop), started(false),
threadNums(tNums),next(0)
{

}

void EventLoopThreadPool::setThreadNums(int threadNums) {
    EventLoopThreadPool::threadNums = threadNums;
}

void EventLoopThreadPool::start() {
    LOG_INFO<< "START THREAD POOL...";
    started = true;
    for(int i=0;i<threadNums;i++){
        auto *t = new EventLoopThread;
        threads.emplace_back(t);
        loops.push_back(t->startLoop());
    }
}

EventLoop *EventLoopThreadPool::getNextLoop() {
    EventLoop *nextloop = baseLoop;
    if(!loops.empty()){
        nextloop = loops[next];
        next = (next+1) % loops.size();
    }
    return nextloop;
}

EventLoopThreadPool::~EventLoopThreadPool() {

}




