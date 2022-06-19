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
#include "util_error.h"
#include "util_fsm.h"
#include "hw_i2c1.h"
#include <stdlib.h>

#define I2C_MSG_BUF_LEN 0x0F

#define MSG_SEND_MIN 2
#define MSG_RECV_MIN 1

/*  */
typedef enum I2C_EVENTS {
  I2C_NEW_DATA,
  I2C_SB_SENT,
  I2C_RPT_SB_SENT,
  I2C_PB_SENT,
  I2C_BYTE_SENT,
  I2C_LAST_BYTE_SENT_W,
  I2C_LAST_BYTE_SENT_R,
  I2C_BYTE_RECV,
  I2C_LAST_BYTE_RECV,
  I2C_ERROR,
  I2C_EVENT_CNT
} I2C_EVENT;

/* message structure for master-sent packets */
typedef struct i2c_msg {
  uint8_t n_send;
  uint8_t n_recv;
  uint8_t *buf_recv[];
  uint8_t *buf_send[];
} i2c_msg_t;

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

void i2c_state_tx_data(void);

void i2c_state_rx_data(void);

void i2c_state_stop_bit(void);

void i2c_state_error(void);

#endif 
