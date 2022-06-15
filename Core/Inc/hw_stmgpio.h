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

/* Function prototypes */

void stmgpio_free_memory(stmgpio_t *gpio_t);

int stmgpio_setup_gpio(stmgpio_t *gpio_t);

int stmgpio_write(stmgpio_t* gpio_t, stmgpio_state_t state);

bool stmgpio_is_valid(stmgpio_t* gpio_t);

stmgpio_state_t stmgpio_read(stmgpio_t* gpio_t);

#endif
