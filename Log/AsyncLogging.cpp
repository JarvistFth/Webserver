//
// Created by jarvist on 3/29/20.
//

#include "AsyncLogging.h"

#include <memory>
#include <utility>
#include <iostream>
#include "LogFile.h"


AsyncLogging::AsyncLogging(std::string basename, int flushInterval): basename(std::move(basename)),
                                                                            flushIntervalN(flushInterval) , latch(1),
                                                                            currentBuffer(std::make_unique<Buffer>()), nextBuffer(std::make_unique<Buffer>()),
                                                                            buffersToFile()
{
    currentBuffer->bzero();
    nextBuffer->bzero();
    buffersToFile.reserve(16);
}

void AsyncLogging::append(const char *logline, size_t len) {
    std::lock_guard<std::mutex>lockGuard(mMutex);
    if(currentBuffer->getAvail() > len){
        currentBuffer->append(logline,len);
    }else{
        buffersToFile.emplace_back(std::move(currentBuffer));
        if(nextBuffer){
            currentBuffer = std::move(nextBuffer);
        }else{
            currentBuffer = std::make_unique<Buffer>();
        }
        currentBuffer->append(logline,len);
        condition.notify_one();
    }
}

void AsyncLogging::threadFunc() {
    std::cout<<"start"<<std::endl;
    latch.countDown();
    LogFile output(basename);
    BufferPtr newBuffer1 = std::make_unique<Buffer>();
    BufferPtr newBuffer2 = std::make_unique<Buffer>();
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    while(isRunning){
        {
            std::unique_lock<std::mutex>lock(mMutex);
            if(buffersToFile.empty()){
                condition.wait_for(lock,std::chrono::duration<int>(flushIntervalN));
//                condition.wait(lock,!buffersToFile.empty());

            }
            buffersToFile.emplace_back(std::move(currentBuffer));
//            currentBuffer.reset();
            currentBuffer = std::move(newBuffer1);
            buffersToWrite.swap(buffersToFile);
            if(!nextBuffer){
                nextBuffer = std::move(newBuffer2);
            }

            for(auto& bf:buffersToWrite){
                output.append(bf->getData(),bf->length());
            }
            if(!newBuffer1){
                newBuffer1 = std::make_unique<Buffer>();
            }
            if(!newBuffer2){
                newBuffer2 = std::make_unique<Buffer>();
            }
            buffersToWrite.clear();
            output.flush();
        }
        output.flush();
    }



}




