#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include "defines.hpp"

// this should be declared in Makefile/Cmake
#define ENABLE_INFO_PRINT   true
#define ENABLE_DEBUG_PRINT  true

#define APP_PRINT                   printf
#define APP_INFO_PRINT(fmt,...)     do { \
                                        if (ENABLE_INFO_PRINT) { \
                                            printf("[info][%s]: " fmt, __func__, ##__VA_ARGS__); \
                                            printf("  [pid:%d][comm:%s]\n", getPID(), getComm()); \
                                        } \
                                    } while(0)

#define APP_DEBUG_PRINT(fmt,...)    do { \
                                        if (ENABLE_DEBUG_PRINT) { \
                                            printf("[debug][%s]: " fmt, __func__, ##__VA_ARGS__); \
                                            printf("  [pid:%d][comm:%s]\n", getPID(), getComm()); \
                                        } \
                                    } while(0)

std::vector<std::string> readInput(void);

void        clearScreen(void);

int         getPID(void);

const char* getComm(void);




#endif // _UTILS_HPP_