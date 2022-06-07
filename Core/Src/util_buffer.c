/**

 */

//#include "util_buffer.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define BUF_LEN 256

typedef struct buffer
{
    void *data;
    void (*callback_func)(void);
    uint16_t head;
    uint16_t tail;
    uint16_t len;
    bool callback_on_push;
    bool callback_on_pop;
} buffer_t;

int init_buf(buffer_t *p_buf, void *p_data[], uint16_t p_len)
{
    if ((NULL == p_buf) || (NULL == p_data) || (0 == p_len)) {
        return EXIT_FAILURE;
    }

    /* Clear data */
    memset(p_data, 0x00, sizeof(p_buf));

    /* Assign given data array to buffer struct, set length, and defaults */
    p_buf->data = p_data;
    p_buf->len = p_len;
    p_buf->callback_func = NULL;
    p_buf->callback_on_push = false;
    p_buf->callback_on_pop = false;

    return EXIT_SUCCESS;
}

void buf_push(buffer_t *p_buf, void *data) {
    /* Add data */
    p_buf->data[++(p_buf->head)] = *data;
    /* Callback */
    if ((p_buf->callback_on_push) && (NULL != (p_buf->callback_func))) {
        (p_buf->callback_func)();
    }
}

void buf_set_callback(buffer_t *p_buf, void (*passed_func_ptr)(void))
{
    if (NULL != passed_func_ptr) {
        p_buf->callback_func = passed_func_ptr;
    }
}

void buf_en_callback_push(buffer_t *p_buf, bool p_bool)
{
    p_buf->callback_on_push = p_bool;
}

void buf_en_callback_pop(buffer_t *p_buf, bool p_bool)
{
    p_buf->callback_on_pop = p_bool;
}
