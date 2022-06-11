#ifndef __UTIL_BUFFER_H
#define __UTIL_BUFFER_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct buffer
{
    uint16_t head;
    uint16_t tail;
    uint16_t len;
    void *data;
} buffer_t;

int buf_init(buffer_t *p_buf, void *p_data, size_t p_len);

int buf_reset(buffer_t *p_buf);

int buf_write(buffer_t *buf, void *data, size_t elem_cnt);

int buf_read(buffer_t *buf, void *data, size_t elem_cnt);

int buf_peek(buffer_t *buf, void *data, size_t elem_cnt);

#endif