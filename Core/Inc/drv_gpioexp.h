#ifndef __DRV_GPIOEXP_H
#define __DRV_GPIOEXP_H

#include "hw_i2c.h"

#include "util_error.h"
#include "util_buffer.h"
#include <stdlib.h>
#include <stdint.h>
 
#define GPIOEXP_I2C_TX_BUF_LEN 0x0F
#define GPIOEXP_I2C_RX_BUF_LEN 0x0F

#define PCF8574_ADDR 0x40

void gpioexp_init(void);

void gpioexp_set_pin(uint8_t pin);

void gpioexp_get_pin(uint8_t pin);


#endif
