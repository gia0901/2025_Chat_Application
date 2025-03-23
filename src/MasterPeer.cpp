#include <unistd.h>
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

    // 3. Init variables
    totalPeers = 0;

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
    masterPeer->initAddr(); // Socket will be binded to this, so that other apps can find and connect to the socket

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

    APP_INFO_PRINT("New peer id[%d] addr[%s] port[%d] has been added into the PeerList", peer.getID(), peer.getAddrInStr().c_str(), peer.getPortNum());

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
    if (id < 0 || id > MAX_CONNECTIONS)
    {
        APP_DEBUG_PRINT("ID[%d] is not in range. NULL is returning...", id);
        return nullptr;
    }
    return &peerList[id];
}

int MasterPeer::sendMessage(int id, std::string msg)
{
    int ret = 0;

    Peer* targetPeer = getChildPeer(id);

    ret = send(targetPeer->getSockFD(), (void*)msg.c_str(), (size_t)msg.size(), 0);

    if (ret < 0)
    {
        APP_DEBUG_PRINT("Failed to send message to Peer ID[%d]", id);
    }
    return ret;
}

void MasterPeer::listPeer(void)
{
    APP_PRINT("\n----------------- Peer List ----------------\n");

    if (peerList.empty())
    {
        APP_PRINT(" There is no peer on the list\n");
        return;
    }
    else
    {
        for (Peer &peer : peerList)
        {
            APP_PRINT(" ID: %d | Addr: %s | Port: %d\n", peer.getID(), peer.getAddrInStr().c_str(), peer.getPortNum());
        }
    }
    APP_PRINT("--------------------------------------------\n");
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

int MasterPeer::connectToPeer(std::string addr, std::string portNum)
{
    if (totalPeers > MAX_CONNECTIONS)
    {
        APP_PRINT("\nPeerList is already full. Exiting now...\n");
        return 0;
    }
    
    int ret = 0;
    int port_num = std::stoi(portNum);

    IS_MASTER_PEER_CREATED();

    Peer *target_peer = new Peer();
    target_peer->setAddrInStr(addr);
    target_peer->setPortNum(port_num);

    /* 1. Initialize the socket */
    ret = target_peer->initSocket();
    if (ret < 0)
    {
        APP_DEBUG_PRINT("Failed to initialize socket for peer at addr[%s] port[%d]", addr.c_str(), port_num);
        return -1;
    }

    /* 2. Set up the address structure for target peer */
    target_peer->getAddrPtr()->sin_family = AF_INET;
    target_peer->getAddrPtr()->sin_port = htons(port_num);

    if (inet_pton(AF_INET, addr.c_str(), &target_peer->getAddrPtr()->sin_addr) < 0)
    {
        APP_DEBUG_PRINT("Invalid address/ Address not supported: %s", addr.c_str());
        return -1;
    }

    APP_INFO_PRINT("Attempting to connect to peer at addr[%s] port[%d]", addr.c_str(), port_num);

    /* 3. Attempt to connect to the peer */
    ret = connect(target_peer->getSockFD(), (struct sockaddr*)target_peer->getAddrPtr(), target_peer->getAddrSize());
    if (ret < 0)
    {
        APP_DEBUG_PRINT("Failed to connect to peer at addr[%s] port[%d], error: %s", addr.c_str(), port_num, strerror(errno));
        return -1;
    }

    /* 4. Update new peer into the list */
    int *pTotalPeers = MasterPeer::getTotalPeerPtr();
    int targetID = *pTotalPeers;
    target_peer->setID(targetID);

    updatePeerList(*target_peer);
    (*pTotalPeers)++; // Update total peers

    /* 5. Create a thread to receive message from it. */
    ret = pthread_create(getReceiveMsgThreadID(targetID), NULL, thd_receiveMsgFromPeer, &targetID);
    if (ret < 0)
    {
        APP_DEBUG_PRINT("failed to create thread for receiving msg for peer id[%d]", targetID);
        return -1;
    }

    return ret;
}

pthread_t* MasterPeer::getReceiveMsgThreadID(int id)
{
    if (id < 0 || id > MAX_CONNECTIONS)
    {
        APP_DEBUG_PRINT("ID[%d] is not in range. NULL is returning...", id);
        return nullptr;
    }
    return &receiveMsgThread[id];
}

int MasterPeer::getTotalPeer(void)
{
    return totalPeers;
}

int* MasterPeer::getTotalPeerPtr(void)
{
    return &totalPeers;
}

void* thd_listenForPeers(void* args)
{
    Peer new_peer = Peer();
    MasterPeer *masterPeer = MasterPeer::getInstance();

    APP_INFO_PRINT("\nListener Thread is created, waiting for Peers to connect");

    while (1)
    {
        /* 0. If peer list is full, hanging and printing out every 10 seconds */
        if (masterPeer->getTotalPeer() > MAX_CONNECTIONS)
        {
            APP_PRINT("\nPeerList is already full...\n");
            sleep(10);
            continue;
        }

        /* 1. Wait & accept a new socket */
        int new_sockfd = new_peer.acceptSocket(masterPeer->getMasterSockFd());
        if (new_sockfd < 0)
        {
            APP_DEBUG_PRINT("accept new peer socket failed. Continue to listen for a new socket...");
            continue;
        }

        /* 2. New peer has been accepted. Update total peers */
        int *pTotalPeers = masterPeer->getTotalPeerPtr();
        int new_peer_id = *(pTotalPeers);
        *(pTotalPeers)++; // update total peers

        /* 3. Convert port num from network-byte-order to integer for reading */
        int portNum = ntohs(new_peer.getAddr().sin_port);

        /* 4. Convert ipv4 into string for reading */
        char addrInStr[IPV4_ADDR_LENGTH];
        inet_ntop(AF_INET, &new_peer.getAddrPtr()->sin_addr, addrInStr, IPV4_ADDR_LENGTH);

        /* 5. Prepare the new peer and add it into the list */
        new_peer.setID(new_peer_id);
        new_peer.setAddrInStr(std::string(addrInStr));
        new_peer.setPortNum(portNum);

        masterPeer->updatePeerList(new_peer);

        /* 6. Now new peer is a part of the list. Create a thread to receive message from it. */
        pthread_t *threadID = masterPeer->getReceiveMsgThreadID(new_peer_id);
        if (threadID == nullptr)
        {
            APP_DEBUG_PRINT("Cannot get Thread handler with Peer ID[%d]!!!", new_peer_id);
            break;
        }
        
        int ret = pthread_create(threadID, NULL, thd_receiveMsgFromPeer, &new_peer_id);
        if (ret < 0)
        {
            APP_DEBUG_PRINT("failed to create thd_receiveMsgFromPeer for id[%d]", new_peer_id);
        }
    }

    APP_DEBUG_PRINT("Master Listener thread failed!!!");
    return nullptr;
}

void* thd_receiveMsgFromPeer(void* args)
{
    int* p_PeerID = (int*)args;

    Peer* targetPeer = MasterPeer::getInstance()->getChildPeer(*p_PeerID);

    char readBuff[MAX_MSG_SIZE] = {0};

    while (1)
    {
        int readBytes = recv(targetPeer->getSockFD(), readBuff, MAX_MSG_SIZE, 0);

        if (readBytes < 0)  // Error
        {
            // add code here!!!
        }
        else if (readBytes == 0) // ???
        {
            // add code here!!!
        }
        else
        {
            APP_PRINT("\n----------------------------------------------------\n");
            APP_PRINT("- From addr[%s] - port[%d]\n", targetPeer->getAddrInStr().c_str(), targetPeer->getPortNum());
            APP_PRINT("- Message: %s\n", readBuff);
            APP_PRINT("----------------------------------------------------\n");
        }
        memset(readBuff, 0, MAX_MSG_SIZE);
    }

    APP_DEBUG_PRINT("Thread received msg for ID[%d] failed", *p_PeerID);

    return nullptr;
}