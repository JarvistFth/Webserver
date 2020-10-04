//
// Created by jarvist on 2020/10/3.
//

#ifndef WEBSERVER_EVENTLOOP_H
#define WEBSERVER_EVENTLOOP_H


#include <thread>
#include <vector>
#include <TimeStamp.h>
#include <mutex>
#include <condition_variable>
#include <functional>
#include "../Timer/TimerManager.h"

class Channel;
class Epoller;
class AsyncWaker;
class EventLoop : NonCopyable {
private:
    bool looping;
    typedef std::vector<Channel*> ChannelList;
    ChannelList activeChannels;

    std::unique_ptr<Epoller> epoller;
    std::unique_ptr<AsyncWaker> asyncWaker;
    std::unique_ptr<TimerManager> timerManager;

    TimeStamp retTime;
    bool quited;
    bool doingPendingFunctors;

    std::mutex mMutex;

    typedef std::function<void()> Functor;
    std::vector<Functor> pendingFunctors;
    void doPendingFunctors();


public:
    EventLoop();
    ~EventLoop();
    const std::thread::id threadID;

    void loop();
    void quit();

    void runInLoop(const Functor& cb);
    void queueInLoop(const Functor& cb);

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    bool isInLoopThread(){
        return threadID == std::this_thread::get_id();
    }
    void assertInLoopThread();

    std::string getThreadIDString(std::thread::id id);

    TimerId runAfter(double delay, const TimerCallback &cb);

    void cancel(const TimerId &timerid);

    TimerId runEvery(double interval, const TimerCallback &cb);

    TimerId runAt(TimeStamp time, const TimerCallback &cb);
};


#endif //WEBSERVER_EVENTLOOP_H
