//
// Created by jarvist on 3/29/20.
//

#ifndef WEBSERVER_LOGGER_H
#define WEBSERVER_LOGGER_H


#include "LogStream.h"
#include "AsyncLogging.h"

class Logger {
private:
    void formatTime();
public:
    enum LOG_LEVEL{
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
    };
    Logger(const char* filename,int line,LOG_LEVEL level=LOG_LEVEL::DEBUG);
    ~Logger();

    LogStream logStream;
    int line;
    LOG_LEVEL level;
    std::string filename;
    typedef void (*OutputFunc)(const char* msg, size_t len);
    typedef void (*FlushFunc)();


    LogStream& stream(){return logStream;};

    static std::string logFileName;
    static std::string getLogFileName(){return logFileName;};
    static void setLogFileName(const std::string& logfilename){logFileName = logfilename;};
    static OutputFunc fileOutput;


//    static std::unique_ptr<AsyncLogging> asyncLogging;



    void setFileOutput(OutputFunc out);
    void setFlush(FlushFunc flushFunc);
};


#define LOG Logger(__FILE__,__LINE__).stream()
#define LOG_DEBUG Logger(__FILE__,__LINE__,Logger::LOG_LEVEL::DEBUG).stream()
#define LOG_INFO Logger(__FILE__,__LINE__,Logger::LOG_LEVEL::INFO).stream()
#define LOG_WARN Logger(__FILE__,__LINE__,Logger::LOG_LEVEL::WARN).stream()
#define LOG_ERROR Logger(__FILE__,__LINE__,Logger::LOG_LEVEL::ERROR).stream()
#define LOG_FATAL Logger(__FILE__,__LINE__,Logger::LOG_LEVEL::FATAL).stream()


#endif //WEBSERVER_LOGGER_H

