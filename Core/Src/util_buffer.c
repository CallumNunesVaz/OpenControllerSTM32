/**

 */

#include "util_buffer.h"

int buf_init(buffer_t *buf, void *data, size_t len)
{
    /* Sanity check */
    if ((NULL == buf) || (NULL == data) || (0 == len)) {
        return EXIT_FAILURE;
    }

    /* link pointers */
    buf->data = data;
    buf->len = len;

    /* Reset buffer */
    buf_reset(buf);

    return EXIT_SUCCESS;
}

int buf_reset(buffer_t *buf)
{
    /* Sanity check */
    if (NULL == buf) {
        return EXIT_FAILURE;
    }

    /* Clear data in buffer */
    memset(buf->data, 0x00, buf->len);

    /* link pointers */
    buf->head = 0;
    buf->tail = 0;
    buf->cnt = 0;

    return EXIT_SUCCESS;
}

int buf_write(buffer_t *buf, void *src_data, size_t elem_cnt) {

    void *wr_start_ptr;
    size_t cnt_from_end;

    /* Sanity check */
    if ((NULL == buf) || (NULL == src_data) || (0 == elem_cnt)) {
        return EXIT_FAILURE;
    }

    /* Find pointer of starting write location */
    wr_start_ptr = buf->data + buf->head;
    /* find how far head is from the end */
    cnt_from_end = buf->len - buf->head;

    /* if total write is more than buffer length, only write last section */
    if (elem_cnt > buf->len) {
        /* increment pointer to last section */
        src_data += elem_cnt - buf->len;
        /* we will write entirety of last section */
        elem_cnt = buf->len;
    }

    /* Copy data */
    if (elem_cnt > cnt_from_end) {
        memcpy(wr_start_ptr, src_data, cnt_from_end);
        memcpy(buf->data, src_data, elem_cnt - cnt_from_end);
    } else {
        memcpy(wr_start_ptr, src_data, elem_cnt);
    }

    /* increment head pointer and count */
    buf->head += elem_cnt;   
    buf->head %= buf->len;
    buf->cnt += elem_cnt;

    /* yay */
    return EXIT_SUCCESS;
}

int buf_read(buffer_t *buf, void *dest_data, size_t elem_cnt) {

    /* Sanity check */
    if (EXIT_FAILURE == buf_peek(buf, dest_data, elem_cnt)) {
        return EXIT_FAILURE;
    }

    /* increment tail pointer, lessen count */
    buf->tail += elem_cnt;
    buf->tail %= buf->len;
    buf->cnt -= elem_cnt;

    /* yay */
    return EXIT_SUCCESS;
}

int buf_peek(buffer_t *buf, void *dest_data, size_t elem_cnt) {

    void *rd_start_ptr;
    size_t cnt_from_end;

    /* Sanity check */
    if ((NULL == buf) || (NULL == dest_data) || (0 == elem_cnt)) {
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
        memcpy(dest_data, rd_start_ptr, cnt_from_end);
        memcpy(dest_data, buf->data, elem_cnt - cnt_from_end);
    } else {
        memcpy(dest_data, rd_start_ptr, elem_cnt);
    }

    /* yay */
    return EXIT_SUCCESS;
}

bool buf_is_empty(buffer_t *buf) {
    return (0 == buf->cnt);
}

size_t buf_get_free_elem(buffer_t *buf) {
    /* if overflow */
    if (buf->cnt > buf->len) {
        return 0;
    } else {
        return (buf->len - buf->cnt);
    }
}

size_t buf_get_free_data(buffer_t *buf, size_t size_of_data) {
    /* if overflow */
    if (buf->cnt > buf->len) {
        return 0;
    } else {
        return (buf->len - buf->cnt) / size_of_data;
    }
}