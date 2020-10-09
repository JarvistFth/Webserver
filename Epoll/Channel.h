//
// Created by jarvist on 2020/10/3.
//

#ifndef WEBSERVER_CHANNEL_H
#define WEBSERVER_CHANNEL_H


#include <NonCopyable.h>
#include <functional>
#include <TimeStamp.h>
#include <sys/epoll.h>

class EventLoop;
class Channel : NonCopyable {
    typedef std::function<void()> EventCallback;
    typedef std::function<void(TimeStamp)> ReadCallback;
private:


    EventLoop* loop;
    int fd;

    int event;
    int revent;

    int status;
    bool eventHandling;

    ReadCallback readCallback;
    EventCallback writeCallback;
    EventCallback errorCallback;
    EventCallback closeCallback;

    static const int noneEvent;
    static const int readEvent;
    static const int writeEvent;

public:
    Channel(EventLoop *loop, int fd);

    virtual ~Channel();

    void update();
    void remove();

    void handleEvent(TimeStamp receivedTime);

    void setReadCallback(const ReadCallback &readCallback);

    void setWriteCallback(const EventCallback &writeCallback);

    void setErrorCallback(const EventCallback &errorCallback);

    void setCloseCallback(const EventCallback &closeCallback);

    int getFd() const;

    void setFd(int fd);

    int getEvent() const;

    void setEvent(int event);

    int getRevent() const;

    void setRevent(int revent);

    int getStatus() const;

    void setStatus(int status);

    bool isNoneEvent(){
        return event == noneEvent;
    }

    bool isWriting(){
        return event == writeEvent;
    }

    EventLoop *getLoop() const;

    void enableRead(){
        event |= readEvent;
        update();
    }

    void enableWrite(){
        event |= writeEvent;
        update();
    }

    void disableRead(){
        event &= (-readEvent);
        update();
    }

    void disableWrite(){
        event &= (-writeEvent);
        update();
    }

    void enableAll(){
        event |= (readEvent | writeEvent);
        update();
    }

    void disableAll(){
        event = noneEvent;
        update();
    };

#ifdef USE_EPOLL_LT
#else
    void enableEpollET(){
        event |= EPOLLET;
        update();
    }
#endif
};


#endif //WEBSERVER_CHANNEL_H
