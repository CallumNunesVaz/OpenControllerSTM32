#ifndef __UTIL_FSM_H
#define __UTIL_FSM_H

#include <stdlib.h>

/* we transition to new state based on current state and event */
typedef struct trans{
    void (*func_ptr_cur)(void);
    int evt;
    void (*func_ptr_new)(void);
} trans_t;

typedef enum state_exec {
    STATE_EXEC_ENTRY,
    STATE_EXEC_RECUR,
    STATE_EXEC_EXIT,
    STATE_EXEC_CNT
} state_exec_class_t;

int init_trans(trans_t *t, int evt, void (*f_cur)(void), void (*f_new)(void));

#endif