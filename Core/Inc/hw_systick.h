/**
 * @file hw_systick.h
 * @author your name (you@domain.com)
 * @brief m
 * @version 0.1
 * @date 2022-05-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __HW_SYSTICK_H
#define __HW_SYSTICK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32f103xb.h"

#define FUNC_PTR_MAX_SIZE 32

static void *(*func_ptrs[FUNC_PTR_MAX_SIZE])(void);

static uint8_t func_ptr_count;

void hw_systick_init(uint16_t tick_freq_hz);

void hw_systick_stop(void);

void hw_systick_start(void);

void hw_systick_reset(void);

void hw_systick_add_callback(void (*passed_func_ptr)(void));

void hw_systick_clear_callbacks(void);

#ifdef __cplusplus
}
#endif

#endif