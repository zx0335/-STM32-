#include "sys.h"
#include "string.h"
#include "stdlib.h"
#include "esp8266.h"
 
#include "usart.h"
#include "usart2.h"
 
#include "delay.h"
#include "led.h"
 
/*用于保存小时，分钟，秒数的变量*/
int hour_return;//小时
int min_return; //分钟
int sec_return; //秒数
 
extern char time[20];
extern char weather[30];
extern char live[3][20];

//WIFI名称和密码
#define ESP8266_WIFI_INFO		"AT+CWJAP=\"zx\",\"zx123456\"\r\n"
//苏宁网址
#define Time_TCP		"AT+CIPSTART=\"TCP\",\"quan.suning.com\",80\r\n"
//苏宁时间API
#define Time_GET		"GET /getSysTime.do HTTP/1.1\r\nHost: quan.suning.com\r\n\r\n"
//心知天气的网址
#define Weather_TCP		"AT+CIPSTART=\"TCP\",\"api.seniverse.com\",80\r\n"
//城市:重庆    语言:英文
//心知天气  天气API
#define Weather_GET		"GET https://api.seniverse.com/v3/weather/now.json?key=SQVT3eFSMXeJcpau-&location=chongqing&language=en&unit=c\r\n"
//心知天气  生活指数API
#define Live_GET "GET https://api.seniverse.com/v3/life/suggestion.json?key=SQVT3eFSMXeJcpau-&location=chongqing&language=en&days=1\r\n"
 
//存放天气数据
unsigned char Weather_buff[600];   //位数是随机确定的
 //ESP8266数据存放
char esp8266_buf[600] = {0};
unsigned short esp8266_cnt = 0, esp8266_cntPre = 0;
//存放时间数据
unsigned char Time_buff[100];   //位数是随机确定的
 
 
/**************************************************************************/
//函数作用：ESP8266_Init初始化函数
//函数名称：ESP8266_Init(void);
//内部参数：
//修改日期：2022年4月18日  下午16：18
/**************************************************************************/
void ESP8266_Init(void)
{	
    ESP8266_Clear();//清除缓冲
    /*让WIFI退出透传模式 要发两次*/
    //while(ESP8266_SendCmd("+++",""));//这是一个死循环，目的结束透传模式
	for(int i=0;i<10;i++)//这是一个死循环，目的结束透传模式
	{
		ESP8266_SendCmd("+++","");
	}
    /*让WIFI清除Flah*/
	
    while(ESP8266_SendCmd("AT+RESTORE\r\n", "OK"));//恢复出厂设置
    printf("0.恢复出厂设置成功\r\n");
    
    while(ESP8266_SendCmd("AT\r\n", "OK"));
	//初始AT应答，看wifi接线是否成功
    printf("1.AT应答成功\r\n");
	
    ESP8266_SendCmd("AT+RST\r\n", "");
    //加一步ESP8266复位操作
    printf("2.RST复位成功\r\n");
    delay_ms(500);
    ESP8266_SendCmd("AT+CIPCLOSE\r\n", "");//断开与服务器的连接
    delay_ms(500);
    
 
    printf("3.CWMODE设置工作模式，保存到Flash\r\n");
 
    while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK"));//没有CUR就是保存到Flash，AT+CWMODE_CUR设置模块工作模式为station，不保存到Flash
 
    printf("4.AT+CIPMUX单连接模式设置成功\r\n");
 
    while(ESP8266_SendCmd("AT+CIPMUX=0\r\n", "OK"));//AT+CIPMUX=0 设置为单连接模式
 
    printf("5.寻找对应的WIFI名称和密码\r\n");
 
    while(ESP8266_SendCmd(ESP8266_WIFI_INFO, "WIFI GOT IP"));
 
    printf("6.ESP8266_Init连接WIFI成功\r\n");
}
 
 
/**************************************************************************/
//函数作用：获取苏宁后台时间
//函数名称：Get_current_time();
//内部参数：
//修改日期：2022年4月18日  晚上20：32
//作者：    大屁桃
/**************************************************************************/
void Get_current_time(void)
{
    ESP8266_Init();	     //连接Wifi的ESP8266初始化
    ESP8266_Clear();
 
    while(ESP8266_SendCmd(Time_TCP, "CONNECT"));
 
    printf("7.访问苏宁服务器成功 OK\r\n");
 
    while(ESP8266_SendCmd("AT+CIPMODE=1\r\n", "OK"));//开启透传模式
 
    printf("8.开启透传模式成功 OK\r\n");
 
    /*sizeof(Time_GET)，必须用sizeof函数，用strlen没有用*/
    ESP8266_SendData((u8 *)Time_GET, sizeof(Time_GET)); //发送AT+CIPSEND  以及 Time_GET
    ESP8266_GetIPD_GET(200, Time_buff); //将串口数据取出来
	for(int i=0;i<600;i++)
	{
			printf("%c",esp8266_buf[i]);
	}
//	printf("\n打印结束\n");
//	strtok(esp8266_buf,"sysTime1\":\"");
//	char* buf = strtok(NULL,"\"}");
//	strcpy(time,buf);
	
	printf("\n打印结束\n");
	
	
	
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
	
	//printf("时间为:%s\n",buf);//2024-04-13 23:52:22
	//while(1){}
    ESP8266_Clear();//清除缓存数据
 
	for(int i=0;i<10;i++)//这是一个死循环，目的结束透传模式
	{
		ESP8266_SendCmd("+++","");
	}
 
    printf("10.退出透传模式成功 OK\r\n");
}

