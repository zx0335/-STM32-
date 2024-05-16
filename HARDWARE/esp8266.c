#include "sys.h"
#include "string.h"
#include "stdlib.h"
#include "esp8266.h"
 
#include "usart.h"
#include "usart2.h"
 
#include "delay.h"
#include "led.h"
 
/*���ڱ���Сʱ�����ӣ������ı���*/
int hour_return;//Сʱ
int min_return; //����
int sec_return; //����
 
extern char time[20];
extern char weather[30];
extern char live[3][20];

//WIFI���ƺ�����
#define ESP8266_WIFI_INFO		"AT+CWJAP=\"zx\",\"zx123456\"\r\n"
//������ַ
#define Time_TCP		"AT+CIPSTART=\"TCP\",\"quan.suning.com\",80\r\n"
//����ʱ��API
#define Time_GET		"GET /getSysTime.do HTTP/1.1\r\nHost: quan.suning.com\r\n\r\n"
//��֪��������ַ
#define Weather_TCP		"AT+CIPSTART=\"TCP\",\"api.seniverse.com\",80\r\n"
//����:����    ����:Ӣ��
//��֪����  ����API
#define Weather_GET		"GET https://api.seniverse.com/v3/weather/now.json?key=SQVT3eFSMXeJcpau-&location=chongqing&language=en&unit=c\r\n"
//��֪����  ����ָ��API
#define Live_GET "GET https://api.seniverse.com/v3/life/suggestion.json?key=SQVT3eFSMXeJcpau-&location=chongqing&language=en&days=1\r\n"
 
//�����������
unsigned char Weather_buff[600];   //λ�������ȷ����
 //ESP8266���ݴ��
char esp8266_buf[600] = {0};
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;
//���ʱ������
unsigned char Time_buff[100];   //λ�������ȷ����
 
 
/**************************************************************************/
//�������ã�ESP8266_Init��ʼ������
//�������ƣ�ESP8266_Init(void);
//�ڲ�������
//�޸����ڣ�2022��4��18��  ����16��18
/**************************************************************************/
void ESP8266_Init(void)
{	
    ESP8266_Clear();//�������
    /*��WIFI�˳�͸��ģʽ Ҫ������*/
    //while(ESP8266_SendCmd("+++",""));//����һ����ѭ����Ŀ�Ľ���͸��ģʽ
	for(int i=0;i<10;i++)//����һ����ѭ����Ŀ�Ľ���͸��ģʽ
	{
		ESP8266_SendCmd("+++","");
	}
    /*��WIFI���Flah*/
	
    while(ESP8266_SendCmd("AT+RESTORE\r\n", "OK"));//�ָ���������
    printf("0.�ָ��������óɹ�\r\n");
    
    while(ESP8266_SendCmd("AT\r\n", "OK"));
	//��ʼATӦ�𣬿�wifi�����Ƿ�ɹ�
    printf("1.ATӦ��ɹ�\r\n");
	
    ESP8266_SendCmd("AT+RST\r\n", "");
    //��һ��ESP8266��λ����
    printf("2.RST��λ�ɹ�\r\n");
    delay_ms(500);
    ESP8266_SendCmd("AT+CIPCLOSE\r\n", "");//�Ͽ��������������
    delay_ms(500);
    
 
    printf("3.CWMODE���ù���ģʽ�����浽Flash\r\n");
 
    while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"));//û��CUR���Ǳ��浽Flash��AT+CWMODE_CUR����ģ�鹤��ģʽΪstation�������浽Flash
 
    printf("4.AT+CIPMUX������ģʽ���óɹ�\r\n");
 
    while(ESP8266_SendCmd("AT+CIPMUX=0\r\n", "OK"));//AT+CIPMUX=0 ����Ϊ������ģʽ
 
    printf("5.Ѱ�Ҷ�Ӧ��WIFI���ƺ�����\r\n");
 
    while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "WIFI GOT IP"));
 
    printf("6.ESP8266_Init����WIFI�ɹ�\r\n");
}
 
 
/**************************************************************************/
//�������ã���ȡ������̨ʱ��
//�������ƣ�Get_current_time();
//�ڲ�������
//�޸����ڣ�2022��4��18��  ����20��32
//���ߣ�    ��ƨ��
/**************************************************************************/
void Get_current_time(void)
{
    ESP8266_Init();	     //����Wifi��ESP8266��ʼ��
    ESP8266_Clear();
 
    while(ESP8266_SendCmd(Time_TCP, "CONNECT"));
 
    printf("7.���������������ɹ� OK\r\n");
 
    while(ESP8266_SendCmd("AT+CIPMODE=1\r\n", "OK"));//����͸��ģʽ
 
    printf("8.����͸��ģʽ�ɹ� OK\r\n");
 
    /*sizeof(Time_GET)��������sizeof��������strlenû����*/
    ESP8266_SendData((u8 *)Time_GET, sizeof(Time_GET)); //����AT+CIPSEND  �Լ� Time_GET
    ESP8266_GetIPD_GET(200, Time_buff); //����������ȡ����
	for(int i=0;i<600;i++)
	{
			printf("%c",esp8266_buf[i]);
	}
//	printf("\n��ӡ����\n");
//	strtok(esp8266_buf,"sysTime1\":\"");
//	char* buf = strtok(NULL,"\"}");
//	strcpy(time,buf);
	
	printf("\n��ӡ����\n");
	
	
	
	/*strtok(esp8266_buf,"tml>");
	strtok(NULL,"\n");
	char* buf = strtok(NULL,"\",");
	strcpy(time,buf);*/
	
	
//	strtok(esp8266_buf,"\"");
//	char* buf = strtok(NULL,"\"");
//	if(buf != NULL)
//		strncpy(time,buf,19);
//	time[19]='\0';
	time[0]='2';
	time[1]='0';
	time[2]='2';
	time[3]='4';
	strtok(esp8266_buf,"24");
	char* buf = strtok(NULL,"\"");
	if(buf != NULL)
		strncpy(time+4,buf,15);
	time[19]='\0';
	
	//printf("ʱ��Ϊ:%s\n",buf);//2024-04-13 23:52:22
	//while(1){}
    ESP8266_Clear();//�����������
 
	for(int i=0;i<10;i++)//����һ����ѭ����Ŀ�Ľ���͸��ģʽ
	{
		ESP8266_SendCmd("+++","");
	}
 
    printf("10.�˳�͸��ģʽ�ɹ� OK\r\n");
}

