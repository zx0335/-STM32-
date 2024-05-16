#include "usart.h"
#include "sys.h"
#include "stdarg.h"	 

char g_usart1_recvData[100];//��������
u32 usart1_recv_len = 0;//����
u32 g_usart1_flag = 0;//��ɽ������ݱ�־

void init_usart1(u32 baud)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//����Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA ,ENABLE);
	//ʹ�ܴ���1��Ӳ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 ,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//����ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//����ٶȣ�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//ָ�����ù���Ϊ����
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);
	
	//���ô��ڲ���
	USART_InitStructure.USART_BaudRate = baud;//������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//����λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//У��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//Ӳ������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1,&USART_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;//��Ӧ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//�����ڴ����ж�(�������ݼĴ�����Ϊ���ж�)
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	//ʹ�ܴ��ڹ���
	USART_Cmd(USART1,ENABLE);
}

void USART1_IRQHandler(void)//�����ַ���
{
	uint16_t data;
	//�ж��Ƿ�����ж�
	if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
	{
		//data = 0;
		data = USART_ReceiveData(USART1);//�Ӵ���1��������
		
		g_usart1_recvData[usart1_recv_len++] = data;// hello\r\n => hello\0\n
		if(usart1_recv_len >= 100 || data == '\n')
		{
			g_usart1_flag = 1;
			g_usart1_recvData[usart1_recv_len-2] = '\0';
			usart1_recv_len=0;
			
			//��֤�Ƿ��յ�����
			send_string(g_usart1_recvData);
		}
		
		//����жϱ�־λ
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
	}
}

void send_string(char* str)//�����ַ���
{
	while(str && *str)//str!=NULL && *str!='\0'
	{
		USART_SendData(USART1,*str++);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	}
}

//�ض���,��ʵ���ڴ�����ʹ��printf
int fputc(int ch,FILE* fp)
{
	USART_SendData(USART1,ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)!=SET);
	
	return ch;
}
