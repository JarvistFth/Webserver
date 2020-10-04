//
// Created by jarvist on 3/29/20.
//

#ifndef WEBSERVER_ASYNCLOGGING_H
#define WEBSERVER_ASYNCLOGGING_H


#include "../Base/NonCopyable.h"
#include "../Base/FixedBuffer.h"
#include "../Base/CountDownLatch.h"
#include <mutex>
#include <thread>
#include <condition_variable>
#include <vector>
#include <functional>
#include <atomic>

class AsyncLogging : NonCopyable {
private:
    std::mutex mMutex;
    std::condition_variable condition;
//    std::thread mThread;
    std::unique_ptr<std::thread> mThread;

    bool isRunning;
    const int flushIntervalN;
    std::string basename;

    void threadFunc();
    typedef FixedBuffer<smallSize> Buffer;
    typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
    typedef BufferVector::value_type BufferPtr;

    BufferPtr currentBuffer;
    BufferPtr nextBuffer;
    BufferVector buffersToFile;
    CountDownLatch latch;

public:
    AsyncLogging(std::string  basename,int flushInterval = 1);
    ~AsyncLogging(){
        if(isRunning){
            stop();
        }
    };

    void append(const char* logline,size_t len);

    void start(){
        isRunning = true;
        mThread = std::make_unique<std::thread>(std::bind(&AsyncLogging::threadFunc,this));
        latch.wait();
    }

    void stop(){
        isRunning = false;
        condition.notify_all();
        mThread->join();
    }

    bool running(){
        return isRunning;
    }



};


#endif //WEBSERVER_ASYNCLOGGING_H
