#include "led.h"

void init_led(void)
{
	//��ʼ���ĸ�����
	GPIO_InitTypeDef GPIO_Initstructure;
	
	//����Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOE,ENABLE);
	
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_OUT;//���ģʽ
	GPIO_Initstructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_100MHz;//����ٶȣ�����
	GPIO_Initstructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//��������
	
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_Init(GPIOF,&GPIO_Initstructure);
	
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOE,&GPIO_Initstructure);
	//��ʼ��Ϊ�ߵ�ƽ
	GPIO_SetBits(GPIOF,GPIO_Pin_9);
	GPIO_SetBits(GPIOF,GPIO_Pin_10);
	GPIO_SetBits(GPIOE,GPIO_Pin_13);
	GPIO_SetBits(GPIOE,GPIO_Pin_14);
}
