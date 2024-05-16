#ifndef __ESP8266_H
#define __ESP8266_H
#include "sys.h"
#include "stdio.h"	
#include "stm32f4xx.h"

#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标志
 
//函数声明
u8* ESP8266_GetIPD_GET(unsigned short timeOut,u8 *buff);
 
void ESP8266_Clear(void);
int ESP8266_WaitRecive(void);
 
int ESP8266_SendCmd(char *cmd, char *res);
void ESP8266_SendData(unsigned char *data, unsigned short len);
 
void ESP8266_Init(void);
void Get_current_time(void);
void Get_current_Weather(void);
void Get_current_Live(void);
void find_and_print_value(char *str, char *key);
//解析苏宁返回数据
void cJSON_Time_Parse(void);
int Get_Year(char *y);
int Get_Moonth(char *m);
int Get_Day(char *d);
int Get_Times(char *h, char *m, char *s);

void getWeather(char* str,int* len,int* n);
void getLive(char str[3][20],int* num);
#endif