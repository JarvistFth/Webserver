//
// Created by jarvist on 4/7/20.
//

#include <sys/timerfd.h>
#include <unistd.h>
#include <iostream>
#include <utility>

#include "TimerManager.h"
#include "../EventLoop/EventLoop.h"
#include "../Utils/TimeStamp.h"
#include "../Base/Types.h"


int createtimerfd(){
    int timerfd = timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK | TFD_CLOEXEC);
    if(timerfd < 0){
        perror("create timerfd failed..");
    }
    return timerfd;

}

struct timespec howMuchTimeFromNow(TimeStamp when)
{
    int64_t microseconds = when.microSecondsSinceEpoch()
                           - TimeStamp::now().microSecondsSinceEpoch();
    if (microseconds < 100)
    {
        microseconds = 100;
    }
    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(
            microseconds / TimeStamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>(
            (microseconds % TimeStamp::kMicroSecondsPerSecond) * 1000);
    return ts;
}

void readTimerfd(int timerfd, TimeStamp now)
{
    uint64_t howmany;
    ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
    std::cout << "TimerManager::handleRead() " << howmany << " at " << now.toFormattedString(true)<<std::endl;
    if (n != sizeof howmany)
    {
        std::cerr<< "TimerManager::handleRead() reads " << n << " bytes instead of 8";
    }
}

void resetTimerfd(int timerfd, TimeStamp expiration)
{
    // wake up eventLoop by timerfd_settime()
    struct itimerspec newValue{};
    struct itimerspec oldValue{};
    memZero(&newValue, sizeof newValue);
    memZero(&oldValue, sizeof oldValue);
    newValue.it_value = howMuchTimeFromNow(expiration);
    int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
    if (ret)
    {
        std::cerr << "timerfd_settime()";
    }
}

TimerManager::TimerManager(EventLoop *eventLoop) :timerfd(createtimerfd()),loop(eventLoop),
timerfdChannel(std::make_unique<Channel>(loop,timerfd)),timerList(),callingExpiredTimers(false)
{
    timerfdChannel->setReadCallback(std::bind(&TimerManager::handleRead,this));
    timerfdChannel->enableRead();
}

TimerManager::~TimerManager() {
    timerfdChannel->disableAll();
    close(timerfd);
}

TimerId TimerManager::addTimer(const TimerCallback& cb, TimeStamp when, double interval) {
    addtimer_ptr = std::make_shared<Timer>(cb,when,interval);
    loop->runInLoop(std::bind(&TimerManager::addTimerInLoop,this,addtimer_ptr));
    return TimerId(addtimer_ptr,addtimer_ptr->getSeq());
}

void TimerManager::addTimerInLoop(const std::shared_ptr<Timer>& timer) {
    bool earlyChange = insert(timer);
    if(earlyChange){
        resetTimerfd(timerfd,timer->getExpiredTime());
    }
}

void TimerManager::cancel(const TimerId& id) {
    loop->runInLoop(
            std::bind(&TimerManager::cancelInLoop, this, id));
}


void TimerManager::cancelInLoop(const TimerId& id) {
    ActiveTimer timer(id.timer_ptr,id.sequence_);
    auto it = activeTimers.find(timer);
    if(it != activeTimers.end()){
        ssize_t n = timerList.erase(TimerPair(it->first->getExpiredTime(),it->first));
        activeTimers.erase(it);
    }else if(callingExpiredTimers){
        cancelTimers.insert(timer);
    }
}

bool TimerManager::insert(const std::shared_ptr<Timer>& timer) {
    bool earlyNodeChange = false ;
    TimeStamp when = timer->getExpiredTime();
    auto it = timerList.begin();
    if(it == timerList.end() || when < it->first){
        earlyNodeChange = true;
    }
    {
        std::pair<TimerList::iterator,bool > result = timerList.insert(TimerPair(when,timer));
    }
    {
        std::pair<ActiveTimerSet::iterator,bool > result = activeTimers.insert(ActiveTimer(timer,timer->getSeq()));
    }
    return earlyNodeChange;
}

void TimerManager::handleRead() {
    TimeStamp now(TimeStamp::now());
    readTimerfd(timerfd,now);
    std::vector<TimerPair> expiredTimers = getExpired(now);
    callingExpiredTimers = true;
    cancelTimers.clear();
//    std::cout<<"run expired"<<"\n";
    for(const TimerPair& it : expiredTimers){
        it.second->run();
    }
    callingExpiredTimers = false;
    reset(expiredTimers,now);
}

std::vector<TimerManager::TimerPair> TimerManager::getExpired(TimeStamp now) {
    std::vector<TimerPair> expiredTimers;
    TimerPair pos(now,std::shared_ptr<Timer>());
    auto end = timerList.lower_bound(pos);
    std::copy(timerList.begin(),end,std::back_inserter(expiredTimers));
    timerList.erase(timerList.begin(),end);
    for(const auto& it : expiredTimers){
        ActiveTimer timer(it.second,it.second->getSeq());
        ssize_t n = activeTimers.erase(ActiveTimer(it.second,it.second->getSeq()));

    }
    return expiredTimers;
}

void TimerManager::reset(const std::vector<TimerPair> &expired, TimeStamp now) {
    TimeStamp nextExpiredTime;
//    std::cout<<"reset\n";
    for(const auto& it : expired){
        ActiveTimer timer(it.second,it.second->getSeq());
        if(it.second->getRepeated() && cancelTimers.find(timer) == cancelTimers.end()){
            it.second->restart(now);
            insert(it.second);
        }
        if(!timerList.empty()){
            nextExpiredTime = timerList.begin()->second->getExpiredTime();
        }
        if(nextExpiredTime.valid()){
            resetTimerfd(timerfd,nextExpiredTime);
        }
    }
    addtimer_ptr.reset();
//    std::cout<<"reset ok\n";
}


