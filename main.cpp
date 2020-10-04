#include <iostream>
#include <Logger.h>
#include <HttpServer.h>
#include "EventLoop/EventLoop.h"
#include "EventLoop/EventLoopThreadPool.h"

int main() {
//    LOG_INFO << "hello";


//    EventLoop eventLoop;
//    EventLoopThreadPool pool(&eventLoop,4);
//    pool.start();
//    eventLoop.loop();

    EventLoop loop;
    HttpServer server(&loop,INetAddress(8004),"httpserver",4);
    server.start();
    loop.loop();
    return 0;
}