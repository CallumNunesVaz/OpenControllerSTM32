#ifndef __HW_I2C1_H
#define __HW_I2C1_H

#include "stm32f103xb.h"
#include "core_cm3.h"
#include "hw_stmgpio.h"
#include "util_error.h"
#include <stdlib.h>

/* Pins */
#define I2C1_SCL_PORT_DEFAULT 'B'
#define I2C1_SCL_PIN_DEFAULT 6
#define I2C1_SDA_PORT_DEFAULT 'B'
#define I2C1_SDA_PIN_DEFAULT 7
#define I2C1_SCL_PORT_REMAP 'B'
#define I2C1_SCL_PIN_REMAP 8
#define I2C1_SDA_PORT_REMAP 'B'
#define I2C1_SDA_PIN_REMAP 9
#define I2C2_SCL_PORT_DEFAULT 'B'
#define I2C2_SCL_PIN_DEFAULT 10
#define I2C2_SDA_PORT_DEFAULT 'B'
#define I2C2_SDA_PIN_DEFAULT 11

/* Perihperals */
#define PERIPH_I2C1 1
#define PERIPH_I2C2 2

/* Interrupt priority */
#define I2C_EV_IRQ_PRIORITY 0x00
#define I2C_ER_IRQ_PRIORITY 0x00

#define APB_BUS_FREQ_MHZ 0x32 // 50 Mhz

#define I2C_READ_BIT 1
#define I2C_WRITE_BIT 0

typedef enum I2C_SPDS
{
    I2C_SPD_STD,
    I2C_SPD_FAST
} I2C_SPD;

typedef enum I2C_PERIPHS
{
    I2C_PERIPH_I2C1,
    I2C_PERIPH_I2C2
} I2C_PERIPH;

typedef enum I2C_AF_REMAPS
{
    I2C1_REMAP_FALSE,
    I2C1_REMAP_TRUE,
    I2C2_REMAP_FALSE
} I2C_AF_REMAP;

typedef enum I2C1_EVTS
{
    I2C_EVT_SB = I2C_SR1_SB,
    I2C_EVT_ADDR = I2C_SR1_ADDR,
    I2C_EVT_ADD10 = I2C_SR1_ADD10,
    I2C_EVT_BTF = I2C_SR1_BTF,
    I2C_EVT_TxE = I2C_SR1_TXE,
    I2C_EVT_RxNE = I2C_SR1_RXNE
} I2C_EVT;

typedef enum I2C1_ERRS
{
    I2C_ERR_BERR = I2C_SR1_BERR,
    I2C_ERR_ARLO = I2C_SR1_ARLO,
    I2C_ERR_AF = I2C_SR1_AF,
    I2C_ERR_OVR = I2C_SR1_OVR,
    I2C_ERR_PECERR = I2C_SR1_PECERR,
    I2C_ERR_TIMEOUT = I2C_SR1_TIMEOUT,
    I2C_ERR_SMBALERT = I2C_SR1_SMBALERT
} I2C_ERR;

/* Debug info */
#ifdef DEBUG_EN
static const char DBG_LIB_NAME[] = "hw_i2c1";
#endif

int i2c_init(I2C_PERIPH periph, I2C_SPD spd, bool remap);

void i2c_reset_periph(I2C_PERIPH periph);

void i2c_enable_periph(I2C_PERIPH periph);

void i2c_ack_bit(I2C_PERIPH periph);

void i2c_nack_bit(I2C_PERIPH periph);

void i2c_disable_periph(I2C_PERIPH periph);

void i2c_start(I2C_PERIPH periph);

void i2c_stop(I2C_PERIPH periph);

int i2c_recv(I2C_PERIPH periph, uint8_t *data);

int i2c_send(I2C_PERIPH periph, uint8_t *data);

//int i2c_set_evt_callback(void (*func_ptr)(void));

//int i2c_set_err_callback(void (*func_ptr)(void));

uint16_t i2c_SR1_dummy_read(I2C_PERIPH periph);

uint16_t i2c_SR2_dummy_read(I2C_PERIPH periph);

I2C_EVT i2c_get_last_event(void);

I2C_ERR i2c_get_last_error(void);

//void i2c_ev_irq_handler(void);

//void i2c_er_irq_handler(void);

#endif
