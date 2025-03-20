#include "defines.hpp"
#include "utils.hpp"
#include "MasterPeer.hpp"


MasterPeer *masterPeer;

int main(int argc, char* argv[])
{
    clearScreen();

    int ret = 0;

    // 0. Get port number from environment parameter
    if (argc < 2)
    {
        APP_DEBUG_PRINT("Wrong syntax. Try: ./chat_app <port_num>");
        exit(EXIT_FAILURE);
    }
    int port = atoi(argv[1]);

    // 1. Get Master Peer instance   
    masterPeer = MasterPeer::getInstance();
    
    // 2. Init MasterPeer Socket
    ret = masterPeer->initSocket(port);
    if (ret == -1)
    {
        APP_DEBUG_PRINT("Failed to init Master Peer");
        exit(EXIT_FAILURE);
    }

    // 3. Init Listener thread for MasterPeer (listen for other peers wanting to connect)
    ret = pthread_create(masterPeer->getListenerThreadID(), NULL, thd_listenForPeers, NULL);
    if (ret < 0)
    {
        APP_DEBUG_PRINT("Failed to create Listener Thread for MasterPeer.");
    }


    // 4. Handle User Requests
    while (1)
    {

    }

    return 0;
}