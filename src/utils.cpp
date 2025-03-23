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
        proc_name = "ERROR";
        return proc_name;
    }

    read_bytes = read(fd, name_buffer, 24);
    if (read_bytes < 0)
    {
        APP_DEBUG_PRINT("Cannot read file %s - Error number: %d", PROCESS_COMM_DIR, errno);
        proc_name = "ERROR";
        return proc_name;
    }

    name_buffer[read_bytes-1] = '\0';

    proc_name = name_buffer;

    close(fd);

    return proc_name;
}

const char* getThreadName(void)
{
    return "";
}

std::vector<std::string> readInput(void)
{
    char cmd_buff[50];

    std::vector<std::string> input;

    if(fgets(cmd_buff,sizeof(cmd_buff),stdin) != NULL)
    {
        //cmd_buff[strcspn(cmd_buff, "\n")] = 0;

        std::istringstream ss(cmd_buff);
        std::string token;
        int tokenIdx = 0;

        while (ss >> token && tokenIdx < 2)
        {
            input.push_back(token);
            tokenIdx++;
            if (tokenIdx == 2)
            {
                std::getline(ss, token);
                
                /* First character is ' ', so remove it */
                if (!token.empty())
                    token.erase(token.begin());
                
                input.push_back(token);
            }
        }
    }

    /* Print all tokens (for debugging)*/
    
    // APP_PRINT("\n");
    // for (std::string item : input)
    // {
    //     APP_DEBUG_PRINT("%s", item.c_str());
    // }

    return input;
}

void clearScreen(void)
{
    system("clear");
}