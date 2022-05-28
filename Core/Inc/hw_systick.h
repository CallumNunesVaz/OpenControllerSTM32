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

#define FUNC_PTR_ARRAY_MAX_LEN 32

#define SYS_TICK_MIN 1000

void hw_systick_init(uint16_t tick_freq_hz);

uint16_t hw_systick_get_freq(void);

void hw_systick_stop(void);

void hw_systick_start(void);

void hw_systick_reset(void);

void blocking_delay_ms(uint32_t delay_ms);

void hw_systick_add_callback(void (*passed_func_ptr)(void));

void hw_systick_remove_callback(void (*passed_func_ptr)(void));

void hw_systick_clear_callbacks(void);

#ifdef __cplusplus
}
#endif

#endif