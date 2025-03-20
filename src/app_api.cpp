#include "MasterPeer.hpp"
#include "app_api.hpp"
#include "utils.hpp"




void App_printMenu(void)
{
    APP_PRINT("\n******************************** Chat Application ********************************\n");
    APP_PRINT("\nUse the commands below:\n");
    APP_PRINT("1. help                             : display user interface options\n");
    APP_PRINT("2. myip                             : display IP address of this app\n");
    APP_PRINT("3. myport                           : display listening port of this app\n");
    APP_PRINT("4. connect <destination> <port no>  : connect to the app of another computer\n");
    APP_PRINT("5. list                             : list all the connections of this app\n");
    APP_PRINT("6. terminate <connection id>        : terminate a connection\n");
    APP_PRINT("7. send <connection id> <message>   : send a message to a connection\n");
    APP_PRINT("8. exit                             : close all connections & terminate this app\n");  
    APP_PRINT("\n**********************************************************************************\n");
}