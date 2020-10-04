cmake_minimum_required(VERSION 3.15)
project(webServer)

set(CMAKE_CXX_STANDARD 14)

include_directories(Base)
include_directories(Log)
include_directories(Utils)
include_directories(Http)
include_directories(EventLoop)
include_directories(Tcp)
include_directories(Timer)
find_package(Threads)

add_executable(webServer
        Base/Buffer.cpp
        Base/Buffer.h
        Base/Copyable.h
        Base/CountDownLatch.h
#        Base/Endian.h
        Base/FixedBuffer.h
        Base/NonCopyable.h
        Base/Thread.cpp
        Base/Thread.h
        Base/ThreadPool.cpp
        Base/ThreadPool.h
        Base/Types.h
        Utils/TimeStamp.h
        Utils/TimeStamp.cpp
        Timer/Timer.h
        Timer/Timer.cpp
        Timer/TimerManager.h
        Timer/TimerManager.cpp
        Tcp/Socket.h
        Tcp/Socket.cpp
        Tcp/SocketsOps.h
        Tcp/SocketsOps.cpp
        Tcp/Acceptor.cpp
        Tcp/Acceptor.h
        Tcp/TcpConnection.cpp
        Tcp/TcpConnection.h
        Tcp/TcpServer.cpp
        Tcp/TcpServer.h
        Tcp/INetAddress.cpp
        Tcp/INetAddress.h

        Http/HttpContext.h
        Http/HttpContext.cpp
        Http/HttpRequest.h
        Http/HttpRequest.cpp
        Http/HttpResponse.h
        Http/HttpResponse.cpp

        Log/AsyncLogging.cpp
        Log/AsyncLogging.h
        Log/FileUtils.cpp
        Log/FileUtils.h
        Log/LogFile.cpp
        Log/LogFile.h
        Log/Logger.cpp
        Log/Logger.h
        Log/LogStream.cpp
        Log/LogStream.h
        main.cpp

        Epoll/Channel.cpp
        Epoll/Channel.h
        EventLoop/EventLoop.cpp
        EventLoop/EventLoop.h
        Epoll/Epoller.cpp
        Epoll/Epoller.h
        EventLoop/EventLoopThread.cpp
        EventLoop/EventLoopThread.h
        EventLoop/EventLoopThreadPool.cpp
        EventLoop/EventLoopThreadPool.h
        EventLoop/AsyncWaker.cpp
        EventLoop/AsyncWaker.h
        Http/HttpServer.cpp Http/HttpServer.h Utils/Callbacks.h)
target_link_libraries(webServer ${CMAKE_THREAD_LIBS_INIT})