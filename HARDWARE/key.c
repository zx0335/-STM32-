#include "key.h"
#include "stm32f4xx.h"

void init_key(void)
{
	GPIO_InitTypeDef GPIO_Initstructure;
	
	//����Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOE,ENABLE);
	
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_IN;//����ģʽ
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_100MHz;//����ٶȣ�����
	GPIO_Initstructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//��������
	
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA,&GPIO_Initstructure);
	
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_Init(GPIOE,&GPIO_Initstructure);
}