//��ȡ��ǰ����
void Get_current_Weather(void)
{
	memset(Weather_buff,0,600);
    ESP8266_Init();	     //����Wifi��ESP8266��ʼ��
	printf("WIFI��ʼ�����\n");
    ESP8266_Clear();
 
    while(ESP8266_SendCmd(Weather_TCP, "CONNECT"));
 
    printf("7.������֪�������ɹ� OK\r\n");
 
    while(ESP8266_SendCmd("AT+CIPMODE=1\r\n", "OK"));//����͸��ģʽ
 
    printf("8.����͸��ģʽ�ɹ� OK\r\n");
 
    /*sizeof(Time_GET)��������sizeof��������strlenû����*/
    ESP8266_SendData((u8 *)Weather_GET, sizeof(Weather_GET)); //����AT+CIPSEND  �Լ� Time_GET
    ESP8266_GetIPD_GET(200, Weather_buff); //����������ȡ����
	for(int i=0;i<300;i++)
	{
		printf("%c",esp8266_buf[i]);
	}
	delay_ms(1000);
	printf("\n��ӡ����\n");
	
	
	char *pch;
    pch = strstr(esp8266_buf, "\"text\":\"");
    if (pch != NULL) 
	{
        pch += strlen("\"text\":\"");  // skip past the beginning of the token
        char *end = strstr(pch, "\"");
        if (end != NULL) 
		{
            *end = '\0';  // null-terminate the token
        }
    }
	strcpy(weather,pch);
	
    ESP8266_Clear();//�����������
 
	for(int i=0;i<10;i++)//����һ����ѭ����Ŀ�Ľ���͸��ģʽ
	{
		ESP8266_SendCmd("+++","");
	}
 
    printf("10.�˳�͸��ģʽ�ɹ� OK\r\n");
}

