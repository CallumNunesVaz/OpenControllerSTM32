/*
GPIO library designed to be relatively lightwieght but human readable.
Very much not memory-safe! libraries using this library must manage assigned GPIO.
*/
#include "hw_gpio.h"

gpio_t *hw_gpio_setup_gpio(gpio_setup_t* gs)
{
  /* Allocate variable */
  gpio_t *g = NULL;

  /* Sanity checks */
  if (('A' <= gs->port) && (gs->port <= 'E') && (gs->dir < DIR_CNT) && (gs->cfg < TYPE_CNT) && (gs->pin <= GPIO_PORT_PIN_MAX))
  {
    /* Allocate memory */
    g = malloc(sizeof(gpio_t));

    /* Turn on ports clock if not already on. Find addr of reg based on char */
    switch (gs->port)
    {
    case 'A':
      RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
      g->port_reg_addr = GPIOA;
      break;
    case 'B':
      RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
      g->port_reg_addr = GPIOB;
      break;
    case 'C':
      RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
      g->port_reg_addr = GPIOC;
      break;
    case 'D':
      RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
      g->port_reg_addr = GPIOD;
      break;
    case 'E':
      RCC->APB2ENR |= RCC_APB2ENR_IOPEEN;
      g->port_reg_addr = GPIOE;
      break;
    }

    /* copy across GPIO parameters */
    g->dir = gs->dir;
    g->cfg = gs->cfg;
    g->pin = gs->pin;
    g->port = gs->port;

    /* Assign low as default state */
    g->state = PIN_LOW;

    /* Set registers based on GPIO settings */
    if (g->pin < 8)
    {
      g->port_reg_addr->CRL &= ~(0x0F << (REG_PIN_CONF_BITLENGTH * g->pin));                                            // reset
      g->port_reg_addr->CRL |= ((uint32_t)(g->dir)) << ((REG_PIN_CONF_BITLENGTH * g->pin) + REG_MODE_OFFSET);           // pin direction / mode
      g->port_reg_addr->CRL |= (((uint32_t)(g->cfg)) & ~(0x04)) << ((REG_PIN_CONF_BITLENGTH * g->pin) + REG_CONF_OFFSET); // pin configuration
    }
    else
    {
      g->port_reg_addr->CRH &= ~(0x0F << (REG_PIN_CONF_BITLENGTH * (g->pin - 0x08)));                                            // reset
      g->port_reg_addr->CRH |= ((uint32_t)(g->dir)) << ((REG_PIN_CONF_BITLENGTH * (g->pin - 0x08)) + REG_MODE_OFFSET);           // pin direction / mode
      g->port_reg_addr->CRH |= (((uint32_t)(g->cfg)) & ~(0x04)) << ((REG_PIN_CONF_BITLENGTH * (g->pin - 0x08)) + REG_CONF_OFFSET); // pin configuration
    }
  }

  /* return pointer to our newly created gpio configuration */
  return g;
}

void hw_gpio_free_memory(gpio_t *gpio)
{
  /* Free memory */
  free(gpio);
}

void hw_gpio_write(gpio_t *gpio, gpio_state_t set_state)
{
  if (PIN_LOW == set_state) {
    hw_gpio_reset(gpio);
  }
  if (PIN_HIGH == set_state) {
    hw_gpio_set(gpio);
  }
}

inline void hw_gpio_set(gpio_t *gpio)
{
    gpio->state = PIN_HIGH;
    gpio->port_reg_addr->BSRR = (0x01 << (uint8_t)(gpio->pin));
}

inline void hw_gpio_reset(gpio_t *gpio)
{
    gpio->state = PIN_LOW;
    gpio->port_reg_addr->BSRR = (0x01 << (uint8_t)(gpio->pin) << 0x10);
}

gpio_state_t hw_gpio_read(gpio_t *gpio)
{
  /* retrieve the GPIO state on function call */
  gpio->state = (gpio_state_t)((gpio->port_reg_addr->IDR) & (0x01 << gpio->pin));
  return gpio->state; 
}