#ifndef __EC_ADC_H
#define __EC_ADC_H

#include "stm32f10x.h"

// ADC配置 - EC传感器使用PA2 (ADC1通道2)
#define EC_ADCx                    ADC1
#define EC_ADC_APBxClock_FUN       RCC_APB2PeriphClockCmd
#define EC_ADC_CLK                 RCC_APB2Periph_ADC1
#define EC_ADC_GPIO_APBxClock_FUN  RCC_APB2PeriphClockCmd
#define EC_ADC_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EC_ADC_PORT                GPIOA
#define EC_ADC_PIN                 GPIO_Pin_4
#define EC_ADC_CHANNEL             ADC_Channel_2
#define EC_ADC_DMA_CHANNEL         DMA1_Channel1

// 外部变量：ADC转换值（通过DMA自动更新）
extern __IO uint16_t EC_ADC_ConvertedValue;

// 函数声明
void EC_ADC_Init(void);
uint16_t EC_ADC_GetValue(void);

#endif /* __EC_ADC_H */