void find_and_print_value(char *str, char *key)
{	
    char *token;
    char *value;
   
    // ʹ��ð����Ϊ�ָ�����ȡ��һ��token
    token = strtok(str, ":");
   
    // ��ȡ�ڶ���token�������"brief"�ֶε�ֵ
    token = strtok(NULL, ":");
   
    // ȥ������
    if(token[0] == '\"') {
        token++;
    }
    if(token[strlen(token) - 1] == '\"') {
        token[strlen(token) - 1] = '\0';
    }
   
    printf("%s��brief��ֵ��: %s\n", key, token);
}
 
//��ȡ��ǰ����ָ��
void Get_current_Live(void)
{
	memset(Weather_buff,0,600);
    ESP8266_Init();	     //����Wifi��ESP8266��ʼ��
	printf("WIFI��ʼ�����\n");
    ESP8266_Clear();
 
    while(ESP8266_SendCmd(Weather_TCP, "CONNECT"));
 
    printf("7.������֪�������ɹ� OK\r\n");
 
    while(ESP8266_SendCmd("AT+CIPMODE=1\r\n", "OK"));//����͸��ģʽ
 
    printf("8.����͸��ģʽ�ɹ� OK\r\n");
 
    /*sizeof(Time_GET)��������sizeof��������strlenû����*/
    ESP8266_SendData((u8 *)Live_GET, sizeof(Live_GET)); //����AT+CIPSEND  �Լ� Time_GET
    ESP8266_GetIPD_GET(200, Weather_buff); //����������ȡ����
	for(int i=0;i<600;i++)
	{
		printf("%c",esp8266_buf[i]);
	}
	delay_ms(1000);
	printf("\n��ӡ����\n");
	

	char *token;
    char *key;
    char *value;

    token = strtok(esp8266_buf, ":,\"{}");
	int i =0;
    while (token != NULL) 
	{
        if (strcmp(token, "makeup") == 0 || strcmp(token, "sport") == 0 || strcmp(token, "uv") == 0) 
		{			
            key = token;
            token = strtok(NULL, ":,\"{}");
            token = strtok(NULL, ":,\"{}");
            value = token;
            printf("%s: %s\n", key, value);
			strcpy(live[i++],value);
        }
        token = strtok(NULL, ":,\"{}");
    }



	
    ESP8266_Clear();//�����������
 
	for(int i=0;i<10;i++)//����һ����ѭ����Ŀ�Ľ���͸��ģʽ
	{
		ESP8266_SendCmd("+++","");
	}
 
    printf("10.�˳�͸��ģʽ�ɹ� OK\r\n");
}


/****************************************************************************************
����׵�ַ�ƶ�11λ��
�·��׵�ַ�ƶ�15λ��
�����׵�ַ�ƶ�17λ��
Сʱ�׵�ַ�ƶ�19λ��
�����׵�ַ�ƶ�21λ��
�����׵�ַ�ƶ�23λ��
*/
#define  YEAR_ADD_DRES 11
#define  MOON_ADD_DRES 15
#define  DAYS_ADD_DRES 17
 
#define  HOURS_ADD_DRES 19
#define  MINUTES_ADD_DRES 21
#define  SECONDS_ADD_DRES 23
 
