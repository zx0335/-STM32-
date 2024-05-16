#ifndef _RTC_H_
#define _RTC_H_

#include "stm32f4xx.h"

extern u32 g_rtc_wup_flag;
void init_rtc(int h,int m,int s,int y,int month,int d);
void rtc_getdatatime(char* buf);
#endif