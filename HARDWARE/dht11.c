#include "dht11.h"
#include "sys.h"
#include "delay.h"

void init_dht11(void)
{
	GPIO_InitTypeDef GPIO_Initstructure;
	
	//开启硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
	
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_OUT;//输出模式
	GPIO_Initstructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_100MHz;//输出速度，高速
	GPIO_Initstructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
	
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOG,&GPIO_Initstructure);
}

//模式切换
void dht11Mode(GPIOMode_TypeDef GPIO_Mode)
{
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode;
	GPIO_Initstructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_100MHz;//输出速度，高速
	GPIO_Initstructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOG,&GPIO_Initstructure);
}

//起始信号
u32 dht11Start()
{
	//stm32发出信号
	dht11Mode(GPIO_Mode_OUT);//主机输出模式
	PGout(9) = 0;
	delay_ms(20);//持续至少 18 ms
	
	PGout(9) = 1;
	delay_us(30);//持续 20-40 us
	
	//dht11响应信号
	dht11Mode(GPIO_Mode_IN);//主机输入模式
	/*不判断dht11的响应,不精确
	while(PGin(9) == 0);
	while(PGin(9) == 1);*/
	u32 i=0;
	while(1)
	{
		delay_us(1);
		i++;
		if(PGin(9) == 0)//低电平
			break;
		if(i>=100)
			return 0;
	}
	
	i=0;
	while(1)
	{
		delay_us(1);
		i++;
		if(PGin(9) == 1)//高电平
			break;
		if(i>=100)
			return 0;
	}
	
	return 1;
}

//读取1个字节的数据
u8 dht11_readByte()
{
	u32 i = 0;
	u8 data = 0;
	
	while(PGin(9) == 1);//等待低电平出现
	for(int i=0;i<8;i++)
	{
		while(PGin(9) == 0);//等待低电平持续完毕,高电平出现
		
		delay_us(40);
		if(PGin(9) == 1)//bit = 1
		{
			data |= 1<<(7-i);//高位先出
			while(PGin(9) == 1);//把剩余高电平消耗完毕
		}
		else
		{
			data &= ~(0<<(7-i));
		}
	}
	return data;
}

//读取温湿度
int dht11Data(char* buf)
{
	while(dht11Start() != 1);
	for(int i=0;i<5;i++)
	{
		buf[i] = dht11_readByte();
	}
	
	//判断校验和
	if(buf[4] != buf[0] + buf[1] + buf[2] + buf[3])
		return -1;
	return 1;
}
