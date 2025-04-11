#ifndef _UTILS_HPP_
#define _UTILS_HPP_

#include "defines.hpp"

// this should be declared in Makefile/Cmake
#define ENABLE_INFO_PRINT   true
#define ENABLE_DEBUG_PRINT  true
#define ENABLE_ERROR_PRINT  true
#define ENABLE_DUMP_LOG     true    // not implement this mechanism yet.

#define APP_PRINT(fmt,...)          printf(fmt, ##__VA_ARGS__);

#define APP_INFO_PRINT(fmt,...)     do { \
                                        if (ENABLE_INFO_PRINT) { \
                                            printf("[info][%s][pid:%d][@%s]: " fmt, __func__, getPID(), getThreadName(), ##__VA_ARGS__); \
                                            printf("\n"); \
                                        } \
                                    } while(0)

#define APP_DEBUG_PRINT(fmt,...)    do { \
                                        if (ENABLE_DEBUG_PRINT) { \
                                            printf("[debug][%s][pid:%d[@%s]: " fmt, __func__, getPID(), getThreadName(), ##__VA_ARGS__); \
                                            printf("\n"); \
                                        } \
                                    } while(0)


#define APP_ERROR_PRINT(fmt,...)    do { \
                                        if (ENABLE_ERROR_PRINT) { \
                                            printf("[%s][pid:%d[@%s][ERROR: %d - %s]: " fmt, __func__, getPID(), getThreadName(), errno, strerror(errno), ##__VA_ARGS__); \
                                            printf("\n"); \
                                        } \
                                    } while(0)

#define UNUSED(X)   (void)(X)       /* To avoid gcc/g++ warnings */

std::vector<std::string> readInput(void);

void        clearScreen(void);

int         getPID(void);

const char* getComm(void);

const char* getThreadName(void);

void        errExit(const char* format, ...);

void        usageError(const char* format, ...);

void        outputError(int errNum, const char* format, va_list ap);

#endif // _UTILS_HPP_