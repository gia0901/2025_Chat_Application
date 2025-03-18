#include "defines.hpp"
#include "utils.hpp"
#include "MasterPeer.hpp"


MasterPeer *masterPeer;

int main(int argc, char* argv[])
{
    int ret = 0;

    // Get port number from environment parameter
    if (argc < 2)
    {
        APP_DEBUG_PRINT("Wrong syntax. Try: ./chat_app <port_num>");
        exit(EXIT_FAILURE);
    }
    int port = atoi(argv[1]);

    // Get Master Peer instance   
    masterPeer = MasterPeer::getInstance();
    
    // Init Master Peer
    ret = masterPeer->init(port);
    if (ret == -1)
    {
        APP_DEBUG_PRINT("Failed to init Master Peer");
        exit(EXIT_FAILURE);
    }



    APP_DEBUG_PRINT("Hello from chat app!");

    return 0;
}