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
#ifndef __HW_GPIO_H
#define __HW_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include "stm32f103xb.h"
#include "hw_systick.h"
#include "stdbool.h"

#define GPIO_PORT_PIN_MAX 15

typedef enum {
  LOW,
  HIGH,
  STATE_CNT
} gpio_state;

typedef enum {
  INPUT,
  OUTPUT_10MHZ,
  OUTPUT_2MHZ,
  OUTPUT_50MHz,
  DIR_CNT
} gpio_dir;

typedef enum {
  IN_ANALOGUE,
  IN_FLOATING,
  IN_PULL,
  RSVD,
  OUT_PUSHPULL,
  OUT_OPENDRAIN,
  OUT_ALT_PUSHPULL,
  OUT_ALT_OPENDRAIN,
  TYPE_CNT
} gpio_cfg;

typedef struct GPIOS {
   gpio_state state;
   gpio_dir dir;
   gpio_cfg cfg;
   char port;
   uint8_t pin;
   GPIO_TypeDef *base_reg_addr;
} GPIO;

typedef struct GPIO_SETUPS {
   gpio_dir dir;
   gpio_cfg cfg;
   char port;
   uint8_t pin;
} GPIO_SETUP;


//void MX_GPIO_Init(void);

void hw_gpio_init(void);

GPIO* hw_gpio_setup_gpio(GPIO_SETUP gs);

void hw_gpio_write(GPIO* gpio, gpio_state state);

gpio_state hw_gpio_read(GPIO gpio);

void hw_gpio_callback(void);

#ifdef __cplusplus
}
#endif

#endif
