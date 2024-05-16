#include "sys.h"
#include "usart.h"
#include "usart2.h"

#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	 


/******���ڶ�*******/
void Usart2_Init(unsigned int baud)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//����Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA ,ENABLE);
	//ʹ�ܴ���2��Ӳ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 ,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//����ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//����ٶȣ�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//ָ�����ù���Ϊ����
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);
	
	//���ô��ڲ���
	USART_InitStructure.USART_BaudRate = baud;//������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//����λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//У��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//Ӳ������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2,&USART_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//�����ڴ����ж�(�������ݼĴ�����Ϊ���ж�)
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	
	//ʹ�ܴ��ڹ���
	USART_Cmd(USART2,ENABLE);
}


void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len)
{
	unsigned short count = 0;
	
	for(; count < len; count++)
	{
		
		USART_SendData(USARTx, *str++);		
		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);		//�ȴ��������
	}
}


/*******����һ��16λ��*************/
//void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
//{
//	uint8_t temp_h, temp_l;
//	
//	/* ȡ���߰�λ */
//	temp_h = (ch&0XFF00)>>8;
//	/* ȡ���Ͱ�λ */
//	temp_l = ch&0XFF;
//	
//	/* ���͸߰�λ */
//	USART_SendData(pUSARTx,temp_h);	
//	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
//	
//	/* ���͵Ͱ�λ */
//	USART_SendData(pUSARTx,temp_l);	
//	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
//}

/*****************  ����һ���ַ� **********************/
//void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
//{
//	/* ����һ���ֽ����ݵ�USART */
//	USART_SendData(pUSARTx,ch);
//		
//	/* �ȴ��������ݼĴ���Ϊ�� */
//	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
//}


/*****************  �����ַ��� **********************/
//void Usart_SendString2( USART_TypeDef * pUSARTx, char *str)
//{
//	unsigned int k=0;
//  do 
//  {
//      Usart_SendByte( pUSARTx, *(str + k) );
//      k++;
//  } while(*(str + k)!='\0');
//  
//  /* �ȴ�������� */
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
//	vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);							//��ʽ��
//	va_end(ap);
//	
//	while(*pStr != 0)
//	{
//		USART_SendData(USARTx, *pStr++);
//		while(USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
//	}

//}
