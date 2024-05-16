#ifndef __DELAY_H__
#define __DELAY_H__
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"

void init_delay(void);
void delay_ms(u32 nms);
void delay_us(u32 nus);

#endif
