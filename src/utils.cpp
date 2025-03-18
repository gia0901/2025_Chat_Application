#include <fcntl.h>
#include <unistd.h>
#include "utils.hpp"


int getPID(void)
{
    return getpid();
}


const char* getComm(void)
{
    return "not implement yet";
}