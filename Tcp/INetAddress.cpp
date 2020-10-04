//
// Created by jarvist on 2020/10/3.
//

#include <Types.h>
#include "INetAddress.h"
#include "SocketsOps.h"

const sockaddr_in &INetAddress::getAddr() const {
    return addr;
}

void INetAddress::setAddr(const sockaddr_in &addr) {
    INetAddress::addr = addr;
}

INetAddress::INetAddress(uint16_t port) {
    memZero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = sockets::hostToNetwork32(INADDR_ANY);
    addr.sin_port = sockets::hostToNetwork16(port);
}

INetAddress::INetAddress(const std::string &ip, uint16_t port) {
    memZero(&addr, sizeof(addr));
    sockets::fromHostPort(ip.c_str(),port,&addr);
}

INetAddress::INetAddress(const struct sockaddr_in &sockaddrIn) {
    addr = sockaddrIn;
}

std::string INetAddress::toIpPortString() const {
    char buf[32];
    sockets::toHostPort(buf, sizeof(buf),addr);
    return buf;
}


