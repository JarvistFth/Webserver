//
// Created by jarvist on 3/19/20.
//

#ifndef WEBSERVER_THREADPOOL_H
#define WEBSERVER_THREADPOOL_H
//#include "Mutex.h"
//#include "Condition.h"
#include <queue>
#include <vector>
#include <iostream>
#include <memory>
#include <thread>
#include <condition_variable>
#include <functional>
#include <future>
//#include <atomic>
#include <stdexcept>

using Task = std::function<void()>;

namespace std{
    class ThreadPool {
    private:
        //open thread num
        int thread_num;
        //max task num in queue size
//        int max_requests;
        //thread pool contains thread
        vector< unique_ptr<thread> > pool;
        //workqueue for Task function
        queue<Task> workqueue;
        //mutex mMutex
        mutex mutex_lock;
        //conditional variable
        condition_variable condition;
        //if thread pool is stopped
        bool isStopped;
        //the num of working thread

    public:
        ThreadPool(int tNum):thread_num(tNum),isStopped(false){};
        ~ThreadPool();

        void init();
        void commit(Task task);
        void runInThread();








    };
}



#endif //WEBSERVER_THREADPOOL_H
