#ifndef _USART_H_
#define _USART_H_

#include "stm32f4xx.h"
#include "stdio.h"

extern char g_usart1_recvData[100];
extern u32 g_usart1_flag;

void init_usart1(u32 baud);
void send_string(char* str);
int fputc(int ch,FILE* fp);

#endif