int DAYS, MOONS, YEARS, TIMES;
///**************************************************************************/
//�������ã���������ʱ�亯��
//�������ƣ�cJSON_Time_Parse();
//�ڲ�������
//�޸����ڣ�2022��4��18��  ����22��11
///**************************************************************************/
void cJSON_Time_Parse(void)
{
    char *data_pt;
    char *day_string;
    char *moon_string;
    char *year_string;
    char *hour_string;
    char *minute_string;
    char *second_string;
 
    data_pt = strstr((const char *)Time_buff, (const char *)"sysTime1");  //Ѱ�ҵ�ʱ�����ĵ�ַ
 
//    printf("%s\r\n",Time_buff);
    if(data_pt != NULL)
    {
        day_string = data_pt + DAYS_ADD_DRES;          //���ڵ�ַ
        moon_string = data_pt + MOON_ADD_DRES;         //�·ݵ�ַ
        year_string = data_pt + YEAR_ADD_DRES;        //��ݵ�ַ
        hour_string = data_pt + HOURS_ADD_DRES;       //Сʱ��ַ
        minute_string = data_pt + MINUTES_ADD_DRES;   //���ӵ�ַ
        second_string = data_pt + SECONDS_ADD_DRES;   //���е�ַ
 
        //��ʱ����Ϣ���ݸ�ȫ�ֱ���
        DAYS = Get_Day(day_string);
        MOONS = Get_Moonth(moon_string);
        YEARS = Get_Year(year_string);
        TIMES = Get_Times(hour_string, minute_string, second_string);
		
		hour_return = TIMES/3600;//Сʱ
		min_return = (TIMES%3600)/60; //����
        sec_return = (TIMES%3600)%60; //����
		printf("ʱ���ȡ������ɹ�\r\n");
 
    }
    else
    {
        printf("ʱ���ȡʧ��\r\n");
    }
}
 
//�õ��꺯�������꿪ʼ���ַ������ȹ��������ʹ�ò�һ���ķ�����
//����ֵ����λ�õĵ�ַ
//����ֵ�� ���͵�10������λ��
int Get_Year(char *y)
{
 
    int year_return;
    char *year_temp;
    char year[5] = {0};
    char i;
//��Ļ�ȡ��Ҫ��ȡһ���ַ�������Ȼû����ȡ
    year_temp = y;
 
    for(i = 0; i < 4; i++)
    {
        year[i] = *year_temp;
        year_temp ++;
    }
 
    year_return =  atoi(&year[0]);
    return year_return;
}
 
//�õ��·ݺ���
//����ֵ���·�λ�õĵ�ַ
//����ֵ�� ���͵�10������λ��
int Get_Moonth(char *m)
{
    int moonth_return;
    moonth_return = atoi(m) / 100000000; //ȡ�·�
    return moonth_return;
}
 
//�õ����ں���
//����ֵ������λ�õĵ�ַ
//����ֵ�� ���͵�10������λ��
int Get_Day(char *d)
{ 
    int day_return;
    day_return = atoi(d) / 1000000; //ȡ����
 
    return day_return;
}
 
//�õ�ʱ��
//����ֵ��ʱ���λ�õĵ�ַ
//����ֵ�� ���͵�10���Ƶ�ʱ��������
int Get_Times(char *h, char *m, char *s)
{
    int time_return;
    int hour_return;
    int min_return;
    int sec_return;
 
    hour_return = atoi(h) / 10000; //ȡСʱ
    min_return = atoi(m) / 100; //ȡ����
    sec_return = atoi(s);   //ȡ����
 
    time_return = hour_return * 3600 + min_return * 60 + sec_return; //ת����������
 
    return time_return;
}
 
/*****************************************************************��������ʱ��END********************************************************************************/
/*************************************************************************************************************************************************/
 
/**************************************************************************/
//�������ã����ڶ��жϺ���
//�������ƣ�USART2_IRQHandler();
//�ڲ�������
//�޸����ڣ�2022��4��18��  ����4��18
/**************************************************************************/
void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //�����ж�
    {
 
        if(esp8266_cnt >= sizeof(esp8266_buf))	esp8266_cnt = 0; //��ֹ���ڱ�ˢ��
 
        esp8266_buf[esp8266_cnt++] = USART2->DR;
 
//		USART_SendData(USART1,USART2->DR);      //�ý��յ������ݴ�ӡ�ڴ���һ��
 
        USART_ClearFlag(USART2, USART_FLAG_RXNE);
    }
}

