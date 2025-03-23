#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include "defines.hpp"

// this should be declared in Makefile/Cmake
#define ENABLE_INFO_PRINT   true
#define ENABLE_DEBUG_PRINT  true

#define APP_PRINT                   printf
#define APP_INFO_PRINT(fmt,...)     do { \
                                        if (ENABLE_INFO_PRINT) { \
                                            printf("[info][%s][pid:%d proc:%s]: " fmt, __func__, getPID(), getComm(), ##__VA_ARGS__); \
                                            printf("\n"); \
                                        } \
                                    } while(0)

#define APP_DEBUG_PRINT(fmt,...)    do { \
                                        if (ENABLE_DEBUG_PRINT) { \
                                            printf("[debug][%s][pid:%d proc:%s]: " fmt, __func__, getPID(), getComm(), ##__VA_ARGS__); \
                                            printf("\n"); \
                                        } \
                                    } while(0)

std::vector<std::string> readInput(void);

void        clearScreen(void);

int         getPID(void);

const char* getComm(void);

const char* getThreadName(void);


#endif // _UTILS_HPP_