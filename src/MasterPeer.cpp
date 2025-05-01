#include <unistd.h>
#include "MasterPeer.hpp"
#include "utils.hpp"

/**
 * @brief   Private macros
 */
#define IS_MASTER_PEER_CREATED()    do {                                                        \
                                        if (masterPeer == nullptr) {                            \
                                            masterPeer = new Peer();                            \
                                        }                                                       \
                                        else {                                                  \
                                            APP_INFO_PRINT("masterPeer is already created.");   \
                                        }                                                       \
                                    } while(0)

#define IS_PEER_VALID(id)           do { } while(0)

#define CHECK_MSG_LENGTH(msg)       do {                                                                                \
                                        if ((msg).length()+1 > MAX_MSG_SIZE) {                                          \
                                            APP_PRINT("Message size exceeds %d bytes. Aborting!!!\n", MAX_MSG_SIZE);    \
                                            return -1;                                                                  \
                                        }                                                                               \
                                    } while(0)
/**
 * @brief   Master Peer Constructor
 * @note    - Initialize Master Peer object
 *          - Initialize Mutex Lock
 * @retval  None
 */
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
    /* 1. Release Singleton */
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

/**
 * @brief   Singleton initialization 
 */
MasterPeer* MasterPeer::pInstance = nullptr;

MasterPeer* MasterPeer::getInstance(void)
{
    if (pInstance == nullptr)
        pInstance = new MasterPeer();

    return pInstance;
}

/**
 * @brief   MasterPeer socket initialization
 * @note    Initialize necessary settings to make MasterPeer ready to work
 * @param   portNum - port number of this process, where MasterPeer will listen on
 * @retval  0:PASS / -1: FAILED
 */
int MasterPeer::init(int portNum)
{
    int ret = 0;

    IS_MASTER_PEER_CREATED();
    
    mutexLock();

    /* 1. Init socket */ 
    masterPeer->setPortNum(portNum); /* Set port number */

    ret = masterPeer->initSocket();  /* Initialize socket */      
    if (ret < 0)
    {
        APP_ERROR_PRINT("Create socket for MasterPeer failed.");
        mutexUnlock();
        return -1;
    }
    
    /* 2. Init address structure */ 
    ret = masterPeer->initAddr(ADDR_TYPE_MASTER);
    if (ret < 0)
    {
        APP_DEBUG_PRINT("Failed to initialize address for MasterPeer.");
        mutexUnlock();
        return -1;
    }

    /* 3. Bind socket to the address */ 
    ret = masterPeer->bindSocket();
    if (ret < 0)
    {
        APP_ERROR_PRINT("Bind socket for MasterPeer failed.");
        mutexUnlock();
        return -1;
    }

    /* 4. Start to listening on Master Socket for other sockets */ 
    ret = masterPeer->listenSocket();
    if (ret < 0)
    {
        APP_ERROR_PRINT("Listen on MasterPeer failed");
        mutexUnlock();
        return -1;
    }

    mutexUnlock();

    APP_INFO_PRINT("Initialize MasterPeer successfully.");
    return ret;   
}

/**
 * @brief   Update current PeerList
 * @param   peer - New peer to be added
 * @retval  0: SUCCESS/ -1: FAILED
 */
int MasterPeer::updatePeerList(Peer peer)
{
    int ret = 0;
    
    mutexLock();
    peerList.push_back(peer);
    mutexUnlock();

    APP_INFO_PRINT("New peer id[%d] addr[%s] port[%d] has been added into the PeerList", peer.getID(), peer.getAddrInStr().c_str(), peer.getPortNum());
    return ret;  
}

/**
 * @brief   Remove a peer from the list
 * @param   id - id of the peer
 * @retval  0: SUCCESS / -1: FAILED
 */
