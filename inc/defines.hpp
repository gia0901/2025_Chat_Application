#ifndef _DEFINES_HPP_
#define _DEFINES_HPP_

#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>
#include <sstream>


#define MAX_CONNECTIONS     10
#define MAX_BACKLOGS        MAX_CONNECTIONS // ?? What is backlog
#define MAX_MSG_SIZE        256

#define PROCESS_COMM_DIR    "/proc/self/comm"   // Read this for Process Name (Comm)

#define IPV4_ADDR_LENGTH    16

#define TERMINATE_CODE      "0x69"  // if a peer sends this value, means it wants to disconnect

#endif // _DEFINES_HPP_