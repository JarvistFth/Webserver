//
// Created by jarvist on 2020/10/3.
//

#ifndef WEBSERVER_EVENTLOOPTHREAD_H
#define WEBSERVER_EVENTLOOPTHREAD_H


#include <NonCopyable.h>
#include <mutex>
#include <thread>
#include <condition_variable>

class EventLoop;
class EventLoopThread : NonCopyable{
private:
    void threadFunc();
    EventLoop* thisloop;
    bool quited;
    std::mutex mMutex;
    std::thread mThread;
    std::condition_variable condition;

public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop* startLoop();
    bool joinable();
    void join();
};


#endif //WEBSERVER_EVENTLOOPTHREAD_H
