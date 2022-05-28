/*
GPIO library designed to be relatively lightwieght but human readable.
Very much not memory-safe! libraries using this library must manage assigned GPIO.
*/
#include "hw_stmgpio.h"

static const bool PORT_A_PINS[GPIO_PORT_PIN_MAX] = {
  true, // PA0
  true, // PA1
  true, // PA2
  true, // PA3
  true, // PA4
  true, // PA5
  true, // PA6
  true, // PA7
  true, // PA8
  true, // PA9
  true, // PA10
  true, // PA11
  true, // PA12
  true, // PA13
  true, // PA14
  true  // PA15
};

static const bool PORT_B_PINS[GPIO_PORT_PIN_MAX] = {
  true, // PB0
  true, // PB1
  true, // PB2
  true, // PB3
  true, // PB4
  true, // PB5
  true, // PB6
  true, // PB7
  true, // PB8
  true, // PB9
  true, // PB10
  true, // PB11
  true, // PB12
  true, // PB13
  true, // PB14
  true  // PB15
};

static const bool PORT_C_PINS[GPIO_PORT_PIN_MAX] = {
  false,// PC0
  false,// PC1
  false,// PC2
  false,// PC3
  false,// PC4
  false,// PC5
  false,// PC6
  false,// PC7
  false,// PC8
  false,// PC9
  false,// PC10
  false,// PC11
  false,// PC12
  true, // PC13
  true, // PC14
  true  // PC15
};

static const bool PORT_D_PINS[GPIO_PORT_PIN_MAX] = {
  true, // PD0
  true, // PD1
  false,// PD2
  false,// PD3
  false,// PD4
  false,// PD5
  false,// PD6
  false,// PD7
  false,// PD8
  false,// PD9
  false,// PD10
  false,// PD11
  false,// PD12
  false,// PD13
  false,// PD14
  false // PD15
};


stmgpio_t *stmgpio_setup_gpio(stmgpio_setup_t* gs)
{
  /* Allocate variable */
  stmgpio_t *g = NULL;

  /* Sanity checks */
  if ((('A' <= gs->port) && (gs->port <= 'E')) && (gs->dir < DIR_CNT) && (gs->cfg < TYPE_CNT) && (gs->pin <= GPIO_PORT_PIN_MAX))
  {
    /* Allocate memory */
    g = malloc(sizeof(stmgpio_t));

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

void stmgpio_free_memory(stmgpio_t *gpio)
{
  /* Free memory */
  free(gpio);
}

void stmgpio_write(stmgpio_t *gpio, stmgpio_state_t set_state)
{
  if (PIN_LOW == set_state) {
    gpio->state = PIN_LOW;
    gpio->port_reg_addr->BSRR = (0x01 << (uint8_t)(gpio->pin) << 0x10);
  }
  if (PIN_HIGH == set_state) {
    gpio->state = PIN_HIGH;
    gpio->port_reg_addr->BSRR = (0x01 << (uint8_t)(gpio->pin));
  }
}

bool stmgpio_is_valid_port_pin(char port, uint8_t pin)
{
  bool is_valid = false;

  /* capitalise port char if not */
  if (('a' <= port) && ('d' >= port))
  {
    port -= 32;
  }

  /* Check port ledger for pin existence */
  if (GPIO_PORT_PIN_MAX <= pin)
  {
    switch (port)
    {
    case 'A':
      is_valid = PORT_A_PINS[pin];
      break;
    case 'B':
      is_valid = PORT_B_PINS[pin];
      break;
    case 'C':
      is_valid = PORT_C_PINS[pin];
      break;
    case 'D':
      is_valid = PORT_D_PINS[pin];
      break;
    default:
      is_valid = false;
      break;
    }
  }

  return is_valid;
}

stmgpio_state_t stmgpio_read(stmgpio_t *gpio)
{
  /* retrieve the GPIO state on function call */
  gpio->state = (stmgpio_state_t)((gpio->port_reg_addr->IDR) & (0x01 << gpio->pin));
  return gpio->state; 
}