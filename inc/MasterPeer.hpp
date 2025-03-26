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
    MasterPeer(const MasterPeer& source) = delete;  /* Delete copy-constructor */

    ~MasterPeer();

    static MasterPeer* getInstance(void);

    int initSocket(int portNum);

    int updatePeerList(Peer peer);

    /* Remove a peer from vector */
    int removePeer(int id);

    /* Terminate a peer with its id from the list */
    int terminatePeer(unsigned int id);

    /* Get Master Socket file discriptor */
    int getMasterSockFd(void);

    /* Locking */
    int mutexLock(void);
    int mutexUnlock(void);

    /* Communicating */
    int sendMessage(int id, std::string msg);
    

    /* Connection */
    int connectToPeer(std::string addr, std::string portNum);

    /* Utils */
    int getTotalPeer(void);
    int* getTotalPeerPtr(void);

    void listPeer(void);

    pthread_t* getListenerThreadID(void);

    pthread_t* getReceiveMsgThreadID(int id);

    Peer getChildPeer(int id);
    Peer* getChildPeerPtr(int id);

};


void* thd_listenForPeers(void* args);

void* thd_receiveMsgFromPeer(void* args);

#endif // _MASTER_PEER_HPP_