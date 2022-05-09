/*
fdgfdh
*/
#include "hw_gpio.h"

void hw_gpio_init(void)
{
  /* GPIO Ports Clock Enable: all of them! */
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
  RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;

  /* Add callback */
  hw_systick_add_callback(hw_gpio_callback);
}

GPIO *hw_gpio_setup_gpio(GPIO gpio)
{
  /* Allocate memory to the new GPIO */
  GPIO *new_gpio = malloc(sizeof(GPIO));

  /* Assign GPIO parameters */
  memcpy(new_gpio, gpio, sizeof(GPIO));

  /* Find base address of register based on character. They're sequentially addressed (A, B, C...) */
  if ((new_gpio->port >= 'A') && (new_gpio->port <= 'E'))
  {
    new_gpio->base_reg_address = GPIOA_BASE + ((uint32_t)(new_gpio->port - 'A') * 0x400);
  }
  else
  {
    // ERROR!
  }

  GPIOC->CRH &= ~GPIO_CRH_CNF13; // Output push-pull
  GPIOC->CRH |= GPIO_CRH_MODE13; // 50MHz
}

void hw_gpio_kill(GPIO *gpio)
{
  /* Disable GPIO */

  /* Free memory */
  free(gpio, sizeof(GPIO));
}

void hw_gpio_write(GPIO *gpio, gpio_state state)
{
}

gpio_state hw_gpio_read(GPIO gpio)
{
}

void hw_gpio_callback(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
}
