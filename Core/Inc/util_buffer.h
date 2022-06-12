#ifndef __UTIL_BUFFER_H
#define __UTIL_BUFFER_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef struct buffer
{
    size_t head;
    size_t tail;
    size_t len;
    size_t cnt;
    void *data;
} buffer_t;

int buf_init(buffer_t *p_buf, void *p_data, size_t p_len);

int buf_reset(buffer_t *p_buf);

int buf_write(buffer_t *buf, void *data, size_t elem_cnt);

int buf_read(buffer_t *buf, void *data, size_t elem_cnt);

int buf_peek(buffer_t *buf, void *data, size_t elem_cnt);

bool buf_is_empty(buffer_t *buf);

size_t buf_get_free_elem(buffer_t *buf);

size_t buf_get_free_data(buffer_t *buf, size_t size_of_data);

#endif