#include "defines.hpp"
#include "utils.hpp"
#include "MasterPeer.hpp"
#include "app_api.hpp"

MasterPeer *masterPeer;

int main(int argc, char* argv[])
{
    clearScreen();

    /* Local variable */
    int ret = 0;
    std::vector<std::string> user_cmd;

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

    /* 4. Run Menu Application */
    App_printMenu();

    /* 5. Handle User Requests */
    while(1)
    {
        APP_INFO_PRINT("Enter the command: ");

        // Get user request 
        user_cmd = readInput();

        // User communicate
        if(user_cmd[0] == "help")
        {

        }
        else if(user_cmd[0] == "connect")
        {

        }
        else if(user_cmd[0] == "send")
        {
            
        }
    }

    

    return 0;
}