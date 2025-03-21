#ifndef _PEER_HPP_
#define _PEER_HPP_

#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "defines.hpp"

// Socket file descriptor
typedef struct SocketFD
{
    int domain;  
    int type;
    int protocol;
}sfd_t;

class Peer
{
private:
    int sockfd;
    sfd_t sock_config;
    int id;
    
    int portNum;

    // sockaddr_in: internet address in ipv4
    struct sockaddr_in addr;

    socklen_t addrSize = sizeof(addr);
    
    std::string addrInStr;

public:
    Peer();

    int initSocket(void);
    int getSockFD(void);
    void setSockFD(int sockfd);
    void setPortNum(int portNum);
    void initDefaultAddr(void);
    int bindSocket(void);
    int listenSocket(void);
    int acceptSocket(int master);
};

#endif // _PEER_HPP_