/**
 * @file hw_systick.c
 * @author your name (you@domain.com)
 * @brief m
 * @version 0.1
 * @date 2022-05-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "hw_systick.h"

void hw_systick_init(uint16_t tick_freq_hz){
    // update clocks first to properly use library
    SystemCoreClockUpdate();
    // Set up with system core clock from system_stm32f1xx.h
    SysTick_Config(SystemCoreClock / tick_freq_hz);
    // above function starts tick
    hw_systick_stop();
}

void hw_systick_stop(void){
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

void hw_systick_start(void){
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

void hw_systick_reset(void){
    SysTick->VAL = 0UL;  
}

void hw_systick_add_callback(void (*passed_func_ptr)(void)){
    // add function pointer to list and increment count
    func_ptrs[func_ptr_count++] = passed_func_ptr;
}

void hw_systick_clear_callbacks(void){
    // reset function pointer count to 0 avoid using callbacks
    func_ptr_count = 0;
}

void SysTick_Handler (void) {
    static uint8_t idx;
    // iterate through list of functions to execute them all
    for (idx = 0; idx <= func_ptr_count; idx++) {
        (*func_ptrs[idx])();
    }
}