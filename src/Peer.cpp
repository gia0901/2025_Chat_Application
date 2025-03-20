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

void Peer::setId(int id)
{
    id = id;
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

void Peer::setAddr(struct sockaddr_in addr)
{
    addr = addr;
}

void Peer::setAddLenght(socklen_t addrSize)
{
    addrSize = addrSize;
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

void Peer::acceptConnection(std::vector<Peer> peerList)
{
    /* Local variables */ 
    char addr_in_str[INET_ADDRSTRLEN];
    int port = ntohs(addr.sin_port);
    static int peerIndex = 0;

    /* Convert IP address from network byte order to string */
    inet_ntop(AF_INET, &addr.sin_addr, addr_in_str, INET_ADDRSTRLEN);

    peerList[peerIndex].setSockFD(sockfd);
    peerList[peerIndex].setId(peerIndex);
    peerList[peerIndex].setPortNum(port);
    peerList[peerIndex].setAddr(addr);
    peerList[peerIndex].setAddLenght(addrSize);

    if(peerIndex == MAX_CONNECTIONS)
    {
        APP_DEBUG_PRINT("Max number of connections reached.");
        return;
    }

    peerIndex++;
}