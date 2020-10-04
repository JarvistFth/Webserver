//
// Created by jarvist on 3/19/20.
//

#ifndef WEBSERVER_NONCOPYABLE_H
#define WEBSERVER_NONCOPYABLE_H


class NonCopyable {
protected:
    NonCopyable()= default;
    ~NonCopyable()= default;

public:
    NonCopyable(const NonCopyable&) = delete;
    void operator=(const NonCopyable&) = delete;

};


#endif //WEBSERVER_NONCOPYABLE_H
