//
// Created by jarvist on 3/31/20.
//

#ifndef WEBSERVER_THREAD_H
#define WEBSERVER_THREAD_H


#include <functional>
#include "NonCopyable.h"
#include "CountDownLatch.h"
#include <cassert>

class Thread : NonCopyable {
    typedef std::function<void()> ThreadFunc;
private:
    void setDefaultName();
    bool started;
    bool joined;
    pthread_t pthreadID;
    pid_t tid;
    ThreadFunc threadFunc;
    std::string name;
    CountDownLatch latch;

public:
    explicit Thread(const ThreadFunc&, const std::string& name = std::string());
    ~Thread();

    void start();

    int join();

    bool isStarted() const{return started;}
    pid_t getTid()const {return tid;}
    const std::string& getName()const {return name;}


};


#endif //WEBSERVER_THREAD_H
