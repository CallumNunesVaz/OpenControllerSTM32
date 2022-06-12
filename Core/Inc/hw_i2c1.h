#ifndef __HW_I2C1_H
#define __HW_I2C1_H

#include "stm32f103xb.h"
#include "hw_stmgpio.h"
#include <stdlib.h>

#define I2C_SCL_PORT 'B'
#define I2C_SCL_PIN 8

#define I2C_SDA_PORT 'B'
#define I2C_SDA_PIN 9

#define APB_BUS_FREQ_MHZ 50

int i2c1_init(void);

void i2c1_reset(void);

void i2c1_enable(void);

void i2c1_disable(void);

uint8_t i2c1_read(void);

void i2c1_write(uint8_t data);

#endif 
