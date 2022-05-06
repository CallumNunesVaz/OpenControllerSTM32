/*
fdgfdh
*/
#include "hw_gpio.h"

void hw_gpio_init(void) {
  /* GPIO Ports Clock Enable */
  RCC->APB2ENR |= (0x1UL << RCC_APB2ENR_IOPAEN_Pos);
  RCC->APB2ENR |= (0x1UL << RCC_APB2ENR_IOPBEN_Pos);
  RCC->APB2ENR |= (0x1UL << RCC_APB2ENR_IOPCEN_Pos);
  RCC->APB2ENR |= (0x1UL << RCC_APB2ENR_IOPDEN_Pos);
}
