//
// Created by jarvist on 3/19/20.
//

#include "ThreadPool.h"

#include <memory>

void std::ThreadPool::init() {
    for(int i=0;i<thread_num;i++){
        pool.emplace_back(std::move(std::make_unique<thread>(std::bind(&ThreadPool::runInThread,this))));
    }
}

void std::ThreadPool::runInThread() {
    while(!isStopped){
        Task task;
        {
            std::unique_lock<mutex> tasklock(mutex_lock);
            this->condition.wait(tasklock,[this]{
                return this->isStopped|| !this->workqueue.empty();
            });
            if(this->isStopped && this->workqueue.empty()){
                return;
            }
            task = std::move(workqueue.front());
            this->workqueue.pop();
        }
        task();
    }

}

void std::ThreadPool::commit(Task task) {
    if(isStopped){
        throw runtime_error("thread pool is stopped");
    }

    {
        std::unique_lock<mutex> commitLock(mutex_lock);
        workqueue.emplace(std::move(task));
        condition.notify_one();
    }


}

std::ThreadPool::~ThreadPool() {
    this->isStopped = true;
    condition.notify_all();
    for(auto &threads:pool){
        if(threads->joinable()){
            threads->join();
        }
    }
}


