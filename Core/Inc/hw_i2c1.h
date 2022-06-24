#ifndef __HW_I2C1_H
#define __HW_I2C1_H

#include "stm32f103xb.h"
#include "core_cm3.h"
#include "hw_stmgpio.h"
#include "util_error.h"
#include <stdlib.h>

/* Pins */
#define I2C_SCL_PORT 'B'
#define I2C_SCL_PIN 8
#define I2C_SDA_PORT 'B'
#define I2C_SDA_PIN 9

/* Interrupt priority */
#define I2C1_EV_IRQ_PRIORITY 0x00
#define I2C1_ER_IRQ_PRIORITY 0x00
/* Interrupt alias */
#define i2c1_ev_irq_handler I2C1_EV_IRQHandler 
#define i2c1_er_irq_handler I2C1_ER_IRQHandler 

#define APB_BUS_FREQ_MHZ 50

#define I2C_READ_BIT 1
#define I2C_WRITE_BIT 0

typedef enum I2C1_EVTS
{
    I2C1_EVT_SB = I2C_SR1_SB,
    I2C1_EVT_ADDR = I2C_SR1_ADDR,
    I2C1_EVT_ADD10 = I2C_SR1_ADD10,
    I2C1_EVT_BTF = I2C_SR1_BTF,
    I2C1_EVT_TxE = I2C_SR1_TXE,
    I2C1_EVT_RxNE = I2C_SR1_RXNE
} I2C1_EVT;

typedef enum I2C1_ERRS
{
    I2C1_ERR_BERR = I2C_SR1_BERR,
    I2C1_ERR_ARLO = I2C_SR1_ARLO,
    I2C1_ERR_AF = I2C_SR1_AF,
    I2C1_ERR_OVR = I2C_SR1_OVR,
    I2C1_ERR_PECERR = I2C_SR1_PECERR,
    I2C1_ERR_TIMEOUT = I2C_SR1_TIMEOUT,
    I2C1_ERR_SMBALERT = I2C_SR1_SMBALERT
} I2C1_ERR;

/* Debug info */
#ifdef DEBUG_EN
static const char DBG_LIB_NAME[] = "hw_i2c1";
#endif

int i2c1_init(void);

void i2c1_reset_periph(void);

void i2c1_enable_periph(void);

void i2c1_ack_bit(void);

void i2c1_nack_bit(void);

void i2c1_disable_periph(void);

void i2c1_start(void);

void i2c1_stop(void);

int i2c1_recv(uint8_t *data);

int i2c1_send(uint8_t *data);

int i2c1_set_evt_callback(void (*func_ptr)(void));

int i2c1_set_err_callback(void (*func_ptr)(void));

uint16_t i2c1_SR1_dummy_read(void);

uint16_t i2c1_SR2_dummy_read(void);

I2C1_EVT i2c1_get_last_event(void);

I2C1_ERR i2c1_get_last_error(void);

void i2c1_ev_irq_handler(void);

void i2c1_er_irq_handler(void);

#endif
