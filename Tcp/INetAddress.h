//
// Created by jarvist on 2020/10/3.
//

#ifndef WEBSERVER_INETADDRESS_H
#define WEBSERVER_INETADDRESS_H


#include <Copyable.h>
#include <netinet/in.h>
#include <string>

class INetAddress : public Copyable{
private:
    struct sockaddr_in addr;

public:
    explicit INetAddress(uint16_t port);
    INetAddress(const std::string& ip, uint16_t port);
    INetAddress(const struct sockaddr_in& sockaddrIn);

    std::string toIpPortString() const ;

    const sockaddr_in &getAddr() const;
    void setAddr(const sockaddr_in &addr);
};


#endif //WEBSERVER_INETADDRESS_H