//==========================================================
//	�������ƣ�	ESP8266_Clear
//
//	�������ܣ�	��ջ���
//
//	��ڲ�����	��
//
//	���ز�����	��
//
//	˵����
//==========================================================
void ESP8266_Clear(void)
{
    memset(esp8266_buf, 0, sizeof(esp8266_buf));
    esp8266_cnt = 0;
 
}
//==========================================================
//	�������ƣ�	ESP8266_SendData
//
//	�������ܣ�	��������
//
//	��ڲ�����	data������
//				len������
//
//	���ز�����	��
//
//	˵����
//==========================================================
void ESP8266_SendData(unsigned char *data, unsigned short len)
{
    char cmdBuf[32];
 
    ESP8266_Clear();								//��ս��ջ���
    sprintf(cmdBuf, "AT+CIPSEND\r\n");		//��������
 
    if(!ESP8266_SendCmd(cmdBuf, ">"))				//�յ���>��ʱ���Է�������
    {
        printf("9.��ʼ����͸������״̬!\r\n");
        /*������������*/
        Usart_SendString(USART2, data, len);		//�����豸������������**********************************
    }
}
 
//==========================================================
//	�������ƣ�	ESP8266_GetIPD
//
//	�������ܣ�	copy�������ݵ�buff��������
//
//	���ز�����	ƽ̨���ص�ԭʼ����
//
//	˵����		copy�������ݵ�buff
//==========================================================
 
unsigned char *ESP8266_GetIPD_GET(unsigned short timeOut, u8 *buff) //����������һ��ȫ�ֱ�����esp8266buf���浽���ȫ�ֱ�������
{
    do
    {
        delay_ms(5);
    }
    while(timeOut--);
 
    //strcpy((char*)buff, (char*)esp8266_buf);
    return buff;
}
 
 
 
//==========================================================
//	�������ƣ�	ESP8266_WaitRecive
//
//	�������ܣ�	�ȴ��������
//
//	��ڲ�����	��
//
//	���ز�����	REV_OK-�������		REV_WAIT-���ճ�ʱδ���
//
//	˵����		ѭ�����ü���Ƿ�������
//==========================================================
int ESP8266_WaitRecive(void)
{
 
    if(esp8266_cnt == 0) 							//������ռ���Ϊ0 ��˵��û�д��ڽ��������У�����ֱ����������������
        return REV_WAIT;
 
    if(esp8266_cnt == esp8266_cntPre)				//�����һ�ε�ֵ�������ͬ����˵���������
    {
        esp8266_cnt = 0;							//��0���ռ���
 
        return REV_OK;								//���ؽ�����ɱ�־
    }
 
    esp8266_cntPre = esp8266_cnt;					//��Ϊ��ͬ
 
    return REV_WAIT;								//���ؽ���δ��ɱ�־
 
}
 
 
//==========================================================
//	�������ƣ�	ESP8266_SendCmd
//
//	�������ܣ�	��������
//
//	��ڲ�����	cmd������
//				res����Ҫ���ķ���ָ��
//
//	���ز�����	0-�ɹ�	1-ʧ��
//
//	˵����
//==========================================================
int ESP8266_SendCmd(char *cmd, char *res)
{
 
    unsigned char timeOut = 250;
 
    Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
 
    while(timeOut--)
    {
        if(ESP8266_WaitRecive() == REV_OK)							//����յ�����
        {
            if(strstr((const char *)esp8266_buf, res) != NULL)		//����������ؼ���
            {
                ESP8266_Clear();									//��ջ���
 
                return 0;
            }
        }
 
        delay_ms(10);
    }
    return 1;
}


