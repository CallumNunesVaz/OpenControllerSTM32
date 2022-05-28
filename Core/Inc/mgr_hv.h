#ifndef __MGR_HV_H
#define __MGR_HV_H

#include "hw_stmgpio.h"
#include "hw_systick.h"
#include <stdint.h>
#include <stdbool.h>

/* Pin defintions */
#define HV_EN_PIN 3
#define HV_EN_PORT 'C'

int hv_init(void);

void hv_rail_enable(uint8_t bout_num);

void hv_rail_disable(uint8_t bout_num);

#endif