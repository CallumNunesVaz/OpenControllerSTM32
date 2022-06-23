#ifndef __UTIL_ERROR_H
#define __UTIL_ERROR_H

#ifndef DEBUG_EN
#define DEBUG_EN
#endif

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define RET_ON_FAIL(p)       \
    if (!p)                  \
    {                        \
        return EXIT_FAILURE; \
    }

#define RET_FAIL_LOG(p)         \
    if (!p)                     \
    {                           \
        dbg_log_generic_fail(); \
        return EXIT_FAILURE;    \
    }

/* Types of debug codes */
typedef enum DBG_CODE_TYPES
{
    DBG_TYPE_ERROR,
    DBG_TYPE_SUCCESS,
    DBG_TYPE_WARNING,
    DBG_TYPE_INFO,
    DBG_TYPE_CNT
} DBG_CODE_TYPE;

typedef enum DBG_CODE_CONTENTS
{
    DBG_CODE_INIT,
    DBG_CODE_OVERFLOW,
    DBG_CODE_TIMEOUT,
    DBG_CODE_DATA_RANGE,
    DBG_CODE_NO_DATA,
    DBG_CODE_CNT
} DBG_CODE_CONTENT;

/* debug code struct */
typedef struct dbg_code
{
    DBG_CODE_TYPE code;
    DBG_CODE_CONTENT content;
    char *lib_name;
    size_t lib_name_len;
} dbg_code_t;

int dbg_log_init(void);

void dbg_log(DBG_CODE_TYPE type, DBG_CODE_CONTENT content, const char *lib_name, const size_t lib_name_len);

void dbg_log_code(dbg_code_t code);

#endif