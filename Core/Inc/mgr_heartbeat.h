#ifndef __MGR_HEARTBEAT_H
#define __MGR_HEARTBEAT_H

#include "hw_gpio.h"
#include "hw_systick.h"
#include <stdint.h>
#include <stdbool.h>

#define HB_LED_PORT 'c'
#define HB_LED_PIN 23

#define LED_STATE_MAX_RES 32

typedef enum HB_MODES
{
    LED_STATIC_OFF = 0b00000000000000000000000000000000,
    LED_STATIC_ON = 0b11111111111111111111111111111111,
    LED_FLASH_SINGLE = 0b00000000000000001111111111111111,
    LED_FLASH_DOUBLE = 0b00000000000000001111110000111111,
    LED_FLASH_TRIPLE = 0b00000000000111110001111100011111
} HB_MODE;

void heartbeat_reset(void);

int heartbeat_init(void);

void heartbeat_stop(void);

void heartbeat_start(void);

void heartbeat_poll(void);

void heartbeat_set_mode(HB_MODE mode);

void heartbeat_set_period_ms(uint16_t period_ms);

void heartbeat_tick_callback(void);

#endif