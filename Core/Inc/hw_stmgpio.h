/* Recursion prevention */

#ifndef __HW_STMGPIO_H
#define __HW_STMGPIO_H

/* Includes */

#include "stm32f103xb.h"
#include "hw_systick.h"
#include <stdbool.h>
#include <stdlib.h>

/* Handy defnitions */

#define GPIO_PORT_PIN_MAX 16

#define REG_PIN_CONF_BITLENGTH 0x04
#define REG_MODE_OFFSET 0x00
#define REG_CONF_OFFSET 0x02

#define gpio_reset(g) stmgpio_write(g, PIN_LOW)

#define gpio_set(g) stmgpio_write(g, PIN_HIGH)

/* Enumerated types and typedefs */

typedef enum stmgpio_state {
  PIN_HIGH,
  PIN_LOW,
  STATE_CNT
} stmgpio_state_t;

typedef enum stmgpio_dir {
  INPUT,
  OUTPUT_10MHZ,
  OUTPUT_2MHZ,
  OUTPUT_50MHz,
  DIR_CNT
} stmgpio_dir_t;

typedef enum stmgpio_pull {
  PULLDOWN,
  PULLUP,
  PULL_CNT
} stmgpio_pull_t;

typedef enum stmgpio_cfg {
  IN_ANALOGUE,
  IN_FLOATING,
  IN_PULL,
  RSVD,
  OUT_PUSHPULL,
  OUT_OPENDRAIN,
  OUT_ALT_PUSHPULL,
  OUT_ALT_OPENDRAIN,
  TYPE_CNT
} stmgpio_cfg_t;

typedef struct stmgpio {
   stmgpio_state_t state;
   stmgpio_dir_t dir;
   stmgpio_pull_t pull;
   stmgpio_cfg_t cfg;
   char port;
   uint8_t pin;
   GPIO_TypeDef *port_reg_addr;
} stmgpio_t;

/* Function prototypes */

void stmgpio_free_memory(stmgpio_t *gpio_t);

int stmgpio_setup(stmgpio_t *gpio_t);

int stmgpio_write(stmgpio_t* gpio_t, stmgpio_state_t state);

bool stmgpio_can_init(stmgpio_t *g);

stmgpio_state_t stmgpio_read(stmgpio_t* gpio_t);

#endif
