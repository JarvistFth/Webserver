//
// Created by jarvist on 3/28/20.
//

#ifndef WEBSERVER_FILEUTILS_H
#define WEBSERVER_FILEUTILS_H



#include "../Base/NonCopyable.h"
#include <cstdio>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#define BUFERSIZE 64*1024

class FileUtils : NonCopyable{
private:
    FILE* fp;
    char buffer[BUFERSIZE];
//    off_t writenBytes;
    size_t write(const char* logline,size_t len);

public:
    explicit FileUtils(std::string filename );
    ~FileUtils(){
        fclose(fp);
    }
    void append(const char* logline,const size_t  len);
    void flush(){
        fflush(fp);
    };


};


#endif //WEBSERVER_FILEUTILS_H
