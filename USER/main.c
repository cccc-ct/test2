#include "stm32f10x.h" //STM32XH
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "dht11.h"
#include "string.h"

extern u8 dat[5];
extern u8 flag;
int main(void){
	u8 res;
	unsigned char *dataPtr =NULL;
	unsigned short timeCount=0;
	delay_ms(3000);
	USART1_Init(115200);
	USART1_printf("DHT11 ok:\n");
	delay_ms(1000);
	while(1){
		USART1_printf("DHT11 OK:\n");
		if(++timeCount >= 500)
		{
			if(DHT_Read()){
				USART1_printf("STM32-Hum=%d .%d STM32-Temp=%d .%d\r\n",dat[0],dat[1],dat[2],dat[3]);
			}
		delay_ms(2000);
		}
	}
}