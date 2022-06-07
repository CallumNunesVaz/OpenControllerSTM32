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
#ifndef __HW_I2C1_H
#define __HW_I2C1_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f103xb.h"
#include "hw_systick.h"
#include "hw_stmgpio.h"
#include <stdbool.h>
#include <stdlib.h>

#define I2C_SCL_PORT 'B'
#define I2C_SCL_PIN 8

#define I2C_SDA_PORT 'B'
#define I2C_SDA_PIN 9

#define APB_BUS_FREQ_MHZ 50

#define BUF_RX_LEN 0x1F
#define BUF_TX_LEN 0x1F

typdef enum I2C_MASTER_STATES {
  IDLE,
  ERROR
} I2C_MASTER_STATE;

#endif 
