#include "dht11.h"
#include "sys.h"
#include "delay.h"

void init_dht11(void)
{
	GPIO_InitTypeDef GPIO_Initstructure;
	
	//����Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
	
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_OUT;//���ģʽ
	GPIO_Initstructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_100MHz;//����ٶȣ�����
	GPIO_Initstructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//��������
	
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOG,&GPIO_Initstructure);
}

//ģʽ�л�
void dht11Mode(GPIOMode_TypeDef GPIO_Mode)
{
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode;
	GPIO_Initstructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_100MHz;//����ٶȣ�����
	GPIO_Initstructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//��������
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOG,&GPIO_Initstructure);
}

//��ʼ�ź�
u32 dht11Start()
{
	//stm32�����ź�
	dht11Mode(GPIO_Mode_OUT);//�������ģʽ
	PGout(9) = 0;
	delay_ms(20);//�������� 18 ms
	
	PGout(9) = 1;
	delay_us(30);//���� 20-40 us
	
	//dht11��Ӧ�ź�
	dht11Mode(GPIO_Mode_IN);//��������ģʽ
	/*���ж�dht11����Ӧ,����ȷ
	while(PGin(9) == 0);
	while(PGin(9) == 1);*/
	u32 i=0;
	while(1)
	{
		delay_us(1);
		i++;
		if(PGin(9) == 0)//�͵�ƽ
			break;
		if(i>=100)
			return 0;
	}
	
	i=0;
	while(1)
	{
		delay_us(1);
		i++;
		if(PGin(9) == 1)//�ߵ�ƽ
			break;
		if(i>=100)
			return 0;
	}
	
	return 1;
}

//��ȡ1���ֽڵ�����
u8 dht11_readByte()
{
	u32 i = 0;
	u8 data = 0;
	
	while(PGin(9) == 1);//�ȴ��͵�ƽ����
	for(int i=0;i<8;i++)
	{
		while(PGin(9) == 0);//�ȴ��͵�ƽ�������,�ߵ�ƽ����
		
		delay_us(40);
		if(PGin(9) == 1)//bit = 1
		{
			data |= 1<<(7-i);//��λ�ȳ�
			while(PGin(9) == 1);//��ʣ��ߵ�ƽ�������
		}
		else
		{
			data &= ~(0<<(7-i));
		}
	}
	return data;
}

//��ȡ��ʪ��
int dht11Data(char* buf)
{
	while(dht11Start() != 1);
	for(int i=0;i<5;i++)
	{
		buf[i] = dht11_readByte();
	}
	
	//�ж�У���
	if(buf[4] != buf[0] + buf[1] + buf[2] + buf[3])
		return -1;
	return 1;
}
