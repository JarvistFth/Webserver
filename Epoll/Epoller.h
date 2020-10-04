//
// Created by jarvist on 2020/10/3.
//

#ifndef WEBSERVER_EPOLLER_H
#define WEBSERVER_EPOLLER_H

#include <NonCopyable.h>
#include <vector>
#include <sys/epoll.h>
#include <map>
#include <TimeStamp.h>
#include <Types.h>

class EventLoop;
class Channel;
class Epoller : NonCopyable{
private:
    typedef std::vector<epoll_event> EventList;
    typedef std::vector<Channel*> ChannelList;
    typedef std::map<int,Channel*> ChannelMap;


private:
    ChannelMap channels;
    EventLoop* myLoop;

    int epfd;
    EventList events;


    void update(int opt, Channel *channel);
    void fillActiveChannels(int nready, ChannelList* activeChannel)const;

    static const int initialEventListSize = 16;
    static const int timeouMs = 5000;

public:
    Epoller(EventLoop* loop);

    virtual ~Epoller();

    TimeStamp poll(ChannelList* channelList);
    void handleExpired();

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    void assertInLoopThread();

};


#endif //WEBSERVER_EPOLLER_H
