#ifndef __HW_I2C1_H
#define __HW_I2C1_H

#include "stm32f103xb.h"
#include "util_common.h"
#include "irq_ctrl.h"
#include "hw_stmgpio.h"
#include <stdlib.h>

#define I2C_SCL_PORT 'B'
#define I2C_SCL_PIN 8

#define I2C_SDA_PORT 'B'
#define I2C_SDA_PIN 9

#define I2C1_EV_IRQ_PRIORITY 0xFFFF
#define I2C1_ER_IRQ_PRIORITY 0xFFFF

#define APB_BUS_FREQ_MHZ 50

int i2c1_init(void);

void i2c1_reset_periph(void);

void i2c1_enable_periph(void);

void i2c1_disable_periph(void);

uint8_t i2c1_read(void);

void i2c1_write(uint8_t data);

void i2c1_ev_irq_handler(void);

void i2c1_er_irq_handler(void);

#endif 
