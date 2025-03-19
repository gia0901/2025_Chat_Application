#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "utils.hpp"

// Return Process ID (PID)
int getPID(void)
{
    return getpid();
}


// Return Process Name
const char* getComm(void)
{
    int fd;
    int read_bytes;
    char name_buffer[24];
    const char* proc_name;

    fd = open(PROCESS_COMM_DIR, O_RDONLY);
    if (fd < 0)
    {
        APP_DEBUG_PRINT("Cannot open file %s - Error number: %d", PROCESS_COMM_DIR, errno);
        proc_name = "NAME_NOT_FOUND";
        return proc_name;
    }

    read_bytes = read(fd, name_buffer, 24);
    if (read_bytes < 0)
    {
        APP_DEBUG_PRINT("Cannot read file %s - Error number: %d", PROCESS_COMM_DIR, errno);
        proc_name = "NAME_NOT_FOUND";
        return proc_name;
    }

    name_buffer[read_bytes-1] = '\0';

    proc_name = name_buffer;

    close(fd);

    return proc_name;
}

void clearScreen(void)
{
    system("clear");
}