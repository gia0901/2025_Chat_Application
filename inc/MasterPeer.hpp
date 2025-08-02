#ifndef _MASTER_PEER_HPP_
#define _MASTER_PEER_HPP_

#include "Peer.hpp"

typedef enum {
    eINCREMENT = 0,
    eDECREMENT = 1,
    eRESET     = 2,
} e_UpdatePeerCounter;

class MasterPeer {
private:
    Peer* masterPeer;
    std::vector<Peer> peerList;
    static MasterPeer* pInstance;
    int peerCounter;
    pthread_mutex_t masterMutex;
    pthread_t listenerThread;
    pthread_t peerHandlerThread[MAX_CONNECTIONS+1];

public:
    MasterPeer();
    MasterPeer(const MasterPeer& source) = delete;  /* (Singleton) Delete copy-constructor */
    ~MasterPeer();

    static MasterPeer* getInstance(void);

    int init(int portNum);

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
    int getPeerCounter(void);
    int* getPeerCounterPtr(void);
    void updatePeerCounter(e_UpdatePeerCounter method);

    void listPeer(void);

    pthread_t* getListenerThreadID(void);
    pthread_t* getPeerHandlerThreadID(int id);

    Peer getChildPeer(int id);
    Peer* getChildPeerPtr(int id);
};


void* thd_listenForPeers(void* args);

void* thd_handlePeer(void* args);

void* thd_monitorPeer(void* args);

#endif // _MASTER_PEER_HPP_