//获取当前天气
void Get_current_Weather(void)
{
	memset(Weather_buff,0,600);
    ESP8266_Init();	     //连接Wifi的ESP8266初始化
	printf("WIFI初始化完成\n");
    ESP8266_Clear();
 
    while(ESP8266_SendCmd(Weather_TCP, "CONNECT"));
 
    printf("7.访问心知服务器成功 OK\r\n");
 
    while(ESP8266_SendCmd("AT+CIPMODE=1\r\n", "OK"));//开启透传模式
 
    printf("8.开启透传模式成功 OK\r\n");
 
    /*sizeof(Time_GET)，必须用sizeof函数，用strlen没有用*/
    ESP8266_SendData((u8 *)Weather_GET, sizeof(Weather_GET)); //发送AT+CIPSEND  以及 Time_GET
    ESP8266_GetIPD_GET(200, Weather_buff); //将串口数据取出来
	for(int i=0;i<300;i++)
	{
		printf("%c",esp8266_buf[i]);
	}
	delay_ms(1000);
	printf("\n打印结束\n");
	
	
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
	
    ESP8266_Clear();//清除缓存数据
 
	for(int i=0;i<10;i++)//这是一个死循环，目的结束透传模式
	{
		ESP8266_SendCmd("+++","");
	}
 
    printf("10.退出透传模式成功 OK\r\n");
}

void find_and_print_value(char *str, char *key)
{	
    char *token;
    char *value;
   
    // 使用冒号作为分隔符获取第一个token
    token = strtok(str, ":");
   
    // 获取第二个token，这就是"brief"字段的值
    token = strtok(NULL, ":");
   
    // 去掉引号
    if(token[0] == '\"') {
        token++;
    }
    if(token[strlen(token) - 1] == '\"') {
        token[strlen(token) - 1] = '\0';
    }
   
    printf("%s的brief的值是: %s\n", key, token);
}
 
//获取当前生活指数
void Get_current_Live(void)
{
	memset(Weather_buff,0,600);
    ESP8266_Init();	     //连接Wifi的ESP8266初始化
	printf("WIFI初始化完成\n");
    ESP8266_Clear();
 
    while(ESP8266_SendCmd(Weather_TCP, "CONNECT"));
 
    printf("7.访问心知服务器成功 OK\r\n");
 
    while(ESP8266_SendCmd("AT+CIPMODE=1\r\n", "OK"));//开启透传模式
 
    printf("8.开启透传模式成功 OK\r\n");
 
    /*sizeof(Time_GET)，必须用sizeof函数，用strlen没有用*/
    ESP8266_SendData((u8 *)Live_GET, sizeof(Live_GET)); //发送AT+CIPSEND  以及 Time_GET
    ESP8266_GetIPD_GET(200, Weather_buff); //将串口数据取出来
	for(int i=0;i<600;i++)
	{
		printf("%c",esp8266_buf[i]);
	}
	delay_ms(1000);
	printf("\n打印结束\n");
	

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



	
    ESP8266_Clear();//清除缓存数据
 
	for(int i=0;i<10;i++)//这是一个死循环，目的结束透传模式
	{
		ESP8266_SendCmd("+++","");
	}
 
    printf("10.退出透传模式成功 OK\r\n");
}


