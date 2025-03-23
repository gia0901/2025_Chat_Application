#ifndef _MASTER_PEER_HPP_
#define _MASTER_PEER_HPP_

#include "Peer.hpp"

class MasterPeer
{
private:
    Peer* masterPeer;
    std::vector<Peer> peerList;
    static MasterPeer* pInstance;
    int totalPeers;
    pthread_t listenerThread;
    pthread_mutex_t masterMutex;
    pthread_t receiveMsgThread[MAX_CONNECTIONS];

public:
    MasterPeer();

    static MasterPeer* getInstance(void);

    int initSocket(int portNum);

    int updatePeerList(Peer peer);

    int removePeer(Peer peer);

    int terminatePeer(unsigned int id);

    int getMasterSockFd(void);

    /* Locking */
    int mutexLock(void);

    int mutexUnlock(void);

    /* Communicating */
    int sendMessage(int id, std::string msg);
    std::string receiveMessage(int id);


    /* Connection */
    int connectToPeer(std::string addr, std::string portNum);

    /* Utils */
    int getTotalPeer(void);
    int* getTotalPeerPtr(void);

    void listPeer(void);

    pthread_t* getListenerThreadID(void);

    pthread_t* getReceiveMsgThreadID(int id);

    Peer* getChildPeer(int id);

};


void* thd_listenForPeers(void* args);

void* thd_receiveMsgFromPeer(void* args);

#endif // _MASTER_PEER_HPP_