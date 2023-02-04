#ifndef LOG_H_
#define LOG_H_

#include <errno.h>
#include <stdlib.h>
#include <stdarg.h>

#include "return_val.h"

#define LOG_LEN 1024
#define MAX_LOG_LEVEL_TAG_LEN 16
#define LOG_LEVEL_PATH "logLevel.txt"

typedef enum {
    LOG_LEVEL_ERROR = 0,
    LOG_LEVEL_WARN,
    LOG_LEVEL_NOTICE,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG
} LogLevel;

// Citation: LOG and logMsg are inspired by
//      - https://stackoverflow.com/questions/8884335/print-the-file-name-line-number-and-function-name-of-a-calling-function-c-pro
//      - https://stackoverflow.com/questions/2849832/c-c-line-number
//      - https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
#define LOG(logLevel, format, ...) logMsg(logLevel, __FILE__, __LINE__, __func__, format __VA_OPT__(,) __VA_ARGS__);

#define DIE(format, ...) \
    LOG(LOG_LEVEL_ERROR, format __VA_OPT__(,) __VA_ARGS__); \
    exit(EXIT_FAILURE);

#define SYS_WARN(format, ...) \
    /* This can cause variable shadowing. */char err[LOG_LEN]; \
    snprintf(err, LOG_LEN, "%s (errno = %d)", __func__, errno); \
    if (getLogLevel() >= LOG_LEVEL_WARN) \
        perror(err); \
    LOG(LOG_LEVEL_WARN, format __VA_OPT__(,) __VA_ARGS__);

#define SYS_ERR(format, ...) \
    /* This can cause variable shadowing. */char err[LOG_LEN]; \
    snprintf(err, LOG_LEN, "%s (errno = %d)", __func__, errno); \
    if (getLogLevel() >= LOG_LEVEL_WARN) \
        perror(err); \
    LOG(LOG_LEVEL_ERROR, format __VA_OPT__(,) __VA_ARGS__);

#define SYS_DIE(format, ...) \
    SYS_ERR(format __VA_OPT__(,) __VA_ARGS__); \
    exit(EXIT_FAILURE);

#define FILE_OPEN_ERR(fileName, die) \
    assert(fileName != NULL); \
    if (die) { \
        SYS_DIE("Failed to open file '%s'.\n", fileName, errno); \
    } \
    else { \
        SYS_WARN("Failed to open file '%s'.\n", fileName, errno); \
    }

#define FILE_WRITE_ERR(fileName, die) \
    assert(fileName != NULL); \
    if (die) { \
        SYS_DIE("Failed to write to file '%s'.\n", fileName); \
    } \
    else { \
        SYS_WARN("Failed to write to file '%s'.\n", fileName); \
    }

#define FILE_READ_ERR(fileName, die) \
    assert(fileName != NULL); \
    if (die) { \
        SYS_DIE("Failed to read from file '%s'.\n", fileName); \
    } \
    else { \
        SYS_WARN("Failed to read from file '%s'.\n", fileName); \
    }

#define FILE_CLOSE_ERR(fileName, die) \
    assert(fileName != NULL); \
    if (die) { \
        SYS_DIE("Failed to close file '%s'.\n", fileName); \
    } \
    else { \
        SYS_WARN("Failed to close file '%s'.\n", fileName); \
    }

void logMsg(LogLevel logLevel, char* file, int line, const char* function, const char* format, ...);
int initLogLevel(void);
LogLevel getLogLevel(void);

#endif