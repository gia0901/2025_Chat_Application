#include "MasterPeer.hpp"
#include "utils.hpp"


#define IS_MASTER_PEER_CREATED()    do { \
                                        if (masterPeer == nullptr) { \
                                            masterPeer = new Peer(); \
                                        } \
                                        else { \
                                            APP_INFO_PRINT("masterPeer is already created."); \
                                        } \
                                    } while(0)

MasterPeer::MasterPeer()
{
    // 1. Allocate Master Peer
    if (masterPeer == nullptr)
        masterPeer = new Peer();
    
    // 2. Init Mutex
    masterMutex = PTHREAD_MUTEX_INITIALIZER;
}

// Singleton
MasterPeer* MasterPeer::pInstance = nullptr;

MasterPeer* MasterPeer::getInstance(void)
{
    if (pInstance == nullptr)
        pInstance = new MasterPeer();

    return pInstance;
}

int MasterPeer::initSocket(int portNum)
{
    int ret = 0;

    IS_MASTER_PEER_CREATED();
    
    mutexLock();

    // 1. Init socket
    masterPeer->portNum = portNum;
    masterPeer->sockfd = socket(AF_INET, SOCK_STREAM, 0);   // Internet socket - Stream
    if (masterPeer->sockfd < 0)
    {
        APP_DEBUG_PRINT("Create socket for MasterPeer failed.");
        mutexUnlock();
        return -1;
    }
    
    // 2. Init address structure 
    //    Socket will be binded to this address struct, so that other apps can find and connect to the socket.
    masterPeer->addr.sin_family = AF_INET;                  // Ipv4 address family
    masterPeer->addr.sin_addr.s_addr = INADDR_ANY;          // Address: 0.0.0.0 -> Bind to ALL available network interfaces
    masterPeer->addr.sin_port = htons(masterPeer->portNum); // Setup Port Number


    // 3. Bind socket to the address
    ret = bind(masterPeer->sockfd, (struct sockaddr*)&masterPeer->addr, sizeof(masterPeer->addr));
    if (ret < 0)
    {
        APP_DEBUG_PRINT("Bind socket for MasterPeer failed.");
        mutexUnlock();
        return -1;
    }

    // 4. Start to listening for other sockets
    ret = listen(masterPeer->sockfd, MAX_CONNECTIONS);
    if (ret < 0)
    {
        APP_DEBUG_PRINT("Listen on MasterPeer failed");
        mutexUnlock();
        return -1;
    }

    mutexUnlock();

    APP_INFO_PRINT("Init socket for MasterPeer successfully.");

    return ret;   
}

int MasterPeer::addPeer(Peer peer)
{
    int ret = 0;
    
    mutexLock();

    peerList.push_back(peer);

    mutexUnlock();

    return ret;  
}
int MasterPeer::removePeer(Peer peer)
{
    int ret = 0;

    return ret;  
}
int MasterPeer::terminatePeer(unsigned int id)
{
    int ret = 0;

    return ret;  
}

void MasterPeer::listPeer(void)
{
    if (peerList.empty())
    {
        APP_DEBUG_PRINT("There is no peer on the list.");
        return;
    }
    for (const Peer &peer : peerList)
    {
        // print with what format ?
    }
}

int MasterPeer::mutexLock(void)
{
    return pthread_mutex_lock(&masterMutex);
}

int MasterPeer::mutexUnlock(void)
{
    return pthread_mutex_unlock(&masterMutex);
}

int MasterPeer::getSockFd(void)
{
    return masterPeer->sockfd;
}

pthread_t* MasterPeer::getListenerThreadID(void)
{
    return &listenerThread;
}

void* thd_listenForPeers(void* args)
{
    Peer new_peer = Peer();
    MasterPeer *masterPeer = MasterPeer::getInstance();

    while (1)
    {
        new_peer.sockfd = accept(masterPeer->getSockFd(), (struct sockaddr*)&new_peer.addr, &new_peer.addrSize);
        if (new_peer.sockfd < 0)
        {
            APP_DEBUG_PRINT("accept new peer socket failed. Continue to listen for a new socket...");
            continue;
        }

        // New peer has been accepted, now update the list.
        masterPeer->addPeer(new_peer);
    }
    return nullptr;
}