#include "i2c.h"
#include "delay.h"
#include "stdio.h"

void init_i2c()
{
	GPIO_InitTypeDef GPIO_Initstructure;
	
	//����Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_OUT;//���
	GPIO_Initstructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_100MHz;//����ٶȣ�����
	GPIO_Initstructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//��������
	//PB8=>SCL  PB9=>SDA
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOB,&GPIO_Initstructure);
	
	//����ʱ,ʱ���ߺ������߶��Ǹߵ�ƽ
	SCL = 1;
	SDAOUT = 1;
}

//ģʽ�л�
void i2cMode(GPIOMode_TypeDef GPIO_Mode)
{
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode;
	GPIO_Initstructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_100MHz;//����ٶȣ�����
	GPIO_Initstructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//��������
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOB,&GPIO_Initstructure);
}

//��ʼ�ź�
void i2cStart()
{
	i2cMode(GPIO_Mode_OUT);
	
	SCL = 1;
	SDAOUT = 1;
	delay_us(5);
	
	SDAOUT = 0;//����������,��ʼ�ź�
	delay_us(5);
	
	SCL = 0;//׼����������,ʱ���ߴ��ڵ͵�ƽ
	delay_us(5);
}

//ֹͣ�ź�
void i2cStop()
{
	i2cMode(GPIO_Mode_OUT);
	
	SCL = 0;
	SDAOUT = 0;
	delay_us(5);
	
	SCL = 1;//������ʱ����
	delay_us(5);
	
	SDAOUT = 1;//����������,��ֹ�ź�
	delay_us(5);
}

//��������
void i2cSend(u8 data)
{
	i2cMode(GPIO_Mode_OUT);
	
	for(int i=0;i<8;i++)
	{	
		SCL = 0;//����ʱ����,׼������(���ݿɸ���)
		delay_us(5);
		
		if(data & (0x1<<(7-i)))
			SDAOUT = 1;
		else 
			SDAOUT = 0;
		delay_us(5);
		
		SCL = 1;//����ʱ����,���ߴӻ����Զ�����(������Ϊ�����ź�)
		delay_us(5);
	}	
	
	SCL = 0;//����ʱ����,׼������(�ӻ��ɸ�������)
	delay_us(5);
}

//�ȴ�Ӧ��
u8 waitAck()
{
	i2cMode(GPIO_Mode_IN);//�л�Ϊ����ģʽ
	
	//����ʱ����,��ȡ����
	SCL = 1;
	delay_us(5);
	
	u8 ack;
	if(SDAIN == 1)//��ЧӦ��
		ack = 1;
	else 		  //��ЧӦ��
		ack = 0;
	
	//����ʱ����
	SCL = 0;
	delay_us(5);
	//printf("ack = %d\n",ack);
	
	return ack;
}

//������
u8 i2cRead()
{
	i2cMode(GPIO_Mode_IN);//�л�Ϊ����ģʽ
	
	u8 recv;
	for(int i=0;i<8;i++)
	{
		SCL = 1;//����ʱ����
		delay_us(5);
		
		if(SDAIN == 1)
			recv |= (0x1<<(7-i));
		else 
			recv &= ~(0x1<<(7-i));
		
		SCL = 0;
		delay_us(5);
	}
	return recv;
}

//����Ӧ��
void sendAck()
{
	i2cMode(GPIO_Mode_OUT);
	
	SDAOUT = 0;//����Ӧ��
	delay_us(5);
	
	SCL = 1;//����ʱ����,���ߴӻ����Զ�����
	delay_us(5);
	
	SCL = 0;//����ʱ����
	delay_us(5);
}