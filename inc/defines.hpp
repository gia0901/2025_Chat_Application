#ifndef _DEFINES_HPP_
#define _DEFINES_HPP_

/* C++ headers */
#include <string>
#include <vector>
#include <sstream>

/* C headers */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/prctl.h>

/* Definitions */
#define MAX_CONNECTIONS     10
#define MAX_BACKLOGS        MAX_CONNECTIONS     /* Waiting queue (when accepting new connections) */
#define MAX_MSG_SIZE        256

#define IPV4_ADDR_LENGTH    16

#define LOG_BUF_SIZE        500

#define PROCESS_COMM_DIR    "/proc/self/comm"   /* Read this to get process name (Comm) */
#define TERMINATE_CODE      "0x69"              /* if a peer sends this code, means it wants to disconnect */
#define CHECK_CONNECT_CODE  "0x70"              /* send this code to a peer to check is it still connected or not */
#define DUMP_LOG_DIR        "dump_log/"

/*-------------------- Typedef -----------------------------*/
typedef struct sockaddr     SA;
typedef struct sockaddr_in  SA_IN;

/*-------------------- Structs and Enums -------------------*/
/**
 * @brief   Initialize address based on the kind of peer
 * @note    Refer to Peer::initAddr()
 */
typedef enum
{
    ADDR_TYPE_MASTER = 0,
    ADDR_TYPE_CLIENT = 1,
} e_AddrType;

/**
 * @brief   Request of message
 */
typedef enum
{
    REQ_CHECK_CONNECTION_STATUS = 0,
    REQ_SEND_MSG = 1,
    REQ_INFORM_TERMINATION = 2, // not used yet
    REQ_ACKING_MSG = 3, // not used yet
    REQ_MAX,
} e_Request;

/**
 * @brief   Message structure for Peer communication  
 */
typedef struct
{
    e_Request req;
    char data[MAX_MSG_SIZE];
} PeerMsg_t;


#endif // _DEFINES_HPP_