/****************************************************************************************
年的首地址移动11位；
月份首地址移动15位；
日期首地址移动17位；
小时首地址移动19位；
分钟首地址移动21位；
秒钟首地址移动23位；
*/
#define  YEAR_ADD_DRES 11
#define  MOON_ADD_DRES 15
#define  DAYS_ADD_DRES 17
 
#define  HOURS_ADD_DRES 19
#define  MINUTES_ADD_DRES 21
#define  SECONDS_ADD_DRES 23
 
int DAYS, MOONS, YEARS, TIMES;
///**************************************************************************/
//函数作用：解析苏宁时间函数
//函数名称：cJSON_Time_Parse();
//内部参数：
//修改日期：2022年4月18日  下午22：11
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
 
    data_pt = strstr((const char *)Time_buff, (const char *)"sysTime1");  //寻找到时间结果的地址
 
//    printf("%s\r\n",Time_buff);
    if(data_pt != NULL)
    {
        day_string = data_pt + DAYS_ADD_DRES;          //日期地址
        moon_string = data_pt + MOON_ADD_DRES;         //月份地址
        year_string = data_pt + YEAR_ADD_DRES;        //年份地址
        hour_string = data_pt + HOURS_ADD_DRES;       //小时地址
        minute_string = data_pt + MINUTES_ADD_DRES;   //分钟地址
        second_string = data_pt + SECONDS_ADD_DRES;   //秒中地址
 
        //将时间信息传递给全局变量
        DAYS = Get_Day(day_string);
        MOONS = Get_Moonth(moon_string);
        YEARS = Get_Year(year_string);
        TIMES = Get_Times(hour_string, minute_string, second_string);
		
		hour_return = TIMES/3600;//小时
		min_return = (TIMES%3600)/60; //分钟
        sec_return = (TIMES%3600)%60; //秒数
		printf("时间获取并处理成功\r\n");
 
    }
    else
    {
        printf("时间获取失败\r\n");
    }
}
 
//得到年函数（以年开始的字符串长度过长，因此使用不一样的方法）
//输入值是年位置的地址
//返回值是 整型的10进制四位数
int Get_Year(char *y)
{
 
    int year_return;
    char *year_temp;
    char year[5] = {0};
    char i;
//年的获取须要提取一次字符串，不然没法读取
    year_temp = y;
 
    for(i = 0; i < 4; i++)
    {
        year[i] = *year_temp;
        year_temp ++;
    }
 
    year_return =  atoi(&year[0]);
    return year_return;
}
 
//得到月份函数
//输入值是月份位置的地址
//返回值是 整型的10进制两位数
int Get_Moonth(char *m)
{
    int moonth_return;
    moonth_return = atoi(m) / 100000000; //取月份
    return moonth_return;
}
 
//得到日期函数
//输入值是日期位置的地址
//返回值是 整型的10进制两位数
int Get_Day(char *d)
{ 
    int day_return;
    day_return = atoi(d) / 1000000; //取日期
 
    return day_return;
}
 
//得到时间
//输入值是时间的位置的地址
//返回值是 整型的10进制的时间总秒数
int Get_Times(char *h, char *m, char *s)
{
    int time_return;
    int hour_return;
    int min_return;
    int sec_return;
 
    hour_return = atoi(h) / 10000; //取小时
    min_return = atoi(m) / 100; //取分钟
    sec_return = atoi(s);   //取秒数
 
    time_return = hour_return * 3600 + min_return * 60 + sec_return; //转换成总秒数
 
    return time_return;
}
 
/*****************************************************************解析苏宁时间END********************************************************************************/
/*************************************************************************************************************************************************/
 