int MasterPeer::removePeer(int id)
{
    int ret = 0;

    if (id < 0 || id > MAX_CONNECTIONS)
    {
        APP_ERROR_PRINT("ID[%d] is not in range!!!", id);
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

/**
 * @brief   Terminate a peer on the list
 * 
 * @note    There're some tasks must be done when terminating a peer:
 *              - Send a message to inform about the termination
 *              - Close child peer's socket
 *              - Kill its handler thread
 *              - Remove it from the list
 * 
 * @param   id - identity of the child peer
 * @retval  0:Pass / -1:Failed
 */
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

    /* Close child peer's socket. After closing, child peer recv() will return '0' */
    getChildPeerPtr(id)->closeSockFd();

    /* Terminate the handler thread */


    /* Remove it from the list */
    ret = removePeer(id);
    if (ret < 0)
    {
        APP_ERROR_PRINT("failed to remove peer id[%d] from the list.", id);
    }

    return ret;  
}

/**
 * @brief   Get a copy of a child peer through its ID
 * @param   id - identity of the child peer
 * @retval  Peer object
 */
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
 * @brief   Get the peer instance through its ID
 * @param   id - identity of the child peer
 * @retval  Peer* - pointer to the child peer
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
 * @brief   Send a message to a peer on the list
 * @param   id - identity of the target peer
 * @param   msg - message to be sent
 * @retval  0: SUCCESS / -1: FAILED
 */
int MasterPeer::sendMessage(int id, std::string msg)
{
    int ret = 0;

    CHECK_MSG_LENGTH(msg);

    APP_INFO_PRINT("id[%d] msg:%s", id, msg.c_str());

    Peer* targetPeer = getChildPeerPtr(id);

    APP_INFO_PRINT("Peer info: fd[%d] port[%d] id[%d] addr[%s]", targetPeer->getSockFD(), targetPeer->getPortNum(), targetPeer->getID(), targetPeer->getAddrInStr().c_str());

    ret = send(targetPeer->getSockFD(), (void*)msg.c_str(), (size_t)msg.size(), 0);
    if (ret < 0)
    {
        APP_ERROR_PRINT("failed to send message to Peer ID[%d] ret[%d]", id, ret);
    }

    return ret;
}

/**
 * @brief   List all the peers on the PeerList
 * @param   None
 * @retval  None
 */
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

/**
 * @brief   Lock mutex
 */
int MasterPeer::mutexLock(void)
{
    return pthread_mutex_lock(&masterMutex);
}

/**
 * @brief   Release mutex
 */
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

    Peer *targetPeer = new Peer();
    targetPeer->setAddrInStr(addr);
    targetPeer->setPortNum(port_num);

    /* 1. Initialize the socket */
    fd = targetPeer->initSocket();
    if (fd < 0)
    {
        APP_ERROR_PRINT("Failed to initialize socket for peer at addr[%s] port[%d]", addr.c_str(), port_num);
        return -1;
    }

    /* 2. Set up the address structure for target peer
          This is a manual approach, should refactor this...
    */
    // targetPeer->getAddrPtr()->sin_family = AF_INET;
    // targetPeer->getAddrPtr()->sin_port = htons(port_num);
    // if (inet_pton(AF_INET, addr.c_str(), &targetPeer->getAddrPtr()->sin_addr) < 0)
    // {
    //     APP_ERROR_PRINT("Invalid address/Address is not supported: %s", addr.c_str());
    //     return -1;
    // }
    ret = targetPeer->initAddr(ADDR_TYPE_CLIENT);
    if (ret < 0)
    {
        APP_DEBUG_PRINT("Failed to initialize address for peer[addr:%s][port:%d]", addr.c_str(), port_num);
    }

    
    APP_INFO_PRINT("Attempting to connect to peer at addr[%s] port[%d]", addr.c_str(), port_num);

    /* 3. Attempt to connect to the peer */
    ret = connect(targetPeer->getSockFD(), (SA*)targetPeer->getAddrPtr(), targetPeer->getAddrSize());
    if (ret < 0)
    {
        APP_ERROR_PRINT("Failed to connect to peer at addr[%s] port[%d]", addr.c_str(), port_num);
        return -1;
    }

    /* 4. Update new peer into the list */
    targetID = peerCounter;
    targetPeer->setID(targetID);

    APP_INFO_PRINT("New peer info: id[%d] port[%d] sockfd[%d] addr[%s]", targetPeer->getID(), targetPeer->getPortNum(), targetPeer->getSockFD(), targetPeer->getAddrInStr().c_str());

    updatePeerList(*targetPeer);
    
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
    else if (method == eRESET)
    {
        peerCounter = 0;
    }
    else
    {
        APP_DEBUG_PRINT("Invalid update method: %d", method);
    }
    mutexUnlock();
}


