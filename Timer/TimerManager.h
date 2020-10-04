//
// Created by jarvist on 4/7/20.
//

#ifndef WEBSERVER_TIMERMANAGER_H
#define WEBSERVER_TIMERMANAGER_H


#include <set>
#include <vector>
#include "../Base/NonCopyable.h"
//#include "EventLoop.h"
#include "../Utils/TimeStamp.h"
#include "../Epoll/Channel.h"
#include "Timer.h"

class EventLoop;
class TimerManager : NonCopyable{

public:
//    TimerManager(){};
    TimerManager(EventLoop* eventLoop);
    ~TimerManager();

    TimerId addTimer(const TimerCallback& cb,TimeStamp when,double interval);
    void cancel(const TimerId& id);

private:
    EventLoop* loop;
    std::shared_ptr<Timer> addtimer_ptr;
    const int timerfd;
    std::unique_ptr<Channel> timerfdChannel;
    bool callingExpiredTimers;

    typedef std::pair<TimeStamp,std::shared_ptr<Timer>> TimerPair;
    typedef std::set<TimerPair> TimerList;
    typedef std::pair<std::shared_ptr<Timer>,int64_t > ActiveTimer;
    typedef std::set<ActiveTimer> ActiveTimerSet;

    ActiveTimerSet activeTimers;
    ActiveTimerSet cancelTimers;
    TimerList timerList;

    void addTimerInLoop(const std::shared_ptr<Timer>& timer);
    void cancelInLoop(const TimerId& id);

    void handleRead();
    std::vector<TimerPair> getExpired(TimeStamp now);
    void reset(const std::vector<TimerPair> &expired, TimeStamp now);
    bool insert(const std::shared_ptr<Timer>& timer);



};


#endif //WEBSERVER_TIMERMANAGER_H
