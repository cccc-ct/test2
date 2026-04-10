#include "stm32f10x.h" //STM32 
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "dht11.h"
#include "esp8266.h"
#include "led.h"
#include "tcpserver.h"
#include "string.h"
#include "relay.h"
extern u8 dat[5];
extern u8  flag;
char buffer1[200];
char buffer2[200];
int main (void){ 
	unsigned short timeCount = 0;	
	USART1_Init(9600);  
	USART1_printf("DHT11 OK:\n"); 
  delay_ms(1000);
	RELAY_Init();//¼ÌµçÆ÷³õÊ¼»¯
	//ESP WiFi Init
	USART2_Init(115200);	
	ESP8266_Init();	//ESP8266	
	USART1_printf("8266_INIT_END\n"); 
  delay_ms(1000);
	
	while(1){
		USART1_printf("DHT11 OK:\n"); 
		if(++timeCount >= 500)// 
		{
      if(DHT_Read()){
				USART1_printf("Hum=%d.%d Temp=%d.%d\r\n",dat[0],dat[1],dat[2],dat[3]);
				TCPServer_SendData();
			}
			delay_ms(3000);
		}
	}
}