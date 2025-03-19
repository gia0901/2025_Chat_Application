#ifndef _PEER_HPP_
#define _PEER_HPP_

#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "defines.hpp"


class Peer
{
public:
    Peer();
    
    int sockfd;
    
    int id;
    
    int portNum;

    // sockaddr_in: internet address in ipv4
    struct sockaddr_in addr;

    socklen_t addrSize = sizeof(addr);
    
    std::string addrInStr;
};

#endif // _PEER_HPP_