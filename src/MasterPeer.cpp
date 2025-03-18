#include "MasterPeer.hpp"


MasterPeer::MasterPeer()
{

}

// Singleton
MasterPeer* MasterPeer::pInstance = nullptr;

MasterPeer* MasterPeer::getInstance(void)
{
    if (pInstance == nullptr)
        pInstance = new MasterPeer();
    return pInstance;
}

int MasterPeer::init(int portNum)
{
    int ret = 0;

    // 0. Init mutex
    masterMutex = PTHREAD_MUTEX_INITIALIZER;

    // 1. Init socket

    // 2.

    // 3. 

    return ret;   
}

int MasterPeer::addPeer(Peer peer)
{
    int ret = 0;

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

void MasterPeer::listPeer(void)
{

}

int MasterPeer::mutex_lock(void)
{
    return pthread_mutex_lock(&masterMutex);
}

int MasterPeer::mutex_unlock(void)
{
    return pthread_mutex_unlock(&masterMutex);
}

pthread_t* MasterPeer::getListenerThreadID(void)
{
    return &listenerThread;
}