/*
GPIO library designed to be relatively lightwieght but human readable.
*/
#include "hw_stmgpio.h"

/*
const bool PORT_A_PINS[GPIO_PORT_PIN_MAX] = {
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

const bool PORT_B_PINS[GPIO_PORT_PIN_MAX] = {
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

const bool PORT_C_PINS[GPIO_PORT_PIN_MAX] = {
    false, // PC0
    false, // PC1
    false, // PC2
    false, // PC3
    false, // PC4
    false, // PC5
    false, // PC6
    false, // PC7
    false, // PC8
    false, // PC9
    false, // PC10
    false, // PC11
    false, // PC12
    true,  // PC13
    true,  // PC14
    true   // PC15
};

const bool PORT_D_PINS[GPIO_PORT_PIN_MAX] = {
    true,  // PD0
    true,  // PD1
    false, // PD2
    false, // PD3
    false, // PD4
    false, // PD5
    false, // PD6
    false, // PD7
    false, // PD8
    false, // PD9
    false, // PD10
    false, // PD11
    false, // PD12
    false, // PD13
    false, // PD14
    false  // PD15
};
*/

/*  */
int stmgpio_setup(stmgpio_t *g)
{
  __IO uint32_t *CRx;
  uint32_t cfgmode = g->cfg | g->dir;
  uint8_t pin_offset;

  /* Find addr of base reg based on char */
  switch (g->port)
  {
  case 'A':
    g->port_reg_addr = (GPIO_TypeDef *)(GPIOA_BASE);
    break;
  case 'B':
    g->port_reg_addr = (GPIO_TypeDef *)(GPIOB_BASE);
    break;
  case 'C':
    g->port_reg_addr = (GPIO_TypeDef *)(GPIOC_BASE);
    break;
  case 'D':
    g->port_reg_addr = (GPIO_TypeDef *)(GPIOD_BASE);
    break;
  case 'E':
    g->port_reg_addr = (GPIO_TypeDef *)(GPIOE_BASE);
    break;
  default:
    return EXIT_FAILURE;
    break;
  }

  /* get pointer to config / mode register CRH or CRL */
  if (g->pin < 8)
  {
    CRx = &(g->port_reg_addr->CRL);
    pin_offset = 0;
  }
  else
  {
    CRx = &(g->port_reg_addr->CRH);
    pin_offset = 8;
  }

  /* Turn on ports clock if not already on */
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN << (g->port - 'A');           // main port io clock
  if ((OUT_ALT_OPENDRAIN == g->cfg) || OUT_ALT_PUSHPULL == g->cfg) // alt function clock
  {
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
  }

  /* MODE/CFG reg */
  *CRx &= ~(REG_PIN_CONF_BITLENGTH_Msk << (REG_PIN_CONF_BITLENGTH * (g->pin - pin_offset)));
  *CRx |= (g->cfg | g->dir) << (REG_PIN_CONF_BITLENGTH * (g->pin - pin_offset));

  /* Set pullup or down depending if type input-pull */
  if (IN_PULL == g->cfg)
  {
    g->port_reg_addr->ODR = ((uint32_t)(g->pull)) << g->pin;
  }

  /* huzzah */
  return EXIT_SUCCESS;
}

int stmgpio_write(stmgpio_t *g, stmgpio_state_t set_state)
{
  if ((STATE_CNT <= set_state) || (NULL == g))
  {
    return EXIT_FAILURE;
  }

  g->state = set_state;
  g->port_reg_addr->BSRR = (0x01 << g->pin << (((uint8_t)set_state) * 0x10));

  return EXIT_SUCCESS;
}

/*
bool stmgpio_can_init(stmgpio_t *g)
{
  bool is_valid = false;

  // capitalise port char if not
  if (('a' <= g->port) && ('d' >= g->port))
  {
    g->port -= 32;
  }

  // Check enums for silly errors
  if ((DIR_CNT >= g->dir) && (TYPE_CNT >= g->cfg) && (PULL_CNT >= g->pull))
  {
    // Check port ledger for pin existence
    if (GPIO_PORT_PIN_MAX > g->pin)
    {
      switch (g->port)
      {
      case 'A':
        is_valid = PORT_A_PINS[g->pin];
        break;
      case 'B':
        is_valid = PORT_B_PINS[g->pin];
        break;
      case 'C':
        is_valid = PORT_C_PINS[g->pin];
        break;
      case 'D':
        is_valid = PORT_D_PINS[g->pin];
        break;
      default:
        is_valid = false;
        break;
      }
    }
  }

  return is_valid;
}
*/

stmgpio_state_t stmgpio_read(stmgpio_t *gpio)
{
  /* retrieve the GPIO state on function call */
  gpio->state = (stmgpio_state_t)((gpio->port_reg_addr->IDR) & (0x01 << gpio->pin));
  return gpio->state;
}