/*-------------------------------- Thread definitions -----------------------------------*/

void* thd_listenForPeers(void* args)
{
    Peer newPeer = Peer();
    MasterPeer *masterPeer = MasterPeer::getInstance();

    static int newPeerID;   /* ID should be available even when the loop ends, so that the thread can fetch its value */

    APP_INFO_PRINT("\nListener Thread is created, waiting for peers to connect");

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
        int new_sockfd = newPeer.acceptSocket(masterPeer->getMasterSockFd());
        if (new_sockfd < 0)
        {
            APP_ERROR_PRINT("accept new peer socket failed. Continue to listen for a new socket...");
            continue;
        }

        /* 2. New peer has been accepted. Update total peers */
        newPeerID = masterPeer->getPeerCounter();
        masterPeer->updatePeerCounter(eINCREMENT);
        
        /* 3. Convert port num from network-byte-order to integer for reading */
        int portNum = ntohs(newPeer.getAddr().sin_port);

        /* 4. Convert ipv4 into string for reading */
        char addrInStr[IPV4_ADDR_LENGTH];
        inet_ntop(AF_INET, &newPeer.getAddrPtr()->sin_addr, addrInStr, IPV4_ADDR_LENGTH);

        /* 5. Prepare the new peer and add it into the list */
        newPeer.setID(newPeerID);
        newPeer.setAddrInStr(std::string(addrInStr));
        newPeer.setPortNum(portNum);

        masterPeer->updatePeerList(newPeer);

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

    APP_ERROR_PRINT("Master Listener thread has broken!!!");
    return nullptr;
}

/**
 * @brief   Thread Handler waits for messages from an individual child peer
 * 
 * @note    There're several system calls for this functionality, 
 *          check: https://man7.org/linux/man-pages/man2/recv.2.html
 * 
 * @param   args - holds peer ID
 * 
 * @retval  nullptr - when loop breaks
 */
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

        if (readBytes == -1)  // Error occurs
        {
            // how to handle the error??
            APP_ERROR_PRINT("Error detected! Start to kill thread and exit...");
            masterPeer->terminatePeer(peerID);
            pthread_exit(NULL);
        }
        else if (readBytes == 0) // EOF detected: Child peer closes the connection (TCP stream socket).
        {
            APP_ERROR_PRINT("EOF detected! Peer ID[%d] has diconnected!", peerID);
            
            int cnt = 0;
            do {
                if (send(targetPeer->getSockFD(), CHECK_CONNECT_CODE, (size_t)sizeof(CHECK_CONNECT_CODE), 0) == -1) {
                    cnt++;
                }
                else {
                    break;
                }
            } while (cnt < 2);
            
            if (cnt >= 2) {
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
                APP_PRINT("\nPeer ID[%d] wants to disconnect...In some next loops we will recv() failed, then remove it from the list...\n", peerID);
                //masterPeer->removePeer(peerID);
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

/**
 * @brief   Thread that monitors every peer's connection in the list
 * @note    Should we need it?
 * @retval  NULL - when loop breaks
 */
void* thd_monitorPeer(void* args)
{

    while (1)
    {

    }
}