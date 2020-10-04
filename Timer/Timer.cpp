//
// Created by jarvist on 4/6/20.
//

#include "Timer.h"
std::atomic_int64_t Timer::timerNum(0);
Timer::Timer(TimerCallback cb, TimeStamp when, double interval) :
    timerCallback(std::move(cb)),expiredTime(when),interval(interval),sequences(timerNum.operator++()),repeated(interval > 0.0)

{

}

void Timer::restart(TimeStamp now) {
    if (repeated)
    {
        expiredTime = addTime(now,interval);
    }
    else
    {
        expiredTime = TimeStamp::invalid();
    }
}

Timer::~Timer() {

}
