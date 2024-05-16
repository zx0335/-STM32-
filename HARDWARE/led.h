#ifndef _LED_H
#define _LED_H

#include "stm32f4xx.h"

#define PEin(x)	*(volatile uint32_t *)(0x42000000 + (GPIOE_BASE + 0x10 - 0x40000000)*32 + x*4)
#define PEout(x) *(volatile uint32_t *)(0x42000000 + (GPIOE_BASE + 0x14 - 0x40000000)*32 + x*4)
#define PFout(x) *(volatile uint32_t *)(0x42000000 + (GPIOF_BASE + 0x14 - 0x40000000)*32 + x*4)
#define PAin(x)	*(volatile uint32_t *)(0x42000000 + (GPIOA_BASE + 0x10 - 0x40000000)*32 + x*4)
#define PAout(x) *(volatile uint32_t *)(0x42000000 + (GPIOA_BASE + 0x14 - 0x40000000)*32 + x*4)

void init_led(void);

#endif
