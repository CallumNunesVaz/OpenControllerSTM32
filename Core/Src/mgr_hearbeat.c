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

#include "mgr_heartbeat.h"

/* Files required variables */

volatile static uint8_t callback_cnt;

static uint8_t snapshot_rbs_mult_cnt;

static uint8_t snapshot_rbs_cnt;

static uint32_t snapshot_lbs_mult;

static uint16_t heartbeat_period_ms;

static uint32_t state_snapshot;

static gpio_t *led;

static hb_mode_t hb_mode;

volatile static bool heartbeat_is_active;

int heartbeat_init(void)
{
    /* Use special gpio setup */
    gpio_setup_t hbled_setup;

    hbled_setup.port = HB_LED_PORT;
    hbled_setup.pin = HB_LED_PIN;
    hbled_setup.cfg = OUT_PUSHPULL;
    hbled_setup.dir = OUTPUT_2MHZ;

    led = hw_gpio_setup_gpio(&hbled_setup);

    if (led == NULL)
    {
        return EXIT_FAILURE;
    }

    /* Set DEFAULTS */
    heartbeat_set_period_ms(1000);
    hb_mode = LED_STATIC_OFF;
    state_snapshot = (uint32_t)hb_mode;

    /* Setup callback for system tick */
    hw_systick_add_callback(heartbeat_tick_callback);

    return EXIT_SUCCESS;
}

void heartbeat_reset(void)
{
    /* Reset counters */
    snapshot_rbs_cnt = 0;
    snapshot_rbs_mult_cnt = 0;
    callback_cnt = 0;
}

void heartbeat_stop(void)
{
    heartbeat_is_active = false;
}

void heartbeat_start(void)
{
    heartbeat_is_active = true;
}

void heartbeat_set_mode(hb_mode_t mode)
{
    hb_mode = mode;
}

void heartbeat_poll(void)
{
    /* Iterate for every time callback was called */
    while (callback_cnt > 0)
    {
        callback_cnt--;

        snapshot_rbs_mult_cnt++;

        /* If counter ticked up enough to move to next bit of snapshot */
        if (snapshot_rbs_mult_cnt >= snapshot_lbs_mult)
        {
            /* reset multiplier counter */
            snapshot_rbs_mult_cnt = 0;

            /* write LED with value of LSB of snapshot */
            hw_gpio_write(led, (gpio_state_t)(state_snapshot & 0x01));

            /* go to next bit of snapshot */
            state_snapshot >>= 1;
            snapshot_rbs_cnt++;

            /* If we've run out of bits in the snapshot, reset snapshot and snapshot counter */
            if (LED_STATE_MAX_RES <= snapshot_rbs_cnt)
            {
                snapshot_rbs_cnt = 0;
                state_snapshot = (uint32_t)hb_mode;
            }
        }
    }
}

void heartbeat_set_period_ms(uint16_t period_ms)
{
    uint16_t systick_freq = hw_systick_get_freq();

    heartbeat_period_ms = period_ms;

    snapshot_lbs_mult = (uint32_t)(((float)systick_freq) / ((float)LED_STATE_MAX_RES) * ((float)heartbeat_period_ms) / 1000.0);

    heartbeat_reset();
}

void heartbeat_tick_callback(void)
{
    if (heartbeat_is_active)
    {
        callback_cnt++;
    }
}