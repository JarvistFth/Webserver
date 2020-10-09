#pragma once

#include <iostream>
#include <NonCopyable.h>


class INetAddress;

// socket fd 的封装
// 在析构时关闭sockfd
// 线程安全，所有操作都转交给OS

class Socket : NonCopyable {
public:
  explicit Socket(int sockfd) : sockfd_(sockfd) {
    // std::cout << "sockfd_ is:  " << sockfd_ << std::endl;
  }

  ~Socket();

  int fd() const { return sockfd_; }

  void forceClose();

  // 服务端建立连接的三个流程
  void bindAddress(const INetAddress &localaddr);
  void listen();

  int accept(INetAddress *peeraddr);

  // 设置地址重用
  void setReuseAddr(bool on);
  void setReusePort(bool on);

  void shutdownWrite();

  /// TCP 的一些操作，也要记住
  void setTcpNoDelay(bool on);
  void setKeepAlive(bool on);

private:
  const int sockfd_;
};