void getWeather(char* str,int* len,int* n)
{
	if(strcmp(str,"Sunny")==0)
	{
		strcpy(str,"������");
		*len = 3;
		n[0] = 52;
		n[1] = 53;
		n[2] = 22;
		return;
	}
	if(strcmp(str,"Clear")==0)
	{
		strcpy(str,"ҹ����");
		*len = 3;
		n[0] = 54;
		n[1] = 55;
		n[2] = 22;
		return;
	}
	if(strcmp(str,"Cloudy")==0)
	{
		strcpy(str,"����");
		*len = 2;
		n[0] = 56;
		n[1] = 23;
		return;
	}
	if(strcmp(str,"Partly Cloudy")==0)
	{
		strcpy(str,"������");
		*len = 4;
		n[0] = 22;
		n[1] = 57;
		n[2] = 56;
		n[3] = 23;
		return;
	}
	if(strcmp(str,"Mostly Cloudy")==0)
	{
		strcpy(str,"�󲿶���");
		*len = 4;
		n[0] = 12;
		n[1] = 58;
		n[2] = 56;
		n[3] = 23;
		return;
	}
	if(strcmp(str,"Overcast")==0)
	{
		strcpy(str,"��");
		*len = 1;
		n[0] = 21;
		return;
	}
	if(strcmp(str,"Shower")==0)
	{
		strcpy(str,"����");
		*len = 2;
		n[0] = 29;
		n[1] = 13;
		return;
	}
	if(strcmp(str,"Thundershower")==0)
	{
		strcpy(str,"������");
		*len = 3;
		n[0] = 17;
		n[1] = 29;
		n[2] = 13;
		return;
	}
	if(strcmp(str,"Thundershower with Hail")==0)
	{
		strcpy(str,"��������б���");
		*len = 7;
		n[0] = 17;
		n[1] = 29;
		n[2] = 13;
		n[3] = 30;
		n[4] = 32;
		n[5] = 18;
		n[6] = 59;
		return;
	}
	if(strcmp(str,"Light rain")==0)
	{
		strcpy(str,"С��");
		*len = 2;
		n[0] = 10;
		n[1] = 13;
		return;
	}
	if(strcmp(str,"Moderate rain")==0)
	{
		strcpy(str,"����");
		*len = 2;
		n[0] = 11;
		n[1] = 13;
		return;
	}
	if(strcmp(str,"Heavy rain")==0)
	{
		strcpy(str,"����");
		*len = 2;
		n[0] = 12;
		n[1] = 13;
		return;
	}
	if(strcmp(str,"Storm")==0)
	{
		strcpy(str,"����");
		*len = 2;
		n[0] = 24;
		n[1] = 13;
		return;
	}
	if(strcmp(str,"Heavy Storm")==0)
	{
		strcpy(str,"����");
		*len = 3;
		n[0] = 12;
		n[1] = 24;
		n[2] = 13;
		return;
	}
	if(strcmp(str,"Severe Storm")==0)
	{
		strcpy(str,"�ش���");
		*len = 4;
		n[0] = 28;
		n[1] = 12;
		n[2] = 24;
		n[0] = 13;
		return;
	}
	if(strcmp(str,"Ice Rain")==0)
	{
		strcpy(str,"����");
		*len = 2;
		n[0] = 60;
		n[1] = 13;
		return;
	}
	if(strcmp(str,"Sleet")==0)
	{
		strcpy(str,"���ѩ");
		*len = 3;
		n[0] = 13;
		n[1] = 37;
		n[2] = 14;
		return;
	}
	if(strcmp(str,"Snow Flurry")==0)
	{
		strcpy(str,"��ѩ");
		*len = 2;
		n[0] = 29;
		n[1] = 14;
		return;
	}
	if(strcmp(str,"Light Snow")==0)
	{
		strcpy(str,"Сѩ");
		*len = 2;
		n[0] = 10;
		n[1] = 14;
		return;
	}
	if(strcmp(str,"Moderate Snow")==0)
	{
		strcpy(str,"��ѩ");
		*len = 2;
		n[0] = 11;
		n[1] = 14;
		return;
	}
	if(strcmp(str,"Heavy Snow")==0)
	{
		strcpy(str,"��ѩ");
		*len = 2;
		n[0] = 12;
		n[1] = 14;
		return;
	}
	if(strcmp(str,"Snowstorm")==0)
	{
		strcpy(str,"��ѩ");
		*len = 2;
		n[0] = 24;
		n[1] = 14;
		return;
	}
	if(strcmp(str,"Dust")==0)
	{
		strcpy(str,"����");
		*len = 2;
		n[0] = 32;
		n[1] = 61;
		return;
	}
	if(strcmp(str,"Sand")==0)
	{
		strcpy(str,"��ɳ");
		*len = 2;
		n[0] = 33;
		n[1] = 16;
		return;
	}
	if(strcmp(str,"Duststorm")==0)
	{
		strcpy(str,"ɳ����");
		*len = 3;
		n[0] = 16;
		n[1] = 61;
		n[2] = 24;
		return;
	}
	if(strcmp(str,"Sandstorm")==0)
	{
		strcpy(str,"ǿɳ����");
		*len = 4;
		n[0] = 27;
		n[1] = 16;
		n[2] = 61;
		n[3] = 24;
		return;
	}
	if(strcmp(str,"Foggy")==0)
	{
		strcpy(str,"��");
		*len = 1;
		n[0] = 19;
		return;
	}
	if(strcmp(str,"Haze")==0)
	{
		strcpy(str,"��");
		*len = 1;
		n[0] = 20;
		return;
	}
	if(strcmp(str,"Windy")==0)
	{
		strcpy(str,"��");
		*len = 1;
		n[0] = 15;
		return;
	}
	if(strcmp(str,"Blustery")==0)
	{
		strcpy(str,"���");
		*len = 2;
		n[0] = 12;
		n[1] = 15;
		return;
	}
	if(strcmp(str,"Hurricane")==0)
	{
		strcpy(str,"쫷�");
		*len = 2;
		n[0] = 25;
		n[1] = 15;
		return;
	}
	if(strcmp(str,"Tropical Storm")==0)
	{
		strcpy(str,"�ȴ��籩");
		*len = 4;
		n[0] = 35;
		n[1] = 62;
		n[2] = 15;
		n[3] = 24;
		return;
	}
	if(strcmp(str,"Tornado")==0)
	{
		strcpy(str,"�����");
		*len = 3;
		n[0] = 63;
		n[1] = 64;
		n[2] = 15;
		return;
	}
	if(strcmp(str,"Cold")==0)
	{
		strcpy(str,"��");
		*len = 1;
		n[0] = 34;
		return;
	}
	if(strcmp(str,"Hot")==0)
	{
		strcpy(str,"��");
		*len = 1;
		n[0] = 35;
		return;
	}
	if(strcmp(str,"Unknown")==0)
	{
		strcpy(str,"δ֪");
		*len = 2;
		n[0] = 38;
		n[1] = 39;
		return;
	}	
}

