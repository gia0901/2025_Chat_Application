#include "MasterPeer.hpp"
#include "utils.hpp"


#define IS_MASTER_PEER_CREATED()    do {                                                      \
                                        if (masterPeer == nullptr) {                          \
                                            masterPeer = new Peer();                          \
                                        }                                                     \
                                        else {                                                \
                                            APP_INFO_PRINT("masterPeer is already created."); \
                                        }                                                     \
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

int MasterPeer::sendMessage(int id, std::string msg)
{
    int ret = 0;

    mutexLock();

    ret = send(peerList[id].getSockFD(), msg.c_str(), msg.size(), 0);

    if (ret < 0)
    {
        APP_DEBUG_PRINT("Sent message to peer id[%d] failed.", id);
    }

    mutexUnlock();

    return ret;
}

int MasterPeer::receiveMessage(int id, std::string &msg)
{
    char readBuffer[MAX_MSG_SIZE];

    /* This is a pending function */
    ssize_t readBytes = recv(peerList[id].getSockFD(), readBuffer, MAX_MSG_SIZE, 0);

    if (readBytes < 0)
    {

    }
    else if (readBytes == 0)
    {

    }
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

int MasterPeer::getMasterSockFd(void)
{
    return masterPeer->getSockFD();
}

pthread_t* MasterPeer::getListenerThreadID(void)
{
    return &listenerThread;
}

std::vector<Peer> MasterPeer::getPeerList(void)
{
    return peerList;
}

void* thd_listenForPeers(void* args)
{
    Peer new_peer = Peer();
    MasterPeer *masterPeer = MasterPeer::getInstance();
    

    while (1)
    {
        int temp = 0;
        temp = (new_peer.acceptSocket(masterPeer->getMasterSockFd()));
        new_peer.setSockFD(temp);

        if (new_peer.getSockFD() < 0)
        {
            APP_DEBUG_PRINT("accept new peer socket failed. Continue to listen for a new socket...");
            continue;
        }

        // New peer has been accepted, now update the list.
        masterPeer->addPeer(new_peer);

        // Accept connection from new peer
        new_peer.acceptConnection(masterPeer->getPeerList());
    }



    return nullptr;
}