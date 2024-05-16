#include "key.h"
#include "stm32f4xx.h"

void init_key(void)
{
	GPIO_InitTypeDef GPIO_Initstructure;
	
	//开启硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOE,ENABLE);
	
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IN;//输入模式
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_100MHz;//输出速度，高速
	GPIO_Initstructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
	
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA,&GPIO_Initstructure);
	
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_Init(GPIOE,&GPIO_Initstructure);
}
