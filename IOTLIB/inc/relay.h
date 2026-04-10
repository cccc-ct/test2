#ifndef __RELAY_H
#define __RELAY_H	 
#include "sys.h"


#define RELAYPORT1	GPIOA	//定义IO接口
#define RELAY1	GPIO_Pin_8	//定义IO接口 Relay Signal Pin connects to STM32 A8 Pin
#define RELAYPORT2	GPIOA	//定义IO接口
#define RELAY2	GPIO_Pin_9	//定义IO接口 Relay Signal Pin connects to STM32 A9 Pin
#define RELAYPORT3	GPIOA	//定义IO接口
#define RELAY3	GPIO_Pin_10	//定义IO接口 Relay Signal Pin connects to STM32 A10 Pin
#define RELAYPORT4	GPIOB	//定义IO接口
#define RELAY4	GPIO_Pin_8	//定义IO接口 Relay Signal Pin connects to STM32 B8 Pin
#define RELAYPORT5	GPIOB	//定义IO接口
#define RELAY5	GPIO_Pin_9	//定义IO接口 Relay Signal Pin connects to STM32 B9 Pin
void RELAY_Init(void);//继电器初始化
void RELAY_1(u8 c);//继电器控制1
void RELAY_2(u8 c);//继电器控制2
void RELAY_3(u8 c);//继电器控制3
void RELAY_4(u8 c);//继电器控制4	
void RELAY_5(u8 c);//继电器控制5

#endif
