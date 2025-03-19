#include "Peer.hpp"
#include "utils.hpp"

Peer::Peer()
{
    
}

int Peer::InitSocket(void)
{
    sock_config.domain   = AF_INET;
    sock_config.type     = SOCK_STREAM;
    sock_config.protocol = 0;

    sockfd = socket(sock_config.domain, sock_config.type, sock_config.protocol);

    return sockfd == 0 ? sockfd : -1;
}

int Peer::GetSockFD(void)
{
    return sockfd;
}

void Peer::SetSockFD(int sockfd)
{
    sockfd = sockfd;
}

void Peer::SetPortNum(int portNum)
{
    portNum = portNum;
}

void Peer::InitAddr(void)
{
    addr.sin_family       = AF_INET;            // Ipv4 address family
    addr.sin_addr.s_addr  = INADDR_ANY;         // Address: 0.0.0.0 -> Bind to ALL available network interfaces
    addr.sin_port         = htons(portNum);    // Setup Port Number Port number
}

int Peer::BindSocket(void)
{
    int ret = 0;
    
    ret = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));

    return ret == 0 ? ret : -1;
}

int Peer::ListenSocket(void)
{
    int ret = 0;

    ret = listen(sockfd, MAX_CONNECTIONS);

    return ret == 0 ? ret : -1;
}

int Peer::AcceptSocket(int masterSockFd)
{
    int ret = 0;
    
    ret = accept(masterSockFd, (struct sockaddr*)&addr, &addrSize);

    return ret == 0 ? ret : -1;
}