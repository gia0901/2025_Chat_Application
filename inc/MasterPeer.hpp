#ifndef _MASTER_PEER_HPP_
#define _MASTER_PEER_HPP_

#include "Peer.hpp"

class MasterPeer
{
private:
    Peer* masterPeer;
    std::vector<Peer> peerList;
    static MasterPeer* pInstance;
    unsigned int totalPeers = 0;
    pthread_t listenerThread;
    pthread_mutex_t masterMutex;

public:
    MasterPeer();

    static MasterPeer* getInstance(void);

    int initSocket(int portNum);

    int addPeer(Peer peer);

    int removePeer(Peer peer);

    int terminatePeer(unsigned int id);

    int getMasterSockFd(void);

    /* Locking */
    int mutexLock(void);

    int mutexUnlock(void);

    /* Communicating */
    int sendMessage(int id, std::string msg);
    std::string receiveMessage(int id);

    void listPeer(void);

    pthread_t* getListenerThreadID(void);
};


void* thd_listenForPeers(void* args);

#endif // _MASTER_PEER_HPP_