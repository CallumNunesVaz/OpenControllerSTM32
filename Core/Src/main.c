/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
// #include "mgr_bout.h"
#include "hw_i2c.h"
// #include "drv_gpioexp.h"

uint8_t i2c_data = 0x55;

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
  /* Configure the system clocks */
  rcc_init();

  /* Configure and initialise the system tick */
  hw_systick_init(1000);

  /* Delay for buggy slow peripherals (looking at you, i2c) */
  //blocking_delay_ms(100);

  /* Configure the system heartbeat */
  heartbeat_init();
  heartbeat_set_pattern_mode(LED_BREATHE);
  heartbeat_set_poll_mode(true);
  heartbeat_set_period_ms(1000);
  heartbeat_start();

  /* Initialise i2c perihperal */
  i2c_init(I2C_PERIPH_I2C1, I2C_SPD_STD, true);
  i2c_start(I2C_PERIPH_I2C1);
  i2c_send_addr(I2C_PERIPH_I2C1, &i2c_data);
  while (!(I2C1->SR1 & (1 << 1)))  ;  // wait for ADDR bit to set

  /* Main loop */
  for (;;)
  {
    // i2c_poll_fsm();
    heartbeat_poll();
  }
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
