#ifndef __GY30_H
#define __GY30_H
#include "stm32f10x.h"
#include "delay.h"

//IO口定义 【和你的工程一致：PB6=SCL  PB7=SDA，无需修改】
#define GY30_SCL_PORT    GPIOB
#define GY30_SCL_PIN     GPIO_Pin_6
#define GY30_SDA_PORT    GPIOB
#define GY30_SDA_PIN     GPIO_Pin_7

//IO口操作宏定义
#define GY30_SCL_H  GPIO_SetBits(GY30_SCL_PORT,GY30_SCL_PIN)
#define GY30_SCL_L  GPIO_ResetBits(GY30_SCL_PORT,GY30_SCL_PIN)
#define GY30_SDA_H  GPIO_SetBits(GY30_SDA_PORT,GY30_SDA_PIN)
#define GY30_SDA_L  GPIO_ResetBits(GY30_SDA_PORT,GY30_SDA_PIN)
#define GY30_SDA_READ  GPIO_ReadInputDataBit(GY30_SDA_PORT,GY30_SDA_PIN)

//GY30(BH1750)设备地址和指令集
#define GY30_ADDR        0x5C        //默认地址 ADDR引脚接GND
#define GY30_POWER_ON    0x01        //上电指令
#define GY30_CONT_H_RES  0x10        //连续高分辨率模式 推荐使用

//函数声明
void GY30_I2C_Delay(void);
void GY30_I2C_Start(void);
void GY30_I2C_Stop(void);
u8 GY30_I2C_Wait_Ack(void);
void GY30_I2C_Ack(void);
void GY30_I2C_NAck(void);
void GY30_I2C_Send_Byte(u8 dat);
u8 GY30_I2C_Read_Byte(u8 ack);
u8 GY30_Write_Cmd(u8 cmd);
void GY30_Init(void);
uint16_t GY30_Read_Lux(void);

#endif