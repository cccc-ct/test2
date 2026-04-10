#include "l9110_fan.h"

void L9110_Fan_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    
    // 使能GPIOA时钟
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 配置PA0、PA1为推挽输出
    GPIO_InitStruct.GPIO_Pin = INA_GPIO_PIN | INB_GPIO_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;  // 50MHz速率
    GPIO_Init(INA_GPIO_PORT, &GPIO_InitStruct);
	
    // 初始停止
    L9110_Fan_Stop();
}

void L9110_Fan_Forward(void)
{
    GPIO_SetBits(INA_GPIO_PORT, INA_GPIO_PIN);  // 置高
    GPIO_ResetBits(INB_GPIO_PORT, INB_GPIO_PIN); // 置低
}

void L9110_Fan_Reverse(void)
{
    GPIO_ResetBits(INA_GPIO_PORT, INA_GPIO_PIN);
    GPIO_SetBits(INB_GPIO_PORT, INB_GPIO_PIN);
}

void L9110_Fan_Stop(void)
{
    GPIO_ResetBits(INA_GPIO_PORT, INA_GPIO_PIN);
    GPIO_ResetBits(INB_GPIO_PORT, INB_GPIO_PIN);
}