/**************************************************************************/
//函数作用：串口二中断函数
//函数名称：USART2_IRQHandler();
//内部参数：
//修改日期：2022年4月18日  下午4：18
/**************************************************************************/
void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收中断
    {
 
        if(esp8266_cnt >= sizeof(esp8266_buf))	esp8266_cnt = 0; //防止串口被刷爆
 
        esp8266_buf[esp8266_cnt++] = USART2->DR;
 
//		USART_SendData(USART1,USART2->DR);      //让接收到的数据打印在串口一上
 
        USART_ClearFlag(USART2, USART_FLAG_RXNE);
    }
}

//==========================================================
//	函数名称：	ESP8266_Clear
//
//	函数功能：	清空缓存
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：
//==========================================================
void ESP8266_Clear(void)
{
    memset(esp8266_buf, 0, sizeof(esp8266_buf));
    esp8266_cnt = 0;
 
}
//==========================================================
//	函数名称：	ESP8266_SendData
//
//	函数功能：	发送数据
//
//	入口参数：	data：数据
//				len：长度
//
//	返回参数：	无
//
//	说明：
//==========================================================
void ESP8266_SendData(unsigned char *data, unsigned short len)
{
    char cmdBuf[32];
 
    ESP8266_Clear();								//清空接收缓存
    sprintf(cmdBuf, "AT+CIPSEND\r\n");		//发送命令
 
    if(!ESP8266_SendCmd(cmdBuf, ">"))				//收到‘>’时可以发送数据
    {
        printf("9.开始处于透传发送状态!\r\n");
        /*发送请求数据*/
        Usart_SendString(USART2, data, len);		//发送设备连接请求数据**********************************
    }
}
 
//==========================================================
//	函数名称：	ESP8266_GetIPD
//
//	函数功能：	copy天气数据到buff数组里面
//
//	返回参数：	平台返回的原始数据
//
//	说明：		copy天气数据到buff
//==========================================================
 
