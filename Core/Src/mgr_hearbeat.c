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

static uint8_t window_cnt_total;

static uint8_t tick_per_window_cnt;

static uint8_t window_cnt;

static uint8_t fade_vals[WINDOW_COUNT_FADE];

static uint32_t ticks_per_window;

static uint16_t heartbeat_period_ms;

static uint32_t state_snapshot;

static stmgpio_t *led;

static hb_mode_t hb_mode;

volatile static uint8_t callback_cnt;

volatile static bool heartbeat_is_active;

volatile static bool poll_mode;

int heartbeat_init(void)
{
    /* Use special gpio setup */
    stmgpio_setup_t hbled_setup;

    hbled_setup.port = HB_LED_PORT;
    hbled_setup.pin = HB_LED_PIN;
    hbled_setup.cfg = OUT_PUSHPULL;
    hbled_setup.dir = OUTPUT_2MHZ;

    led = stmgpio_setup_gpio(&hbled_setup);

    if (NULL == led)
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
    window_cnt = 0;
    tick_per_window_cnt = 0;
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

void heartbeat_set_pattern_mode(hb_mode_t mode)
{
    hb_mode = mode;
    heartbeat_set_period_ms(heartbeat_period_ms);
    heartbeat_reset();
}

void heartbeat_set_poll_mode(bool mode)
{
    poll_mode = mode;
}

bool heartbeat_get_poll_mode(void)
{
    return poll_mode;
}

uint32_t heartbeat_lower_multiple(uint32_t number, uint32_t multiple)
{
    uint32_t remainder;

    remainder = number % multiple;

    number -= remainder;

    return number;
}

void heartbeat_poll(void)
{
    /* Iterate for every time callback was called. Gotta catch up. */
    while (callback_cnt > 0)
    {
        callback_cnt--;

        if (LED_PULSE == hb_mode)
        {
            /* Set high for particular number of ticks per window */
            if (tick_per_window_cnt < fade_vals[window_cnt])
            {
                stmgpio_write(led, PIN_HIGH);
            }
            else
            {
                stmgpio_write(led, PIN_LOW);
            }

            /* Increment tick counter */
            tick_per_window_cnt++;

            /* If counter ticked up enough to move to next window */
            if (tick_per_window_cnt >= ticks_per_window)
            {
                /* Increment window counter, reset tick count */
                window_cnt++;
                tick_per_window_cnt = 0;

                /* If we've run out of windows, reset completely */
                if (window_cnt_total <= window_cnt)
                {
                    window_cnt = 0;
                }
            }
        }
        else /* All other modes */
        {
            /* Increment tick counter */
            tick_per_window_cnt++;

            /* If counter ticked up enough to move to next window (bit of snapshot) */
            if (tick_per_window_cnt >= ticks_per_window)
            {
                /* reset window tick counter */
                tick_per_window_cnt = 0;

                /* write LED with value of LSB of snapshot */
                stmgpio_write(led, (stmgpio_state_t)(state_snapshot & 0x01));

                /* go to next bit of snapshot */
                state_snapshot >>= 1;
                window_cnt++;

                /* If we've run out of bits in the snapshot, reset snapshot and snapshot counter */
                if (window_cnt_total <= window_cnt)
                {
                    window_cnt = 0;
                    state_snapshot = (uint32_t)hb_mode;
                }
            }
        }
    }
}

void heartbeat_set_period_ms(uint16_t period_ms)
{
    uint16_t idx;
    uint16_t systick_freq_hz = hw_systick_get_freq();

    /* Set boundaries */
    if (3000 <= period_ms)
    {
        period_ms = 3000;
    }
    if (500 >= period_ms)
    {
        period_ms = 500;
    }

    /* Different window counts requierd for different modes */
    if (LED_PULSE == hb_mode)
    {
        window_cnt_total = WINDOW_COUNT_FADE;
    }
    else
    {
        window_cnt_total = WINDOW_COUNT_FLASH;
    }

    /* find heartbeat as lower multiple to keep division easy */
    heartbeat_period_ms = heartbeat_lower_multiple(period_ms, window_cnt_total);

    /* calculate ticks per window based on chosen heartbeat period and the set systick frequency */
    ticks_per_window = (uint32_t)(((float)systick_freq_hz) / ((float)window_cnt_total) * ((float)heartbeat_period_ms) / 1000.0);

    /* calculate fade triangle */
    for (idx = 0; idx < (window_cnt_total / 2); idx++)
    {
        fade_vals[idx] = (uint8_t)(2.0 * (((float)idx) / ((float)window_cnt_total)) * ((float)ticks_per_window));
        fade_vals[window_cnt_total - idx - 1] = fade_vals[idx];
    }

    /* Reset to avoid surprises */
    heartbeat_reset();
}

void heartbeat_tick_callback(void)
{
    /* only take action if told to */
    if (heartbeat_is_active)
    {
        callback_cnt++;
    }
    /* if not manualy polled, do all the stuff in the callback (intensive) */
    if (!poll_mode) {
        heartbeat_poll();
    }
}