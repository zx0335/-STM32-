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
	
	//����ֻ��һ��,�ұ������жϳ�ʼ��֮ǰ
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//��ռ���ȼ�ռ4λ
	
	//ʹ��ʱ��Դ
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	//������ʱ��ݼĴ���
	PWR_BackupAccessCmd(ENABLE);
	
	//ʹ��LSE,32768Hz
	RCC_LSEConfig(RCC_LSE_ON);
	
	//�ȴ�LSE����
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
	
	//ѡ��LSE��ΪRTCʱ��
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
	//ʹ��RTC
	RCC_RTCCLKCmd(ENABLE);
	//�ȴ�ͬ��
	RTC_WaitForSynchro();
	

	//����ʱ��(BCD��)	17:30:10
	RTC_TimeStructure.RTC_Hours = h/10*16+h%10;
	RTC_TimeStructure.RTC_Minutes = m/10*16+m%10;
	RTC_TimeStructure.RTC_Seconds = s/10*16+s%10;
	RTC_TimeStructure.RTC_H12 = RTC_H12_PM;
	RTC_SetTime(RTC_Format_BCD,&RTC_TimeStructure);
	
	//��������   2024/03/16 ������
	RTC_DateStructure.RTC_Year = y/10*16+y%10;
	RTC_DateStructure.RTC_Month = month/10*16+month%10;
	RTC_DateStructure.RTC_Date = d/10*16+d%10;
	RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Saturday;
	RTC_SetDate(RTC_Format_BCD,&RTC_DateStructure);
	
	//����õĲ���д�뵽���ݼĴ�����
	RTC_WriteBackupRegister(RTC_BKP_DR0, 0x1234);
	
	//ck_spre(1Hz) = RTCCLK(LSE) / (uwSynchPrediv + 1) / (uwAsynchPrediv + 1)
	RTC_InitStructure.RTC_AsynchPrediv = 127;//�첽��Ƶ
	RTC_InitStructure.RTC_SynchPrediv = 255;//ͬ����Ƶ
	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;//24Сʱ��ʱ��
	RTC_Init(&RTC_InitStructure);
	
	//����ʱ��Դ�ͼ���ֵ֮ǰ,��Ҫ�رջ����ж�
	RTC_WakeUpCmd(DISABLE);
	
	//RTC����ʱ��Դ
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);//16λ������װ�ؼĴ���
	
	//���ü���ֵΪ1
	RTC_SetWakeUpCounter(0);//�Ĵ����Զ�+1
	
	//ʹ�ܻ��Ѽ�����
	RTC_WakeUpCmd(ENABLE);
	
	//ʹ���ж�ǰҪ����жϱ�־λ,���򲻻ᴥ���ж�
	RTC_ClearITPendingBit(RTC_IT_WUT);
	EXTI_ClearITPendingBit(EXTI_Line22);
	
	//���д��������ж�
	RTC_ITConfig(RTC_IT_WUT,ENABLE);
	
	//����NVIC
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;//�ж�0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//�����ж���22
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;//ָ���ⲿ�ж���22
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//�ж�ģʽ
	//�����ش���(�ɿ�ʱ) EXTI_Trigger_Rising
	//�½��ش���������ʱ��EXTI_Trigger_Falling
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

//�ж���Ӧ����
void RTC_WKUP_IRQHandler(void)
{
	if(RTC_GetITStatus(RTC_IT_WUT) != RESET)
	{
		g_rtc_wup_flag = 1;
		
		//����жϱ�־λ
		RTC_ClearITPendingBit(RTC_IT_WUT);
		EXTI_ClearITPendingBit(EXTI_Line22);
	}
}