unsigned char *ESP8266_GetIPD_GET(unsigned short timeOut, u8 *buff) //这里我用了一个全局变量将esp8266buf储存到这个全局变量里面
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
//	函数名称：	ESP8266_WaitRecive
//
//	函数功能：	等待接收完成
//
//	入口参数：	无
//
//	返回参数：	REV_OK-接收完成		REV_WAIT-接收超时未完成
//
//	说明：		循环调用检测是否接收完成
//==========================================================
int ESP8266_WaitRecive(void)
{
 
    if(esp8266_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
        return REV_WAIT;
 
    if(esp8266_cnt == esp8266_cntPre)				//如果上一次的值和这次相同，则说明接收完毕
    {
        esp8266_cnt = 0;							//清0接收计数
 
        return REV_OK;								//返回接收完成标志
    }
 
    esp8266_cntPre = esp8266_cnt;					//置为相同
 
    return REV_WAIT;								//返回接收未完成标志
 
}
 
 
//==========================================================
//	函数名称：	ESP8266_SendCmd
//
//	函数功能：	发送命令
//
//	入口参数：	cmd：命令
//				res：需要检查的返回指令
//
//	返回参数：	0-成功	1-失败
//
//	说明：
//==========================================================
int ESP8266_SendCmd(char *cmd, char *res)
{
 
    unsigned char timeOut = 250;
 
    Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
 
    while(timeOut--)
    {
        if(ESP8266_WaitRecive() == REV_OK)							//如果收到数据
        {
            if(strstr((const char *)esp8266_buf, res) != NULL)		//如果检索到关键词
            {
                ESP8266_Clear();									//清空缓存
 
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
		strcpy(str,"白天晴");
		*len = 3;
		n[0] = 52;
		n[1] = 53;
		n[2] = 22;
		return;
	}
	if(strcmp(str,"Clear")==0)
	{
		strcpy(str,"夜晚晴");
		*len = 3;
		n[0] = 54;
		n[1] = 55;
		n[2] = 22;
		return;
	}
	if(strcmp(str,"Cloudy")==0)
	{
		strcpy(str,"多云");
		*len = 2;
		n[0] = 56;
		n[1] = 23;
		return;
	}
	if(strcmp(str,"Partly Cloudy")==0)
	{
		strcpy(str,"晴间多云");
		*len = 4;
		n[0] = 22;
		n[1] = 57;
		n[2] = 56;
		n[3] = 23;
		return;
	}
	if(strcmp(str,"Mostly Cloudy")==0)
	{
		strcpy(str,"大部多云");
		*len = 4;
		n[0] = 12;
		n[1] = 58;
		n[2] = 56;
		n[3] = 23;
		return;
	}
	if(strcmp(str,"Overcast")==0)
	{
		strcpy(str,"阴");
		*len = 1;
		n[0] = 21;
		return;
	}
	if(strcmp(str,"Shower")==0)
	{
		strcpy(str,"阵雨");
		*len = 2;
		n[0] = 29;
		n[1] = 13;
		return;
	}
	if(strcmp(str,"Thundershower")==0)
	{
		strcpy(str,"雷阵雨");
		*len = 3;
		n[0] = 17;
		n[1] = 29;
		n[2] = 13;
		return;
	}
	if(strcmp(str,"Thundershower with Hail")==0)
	{
		strcpy(str,"雷阵雨伴有冰雹");
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
		strcpy(str,"小雨");
		*len = 2;
		n[0] = 10;
		n[1] = 13;
		return;
	}
	if(strcmp(str,"Moderate rain")==0)
	{
		strcpy(str,"中雨");
		*len = 2;
		n[0] = 11;
		n[1] = 13;
		return;
	}
	if(strcmp(str,"Heavy rain")==0)
	{
		strcpy(str,"大雨");
		*len = 2;
		n[0] = 12;
		n[1] = 13;
		return;
	}
	if(strcmp(str,"Storm")==0)
	{
		strcpy(str,"暴雨");
		*len = 2;
		n[0] = 24;
		n[1] = 13;
		return;
	}
	if(strcmp(str,"Heavy Storm")==0)
	{
		strcpy(str,"大暴雨");
		*len = 3;
		n[0] = 12;
		n[1] = 24;
		n[2] = 13;
		return;
	}
	if(strcmp(str,"Severe Storm")==0)
	{
		strcpy(str,"特大暴雨");
		*len = 4;
		n[0] = 28;
		n[1] = 12;
		n[2] = 24;
		n[0] = 13;
		return;
	}
	if(strcmp(str,"Ice Rain")==0)
	{
		strcpy(str,"冻雨");
		*len = 2;
		n[0] = 60;
		n[1] = 13;
		return;
	}
	if(strcmp(str,"Sleet")==0)
	{
		strcpy(str,"雨夹雪");
		*len = 3;
		n[0] = 13;
		n[1] = 37;
		n[2] = 14;
		return;
	}
	if(strcmp(str,"Snow Flurry")==0)
	{
		strcpy(str,"阵雪");
		*len = 2;
		n[0] = 29;
		n[1] = 14;
		return;
	}
	if(strcmp(str,"Light Snow")==0)
	{
		strcpy(str,"小雪");
		*len = 2;
		n[0] = 10;
		n[1] = 14;
		return;
	}
	if(strcmp(str,"Moderate Snow")==0)
	{
		strcpy(str,"中雪");
		*len = 2;
		n[0] = 11;
		n[1] = 14;
		return;
	}
	if(strcmp(str,"Heavy Snow")==0)
	{
		strcpy(str,"大雪");
		*len = 2;
		n[0] = 12;
		n[1] = 14;
		return;
	}
	if(strcmp(str,"Snowstorm")==0)
	{
		strcpy(str,"暴雪");
		*len = 2;
		n[0] = 24;
		n[1] = 14;
		return;
	}
	if(strcmp(str,"Dust")==0)
	{
		strcpy(str,"浮尘");
		*len = 2;
		n[0] = 32;
		n[1] = 61;
		return;
	}
	if(strcmp(str,"Sand")==0)
	{
		strcpy(str,"扬沙");
		*len = 2;
		n[0] = 33;
		n[1] = 16;
		return;
	}
	if(strcmp(str,"Duststorm")==0)
	{
		strcpy(str,"沙尘暴");
		*len = 3;
		n[0] = 16;
		n[1] = 61;
		n[2] = 24;
		return;
	}
	if(strcmp(str,"Sandstorm")==0)
	{
		strcpy(str,"强沙尘暴");
		*len = 4;
		n[0] = 27;
		n[1] = 16;
		n[2] = 61;
		n[3] = 24;
		return;
	}
	if(strcmp(str,"Foggy")==0)
	{
		strcpy(str,"雾");
		*len = 1;
		n[0] = 19;
		return;
	}
	if(strcmp(str,"Haze")==0)
	{
		strcpy(str,"霾");
		*len = 1;
		n[0] = 20;
		return;
	}
	if(strcmp(str,"Windy")==0)
	{
		strcpy(str,"风");
		*len = 1;
		n[0] = 15;
		return;
	}
	if(strcmp(str,"Blustery")==0)
	{
		strcpy(str,"大风");
		*len = 2;
		n[0] = 12;
		n[1] = 15;
		return;
	}
	if(strcmp(str,"Hurricane")==0)
	{
		strcpy(str,"飓风");
		*len = 2;
		n[0] = 25;
		n[1] = 15;
		return;
	}
	if(strcmp(str,"Tropical Storm")==0)
	{
		strcpy(str,"热带风暴");
		*len = 4;
		n[0] = 35;
		n[1] = 62;
		n[2] = 15;
		n[3] = 24;
		return;
	}
	if(strcmp(str,"Tornado")==0)
	{
		strcpy(str,"龙卷风");
		*len = 3;
		n[0] = 63;
		n[1] = 64;
		n[2] = 15;
		return;
	}
	if(strcmp(str,"Cold")==0)
	{
		strcpy(str,"冷");
		*len = 1;
		n[0] = 34;
		return;
	}
	if(strcmp(str,"Hot")==0)
	{
		strcpy(str,"热");
		*len = 1;
		n[0] = 35;
		return;
	}
	if(strcmp(str,"Unknown")==0)
	{
		strcpy(str,"未知");
		*len = 2;
		n[0] = 38;
		n[1] = 39;
		return;
	}	
}

void getLive(char str[3][20],int* num)
{
	//化妆 Makeup
	if(strncmp(str[0],"Moisturizing",12)==0)
	{
		memset(str[0],0,20);
		strcpy(str[0],"保湿");
		num[0]=81;
		num[1]=82;
	}
	else if(strncmp(str[0],"Sunscreen",9)==0)
	{
		memset(str[0],0,20);
		strcpy(str[0],"防晒");
		num[0]=79;
		num[1]=80;
	}
	else if(strncmp(str[0],"Anti-oil",8)==0)
	{
		memset(str[0],0,20);
		strcpy(str[0],"去油");
		num[0]=83;
		num[1]=84;
	}
	else
	{
		memset(str[0],0,20);
		strcpy(str[0],"保湿");
		num[0]=81;
		num[1]=82;
	}
	
	//运动 Sport
	if(strncmp(str[1],"Not Good",8)==0)
	{
		memset(str[1],0,20);
		strcpy(str[1],"不宜");
		num[2]=85;
		num[3]=73;
	}
	else if(strncmp(str[1],"Great",8)==0)
	{
		memset(str[1],0,20);
		strcpy(str[1],"适宜");
		num[2]=72;
		num[3]=73;
	}
	else
	{
		memset(str[1],0,20);
		strcpy(str[1],"适宜");
		num[2]=72;
		num[3]=73;
	}
	
	//紫外线 uv
	if(strncmp(str[2],"Very Low",8)==0)
	{
		memset(str[2],0,20);
		strcpy(str[2],"较弱");
		num[4]=74;
		num[5]=78;
	}
	else if(strncmp(str[2],"Low",3)==0)
	{
		memset(str[2],0,20);
		strcpy(str[2],"较弱");
		num[4]=74;
		num[5]=78;
	}
	else if(strncmp(str[2],"Moderate",8)==0)
	{
		memset(str[2],0,20);
		strcpy(str[2],"中等");
		num[4]=76;
		num[5]=77;
	}
	else
	{
		memset(str[2],0,20);
		strcpy(str[2],"较强");
		num[4]=74;
		num[5]=75;
	}
}