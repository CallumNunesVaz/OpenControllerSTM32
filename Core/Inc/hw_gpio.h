/* Recursion prevention */

#ifndef __HW_gpio_t_H
#define __HW_gpio_t_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes */

#include "stm32f103xb.h"
#include "hw_systick.h"
#include <stdbool.h>
#include <stdlib.h>

/* Handy defnitions */

//#define gpio_t_INIT_MAX 80

#define GPIO_PORT_PIN_MAX 15

#define REG_PIN_CONF_BITLENGTH 0x04
#define REG_MODE_OFFSET 0x00
#define REG_CONF_OFFSET 0x02

/* Enumerated types and typedefs */

typedef enum gpio_state {
  PIN_LOW,
  PIN_HIGH,
  STATE_CNT
} gpio_state_t;

typedef enum gpio_dir {
  INPUT,
  OUTPUT_10MHZ,
  OUTPUT_2MHZ,
  OUTPUT_50MHz,
  DIR_CNT
} gpio_dir_t;

typedef enum gpio_cfg {
  IN_ANALOGUE,
  IN_FLOATING,
  IN_PULL,
  RSVD,
  OUT_PUSHPULL,
  OUT_OPENDRAIN,
  OUT_ALT_PUSHPULL,
  OUT_ALT_OPENDRAIN,
  TYPE_CNT
} gpio_cfg_t;

typedef struct gpio {
   gpio_state_t state;
   gpio_dir_t dir;
   gpio_cfg_t cfg;
   char port;
   uint8_t pin;
   GPIO_TypeDef *port_reg_addr;
} gpio_t;

typedef struct gpio_setup {
   gpio_dir_t dir;
   gpio_cfg_t cfg;
   char port;
   uint8_t pin;
} gpio_setup_t;

/* Function prototypes */

void hw_gpio_free_memory(gpio_t *gpio_t);

gpio_t* hw_gpio_setup_gpio(gpio_setup_t *gs);

void hw_gpio_write(gpio_t* gpio_t, gpio_state_t set_state);

inline void hw_gpio_set(gpio_t *gpio_t);

inline void hw_gpio_reset(gpio_t *gpio_t);

gpio_state_t hw_gpio_read(gpio_t* gpio_t);

#ifdef __cplusplus
}
#endif

#endif
