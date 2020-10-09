//
// Created by jarvist on 3/26/20.
//

#include "Buffer.h"
#include "Types.h"
#include <error.h>
#include <sys/uio.h>
#include <unistd.h>

const char Buffer::kCRLF[] = "\r\n";


const size_t Buffer::kCheapPrependable;
const size_t Buffer::kInitSize;

#ifdef USE_EPOLL_LT

ssize_t Buffer::readFd(int fd, int *savedErrno) {
    char extrabuf[65536]; // 64*1024
    struct iovec vec[2];
    const size_t writable = writeableBytes();
    vec[0].iov_base = begin() + writeIndex;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;
    // readv()代表分散读， 即将数据从文件描述符读到分散的内存块中
    const ssize_t n = readv(fd, vec, 2);
    if (n < 0) {
        *savedErrno = errno;
    } else if (implicit_cast<size_t>(n) <= writable) {
        writeIndex += n;
    } else {
        writeIndex = buffer.size();
        append(extrabuf, n - writable);
    }
    return n;
}
ssize_t Buffer::writeFd(int fd, int *savedErrno) {
    ssize_t n = ::write(fd, peek(), readableBytes());

    if (n > 0) {
        retrieve(n);
    }
    return 0;
}

#else
ssize_t Buffer::readFdET(int fd, int *savedErrno) {
    char extrabuf[65536];
    struct iovec vec[2];

    size_t writable = writeableBytes();
    vec[0].iov_base = begin() + writeIndex;
    vec[0].iov_len = writable;

    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);

    ssize_t readLen = 0;

    for(;;){
        ssize_t n = readv(fd,vec,2);
        if(n < 0){
            if(errno == EAGAIN) {
                *savedErrno = errno;
                break;
            }
//            }else if(errno == EINTR){
//                continue;
//            }
            return -1;
        }else if(n == 0){
            //read nothing, peer closed conn
            return 0;
        }else if(implicit_cast<size_t>(n) <= writable){
            writeIndex += n;
        }else{
            writeIndex = buffer.size();
            append(extrabuf,n - writable);
        }

        writable = writeableBytes();
        vec[0].iov_base  = begin() + writeIndex;
        vec[0].iov_len = writable;
        readLen += n;
    }
    return readLen;
}
ssize_t Buffer::writeFdET(int fd, int *savedErrno) {
    ssize_t writeSum = 0;

    for(;;){
        ssize_t n = ::write(fd,peek(),readableBytes());
        if(n > 0){
            writeSum += n;
            retrieve(n);
            if(readableBytes() == 0){
                return writeSum;
            }
        }else if(n < 0){
            if(errno == EAGAIN){
                break;
            }
//            else if(errno == EINTR){
//                continue;
//            }
            else{
                return -1;
            }
        }else{
            return 0;
        }
    }
    return writeSum;
}
#endif