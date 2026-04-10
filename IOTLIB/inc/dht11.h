//dth11.h³ÌÐò
#ifndef __DHT11_H
#define __DHT11_H
 
#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"

 
void DHT11_GPIO_OUT(void);
void DHT11_GPIO_IN(void);
uint8_t DHT_Read_Byte(void);
uint8_t DHT_Read(void);
 
#endif 
