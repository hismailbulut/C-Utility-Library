#pragma once

#ifdef _DEBUG

#include <stdarg.h>
#include <stdio.h>

#define DEBUG_LOG_INFO(message, args...)  \
    {                                     \
        fprintf(stdout, "    [INFO]:");   \
        fprintf(stdout, message, ##args); \
        fprintf(stdout, "\n");            \
    }

#define DEBUG_LOG_WARN(message, args...)   \
    {                                      \
        fprintf(stdout, "    [WARNING]:"); \
        fprintf(stdout, message, ##args);  \
        fprintf(stdout, "\n");             \
    }

#define DEBUG_LOG_ERROR(message, args...) \
    {                                     \
        fprintf(stdout, "    [ERROR]:");  \
        fprintf(stdout, message, ##args); \
        fprintf(stdout, "\n");            \
    }
#define DEBUG_LOG_FATAL(message, args...) \
    {                                     \
        fprintf(stdout, "    [FATAL]:");  \
        fprintf(stdout, message, ##args); \
        fprintf(stdout, "\n");            \
        exit(EXIT_FAILURE);               \
    }

#define BREAK()                                \
    {                                          \
        fprintf(stdout,                        \
                "\
BREAK IN CODE:\n\
         FILE: %s\n\
         FUNC: %s\n\
         LINE: %d\n",                          \
                __FILE__, __func__, __LINE__); \
        __debugbreak();                        \
    }

#define BREAK_MSG(message, args...)            \
    {                                          \
        fprintf(stdout,                        \
                "\
BREAK IN CODE:\n\
         FILE: %s\n\
         FUNC: %s\n\
         LINE: %d\n\
      MESSAGE: ",                              \
                __FILE__, __func__, __LINE__); \
        fprintf(stdout, message, ##args);      \
        fprintf(stdout, "\n");                 \
        __debugbreak();                        \
    }

#define ASSERT_BREAK(cond)                                \
    {                                                     \
        if (!(cond)) {                                    \
            fprintf(stdout,                               \
                    "\
ASSERTION FAILED: %s\n\
            FILE: %s\n\
            FUNC: %s\n\
            LINE: %d\n",                                  \
                    #cond, __FILE__, __func__, __LINE__); \
            __debugbreak();                               \
        }                                                 \
    }

#define ASSERT_BREAK_MSG(cond, message, args...)          \
    {                                                     \
        if (!(cond)) {                                    \
            fprintf(stdout,                               \
                    "\
ASSERTION FAILED: %s\n\
            FILE: %s\n\
            FUNC: %s\n\
            LINE: %d\n\
         MESSAGE: ",                                      \
                    #cond, __FILE__, __func__, __LINE__); \
            fprintf(stdout, message, ##args);             \
            fprintf(stdout, "\n");                        \
            __debugbreak();                               \
        }                                                 \
    }

#else

#define DEBUG_LOG_INFO(message, args...)
#define DEBUG_LOG_WARN(message, args...)
#define DEBUG_LOG_ERROR(message, args...)
#define BREAK()
#define BREAK_MSG(message, args...)
#define ASSERT_BREAK(cond)
#define ASSERT_BREAK_MSG(cond, message, args...)

#endif
