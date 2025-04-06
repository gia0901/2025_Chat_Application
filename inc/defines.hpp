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
#define MAX_BACKLOGS        MAX_CONNECTIONS     // waiting queue (when accepting new connections)
#define MAX_MSG_SIZE        4096

#define IPV4_ADDR_LENGTH    16

#define PROCESS_COMM_DIR    "/proc/self/comm"   // Read this for Process Name (Comm)
#define TERMINATE_CODE      "0x69"              // if a peer sends this code, means it wants to disconnect

/* Typedef */
typedef struct sockaddr     SA;
typedef struct sockaddr_in  SA_IN;

#endif // _DEFINES_HPP_