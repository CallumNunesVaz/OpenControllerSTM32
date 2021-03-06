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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f103xb.h"
#include "core_cm3.h"
//#include "hw_adc1.h"
//#include "hw_crc.h"
//#include "hw_gpio.h"
//#include "hw_i2c1.h"
//#include "hw_spi2.h"
//#include "hw_usart1.h"
#include "hw_systick.h"
#include <stdint.h>
#include <stdlib.h>
#include "util_error.h"

#include "mgr_heartbeat.h"

int main(void);

void hw_system_clocks_init(void);

void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
