#ifndef _PEER_HPP_
#define _PEER_HPP_

#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "defines.hpp"

/* Socket file descriptor information */
typedef struct SocketFD
{
    int domain;  
    int type;
    int protocol;
}sfd_t;

/* Peer class */
class Peer
{
private:
    int sockfd;
    
    sfd_t sock_config;

    int id;
    
    int portNum;

    SA_IN addr;    /* sockaddr_in: internet socket for ipv4 address */

    socklen_t addrSize = sizeof(addr);
    
    std::string addrInStr;

public:
    Peer();
    ~Peer();

    int initSocket(void);
    int getSockFD(void);
    void setSockFD(int sockfd);
    void closeSockFd(void);
    
    int initAddr(e_AddrType addrType);
    void setAddr(SA_IN addr);
    SA_IN  getAddr(void);
    SA_IN* getAddrPtr(void);
    int getAddrSize(void);

    int bindSocket(void);
    int listenSocket(void);
    int acceptSocket(int master);

    int getID(void);
    void setID(int id);

    std::string getAddrInStr(void);
    void setAddrInStr(std::string addr);

    int getPortNum(void);
    void setPortNum(int portNum);

    
};

#endif // _PEER_HPP_