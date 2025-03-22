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

#endif // _DEFINES_HPP_