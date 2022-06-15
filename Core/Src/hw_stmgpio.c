/*
GPIO library designed to be relatively lightwieght but human readable.
Very much not memory-safe! libraries using this library must manage assigned GPIO.
*/
#include "hw_stmgpio.h"

/*  */
int stmgpio_setup_gpio(stmgpio_t *g)
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