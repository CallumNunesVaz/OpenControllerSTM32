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

#include "mgr_bout.h"

/* Files required variables */

static const char bout_gpio_ports[BOUT_CNT] = {PORT_BOUT_0,PORT_BOUT_1,PORT_BOUT_2,PORT_BOUT_3,PORT_BOUT_4,PORT_BOUT_5,PORT_BOUT_6,PORT_BOUT_7};

static const uint8_t bout_gpio_pins[BOUT_CNT] = {PIN_BOUT_0,PIN_BOUT_1,PIN_BOUT_2,PIN_BOUT_3,PIN_BOUT_4,PIN_BOUT_5,PIN_BOUT_6,PIN_BOUT_7};

static stmgpio_t bout_gpio[BOUT_CNT];

volatile static uint8_t callback_cnt;

volatile static bool bouts_is_active;

int bout_init(void)
{
    uint8_t idx;

    /* set up each of the gpio */
    for (idx = 0; idx < BOUT_CNT; idx++) {
        bout_gpio[idx].port = bout_gpio_ports[idx];
        bout_gpio[idx].pin = bout_gpio_pins[idx];
        bout_gpio[idx].cfg = OUT_PUSHPULL;
        bout_gpio[idx].dir = OUTPUT_2MHZ;
        RET_ON_FAIL(stmgpio_setup(&bout_gpio[idx]));
    }

    /* Setup callback for system tick */
    hw_systick_add_callback(bout_tick_callback);

    /* Defaults */
    bout_reset_lib();

    return EXIT_SUCCESS;
}

void bout_reset_lib(void)
{
    uint8_t idx;
    /* Set all pins to low by default */
    for (idx = 0; idx < BOUT_CNT; idx++) {
        stmgpio_write(&bout_gpio[idx], PIN_LOW);
    }
}

void bout_reset(uint8_t bout_num)
{
    /* write if its a relevant number */
    if (BOUT_CNT > bout_num) {
        stmgpio_write(&bout_gpio[bout_num], PIN_LOW);
    }
}

void bout_set(uint8_t bout_num)
{
    /* write if its a relevant number */
    if (BOUT_CNT > bout_num) {
        stmgpio_write(&bout_gpio[bout_num], PIN_HIGH);
    }
}

void bout_stop(void)
{
    bouts_is_active = false;
}

void bout_start(void)
{
    bouts_is_active = true;
}

void bout_poll(void)
{
    /*  */
    while (callback_cnt > 0) {
        callback_cnt--;
    }
}

void bout_tick_callback(void)
{
    if (bouts_is_active)
    {
        callback_cnt++;
    }
}