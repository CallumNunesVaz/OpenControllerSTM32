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

GPIO *hw_gpio_setup_gpio(GPIO_SETUP gs)
{
  /* Allocate variable */
  GPIO *g;

  /* Sanity checks */
  if (('A' <= gs.port) && (gs.port <= 'E') && (gs.dir < DIR_CNT) && (gs.cfg < TYPE_CNT) && (gs.pin <= GPIO_PORT_PIN_MAX))
  {
    /* Allocate memory */
    GPIO *g = malloc(sizeof(GPIO));

    /* Find base address of register based on character. They're sequentially addressed (A, B, C...) */
    // g->base_reg_addr = (GPIO_TypeDef *)(GPIOA_BASE + ((uint32_t)(g->port - 'A') * 0x400));

    /* Find base address of register based on character. Abstract from CMSIS layer */
    switch (gs.port)
    {
    case 'A':
      g->base_reg_addr = GPIOA;
      break;
    case 'B':
      g->base_reg_addr = GPIOB;
      break;
    case 'C':
      g->base_reg_addr = GPIOC;
      break;
    case 'D':
      g->base_reg_addr = GPIOD;
      break;
    case 'E':
      g->base_reg_addr = GPIOE;
      break;
    default:
      break;
    }

    /* copy across GPIO parameters */
    g->dir = gs.dir;
    g->cfg = gs.cfg;
    g->pin = gs.pin;
    g->port = gs.port;

    /* Assign low as default state */
    g->state = PIN_LOW;

    /* Set registers based on GPIO settings */
    if (g->pin < 8) {
      g->base_reg_addr->CRL &= ~(0xF << (0x04 * g->pin));                            // reset
      g->base_reg_addr->CRL |= ((uint32_t)(g->dir)) << (0x04 * g->pin);              // pin direction / mode
      g->base_reg_addr->CRL |= (((uint32_t)(g->cfg)) >> 0x01) << ((0x04 * g->pin) + 0x02); // pin configuration
    } else {
      g->base_reg_addr->CRH &= ~(0xF << (0x04 * (g->pin - 0x08)));                                  // reset
      g->base_reg_addr->CRH |= ((uint32_t)(g->dir)) << (0x04 * (g->pin - 0x08));              // pin direction / mode
      g->base_reg_addr->CRH |= (((uint32_t)(g->cfg)) >> 0x01) << ((0x04 * (g->pin - 0x08)) + 0x02); // pin configuration
    }

    /* return pointer to our newly created gpio configuration */
    return g;
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
