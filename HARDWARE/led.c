#include "led.h"

void init_led(void)
{
	//初始化四个引脚
	GPIO_InitTypeDef GPIO_Initstructure;
	
	//开启硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOE,ENABLE);
	
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_OUT;//输出模式
	GPIO_Initstructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_100MHz;//输出速度，高速
	GPIO_Initstructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
	
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_Init(GPIOF,&GPIO_Initstructure);
	
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOE,&GPIO_Initstructure);
	//初始化为高电平
	GPIO_SetBits(GPIOF,GPIO_Pin_9);
	GPIO_SetBits(GPIOF,GPIO_Pin_10);
	GPIO_SetBits(GPIOE,GPIO_Pin_13);
	GPIO_SetBits(GPIOE,GPIO_Pin_14);
}
