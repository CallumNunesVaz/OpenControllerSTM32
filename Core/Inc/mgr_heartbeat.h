/**
 * @file mgr_heartbeat.h
 * @author your name (you@domain.com)
 * @brief manager for the hearbeat LED. Assumed active high LED
 * @version 0.1
 * @date 2022-05-05
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef __MGR_HEARTBEAT_H
#define __MGR_HEARTBEAT_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "hw_gpio.h"
#include "hw_systick.h"
#include <stdint.h>
#include <stdbool.h>

#define HB_LED_PORT 'c'
#define HB_LED_PIN 23

    uint8_t lbs_count;

    uint32_t state_snapshot;

    typedef enum HB_MODES
    {
        LED_STATIC_OFF =   0b00000000000000000000000000000000,
        LED_STATIC_ON =    0b11111111111111111111111111111111,
        LED_FLASH_SINGLE = 0b00000000000000001111111111111111,
        LED_FLASH_DOUBLE = 0b00000000000000001111110000111111,
        LED_FLASH_TRIPLE = 0b00000000000111110001111100011111
    } HB_MODE;

    static GPIO *led;

    static HB_MODE hb_mode;

    static bool is_active;

    void heartbeat_init(HB_MODE mode);

    void heartbeat_stop(void);

    void heartbeat_start(void);

    void heartbeat_set_mode(HB_MODE mode);

    void heartbeat_set_period(uint16_t period_ms);

    void heartbeat_tick_callback(void);

#ifdef __cplusplus
}
#endif

#endif