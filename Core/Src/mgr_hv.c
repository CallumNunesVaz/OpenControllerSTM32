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

#include "mgr_hv.h"

/* Files required variables */

static stmgpio_t *hv_en_gpio;

volatile static bool hv_is_active;

int hv_init(void)
{
    /* Use special gpio setup */
    stmgpio_setup_t hv_en_gpio_setup;

    hv_en_gpio_setup.port = HV_EN_PORT;
    hv_en_gpio_setup.pin = HV_EN_PIN;
    hv_en_gpio_setup.cfg = OUT_PUSHPULL;
    hv_en_gpio_setup.dir = OUTPUT_2MHZ;

    hv_en_gpio = stmgpio_setup_gpio(&hv_en_gpio_setup);

    if (NULL == hv_en_gpio)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void hv_rail_enable(uint8_t bout_num)
{
    
}

void hv_rail_disable(uint8_t bout_num)
{
    
}