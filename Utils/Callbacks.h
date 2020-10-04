//
// Created by jarvist on 2020/10/4.
//

#ifndef WEBSERVER_CALLBACKS_H
#define WEBSERVER_CALLBACKS_H


#include <memory>
#include <functional>
#include "TimeStamp.h"
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;


class TcpConnection;
class Buffer;


typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef std::function<void()> TimerCallback;
typedef std::function<void(const TcpConnectionPtr &)> ConnectionCallback;

typedef std::function<void(const TcpConnectionPtr &, Buffer *buf, TimeStamp)>
        OnMessageCallback;

typedef std::function<void(const TcpConnectionPtr &)> WriteCompleteCallback;

typedef std::function<void(const TcpConnectionPtr &)> CloseCallback;
#endif //WEBSERVER_CALLBACKS_H
