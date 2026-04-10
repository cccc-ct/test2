#ifndef ADC_H
#define ADC_H
#include "stm32f10x_adc.h"
u16 Get_Adc(u8 ch);
u16 Get_Adc_Average(u8 ch,u8 times);
void adc_init(void);
#endif
