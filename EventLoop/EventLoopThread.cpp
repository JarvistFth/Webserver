//
// Created by jarvist on 2020/10/3.
//

#include <Logger.h>
#include "EventLoopThread.h"
#include "EventLoop.h"

EventLoopThread::EventLoopThread() : thisloop(nullptr),quited(false) {

}

EventLoopThread::~EventLoopThread() {
    quited = true;
    thisloop->quit();
    mThread.join();
}

EventLoop *EventLoopThread::startLoop() {
    mThread = std::thread(std::bind(&EventLoopThread::threadFunc, this));
    {
        std::unique_lock<std::mutex> lock(mMutex);
        condition.wait(lock, [this] { return this->thisloop != nullptr; });
    }
    return thisloop;
}

bool EventLoopThread::joinable() {
    return mThread.joinable();
}



void EventLoopThread::threadFunc() {
    EventLoop loop;
    {
        std::unique_lock<std::mutex> lock(mMutex);
        thisloop = &loop;
        condition.notify_one();
    }
    loop.loop();
}

void EventLoopThread::join() {
    mThread.join();
}





