/*
GPIO library designed to be relatively lightwieght but human readable.
*/
#include "hw_stmgpio.h"


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

const bool PORT_D_PINS[GPIO_PORT_PIN_MAX] = {
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

/*  */
int stmgpio_setup(stmgpio_t *g)
{
  /* Sanity checks */
  if (!stmgpio_can_init(g))
  {
    /* if at first you don't succeed, get it right */
    return EXIT_FAILURE;
  }

  /* Turn on ports clock if not already on */
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN << (g->port - 'A');

  /* Find addr of base reg based on char */
  g->port_reg_addr = (GPIO_TypeDef *)(APB2PERIPH_BASE + (0x00000800UL + (0x00000400UL * (g->port - 'A'))));

  /* Set registers based on GPIO settings */
  if (g->pin < 8)
  {
    /* Reset register */
    g->port_reg_addr->CRL &= ~(0x0F << (REG_PIN_CONF_BITLENGTH * g->pin));
    /* Set pin direction and mode */
    g->port_reg_addr->CRL |= ((uint32_t)(g->dir)) << ((REG_PIN_CONF_BITLENGTH * g->pin) + REG_MODE_OFFSET);
    /* Set pin configuration */
    g->port_reg_addr->CRL |= (((uint32_t)(g->cfg)) & ~(0x04)) << ((REG_PIN_CONF_BITLENGTH * g->pin) + REG_CONF_OFFSET);
  }
  else
  {
    /* Reset register */
    g->port_reg_addr->CRH &= ~(0x0F << (REG_PIN_CONF_BITLENGTH * (g->pin - 0x08)));
    /* Set pin direction and mode */
    g->port_reg_addr->CRH |= ((uint32_t)(g->dir)) << ((REG_PIN_CONF_BITLENGTH * (g->pin - 0x08)) + REG_MODE_OFFSET);
    /* Set pin configuration */
    g->port_reg_addr->CRH |= (((uint32_t)(g->cfg)) & ~(0x04)) << ((REG_PIN_CONF_BITLENGTH * (g->pin - 0x08)) + REG_CONF_OFFSET);
  }

  /* Set pullup or down depending on configuration */
  if (IN_PULL == g->cfg)
  {
    g->port_reg_addr->ODR = ((uint32_t)(g->pull)) << g->pin;
  }

  /* Set default low if output */
  if ((OUT_PUSHPULL == g->cfg) || (OUT_OPENDRAIN == g->cfg))
  {
    stmgpio_write(g, PIN_LOW);
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

bool stmgpio_can_init(stmgpio_t *g)
{
  bool is_valid = false;

  /* capitalise port char if not */
  if (('a' <= g->port) && ('d' >= g->port))
  {
    g->port -= 32;
  }

  /* Check enums for silly errors */
  if ((DIR_CNT >= g->dir) && (TYPE_CNT >= g->cfg) && (PULL_CNT >= g->pull))
  {
    /* Check port ledger for pin existence */
    if (GPIO_PORT_PIN_MAX <= g->pin)
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
}

stmgpio_state_t stmgpio_read(stmgpio_t *gpio)
{
  /* retrieve the GPIO state on function call */
  gpio->state = (stmgpio_state_t)((gpio->port_reg_addr->IDR) & (0x01 << gpio->pin));
  return gpio->state;
}