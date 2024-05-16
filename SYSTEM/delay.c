#include "delay.h"

void init_delay(void)
{
	//选择系统定时器的时钟源【推荐：SysTick_CLKSource_HCLK_Div8，延时时间比较长】
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);//168MHz/8=21MHz
	//21MHz是频率，意味着计数21000000次是1s，计数21000次是1ms,计数21次是1us
	//21M 定时器一个周期
}

//毫秒级延时
void delay_ms(u32 nms)
{
	vTaskDelay(nms);
}

//微秒级延时
void delay_us(u32 nus)
{
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=SysTick->LOAD;	//系统定时器的重载值	    	 
	ticks=nus*(SystemCoreClock/1000000);//需要的节拍数 
	told=SysTick->VAL;        	//刚进入时的计数器值
	/* 挂起调度器[可选,会导致高优先级任务无法抢占当前任务，但能够提高当前任务时间的精确性] */
	vTaskSuspendAll();	
	while(1)
	{
		tnow=SysTick->VAL;
		if(tnow!=told)
		{	 
			/* SYSTICK是一个递减的计数器 */
			if(tnow<told)
				tcnt+=told-tnow;		
			else 
				tcnt+=reload-tnow+told;	  
			told=tnow;
			/* 时间超过/等于要延迟的时间,则退出。*/
			if(tcnt>=ticks)
				break;			
		}  
	}
	/* 恢复调度器[可选] */
	xTaskResumeAll();
}
