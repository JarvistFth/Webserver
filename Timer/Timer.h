//
// Created by jarvist on 4/6/20.
//

#ifndef WEBSERVER_TIMER_H
#define WEBSERVER_TIMER_H

#include <memory>
#include <functional>
#include <queue>
#include <atomic>
#include <utility>
#include "../Utils/TimeStamp.h"

typedef std::function<void()> TimerCallback;

class Timer {
private:
    TimerCallback timerCallback;
    TimeStamp expiredTime;
    const double interval;
    static std::atomic_int64_t timerNum;

    const int64_t sequences;
    bool repeated;

public:
    Timer(TimerCallback cb,TimeStamp when,double interval);
    ~Timer();

    void run(){timerCallback();}

    TimeStamp getExpiredTime ()const {
        return expiredTime;
    }
    int64_t getSeq() const {
        return sequences;
    }

    static short getTimerNum() { return timerNum;};
    bool getRepeated()const {return repeated;}

    void restart(TimeStamp now);


};


class TimerId :Copyable
{
    friend class TimerManager;
public:
    TimerId():sequence_(0){};

    TimerId(std::shared_ptr<Timer> timer, int64_t seq): timer_ptr(std::move(timer)),sequence_(seq)
    {

    }

    // default copy-ctor, dtor and assignment are okay



private:
    std::shared_ptr<Timer> timer_ptr;
//    std::shared_ptr<Timer> timer;
    int64_t sequence_;
};

#endif //WEBSERVER_TIMER_H
