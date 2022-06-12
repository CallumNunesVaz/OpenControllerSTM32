/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MGR_I2C_H
#define __MGR_I2C_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f103xb.h"
#include "hw_systick.h"
#include "util_buffer.h"
#include "util_fsm.h"
#include <stdlib.h>

#define I2C_MSG_BUF_LEN 0x0F

/*  */
typedef enum i2c_event {
  I2C_NEW_DATA,
  I2C_START_FIN,
  I2C_TX_FIN,
  I2C_RX_FIN,
  I2C_STOP_FIN,
  I2C_EVENT_CNT
} i2c_event_t;

/* message structure for master-sent packets */
typedef struct i2c_msg {
  uint8_t n_send;
  uint8_t n_recv;
  uint8_t *buf_send[];
  uint8_t *buf_recv[];
} i2c_msg_t;

typedef enum i2c_errors {
  I2C_ERROR_TX_TIMEOUT,
  I2C_ERROR_RX_TIMEOUT,
  I2C_ERROR_TX_BUF_OVF,
  I2C_ERROR_TX_BUF_OVF,
  I2C_ERROR_NACK,
} i2c_error;

int i2c_init(void);

void i2c_reset(void);

void i2c_enable(void);

void i2c_disable(void);

uint8_t i2c_read_byte(void);

void i2c_read_byte_array(uint8_t *data[], size_t len);

void i2c_write_byte(uint8_t data);

void i2c_write_byte_array(uint8_t *data[], size_t len);

void i2c_poll_fsm(void);

void i2c_state_idle(void);

void i2c_state_start_bit(void);

void i2c_state_tx_byte(void);

void i2c_state_rx_byte(void);

void i2c_state_stop_bit(void);

void i2c_state_error(void);

#endif 
