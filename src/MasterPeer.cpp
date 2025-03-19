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
    
    // 1. Init socket
    masterPeer->portNum = portNum;
    masterPeer->sockfd = socket(AF_INET, SOCK_STREAM, 0);   // Internet socket - Stream
    if (masterPeer->sockfd < 0)
    {
        APP_DEBUG_PRINT("Create socket for MasterPeer failed.");
        return -1;
    }
    
    // 2. Init address structure 
    //    Socket will be binded to this, so that other apps can find and connect to the socket.
    masterPeer->addr.sin_family = AF_INET;
    masterPeer->addr.sin_addr.s_addr = INADDR_ANY;
    masterPeer->addr.sin_port = htons(masterPeer->portNum);


    // 3. Bind socket to the address
    ret = bind(masterPeer->sockfd, (struct sockaddr*)&masterPeer->addr, sizeof(masterPeer->addr));
    if (ret < 0)
    {
        APP_DEBUG_PRINT("Bind socket for MasterPeer failed.");
        return -1;
    }

    // 4. Start to listening for other sockets
    ret = listen(masterPeer->sockfd, MAX_CONNECTIONS);
    if (ret < 0)
    {
        APP_DEBUG_PRINT("Listen on MasterPeer failed");
        return -1;
    }

    APP_INFO_PRINT("Init socket for MasterPeer successfully.");

    return ret;   
}

int MasterPeer::addPeer(Peer peer)
{
    int ret = 0;

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

}

int MasterPeer::mutex_lock(void)
{
    return pthread_mutex_lock(&masterMutex);
}

int MasterPeer::mutex_unlock(void)
{
    return pthread_mutex_unlock(&masterMutex);
}

pthread_t* MasterPeer::getListenerThreadID(void)
{
    return &listenerThread;
}