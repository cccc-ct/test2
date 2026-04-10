#ifndef __L9110_FAN_H
#define __L9110_FAN_H

#include "sys.h"  

// 引脚配置
#define INA_GPIO_PORT    GPIOA
#define INA_GPIO_PIN     GPIO_Pin_0
#define INB_GPIO_PORT    GPIOA
#define INB_GPIO_PIN     GPIO_Pin_1

// 函数声明
void L9110_Fan_Init(void);
void L9110_Fan_Forward(void);
void L9110_Fan_Reverse(void);
void L9110_Fan_Stop(void);

#endif