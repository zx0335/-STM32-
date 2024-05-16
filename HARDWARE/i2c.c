#include "i2c.h"
#include "delay.h"
#include "stdio.h"

void init_i2c()
{
	GPIO_InitTypeDef GPIO_Initstructure;
	
	//开启硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode_OUT;//输出
	GPIO_Initstructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_100MHz;//输出速度，高速
	GPIO_Initstructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
	//PB8=>SCL  PB9=>SDA
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOB,&GPIO_Initstructure);
	
	//空闲时,时钟线和数据线都是高电平
	SCL = 1;
	SDAOUT = 1;
}

//模式切换
void i2cMode(GPIOMode_TypeDef GPIO_Mode)
{
	GPIO_InitTypeDef GPIO_Initstructure;
	GPIO_Initstructure.GPIO_Mode = GPIO_Mode;
	GPIO_Initstructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_Initstructure.GPIO_Speed = GPIO_Speed_100MHz;//输出速度，高速
	GPIO_Initstructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//无上下拉
	GPIO_Initstructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOB,&GPIO_Initstructure);
}

//起始信号
void i2cStart()
{
	i2cMode(GPIO_Mode_OUT);
	
	SCL = 1;
	SDAOUT = 1;
	delay_us(5);
	
	SDAOUT = 0;//数据线拉低,起始信号
	delay_us(5);
	
	SCL = 0;//准备发送数据,时钟线处于低电平
	delay_us(5);
}

//停止信号
void i2cStop()
{
	i2cMode(GPIO_Mode_OUT);
	
	SCL = 0;
	SDAOUT = 0;
	delay_us(5);
	
	SCL = 1;//先拉高时钟线
	delay_us(5);
	
	SDAOUT = 1;//数据线拉高,终止信号
	delay_us(5);
}

//发送数据
void i2cSend(u8 data)
{
	i2cMode(GPIO_Mode_OUT);
	
	for(int i=0;i<8;i++)
	{	
		SCL = 0;//拉低时钟线,准备数据(数据可更改)
		delay_us(5);
		
		if(data & (0x1<<(7-i)))
			SDAOUT = 1;
		else 
			SDAOUT = 0;
		delay_us(5);
		
		SCL = 1;//拉高时钟线,告诉从机可以读数据(数据线为控制信号)
		delay_us(5);
	}	
	
	SCL = 0;//拉低时钟线,准备数据(从机可更改数据)
	delay_us(5);
}

//等待应答
u8 waitAck()
{
	i2cMode(GPIO_Mode_IN);//切换为输入模式
	
	//拉高时钟线,读取数据
	SCL = 1;
	delay_us(5);
	
	u8 ack;
	if(SDAIN == 1)//无效应答
		ack = 1;
	else 		  //有效应答
		ack = 0;
	
	//拉低时钟线
	SCL = 0;
	delay_us(5);
	//printf("ack = %d\n",ack);
	
	return ack;
}

//读数据
u8 i2cRead()
{
	i2cMode(GPIO_Mode_IN);//切换为输入模式
	
	u8 recv;
	for(int i=0;i<8;i++)
	{
		SCL = 1;//拉高时钟线
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

//发送应答
void sendAck()
{
	i2cMode(GPIO_Mode_OUT);
	
	SDAOUT = 0;//发送应答
	delay_us(5);
	
	SCL = 1;//拉高时钟线,告诉从机可以读数据
	delay_us(5);
	
	SCL = 0;//拉低时钟线
	delay_us(5);
}