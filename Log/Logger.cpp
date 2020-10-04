//
// Created by jarvist on 3/29/20.
//

#include <iostream>
#include <sys/time.h>
#include "Logger.h"

std::string Logger::logFileName = "/home/jarvist/WebServer.log";
static AsyncLogging* asyncLogging;
//std::unique_ptr<AsyncLogging> Logger::asyncLogging = std::make_unique<AsyncLogging>(Logger::getLogFileName());
static std::once_flag initflag;

const char* LogLevelName[5]={
        "[DEBUG] ",
        "[INFO] ",
        "[WARN] ",
        "[ERROR] ",
        "[FATAL] ",
};


void once_init() {
    asyncLogging = new AsyncLogging(Logger::getLogFileName());
    asyncLogging->start();

}

void output(const char* msg, size_t len) {
//    std::call_once(initflag,[&](){Logger::asyncLogging->start();std::cout<<Logger::asyncLogging->running()<<std::endl;});
        std::call_once(initflag,&once_init);
        asyncLogging->append(msg,len);
    }
void defaultOutput(const char* msg, size_t len)
{
    size_t n = fwrite(msg, 1, len, stdout);
    //FIXME check n
    (void)n;
}

void defaultFlush()
{
    fflush(stdout);
}
Logger::OutputFunc g_output = output;
Logger::FlushFunc g_flush = defaultFlush;

Logger::Logger(const char *filename, int line, LOG_LEVEL level):filename(filename),line(line),level(level) {
    formatTime();
}

void Logger::setFileOutput(Logger::OutputFunc out){
    g_output = output;
}



Logger::~Logger()
{
    logStream.operator<<(" in:").operator<<(filename).operator<<(":").operator<<(line).operator<<('\n');
    const LogStream::Buffer& buf(logStream.getBuffer());
    g_output(buf.getData(),buf.length());
//    output
//    asyncLogging->stop();
//    if(asyncLogging){
//        delete(asyncLogging);
//    }
}




void Logger::formatTime() {
    struct timeval tv;
    time_t time;
    char str_t[26] = {0};
    gettimeofday(&tv,NULL);
    time = tv.tv_sec;
    struct tm* timepointer = localtime(&time);
    strftime(str_t,26,"%Y-%m-%d %H:%M:%S:", timepointer);
    logStream << str_t;
    logStream.operator<<(LogLevelName[level]) ;

}



