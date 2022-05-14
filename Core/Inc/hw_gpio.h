/* Recursion prevention */

#ifndef __HW_GPIO_H
#define __HW_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif


/* Includes */

#include "stm32f103xb.h"
#include "hw_systick.h"
#include "stdbool.h"

/* Handy defnitions */

//#define GPIO_INIT_MAX 80

#define GPIO_PORT_PIN_MAX 15

#define REG_PIN_CONF_BITLENGTH 0x04
#define REG_MODE_OFFSET 0x00
#define REG_CONF_OFFSET 0x02

/* Enumerated types and typedefs */

typedef enum {
  PIN_LOW,
  PIN_HIGH,
  STATE_CNT
} gpio_state;

typedef enum {
  INPUT,
  OUTPUT_10MHZ,
  OUTPUT_2MHZ,
  OUTPUT_50MHz,
  DIR_CNT
} gpio_dir;

typedef enum {
  IN_ANALOGUE,
  IN_FLOATING,
  IN_PULL,
  RSVD,
  OUT_PUSHPULL,
  OUT_OPENDRAIN,
  OUT_ALT_PUSHPULL,
  OUT_ALT_OPENDRAIN,
  TYPE_CNT
} gpio_cfg;

typedef struct GPIOS {
   gpio_state state;
   gpio_dir dir;
   gpio_cfg cfg;
   char port;
   uint8_t pin;
   GPIO_TypeDef *port_reg_addr;
} GPIO;

typedef struct GPIO_SETUPS {
   gpio_dir dir;
   gpio_cfg cfg;
   char port;
   uint8_t pin;
} GPIO_SETUP;


/* Variables */

//static uint8_t gpio_init_count = 0;

//static bool hw_gpio_initialised = false;

//static GPIO *gpio_list[GPIO_INIT_MAX];

/* Function prototypes */

//void hw_gpio_init(void);

void hw_gpio_free_memory(GPIO *gpio);

GPIO* hw_gpio_setup_gpio(GPIO_SETUP gs);

void hw_gpio_write(GPIO* gpio, gpio_state set_state);

void hw_gpio_set(GPIO *gpio);

void hw_gpio_reset(GPIO *gpio);

gpio_state hw_gpio_read(GPIO* gpio);

//void hw_gpio_callback(void);

#ifdef __cplusplus
}
#endif

#endif
