#include "stm32f10x.h" //STM32头文件
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "dht11.h"
#include "esp8266.h"
#include "led.h"
#include "tcpserver.h"
//c库
#include "string.h"
//dht11添加变量
extern u8 dat[5];
extern u8  flag;

int main (void){//主程序
	u8 res;
	unsigned char *dataPtr = NULL;
	unsigned short timeCount = 0;	//发送间隔变量
	char dataStr='a';
	//初始化程序
	delay_ms(10000);
	//NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	LED_Init();//LED初始化
	USART1_Init(115200); //串口初始化（参数是波特率）
	USART1_printf("DHT11 OK:\n");//初始化结束
  delay_ms(1000);
	//ESP WiFi Init
	USART2_Init(115200);	
	ESP8266_Init();					//初始化ESP8266	
	USART1_printf("8266_INIT_END\n");//初始化结束
  delay_ms(1000);
	while(1){
		USART1_printf("DHT11 OK:\n");//初始化结束
		if(++timeCount >= 500)		//时间间隔5s
		{
      if(DHT_Read()){
				USART1_printf("Hum= %d .%d Temp=%d .%d\r\n",dat[0],dat[1],dat[2],dat[3]);
				TCPServer_SendData();
		}
		delay_ms(2000); //延时，刷新数据的频率（不得小于1秒）
		}
	}
}




