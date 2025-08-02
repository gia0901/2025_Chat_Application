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
constexpr int MAX_CONNECTIONS   = 10;
constexpr int MAX_BACKLOGS      = MAX_CONNECTIONS;              /* Waiting queue (when accepting new connections) */
constexpr int MAX_MSG_SIZE      = 256;

constexpr int IPV4_ADDR_LENGTH  = 16;
constexpr int LOG_BUF_SIZE      = 500;

constexpr const char* PROCESS_COMM_DIR   = "/proc/self/comm";   /* Read this to get process name (Comm) */
constexpr const char* TERMINATE_CODE     = "0x69";              /* if a peer sends this code, means it wants to disconnect */
constexpr const char* CHECK_CONNECT_CODE = "0x70";              /* send this code to a peer to check is it still connected or not */
constexpr const char* DUMP_LOG_DIR       = "dump_log/";

/*-------------------- Typedef -----------------------------*/
using SA    = struct sockaddr;
using SA_IN = struct sockaddr_in;

/*-------------------- Structs and Enums -------------------*/
/**
 * @brief   Initialize address based on the kind of peer
 * @note    Refer to Peer::initAddr()
 */
enum class eAddrType {
    MASTER = 0,
    CLIENT = 1,
};

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
typedef struct {
    e_Request req;
    char data[MAX_MSG_SIZE];
} PeerMsg_t;

/**
 * @brief   Socket file descriptor information
 */
typedef struct SocketFD {
    int domain;  
    int type;
    int protocol;
} sfd_t;

#endif // _DEFINES_HPP_