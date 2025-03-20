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

    sockfd = socket(sock_config.domain, sock_config.type, sock_config.protocol);

    return sockfd;
}

int Peer::getSockFD(void)
{
    return sockfd;
}

void Peer::setSockFD(int sockfd)
{
    sockfd = sockfd;
}

void Peer::setPortNum(int portNum)
{
    portNum = portNum;
}

void Peer::initAddr(void)
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
    int ret = 0;
    
    ret = accept(masterSockFd, (struct sockaddr*)&addr, &addrSize);

    return ret;
}