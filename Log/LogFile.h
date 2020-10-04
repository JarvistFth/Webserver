//
// Created by jarvist on 3/28/20.
//

#ifndef WEBSERVER_LOGFILE_H
#define WEBSERVER_LOGFILE_H


#include "../Base/NonCopyable.h"
#include <memory>
#include <mutex>
#include <string>
#include "FileUtils.h"
typedef std::mutex Mutex;
//using Unique_ptr = std::unique_ptr<FileUtils >;
class LogFile : NonCopyable{
private:
    const int flushEveryN;
    const std::string basename;
    int count;

    Mutex mMutex;
    std::unique_ptr<FileUtils> file;
//    Unique_ptr file;
    void append_unlock(const char* logline,size_t len);

public:
    LogFile(const std::string& Basename,int FlushEveryN = 1);
    ~LogFile();
    void append(const char* logline, size_t len);
    void append_unlock(char* logline, size_t len);
    void flush();


};




#endif //WEBSERVER_LOGFILE_H
