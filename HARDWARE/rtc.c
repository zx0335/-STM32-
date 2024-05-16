#include "rtc.h"
#include "stdio.h"

u32 g_rtc_wup_flag = 0;

void init_rtc(int h,int m,int s,int y,int month,int d)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_InitTypeDef RTC_InitStructure;
	RTC_DateTypeDef RTC_DateStructure;
	
	//分组只需一次,且必须在中断初始化之前
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//抢占优先级占4位
	
	//使能时钟源
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	//允许访问备份寄存器
	PWR_BackupAccessCmd(ENABLE);
	
	//使能LSE,32768Hz
	RCC_LSEConfig(RCC_LSE_ON);
	
	//等待LSE就绪
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
	
	//选择LSE作为RTC时钟
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
	//使能RTC
	RCC_RTCCLKCmd(ENABLE);
	//等待同步
	RTC_WaitForSynchro();
	

	//设置时间(BCD码)	17:30:10
	RTC_TimeStructure.RTC_Hours = h/10*16+h%10;
	RTC_TimeStructure.RTC_Minutes = m/10*16+m%10;
	RTC_TimeStructure.RTC_Seconds = s/10*16+s%10;
	RTC_TimeStructure.RTC_H12 = RTC_H12_PM;
	RTC_SetTime(RTC_Format_BCD,&RTC_TimeStructure);
	
	//设置日期   2024/03/16 星期六
	RTC_DateStructure.RTC_Year = y/10*16+y%10;
	RTC_DateStructure.RTC_Month = month/10*16+month%10;
	RTC_DateStructure.RTC_Date = d/10*16+d%10;
	RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Saturday;
	RTC_SetDate(RTC_Format_BCD,&RTC_DateStructure);
	
	//将配好的参数写入到备份寄存器中
	RTC_WriteBackupRegister(RTC_BKP_DR0, 0x1234);
	
	//ck_spre(1Hz) = RTCCLK(LSE) / (uwSynchPrediv + 1) / (uwAsynchPrediv + 1)
	RTC_InitStructure.RTC_AsynchPrediv = 127;//异步分频
	RTC_InitStructure.RTC_SynchPrediv = 255;//同步分频
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;//24小时计时法
	RTC_Init(&RTC_InitStructure);
	
	//配置时钟源和计数值之前,需要关闭唤醒中断
	RTC_WakeUpCmd(DISABLE);
	
	//RTC唤醒时钟源
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);//16位唤醒重装载寄存器
	
	//设置计数值为1
	RTC_SetWakeUpCounter(0);//寄存器自动+1
	
	//使能唤醒计数器
	RTC_WakeUpCmd(ENABLE);
	
	//使能中断前要清空中断标志位,否则不会触发中断
	RTC_ClearITPendingBit(RTC_IT_WUT);
	EXTI_ClearITPendingBit(EXTI_Line22);
	
	//运行触发唤醒中断
	RTC_ITConfig(RTC_IT_WUT,ENABLE);
	
	//配置NVIC
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;//中断0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//配置中断线22
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;//指定外部中断线22
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//中断模式
	//上升沿触发(松开时) EXTI_Trigger_Rising
	//下降沿触发（按下时）EXTI_Trigger_Falling
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising ;	
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

void rtc_getdatatime(char* buf)
{
	RTC_DateTypeDef RTC_DateStruct;
	RTC_TimeTypeDef RTC_TimeStruct;
	
	RTC_GetDate(RTC_Format_BCD,&RTC_DateStruct);
	RTC_GetTime(RTC_Format_BCD,&RTC_TimeStruct);
	
	sprintf(buf,"%02x/%02x/%02x %02x:%02x:%02x",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date,
		RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds);
}

//中断响应函数
void RTC_WKUP_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_WUT) != RESET)
	{
		g_rtc_wup_flag = 1;
		
		//清空中断标志位
		RTC_ClearITPendingBit(RTC_IT_WUT);
		EXTI_ClearITPendingBit(EXTI_Line22);
	}
}