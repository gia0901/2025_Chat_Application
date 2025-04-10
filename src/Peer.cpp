#include "Peer.hpp"
#include "utils.hpp"

Peer::Peer()
{
    sock_config = {0};
    sockfd = -1;
    id = -1;
    portNum = -1;
    addr = {0};
    addrInStr = "";
}

Peer::~Peer()
{
    //close(sockfd);  /*< Close socket fd >*/
    // if a local peer is created, destructor will be called after the function finished -> Bad file descriptor
    // should not close here, check what should do later
}

int Peer::initSocket(void)
{
    sock_config.domain   = AF_INET;
    sock_config.type     = SOCK_STREAM;
    sock_config.protocol = 0;

    this->sockfd = socket(sock_config.domain, sock_config.type, sock_config.protocol);

    return this->sockfd;
}

int Peer::getSockFD(void)
{
    return this->sockfd;
}

void Peer::setSockFD(int sockfd)
{
    this->sockfd = sockfd;
}

void Peer::initAddr(void)
{
    addr.sin_family       = AF_INET;            /* Ipv4 address family */
    addr.sin_addr.s_addr  = INADDR_ANY;         /* Address: 0.0.0.0 -> Bind to ALL available network interfaces */
    addr.sin_port         = htons(portNum);     /* Setup Port Number */
}

int Peer::bindSocket(void)
{
    int ret = 0;
    
    ret = bind(this->sockfd, (SA*)&this->addr, sizeof(this->addr));

    return ret;
}

int Peer::listenSocket(void)
{
    int ret = 0;

    ret = listen(sockfd, MAX_BACKLOGS);

    return ret;
}

int Peer::acceptSocket(int masterSockFd)
{
    int new_sockfd = 0;
    
    new_sockfd = accept(masterSockFd, (SA*)&addr, &addrSize);

    this->sockfd = new_sockfd;

    return new_sockfd;
}

int Peer::getID(void)
{
    return this->id;
}

void Peer::setID(int id)
{
    this->id = id;
}

void Peer::setAddr(SA_IN addr)
{
    this->addr = addr;
}

SA_IN Peer::getAddr(void)
{
    return this->addr;
}

SA_IN* Peer::getAddrPtr(void)
{
    return &this->addr;
}

int Peer::getAddrSize(void)
{
    return sizeof(this->addr);
}

std::string Peer::getAddrInStr(void)
{
    return this->addrInStr;
}

void Peer::setAddrInStr(std::string addr)
{
    this->addrInStr = addr;
}

int Peer::getPortNum(void)
{
    return this->portNum;
}

void Peer::setPortNum(int portNum)
{
    this->portNum = portNum;
}

void Peer::closeSockFd(void)
{
    close(sockfd);
}