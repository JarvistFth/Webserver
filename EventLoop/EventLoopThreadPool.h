//
// Created by jarvist on 2020/10/3.
//

#ifndef WEBSERVER_EVENTLOOPTHREADPOOL_H
#define WEBSERVER_EVENTLOOPTHREADPOOL_H


#include <NonCopyable.h>
#include <thread>
#include <vector>

class EventLoop;
class EventLoopThread;
class EventLoopThreadPool : NonCopyable {
private:
    EventLoop* baseLoop;
    bool started;
    int threadNums;
    int next;

    std::vector<std::unique_ptr<EventLoopThread>> threads;
    std::vector<EventLoop*> loops;

public:
    EventLoopThreadPool(EventLoop* baseloop);
    EventLoopThreadPool(EventLoop* baseloop,int);
    ~EventLoopThreadPool();


    void setThreadNums(int threadNums);

    void start();
    EventLoop* getNextLoop();
};


#endif //WEBSERVER_EVENTLOOPTHREADPOOL_H
