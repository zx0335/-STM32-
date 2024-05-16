#include "key.h"
#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "usart2.h"
#include "string.h"
#include "stdio.h"
#include "rtc.h"
#include "bmp.h"
#include "dht11.h"
#include "oled.h"
#include "i2c.h"
#include "esp8266.h"
#include "FreeRTOS.h"
#include "task.h"

TaskHandle_t appTaskHandle;
TaskHandle_t ESP8266Handle;
TaskHandle_t TIMEHandle;
char time[20];
char weather[30];
char live[3][20];
int len = 0;
int num[8] = {0};
int liveNum[6] = {0};
int hour;
int minute;
int second;
int next = 1;
int temper;//温度
int hum;//湿度

void showSport()
{
	OLED_ShowCHinese(20,3,86);//运
	OLED_ShowCHinese(40,3,87);//动
	OLED_ShowCHinese(56,3,40);//:
	OLED_ShowCHinese(72,3,liveNum[2]);//
	OLED_ShowCHinese(88,3,liveNum[3]);//
	
	OLED_ShowCHinese(20,6,90);//护
	OLED_ShowCHinese(40,6,89);//肤
	OLED_ShowCHinese(56,6,40);//:
	OLED_ShowCHinese(72,6,liveNum[0]);//
	OLED_ShowCHinese(88,6,liveNum[1]);//
}

void showDHT11()
{
	char dht11[5] = {0};
	while(dht11Data(dht11)!=1);
	//printf("温度:%d.%d℃,湿度:%d.%dRH\n",dht11[2],dht11[3],dht11[0],dht11[1]);
	//获取温湿度
	temper = dht11[2];
	hum = dht11[0];
	
	OLED_ShowCHinese(16,3,69);//温
	OLED_ShowCHinese(32,3,47);//度
	OLED_ShowCHinese(48,3,40);//:
	OLED_ShowCHinese(64,3,temper/10);//1
	OLED_ShowCHinese(80,3,temper%10);//2
	OLED_ShowCHinese(96,3,65);//℃
	OLED_ShowCHinese(16,6,46);//湿
	OLED_ShowCHinese(32,6,47);//度
	OLED_ShowCHinese(48,6,40);//:
	OLED_ShowCHinese(64,6,hum/10);//1
	OLED_ShowCHinese(80,6,hum%10);//2
	OLED_ShowCHinese(96,6,68);//RH
}

void showWeather()
{
	if(len<=3)
	{
		OLED_ShowCHinese(20,3,53);//天
		OLED_ShowCHinese(40,3,67);//气
		OLED_ShowCHinese(56,3,40);//:
	}
	int temp = 72;
	int temp2 = 48;
	for(int i=0;i<len;i++)//显示天气
	{
		//printf("num[%d] = %d\n",i,num[i]);
		if(len<=3)
		{
			OLED_ShowCHinese(temp,3,num[i]);
			temp+=16;
		}
		else
		{
			OLED_ShowCHinese(0,3,53);//天
			OLED_ShowCHinese(16,3,67);//气
			OLED_ShowCHinese(32,3,40);//:
			OLED_ShowCHinese(temp2,3,num[i]);
			temp2+=16;
		}
	}
	
	//辐射
	OLED_ShowCHinese(20,6,70);//辐
	OLED_ShowCHinese(40,6,71);//射
	OLED_ShowCHinese(56,6,40);//:
	OLED_ShowCHinese(72,6,liveNum[4]);//
	OLED_ShowCHinese(88,6,liveNum[5]);//
}

void ESP8266_DHT11Task(void* arg)//天气和温湿度
{
	memset(weather,0,30);
	Get_current_Weather();//获取天气
	memset(num,0,8);
	getWeather(weather,&len,num);
	printf("天气 = %s\n",weather);
	next = 0;
	
	vTaskDelete(ESP8266Handle);
}

void TIMETask(void* arg)//屏幕上方持续显示时间
{ 	
	char* p[3];
	while(1)
	{
		Get_current_time();
		printf("时间 = %s\n",time);//time ="2024-04-13 23:52:22"
		
		strtok(time,"-");
		strtok(NULL,"-");
		strtok(NULL," ");
		p[0] = strtok(NULL,":");
		p[1] = strtok(NULL,":");
		p[2] = strtok(NULL,":");
		hour = atoi(p[0])%100;
		minute = atoi(p[1]);
		second = atoi(p[2]);
		if(!(hour==0&&minute==0&&second==0))
			break;
	}
	
	OLED_Clear();
	showWeather();
	while(1)//时间循环
	{
		OLED_ShowCHinese(0,0,hour/10);
		OLED_ShowCHinese(16,0,hour%10);
		OLED_ShowCHinese(32,0,40);//h:m
		OLED_ShowCHinese(48,0,minute/10);
		OLED_ShowCHinese(64,0,minute%10);
		OLED_ShowCHinese(80,0,40);//m:s
		OLED_ShowCHinese(96,0,second/10);
		OLED_ShowCHinese(112,0,second++%10);
		//printf("%d:%d:%d\n",hour,minute,second);55555
		
		if(second>=60)
		{
			second = 0;
			minute++;
		}
		if(minute>=60)
		{
			minute=0;
			hour++;
		}
		if(hour>=24)
		{
			hour=0;
		}
		
		if(second == 0 || second == 30)//
		{
			OLED_Clear_Area();
			showWeather();
		}
		else if(second == 10 || second == 40)//
		{
			OLED_Clear_Area();
			showDHT11();
		}
		else if(second == 20 || second == 50)//
		{
			OLED_Clear_Area();
			showSport();
		}	
		
		if(second%10==0)
			delay_ms(500);
		else
			delay_ms(800);
	}

	vTaskDelete(TIMEHandle);
}

void appTask(void* arg)//系统初始化
{
	Usart2_Init(115200);
	OLED_Init();	//初始化OLED 
	OLED_Clear(); 	//清屏
	OLED_ShowCHinese(20,3,48);//显示"正在开机"
	OLED_ShowCHinese(44,3,49);
	OLED_ShowCHinese(68,3,50);
	OLED_ShowCHinese(92,3,51);	

	Get_current_Live();
	getLive(live,liveNum);
	printf( "makeup: %s\n", live[0]);
	printf( "sport: %s\n", live[1]);
	printf( "uv: %s\n", live[2]);
	for(int i=0;i<6;i++)
		printf("num[%d] = %d\n",i,liveNum[i]);
	
	while(xTaskCreate(ESP8266_DHT11Task,"ESP8266_DHT11Task",512,NULL,7,&ESP8266Handle)!=pdPASS);//天气温湿度管理
	while(next);
	while(xTaskCreate(TIMETask,"TIMETask",512,NULL,7,&TIMEHandle)!=pdPASS);//时间管理
	//删除任务
	vTaskDelete(appTaskHandle);
}

int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	init_led();
	init_key();
	init_delay();
	init_usart1(115200);
	init_dht11();
	printf("程序开始运行\n");
	
	BaseType_t ret = xTaskCreate(appTask,"appTask",512,NULL,6,&appTaskHandle);
	if(ret != pdPASS)
	{
		printf("创建失败\n");
		return -1;
	}
	
	//开启任务调度
	vTaskStartScheduler();
}
