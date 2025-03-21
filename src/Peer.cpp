#include "Peer.hpp"
#include "utils.hpp"

Peer::Peer()
{
    
}

int Peer::initSocket(void)
{
    sock_config.domain   = AF_INET;
    sock_config.type     = SOCK_STREAM;
    sock_config.protocol = 0;

    this->sockfd = socket(sock_config.domain, sock_config.type, sock_config.protocol);

    return sockfd;
}

int Peer::getSockFD(void)
{
    return this->sockfd;
}

void Peer::setSockFD(int sockfd)
{
    this->sockfd = sockfd;
}

void Peer::setPortNum(int portNum)
{
    this->portNum = portNum;
}

void Peer::initDefaultAddr(void)
{
    addr.sin_family       = AF_INET;            // Ipv4 address family
    addr.sin_addr.s_addr  = INADDR_ANY;         // Address: 0.0.0.0 -> Bind to ALL available network interfaces
    addr.sin_port         = htons(portNum);    // Setup Port Number Port number
}

int Peer::bindSocket(void)
{
    int ret = 0;
    
    ret = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));

    return ret;
}

int Peer::listenSocket(void)
{
    int ret = 0;

    ret = listen(sockfd, MAX_CONNECTIONS);

    return ret;
}

int Peer::acceptSocket(int masterSockFd)
{
    int new_sockfd = 0;
    
    new_sockfd = accept(masterSockFd, (struct sockaddr*)&addr, &addrSize);

    this->sockfd = new_sockfd;

    return new_sockfd;
}

int Peer::getID(void)
{
    return id;
}

void Peer::setID(int id)
{
    this->id = id;
}

std::string Peer::getAddrInStr(void)
{
    return addrInStr;
}

void Peer::setAddrInStr(std::string addr)
{
    this->addrInStr = addr;
}

int Peer::getPortNum(void)
{
    return portNum;
}

void Peer::setPortNum(int portNum)
{
    this->portNum = portNum;
}