void getLive(char str[3][20],int* num)
{
	//��ױ Makeup
	if(strncmp(str[0],"Moisturizing",12)==0)
	{
		memset(str[0],0,20);
		strcpy(str[0],"��ʪ");
		num[0]=81;
		num[1]=82;
	}
	else if(strncmp(str[0],"Sunscreen",9)==0)
	{
		memset(str[0],0,20);
		strcpy(str[0],"��ɹ");
		num[0]=79;
		num[1]=80;
	}
	else if(strncmp(str[0],"Anti-oil",8)==0)
	{
		memset(str[0],0,20);
		strcpy(str[0],"ȥ��");
		num[0]=83;
		num[1]=84;
	}
	else
	{
		memset(str[0],0,20);
		strcpy(str[0],"��ʪ");
		num[0]=81;
		num[1]=82;
	}
	
	//�˶� Sport
	if(strncmp(str[1],"Not Good",8)==0)
	{
		memset(str[1],0,20);
		strcpy(str[1],"����");
		num[2]=85;
		num[3]=73;
	}
	else if(strncmp(str[1],"Great",8)==0)
	{
		memset(str[1],0,20);
		strcpy(str[1],"����");
		num[2]=72;
		num[3]=73;
	}
	else
	{
		memset(str[1],0,20);
		strcpy(str[1],"����");
		num[2]=72;
		num[3]=73;
	}
	
	//������ uv
	if(strncmp(str[2],"Very Low",8)==0)
	{
		memset(str[2],0,20);
		strcpy(str[2],"����");
		num[4]=74;
		num[5]=78;
	}
	else if(strncmp(str[2],"Low",3)==0)
	{
		memset(str[2],0,20);
		strcpy(str[2],"����");
		num[4]=74;
		num[5]=78;
	}
	else if(strncmp(str[2],"Moderate",8)==0)
	{
		memset(str[2],0,20);
		strcpy(str[2],"�е�");
		num[4]=76;
		num[5]=77;
	}
	else
	{
		memset(str[2],0,20);
		strcpy(str[2],"��ǿ");
		num[4]=74;
		num[5]=75;
	}
}