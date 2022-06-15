#ifndef __UTIL_FSM_H
#define __UTIL_FSM_H

#include <stdlib.h>

/* we transition to new state based on current state and event */
typedef struct trans{
    void (*func_ptr_cur)(void);
    int evt;
    void (*func_ptr_new)(void);
} trans_t;

#endif