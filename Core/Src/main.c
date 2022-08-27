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
#include "mgr_bout.h"
#include "mgr_i2c.h"
#include "drv_gpioexp.h"

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
  uint8_t idx;

  /* Configure the system clocks */
  hw_system_clocks_init();

  /* Configure and initialise the system tick */
  hw_systick_init(1000);

  /* Configure the system heartbeat */
  ASSERT_INT(heartbeat_init());
  heartbeat_set_pattern_mode(LED_BREATHE);
  heartbeat_set_poll_mode(true);
  heartbeat_set_period_ms(1000);
  heartbeat_start();

  /* Initialise binary outputs */
  ASSERT_INT(bout_init());

  ASSERT_INT(i2c_init());

  /* interrupts (move to new file!) */
  //__enable_irq();

  gpioexp_init();

  /* Initialised everything! */
  // dbg_log(DBG_TYPE_SUCCESS, DBG_CODE_INIT, DBG_NAME, sizeof(DBG_NAME));

  /* Main loop */
  while (1)
  {
    i2c_poll_fsm();
    heartbeat_poll();
    // for (idx = 0; idx < 7; idx++) {
    //   bout_reset_lib();
    //   bout_set(idx);
    // blocking_delay_ms(500);
    //}
  }
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void hw_system_clocks_init(void)
{
  /* Internal 8MHz HSI Configuration:
   * ON
   * Trim set to default = 16
   * Cal at startup default
   */
  RCC->CR |= RCC_CR_HSION_Pos;
  while (!(RCC->CR & RCC_CR_HSIRDY))
    ;

  /* External 8MHz HSE Configuration:
   * ON
   * Bypass to false
   *
   */
  RCC->CR |= RCC_CR_HSEON;
  RCC->CR &= ~RCC_CR_HSEBYP;
  while (!(RCC->CR & RCC_CR_HSERDY))
    ;

  /* Flash latency configuration:
   * Enable pre-fetch buffer
   * Disable Flash half cycle access
   * Set wait states (latency) to two, as main clock will be 72MHz
   */
  FLASH->ACR |= FLASH_ACR_PRFTBE;
  FLASH->ACR &= ~FLASH_ACR_HLFCYA;
  FLASH->ACR &= ~FLASH_ACR_LATENCY; // reset
  FLASH->ACR |= FLASH_ACR_LATENCY_1;

  /* Clock configuration
   * Disable PLL
   * Microcontroller clock out disabled!
   * USB Prescalar set for 72MHz PLL output
   * PLL source is 8MHz HSE clock through PREDIV1=1
   * ADC Prescalar set to divide by 8
   * APB high speed set to divide by 1 = 72MHz
   * APB low speed set to divide by 2 = 36 MHz
   * PLL set to 9x from 8MHz HSE to make 72MHz
   * Enable PLL
   * AHB set to divide by 1 = 72MHz
   * PLL as system clock
   */
  RCC->CR &= ~RCC_CR_PLLON; // clear to allow changes
  RCC->CFGR &= ~RCC_CFGR_MCO;
  RCC->CFGR &= ~RCC_CFGR_USBPRE;
  RCC->CFGR |= RCC_CFGR_PLLSRC;
  RCC->CFGR |= RCC_CFGR_ADCPRE;
  RCC->CFGR &= ~RCC_CFGR_PPRE2;
  RCC->CFGR &= ~RCC_CFGR_PPRE1; // reset
  RCC->CFGR |= RCC_CFGR_PPRE1_2;
  RCC->CFGR &= ~RCC_CFGR_PLLMULL; // reset
  RCC->CFGR |= RCC_CFGR_PLLMULL9;
  RCC->CR |= RCC_CR_PLLON;
  while (!(RCC->CR & RCC_CR_PLLRDY))
    ;
  RCC->CFGR &= ~RCC_CFGR_HPRE;
  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_SW_PLL;
  while (!(RCC->CFGR & RCC_CFGR_SWS_PLL))
    ;

  /* Inform core libraries of change to clocks
   */
  SystemCoreClockUpdate();
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
