//
// Created by jarvist on 3/31/20.
//

#include "Thread.h"

Thread::Thread(const Thread::ThreadFunc &Func, const std::string &name) : pthreadID(0),tid(0),
threadFunc(Func),name(name),latch(1),started(false),joined(false)
{
    setDefaultName();
}

Thread::~Thread() {
    if(started && !joined){
        pthread_detach(pthreadID);
    }
}

void Thread::start() {

}



void Thread::setDefaultName() {
    if(name.empty()){
        char buf[32];
        snprintf(buf, sizeof buf, "Thread");
        name = buf;
    }
}

int Thread::join() {
    assert(started);
    assert(!joined);
    joined = true;
    return pthread_join(pthreadID,nullptr);
}


