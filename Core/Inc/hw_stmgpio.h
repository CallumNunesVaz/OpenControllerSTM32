/* Recursion prevention */

#ifndef __HW_STMGPIO_H
#define __HW_STMGPIO_H

/* Includes */

#include "stm32f103xb.h"
#include "hw_systick.h"
#include "util_error.h"
#include <stdbool.h>
#include <stdlib.h>

/* Handy defnitions */

#define GPIO_PORT_PIN_MAX 16

#define REG_PIN_CONF_BITLENGTH 0x04
#define REG_PIN_CONF_BITLENGTH_Msk 0x0F
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
  INPUT = 0b00,
  OUTPUT_10MHZ = 0b01,
  OUTPUT_2MHZ = 0b10,
  OUTPUT_50MHZ = 0b11,
  DIR_CNT = 0x04
} stmgpio_dir_t;

typedef enum stmgpio_pull {
  PULLDOWN,
  PULLUP,
  PULL_CNT
} stmgpio_pull_t;

typedef enum stmgpio_cfg {
  IN_ANALOGUE = 0b0000,
  IN_FLOATING = 0b0100,
  IN_PULL = 0b1000,
  //RSVD = 0b1100, // Removed to avoid conflict
  OUT_PUSHPULL = 0b0000,
  OUT_OPENDRAIN = 0b0100,
  OUT_ALT_PUSHPULL = 0b1000,
  OUT_ALT_OPENDRAIN = 0b1100,
  TYPE_CNT = 0x08
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

//bool stmgpio_can_init(stmgpio_t *g);

stmgpio_state_t stmgpio_read(stmgpio_t* gpio_t);

#endif
