#ifndef _MASTER_PEER_HPP_
#define _MASTER_PEER_HPP_

#include "Peer.hpp"

class MasterPeer
{
private:
    Peer* masterPeer;
    std::vector<Peer> peerList;
    static MasterPeer* pInstance;
    int totalPeers = 0;
    pthread_t listenerThread;
    pthread_mutex_t masterMutex;

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
    int connectToPeer(std::string addr, int portNum);

    /* Utils */
    void listPeer(void);

    pthread_t* getListenerThreadID(void);

    Peer* getChildPeer(int id);

};


void* thd_listenForPeers(void* args);

#endif // _MASTER_PEER_HPP_