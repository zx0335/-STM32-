#include "delay.h"

void init_delay(void)
{
	//ѡ��ϵͳ��ʱ����ʱ��Դ���Ƽ���SysTick_CLKSource_HCLK_Div8����ʱʱ��Ƚϳ���
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);//168MHz/8=21MHz
	//21MHz��Ƶ�ʣ���ζ�ż���21000000����1s������21000����1ms,����21����1us
	//21M ��ʱ��һ������
}

//���뼶��ʱ
void delay_ms(u32 nms)
{
	vTaskDelay(nms);
}

//΢�뼶��ʱ
void delay_us(u32 nus)
{
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=SysTick->LOAD;	//ϵͳ��ʱ��������ֵ	    	 
	ticks=nus*(SystemCoreClock/1000000);//��Ҫ�Ľ����� 
	told=SysTick->VAL;        	//�ս���ʱ�ļ�����ֵ
	/* ���������[��ѡ,�ᵼ�¸����ȼ������޷���ռ��ǰ���񣬵��ܹ���ߵ�ǰ����ʱ��ľ�ȷ��] */
	vTaskSuspendAll();	
	while(1)
	{
		tnow=SysTick->VAL;
		if(tnow!=told)
		{	 
			/* SYSTICK��һ���ݼ��ļ����� */
			if(tnow<told)
				tcnt+=told-tnow;		
			else 
				tcnt+=reload-tnow+told;	  
			told=tnow;
			/* ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳���*/
			if(tcnt>=ticks)
				break;			
		}  
	}
	/* �ָ�������[��ѡ] */
	xTaskResumeAll();
}
