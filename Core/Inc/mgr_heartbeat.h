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
extern "C" {
#endif

#include "hw_gpio.h"
#include <stdint.h>

#define HB_LED_PORT 'c'
#define HB_LED_PIN 23

enum heartbeatmode {
    STATIC_OFF,
    STATIC_ON,
    FLASH_SINGLE,
    FLASH_DOUBLE,
    FLASH_TRIPLE
};

void heartbeat_init(void);

void heartbeat_stop(void);

void heartbeat_start(void);

void heartbeat_set_mode(heartbeatmode mode);

void heartbeat_set_freq(uint8_t freq_hz);

void heartbeat_tick_callback(void);

#ifdef __cplusplus
}
#endif

#endif