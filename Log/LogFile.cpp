//
// Created by jarvist on 3/28/20.
//

#include "LogFile.h"

#include <memory>



LogFile::LogFile(const std::string &Basename, int FlushEveryN):
    basename(Basename),flushEveryN(FlushEveryN),count(0)
{
    file = std::make_unique<FileUtils>(Basename);
}

LogFile::~LogFile() = default;

void LogFile::append(const char *logline, size_t len) {
    {
        std::lock_guard<std::mutex>lockGuard(mMutex);
        append_unlock(logline,len);

    }


}

void LogFile::append_unlock(const char *logline, size_t len) {
    file->append(logline,len);
    count++;
    if(count >= flushEveryN){
        count=0;
        file->flush();
    }
}


void LogFile::flush() {
    std::lock_guard<std::mutex>lockGuard(mMutex);
    file->flush();

}


