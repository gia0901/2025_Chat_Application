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

    /* 1. Init socket */ 
    masterPeer->setPortNum(portNum); // Set port number
    ret = masterPeer->initSocket();  // Internet socket - Stream          
    
    if (ret < 0)
    {
        APP_DEBUG_PRINT("Create socket for MasterPeer failed.");
        mutexUnlock();
        return -1;
    }
    
    /*2. Init address structure */ 
    ret = masterPeer->initSocket(); // Socket will be binded to this, so that other apps can find and connect to the socket

    /* 3. Bind socket to the address */ 
    ret = masterPeer->bindSocket();

    if (ret < 0)
    {
        APP_DEBUG_PRINT("Bind socket for MasterPeer failed.");
        mutexUnlock();
        return -1;
    }

    /* 4. Start to listening for other sockets */ 
    ret = masterPeer->listenSocket();
    
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

int MasterPeer::updatePeerList(Peer peer)
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

Peer* MasterPeer::getChildPeer(int id)
{
    if (id < 0 || id >= totalPeers)
    {
        APP_DEBUG_PRINT("ID[%d] is not in range. Please check!", id);
        return nullptr;
    }
    return &peerList[id];
}

int MasterPeer::sendMessage(int id, std::string msg)
{
    int ret = 0;

    ret = send(peerList[id].getSockFD(), (void*)msg.c_str(), (size_t)msg.size(), 0);

    if (ret < 0)
    {
        APP_DEBUG_PRINT("Failed to send message to Peer ID[%d]", id);
    }
    return ret;
}

std::string MasterPeer::receiveMessage(int id)
{
    std::string result;
    int readBytes;
    char readBuff[MAX_MSG_SIZE];

    /* Pending function */
    readBytes = recv(peerList[id].getSockFD(), readBuff, MAX_MSG_SIZE, 0);
    if (readBytes < 0)
    {

    }
    else
    {
        result = readBuff;
    }
    return result;
}

void MasterPeer::listPeer(void)
{
    APP_PRINT("\n-------------- Peer List -------------\n");

    if (peerList.empty())
    {
        APP_DEBUG_PRINT("There is no peer on the list.");
        return;
    }
    else
    {
        for (Peer &peer : peerList)
        {
            APP_PRINT(" ID: %d | Addr: %s | Port: %d\n", peer.getID(), peer.getAddrInStr().c_str(), peer.getPortNum());
            APP_PRINT("--------------------------------------\n");
        }
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

int MasterPeer::getMasterSockFd(void)
{
    return masterPeer->getSockFD();
}

pthread_t* MasterPeer::getListenerThreadID(void)
{
    return &listenerThread;
}

int MasterPeer::connectToPeer(std::string addr, int portNum)
{
    int ret = 0;

    return ret;
}

void* thd_listenForPeers(void* args)
{
    Peer new_peer = Peer();
    MasterPeer *masterPeer = MasterPeer::getInstance();

    while (1)
    {
        int new_sockfd = new_peer.acceptSocket(masterPeer->getMasterSockFd());
        
        if (new_sockfd < 0)
        {
            APP_DEBUG_PRINT("accept new peer socket failed. Continue to listen for a new socket...");
            continue;
        }

        // New peer has been accepted.
        // Now update its information and add into the list
        


        masterPeer->updatePeerList(new_peer);
    }

    return nullptr;
}