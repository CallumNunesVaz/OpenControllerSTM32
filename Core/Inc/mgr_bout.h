#ifndef __MGR_BOUT_H
#define __MGR_BOUT_H

#include "hw_stmgpio.h"
#include "hw_systick.h"
#include <stdint.h>
#include <stdbool.h>

/* Pin defintions */
#define PIN_BOUT_0 15
#define PORT_BOUT_0 'A'
#define PIN_BOUT_1 0
#define PORT_BOUT_1 'B'
#define PIN_BOUT_2 1
#define PORT_BOUT_2 'B'
#define PIN_BOUT_3 3
#define PORT_BOUT_3 'B'
#define PIN_BOUT_4 4
#define PORT_BOUT_4 'B'
#define PIN_BOUT_5 5
#define PORT_BOUT_5 'B'
#define PIN_BOUT_6 11
#define PORT_BOUT_6 'B'
#define PIN_BOUT_7 6
#define PORT_BOUT_7 'B'

#define BOUT_CNT 8

int bout_init(void);

void bout_reset_lib(void);

void bout_reset(uint8_t bout_num);

void bout_set(uint8_t bout_num);

void bout_stop(void);

void bout_start(void);

void bout_poll(void);

void bout_tick_callback(void);


#endif