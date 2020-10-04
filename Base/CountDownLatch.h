//
// Created by jarvist on 3/29/20.
//

#ifndef WEBSERVER_COUNTDOWNLATCH_H
#define WEBSERVER_COUNTDOWNLATCH_H


#include "NonCopyable.h"
#include <mutex>
#include <condition_variable>

class CountDownLatch : NonCopyable{
private:
    std::mutex mMutex;
    std::condition_variable condition;
    int count;

public:
    explicit CountDownLatch(int c):count(c){

    };
    void wait(){
        std::unique_lock<std::mutex>lock (mMutex);
        while(count > 0){
            condition.wait(lock);
        }

    }
    void countDown(){
        std::unique_lock<std::mutex>lock (mMutex);
        --count;
        if(count == 0){
            condition.notify_all();
        }

    }
    int getCount(){
        std::lock_guard<std::mutex>lockGuard (mMutex);
        return count;
    }


};


#endif //WEBSERVER_COUNTDOWNLATCH_H
