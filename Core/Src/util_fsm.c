#include "util_fsm.h"

int init_trans(trans_t *t, const int evt, void (*f_cur)(void), void (*f_new)(void)) {
    /* check */
    if ((NULL == t) || (NULL == f_cur) || (NULL == f_new)) {
        return EXIT_FAILURE;
    }

    /* assign */
    t->evt = evt;
    t->func_ptr_cur = f_cur;
    t->func_ptr_new = f_new;

    /* send it */
    return EXIT_SUCCESS;
}