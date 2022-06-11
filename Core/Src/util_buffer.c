/**

 */

#include "util_buffer.h"

int buf_init(buffer_t *p_buf, void *p_data, size_t p_len)
{
    /* Sanity check */
    if ((NULL == p_buf) || (NULL == p_data) || (0 == p_len)) {
        return EXIT_FAILURE;
    }

    /* link pointers */
    p_buf->data = p_data;
    p_buf->len = p_len;

    /* Reset buffer */
    buf_reset(p_buf);

    return EXIT_SUCCESS;
}

int buf_reset(buffer_t *p_buf)
{
    /* Sanity check */
    if (NULL == p_buf) {
        return EXIT_FAILURE;
    }

    /* Clear data in buffer */
    memset(p_buf->data, 0x00, p_buf->len);

    /* link pointers */
    p_buf->head = 0;
    p_buf->tail = 0;

    return EXIT_SUCCESS;
}

int buf_write(buffer_t *buf, void *data, size_t elem_cnt) {

    void *wr_start_ptr;
    size_t cnt_from_end;

    /* Sanity check */
    if ((NULL == buf) || (NULL == data) || (0 == elem_cnt)) {
        return EXIT_FAILURE;
    }

    /* Find pointer of starting write location */
    wr_start_ptr = buf->data + buf->head;
    /* find how far head is from the end */
    cnt_from_end = buf->len - buf->head;

    /* if total write is more than buffer length, only write last section */
    if (elem_cnt > buf->len) {
        /* increment pointer to last section */
        data += elem_cnt - buf->len;
        /* we will write entirety of last section */
        elem_cnt = buf->len;
    }

    /* Copy data */
    if (elem_cnt > cnt_from_end) {
        memcpy(wr_start_ptr, data, cnt_from_end);
        memcpy(buf->data, data, elem_cnt - cnt_from_end);
    } else {
        memcpy(wr_start_ptr, data, elem_cnt);
    }

    /* increment head pointer */
    buf->head += elem_cnt;
    buf->head %= buf->len;

    /* yay */
    return EXIT_SUCCESS;
}

int buf_read(buffer_t *buf, void *data, size_t elem_cnt) {

    /* Sanity check */
    if (EXIT_FAILURE == buf_peek(buf, data, elem_cnt)) {
        return EXIT_FAILURE;
    }

    /* increment head pointer */
    buf->tail += elem_cnt;
    buf->tail %= buf->len;

    /* yay */
    return EXIT_SUCCESS;
}

int buf_peek(buffer_t *buf, void *data, size_t elem_cnt) {

    void *rd_start_ptr;
    size_t cnt_from_end;

    /* Sanity check */
    if ((NULL == buf) || (NULL == data) || (0 == elem_cnt)) {
        return EXIT_FAILURE;
    }

    /* Find pointer of starting read location */
    rd_start_ptr = buf->data + buf->tail;
    /* find how far tail is from the end */
    cnt_from_end = buf->len - buf->tail;

    /* if total read is more than buffer length, only read unique bytes */
    if (elem_cnt > buf->len) {
        elem_cnt = buf->len;
    }

    /* Copy data */
    if (elem_cnt > cnt_from_end) {
        memcpy(data, rd_start_ptr, cnt_from_end);
        memcpy(data, buf->data, elem_cnt - cnt_from_end);
    } else {
        memcpy(data, rd_start_ptr, elem_cnt);
    }

    /* yay */
    return EXIT_SUCCESS;
}