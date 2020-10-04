//
// Created by jarvist on 3/28/20.
//

#include <string.h>
#include "FileUtils.h"

size_t FileUtils::write(const char *logline, size_t len) {
    return fwrite_unlocked(logline,1,len,fp);
}

FileUtils::FileUtils(std::string filename) : fp(fopen(filename.c_str(), "ae")){
    setbuffer(fp,buffer, sizeof(buffer));
}

void FileUtils::append(const char *logline, size_t len) {
    char errorbuf[512];
    while (len){
        size_t x = write(logline,len);
        if(x==0){
            int err = ferror(fp);
            if(err){
                fprintf(stderr,"Append Error, %s\n",strerror_r(err,errorbuf, sizeof(errorbuf)));
            }
            break;
        }
        logline += x;
        len -= x;
    }
}
