#ifndef _DEFINES_HPP_
#define _DEFINES_HPP_

#include <stdio.h>
#include <string>
#include <vector>

#define MAX_CONNECTIONS     10
#define MAX_BACKLOGS        MAX_CONNECTIONS // ?? What is backlog
#define MAX_MSG_SIZE        256

#define PROCESS_COMM_DIR    "/proc/self/comm"   // Read this for Process Name (Comm)


#endif // _DEFINES_HPP_