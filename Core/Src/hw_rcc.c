#include "hw_rcc.h"

/**
 * @brief System Clock Configuration
 * @retval None
 */
void rcc_init(void)
{
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

  /* Clock configuration */
  RCC->CR &= ~RCC_CR_PLLON; // Disable PLL to allow changes
  RCC->CFGR &= ~RCC_CFGR_MCO; // Microcontroller clock out disabled!
  RCC->CFGR &= ~RCC_CFGR_USBPRE; // USB Prescalar /1.5 (set for 72MHz PLL output)
  RCC->CFGR &= RCC_CFGR_PLLXTPRE; // PLL entry HSE is NOT divided
  RCC->CFGR |= RCC_CFGR_PLLSRC; // PLL source is 8MHz HSE clock NOT divided
  RCC->CFGR &= ~RCC_CFGR_PPRE2; // clear...
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV2; // APB high speed (2) set to divide by 2 = 36 MHz
  RCC->CFGR &= ~RCC_CFGR_PPRE1; // clear...
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; // APB low speed (1) set to divide by 2 = 36 MHz
  RCC->CFGR &= ~RCC_CFGR_HPRE; // AHB clock NOT divided (72 MHz)
  RCC->CFGR &= ~RCC_CFGR_SW; // clear...
  RCC->CFGR |= RCC_CFGR_SW_PLL; // PLL set as system clock (72 MHz)
  RCC->CFGR &= ~RCC_CFGR_HPRE; // SYSCLK is NOT divided
  RCC->CFGR |= RCC_CFGR_ADCPRE; // ADC Prescalar set to divide by 8
  RCC->CFGR &= ~RCC_CFGR_PLLMULL; // clear...
  RCC->CFGR |= RCC_CFGR_PLLMULL9; // PLL set to 9x from 8MHz HSE to make 72MHz
  RCC->CR |= RCC_CR_PLLON; // PLL can be enabled now
  while (!(RCC->CR & RCC_CR_PLLRDY)); // check PLL is ready
  while (!(RCC->CFGR & RCC_CFGR_SWS_PLL)); // Check PLL is being used as sys clock

  /* Internal 8MHz HSI Configuration:
   * OFF!
   * Trim left default = 16
   * Cal at startup default
   */
  RCC->CR &= ~RCC_CR_HSION;
  //RCC->CR |= RCC_CR_HSION;
  //while (!(RCC->CR & RCC_CR_HSIRDY));

  /* Inform core CMSIS libraries of change to clocks
   */
  SystemCoreClockUpdate();
}

