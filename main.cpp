#include "defines.hpp"
#include "utils.hpp"
#include "MasterPeer.hpp"
#include "app_api.hpp"

MasterPeer *masterPeer;
AppAPI appAPI;

int main(int argc, char* argv[])
{
    int ret = 0;
    std::vector<std::string> user_cmd;

    clearScreen();

    /* 0. Get port number from environment parameter */
    if (argc < 2) {
        APP_DEBUG_PRINT("Wrong syntax. Try: ./chat_app <port_num>");
        exit(EXIT_FAILURE);
    }
    int port = std::stoi(argv[1]);

    /* 1. Get Master Peer instance */  
    masterPeer = MasterPeer::getInstance();
    
    /* 2. Init MasterPeer Socket */
    ret = masterPeer->init(port);
    if (ret == -1) {
        APP_DEBUG_PRINT("Failed to init Master Peer");
        exit(EXIT_FAILURE);
    }

    /* 3. Init Listener thread for MasterPeer (listen for other peers wanting to connect) */
    ret = pthread_create(masterPeer->getListenerThreadID(), NULL, thd_listenForPeers, NULL);
    if (ret < 0) {
        APP_DEBUG_PRINT("Failed to create Listener Thread for MasterPeer.");
    }

    /* 4. Run Menu Application */
    appAPI.PrintMenu();

    /* 5. Handle User Requests */
    while(1) {
        APP_PRINT("\nEnter the command: ");

        // Get user request 
        user_cmd = readInput();

        // User communicate
        if (user_cmd[0] == "help") {
            appAPI.PrintMenu();
        }
        else if (user_cmd[0] == "connect") {
            if (user_cmd.size() == 3)
                masterPeer->connectToPeer(user_cmd[1], user_cmd[2]);
            else
                APP_PRINT("\nWrong input. Please try again!\n");
        }
        else if (user_cmd[0] == "send") {
            if (user_cmd.size() == 3)
                masterPeer->sendMessage(std::stoi(user_cmd[1]), user_cmd[2]);
            else
                APP_PRINT("\nWrong input. Please try again!\n");
        }
        else if (user_cmd[0] == "terminate") {
            ret = masterPeer->terminatePeer(std::stoi(user_cmd[1]));
        }
        else if (user_cmd[0] == "list") {
            masterPeer->listPeer();
        }
        else if (user_cmd[0] == "exit") {
            APP_PRINT("\nExiting app...\n");
            // Handle necessary actions before exit...
            return 0;
        }
        else {
            APP_PRINT("\nWrong command. Please try again!\n");
            continue;
        }

        if (ret < 0) {
            APP_PRINT("\nFailed to execute request!!!\n");
        }
    }

    return 0;
}