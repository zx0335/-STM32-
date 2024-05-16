#ifndef _DHT11_H_
#define _DHT11_H_

#include "stm32f4xx.h"

void init_dht11(void);
void dht11Mode(GPIOMode_TypeDef GPIO_Mode);
u32 dht11Start();
int dht11Data(char* buf);

#endif