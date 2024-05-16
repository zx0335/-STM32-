#include "sys.h"
#include "usart.h"
#include "usart2.h"

#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 


/******串口二*******/
void Usart2_Init(unsigned int baud)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//开启硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA ,ENABLE);
	//使能串口2的硬件时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 ,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//输出速度，高速
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//指定复用功能为串口
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);
	
	//配置串口参数
	USART_InitStructure.USART_BaudRate = baud;//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流控
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2,&USART_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//响应优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//允许串口触发中断(接受数据寄存器不为空中断)
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	
	//使能串口工作
	USART_Cmd(USART2,ENABLE);
}


void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{
	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		
		USART_SendData(USARTx, *str++);		
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//等待发送完成
	}
}


/*******发送一个16位数*************/
//void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
//{
//	uint8_t temp_h, temp_l;
//	
//	/* 取出高八位 */
//	temp_h = (ch&0XFF00)>>8;
//	/* 取出低八位 */
//	temp_l = ch&0XFF;
//	
//	/* 发送高八位 */
//	USART_SendData(pUSARTx,temp_h);	
//	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
//	
//	/* 发送低八位 */
//	USART_SendData(pUSARTx,temp_l);	
//	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
//}

/*****************  发送一个字符 **********************/
//void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
//{
//	/* 发送一个字节数据到USART */
//	USART_SendData(pUSARTx,ch);
//		
//	/* 等待发送数据寄存器为空 */
//	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
//}


/*****************  发送字符串 **********************/
//void Usart_SendString2( USART_TypeDef * pUSARTx, char *str)
//{
//	unsigned int k=0;
//  do 
//  {
//      Usart_SendByte( pUSARTx, *(str + k) );
//      k++;
//  } while(*(str + k)!='\0');
//  
//  /* 等待发送完成 */
//  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
//  {}
//}

//void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...)
//{

//	unsigned char UsartPrintfBuf[296];
//	va_list ap;
//	unsigned char *pStr = UsartPrintfBuf;
//	
//	va_start(ap, fmt);
//	vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//格式化
//	va_end(ap);
//	
//	while(*pStr != 0)
//	{
//		USART_SendData(USARTx, *pStr++);
//		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
//	}

//}
