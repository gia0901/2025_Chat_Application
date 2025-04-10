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

#define IS_PEER_LIST_FULL()         do { \
                                    } while(0)

MasterPeer::MasterPeer(void)
{
    /* 1. Allocate Master Peer */
    if (masterPeer == nullptr)
        masterPeer = new Peer();
    
    /* 2. Init Mutex */
    masterMutex = PTHREAD_MUTEX_INITIALIZER;

    /* 3. Init variables */
    peerCounter = 0;
}

MasterPeer::~MasterPeer(void)
{
    /* 1. Release Singleton instance */
    if (pInstance != nullptr)
    {
        delete pInstance;
        pInstance = nullptr;
    }
        
    /* 2. Release Master Peer */
    if (masterPeer != nullptr)
    {
        delete masterPeer;
        masterPeer = nullptr;
    }
}

// Singleton
MasterPeer* MasterPeer::pInstance = nullptr;

MasterPeer* MasterPeer::getInstance(void)
{
    if (pInstance == nullptr)
        pInstance = new MasterPeer();

    return pInstance;
}

/**
 * @brief: initialize MasterPeer socket
 */
int MasterPeer::init(int portNum)
{
    int ret = 0;

    IS_MASTER_PEER_CREATED();
    
    mutexLock();

    /* 1. Init socket */ 
    masterPeer->setPortNum(portNum); // Set port number
    ret = masterPeer->initSocket();  // Internet socket - Stream          
    
    if (ret < 0)
    {
        APP_ERROR_PRINT("Create socket for MasterPeer failed.");
        mutexUnlock();
        return -1;
    }
    
    /*2. Init address structure */ 
    masterPeer->initAddr(); // Socket will be binded to this, so that other apps can find and connect to the socket

    /* 3. Bind socket to the address */ 
    ret = masterPeer->bindSocket();

    if (ret < 0)
    {
        APP_ERROR_PRINT("Bind socket for MasterPeer failed.");
        mutexUnlock();
        return -1;
    }

    /* 4. Start to listening for other sockets */ 
    ret = masterPeer->listenSocket();
    
    if (ret < 0)
    {
        APP_ERROR_PRINT("Listen on MasterPeer failed");
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

int MasterPeer::removePeer(int id)
{
    int ret = 0;

    if (id < 0 || id > MAX_CONNECTIONS)
    {
        APP_ERROR_PRINT("ID[%d] is not in range.", id);
        return -1;
    }

    mutexLock();

    /* 1. Close connection (socket fd) of child peer */
    Peer* childPeer = getChildPeerPtr(id);
    childPeer->closeSockFd();

    /* 2. Remove it from peer list */
    std::vector<Peer>::iterator it = peerList.begin()+id;

    peerList.erase(it);

    mutexUnlock();

    return ret;  
}

int MasterPeer::terminatePeer(unsigned int id)
{
    int ret = 0;

    /* Send terminate code to the peer through its ID */
    ret = sendMessage(id, std::string(TERMINATE_CODE));
    if (ret < 0)
    {
        APP_ERROR_PRINT("failed to send terminate code to peer id[%d]", id);
        return -1;
    }

    /* Terminate the handler thread */

    /* Remove it from the list */
    ret = removePeer(id);

    if (ret < 0)
    {
        APP_ERROR_PRINT("failed to remove peer id[%d] from the list.", id);
    }

    return ret;  
}

Peer MasterPeer::getChildPeer(int id)
{
    if (id < 0 || id > MAX_CONNECTIONS)
    {
        APP_ERROR_PRINT("ID[%d] is not in range. NULL is returning...", id);
        return Peer();
    }
    return peerList[id];
}

/**
 * @brief: get the peer instance through its ID
 */
Peer* MasterPeer::getChildPeerPtr(int id)
{
    if (id < 0 || id > MAX_CONNECTIONS)
    {
        APP_ERROR_PRINT("ID[%d] is not in range. NULL is returning...", id);
        return nullptr;
    }
    return &peerList[id];
}

/**
 * @brief: send a message to a peer on the list
 */
int MasterPeer::sendMessage(int id, std::string msg)
{
    int ret = 0;

    APP_INFO_PRINT("id[%d] msg:%s", id, msg.c_str());

    Peer* targetPeer = getChildPeerPtr(id);

    // debug
    APP_INFO_PRINT("Peer info: fd[%d] port[%d] id[%d] addr[%s]", targetPeer->getSockFD(), targetPeer->getPortNum(), targetPeer->getID(), targetPeer->getAddrInStr().c_str());

    ret = send(targetPeer->getSockFD(), (void*)msg.c_str(), (size_t)msg.size(), 0);

    if (ret < 0)
    {
        APP_ERROR_PRINT("failed to send message to Peer ID[%d] ret[%d]", id, ret);
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
    if (peerCounter > MAX_CONNECTIONS)
    {
        APP_PRINT("\nPeerList is already full. Exiting now...\n");
        return 0;
    }
    
    int fd = -1;
    int ret = 0;
    int port_num = std::stoi(portNum);
    static int targetID;

    IS_MASTER_PEER_CREATED();

    Peer *target_peer = new Peer();
    target_peer->setAddrInStr(addr);
    target_peer->setPortNum(port_num);

    /* 1. Initialize the socket */
    fd = target_peer->initSocket();
    if (fd < 0)
    {
        APP_ERROR_PRINT("Failed to initialize socket for peer at addr[%s] port[%d]", addr.c_str(), port_num);
        return -1;
    }

    /* 2. Set up the address structure for target peer */
    target_peer->getAddrPtr()->sin_family = AF_INET;
    target_peer->getAddrPtr()->sin_port = htons(port_num);

    if (inet_pton(AF_INET, addr.c_str(), &target_peer->getAddrPtr()->sin_addr) < 0)
    {
        APP_ERROR_PRINT("Invalid address/ Address not supported: %s", addr.c_str());
        return -1;
    }

    APP_INFO_PRINT("Attempting to connect to peer at addr[%s] port[%d]", addr.c_str(), port_num);

    /* 3. Attempt to connect to the peer */
    ret = connect(target_peer->getSockFD(), (SA*)target_peer->getAddrPtr(), target_peer->getAddrSize());
    if (ret < 0)
    {
        APP_ERROR_PRINT("Failed to connect to peer at addr[%s] port[%d], error: %s", addr.c_str(), port_num, strerror(errno));
        return -1;
    }

    /* 4. Update new peer into the list */
    targetID = peerCounter;
    target_peer->setID(targetID);

    APP_INFO_PRINT("New peer info: id[%d] port[%d] sockfd[%d] addr[%s]", target_peer->getID(), target_peer->getPortNum(), target_peer->getSockFD(), target_peer->getAddrInStr().c_str());

    updatePeerList(*target_peer);
    
    /* 5. Create a thread to receive message from it. */
    ret = pthread_create(getPeerHandlerThreadID(targetID), NULL, thd_handlePeer, &targetID);
    if (ret < 0)
    {
        APP_ERROR_PRINT("failed to create thread for receiving msg for peer id[%d]", targetID);
        return -1;
    }

    updatePeerCounter(eINCREMENT); // Update total peers

    return ret;
}

pthread_t* MasterPeer::getPeerHandlerThreadID(int id)
{
    if (id < 0 || id > MAX_CONNECTIONS)
    {
        APP_ERROR_PRINT("ID[%d] is not in range. NULL is returning...", id);
        return nullptr;
    }
    return &peerHandlerThread[id];
}

int MasterPeer::getPeerCounter(void)
{
    return peerCounter;
}

int* MasterPeer::getPeerCounterPtr(void)
{
    return &peerCounter;
}

void MasterPeer::updatePeerCounter(e_UpdatePeerCounter method)
{
    mutexLock();
    if (method == eINCREMENT)
    {
        peerCounter++;
    }
    else if (method == eDECREMENT)
    {
        if (peerCounter > 0)
            peerCounter--;
        else
            peerCounter = 0;
    }
    else
    {
        peerCounter = 0;
    }
    mutexUnlock();
}

void* thd_listenForPeers(void* args)
{
    Peer new_peer = Peer();
    MasterPeer *masterPeer = MasterPeer::getInstance();

    APP_INFO_PRINT("\nListener Thread is created, waiting for Peers to connect");

    while (1)
    {
        /* 0. If peer list is full, hanging and printing out every 10 seconds */
        if (masterPeer->getPeerCounter() > MAX_CONNECTIONS)
        {
            APP_PRINT("\nPeerList is already full...\n");
            sleep(10);
            continue;
        }

        /* 1. Wait & accept a new socket */
        int new_sockfd = new_peer.acceptSocket(masterPeer->getMasterSockFd());
        if (new_sockfd < 0)
        {
            APP_ERROR_PRINT("accept new peer socket failed. Continue to listen for a new socket...");
            continue;
        }

        /* 2. New peer has been accepted. Update total peers */
        int newPeerID = masterPeer->getPeerCounter();
        masterPeer->updatePeerCounter(eINCREMENT);
        
        /* 3. Convert port num from network-byte-order to integer for reading */
        int portNum = ntohs(new_peer.getAddr().sin_port);

        /* 4. Convert ipv4 into string for reading */
        char addrInStr[IPV4_ADDR_LENGTH];
        inet_ntop(AF_INET, &new_peer.getAddrPtr()->sin_addr, addrInStr, IPV4_ADDR_LENGTH);

        /* 5. Prepare the new peer and add it into the list */
        new_peer.setID(newPeerID);
        new_peer.setAddrInStr(std::string(addrInStr));
        new_peer.setPortNum(portNum);

        masterPeer->updatePeerList(new_peer);

        /* 6. Now new peer is a part of the list. Create a thread to receive message from it. */
        pthread_t* threadID = masterPeer->getPeerHandlerThreadID(newPeerID);
        if (threadID == nullptr)
        {
            APP_ERROR_PRINT("Cannot get Thread handler with Peer ID[%d]!!!", newPeerID);
            break;
        }
        
        int ret = pthread_create(threadID, NULL, thd_handlePeer, &newPeerID);
        if (ret < 0)
        {
            APP_ERROR_PRINT("failed to create thd_handlePeer for id[%d]", newPeerID);
        }
    }

    APP_ERROR_PRINT("Master Listener thread failed!!!");
    return nullptr;
}

void* thd_handlePeer(void* args)
{
    int peerID = *((int*)args);

    MasterPeer* masterPeer = MasterPeer::getInstance();

    Peer* targetPeer = masterPeer->getChildPeerPtr(peerID);

    char readBuff[MAX_MSG_SIZE] = {0};

    APP_INFO_PRINT("Peer info: id[%d] port[%d] sockfd[%d] addr[%s]", targetPeer->getID(), targetPeer->getPortNum(), targetPeer->getSockFD(), targetPeer->getAddrInStr().c_str());

    while (1)
    {
        int readBytes = recv(targetPeer->getSockFD(), readBuff, MAX_MSG_SIZE, 0);

        if (readBytes < 0)  // Error
        {
            // add code here!!!
            APP_ERROR_PRINT("Error detected!");
        }
        else if (readBytes == 0) // ???
        {
            // EOF detected:
            // 1. Everything has been read.
            // 2. A peer has disconnected without informing
            APP_ERROR_PRINT("EOF detected! Re-checking connection with peer ID[%d]", peerID);
            
            int cnt = 0;
            do {
                if (send(targetPeer->getSockFD(), CHECK_CONNECT_CODE, (size_t)sizeof(CHECK_CONNECT_CODE), 0) == -1) {
                    cnt++;
                }
                else {
                    break;
                }
            } while (cnt < 5);
            
            if (cnt == 5) {
                APP_ERROR_PRINT("peer ID[%d] can't be connected, remove peer & destroy thread!!!", peerID);
                masterPeer->removePeer(peerID);
                break;
            }
            else {
                APP_DEBUG_PRINT("peer ID[%d] is still working! Thread is resuming...", peerID);
                continue;
            }
        }
        else
        {
            if (!strcmp(readBuff, TERMINATE_CODE))
            {
                /* Child peer wants to disconnect. It will do the cleaning in its side. Here we should clear our side only */
                masterPeer->removePeer(peerID);
            }
            else if (!strcmp(readBuff, CHECK_CONNECT_CODE))
            {
                /* Target peer wants to check current connection */
                // what should we do here?
            }
            
            APP_PRINT("\n----------------------------------------------------\n");
            APP_PRINT("- From addr[%s] - port[%d]\n", targetPeer->getAddrInStr().c_str(), targetPeer->getPortNum());
            APP_PRINT("- Message: %s\n", readBuff);
            APP_PRINT("----------------------------------------------------\n");
        }
        memset(readBuff, 0, MAX_MSG_SIZE);
    }

    APP_DEBUG_PRINT("Thread received msg for ID[%d] failed", peerID);

    return nullptr;
}