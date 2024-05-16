#include "usart.h"
#include "sys.h"
#include "stdarg.h"	 

char g_usart1_recvData[100];//保存数据
u32 usart1_recv_len = 0;//长度
u32 g_usart1_flag = 0;//完成接收数据标志

void init_usart1(u32 baud)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//开启硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA ,ENABLE);
	//使能串口1的硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 ,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//输出速度，高速
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//指定复用功能为串口
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	//配置串口参数
	USART_InitStructure.USART_BaudRate = baud;//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流控
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1,&USART_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//允许串口触发中断(接受数据寄存器不为空中断)
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	//使能串口工作
	USART_Cmd(USART1,ENABLE);
}

void USART1_IRQHandler(void)//接收字符串
{
	uint16_t data;
	//判断是否产生中断
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{
		//data = 0;
		data = USART_ReceiveData(USART1);//从串口1接收数据
		
		g_usart1_recvData[usart1_recv_len++] = data;// hello\r\n => hello\0\n
		if(usart1_recv_len >= 100 || data == '\n')
		{
			g_usart1_flag = 1;
			g_usart1_recvData[usart1_recv_len-2] = '\0';
			usart1_recv_len=0;
			
			//验证是否收到数据
			send_string(g_usart1_recvData);
		}
		
		//清空中断标志位
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
}

void send_string(char* str)//发送字符串
{
	while(str && *str)//str!=NULL && *str!='\0'
	{
		USART_SendData(USART1,*str++);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	}
}

//重定向,以实现在串口中使用printf
int fputc(int ch,FILE* fp)
{
	USART_SendData(USART1,ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)!=SET);
	
	return ch;
}
