/*
GPIO library designed to be relatively lightwieght but human readable.
Very much not memory-safe! libraries using this library must manage assigned GPIO.
*/
#include "hw_gpio.h"

/*
void hw_gpio_init(void)
{
  /* Add callback 
  hw_systick_add_callback(hw_gpio_callback);

  /* Set global counter 
  gpio_init_count = 0;

  /* Set flag 
  hw_gpio_initialised = true;
}
*/

gpio_t *hw_gpio_setup_gpio(gpio_setup_t* gs)
{
  /* Allocate variable */
  gpio_t *g = NULL;

  /* initialise library if not done already */
  /*
  if (!hw_gpio_initialised)
  {
    hw_gpio_init();
  }
  */

  /* Sanity checks */
  //if (('A' <= gs.port) && (gs.port <= 'E') && (gs.dir < DIR_CNT) && (gs.cfg < TYPE_CNT) && (gs.pin <= GPIO_PORT_PIN_MAX) && (gpio_init_count <= GPIO_INIT_MAX))
  if (('A' <= gs->port) && (gs->port <= 'E') && (gs->dir < DIR_CNT) && (gs->cfg < TYPE_CNT) && (gs->pin <= GPIO_PORT_PIN_MAX))
  {
    /* Allocate memory */
    g = malloc(sizeof(gpio_t));

    /* Find base address of register based on character. They're sequentially addressed (A, B, C...) */
    // g->port_reg_addr = (GPIO_TypeDef *)(GPIOA_BASE + ((uint32_t)(g->port - 'A') * 0x400));

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
      g->port_reg_addr->CRL |= (((uint32_t)(g->cfg)) >> 0x01) << ((REG_PIN_CONF_BITLENGTH * g->pin) + REG_CONF_OFFSET); // pin configuration
    }
    else
    {
      g->port_reg_addr->CRH &= ~(0x0F << (REG_PIN_CONF_BITLENGTH * (g->pin - 0x08)));                                            // reset
      g->port_reg_addr->CRH |= ((uint32_t)(g->dir)) << ((REG_PIN_CONF_BITLENGTH * (g->pin - 0x08)) + REG_MODE_OFFSET);           // pin direction / mode
      g->port_reg_addr->CRH |= (((uint32_t)(g->cfg)) >> 0x01) << ((REG_PIN_CONF_BITLENGTH * (g->pin - 0x08)) + REG_CONF_OFFSET); // pin configuration
    }

    /* increment global counter */
    //gpio_init_count++;
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
  if (set_state == PIN_LOW) {
    hw_gpio_reset(gpio);
  }
  if (set_state == PIN_HIGH) {
    hw_gpio_set(gpio);
  }
}

void hw_gpio_set(gpio_t *gpio)
{
    gpio->state = PIN_HIGH;
    gpio->port_reg_addr->BSRR |= 0x01 << gpio->pin;
}

void hw_gpio_reset(gpio_t *gpio)
{
    gpio->state = PIN_LOW;
    gpio->port_reg_addr->BSRR |= 0x01 << (gpio->pin + 0x0F);
}

gpio_state_t hw_gpio_read(gpio_t *gpio)
{
  /* retrieve the GPIO state on function call */
  gpio->state = (gpio_state_t)((gpio->port_reg_addr->IDR) & (0x01 << gpio->pin));
  return gpio->state; 
}

/*
void hw_gpio_callback(void)
{
  static uint8_t idx;

  /* For all logged GPIO retrieve the GPIO state 
  for (idx = 0; idx < gpio_init_count; idx++) {
    gpio_list[idx]->state = (gpio_state)((gpio_list[idx]->port_reg_addr->IDR) & (0x01 << gpio_list[idx]->pin));
  }
}
*/
