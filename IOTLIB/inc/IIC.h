#ifndef __IIC_H
#define __IIC_H




#include "stm32f10x.h"
#include "delay.h"
#include "sys.h" 


/*********************************
**	引脚选择：					**
**			SCL ---> PXx		**
**			SDA ---> PXx		**
**********************************/
	
/*******************************保持时间***********************************/  
#define  IIC_Delay()		{	delay_us(5);	}	   		   

/*************************** IO方向设置(GPIOx * 4) ****************************/
 
					/**		PXx 上拉输入模式		**/
#define  SDA_IN()  {GPIOB -> CRL &= 0X0FFFFFFF; GPIOB -> CRL |= (u32)8 << 28;}
					/**		PXx 推挽输出模式		**/
#define  SDA_OUT() {GPIOB -> CRL &= 0X0FFFFFFF; GPIOB -> CRL |= (u32)3 << 28;}





/*************************** IO操作函数 *************************/	 
#define 		IIC_SCL    		PBout(6) 	//SCL 输出
#define 		IIC_SDA    		PBout(7) 	//SDA 输出	
#define 		Read_SDA   		PBin (7)  	//SDA 输入





/***************************** IIC所有操作函数 **********************************/

void 				IIC_Init				(void);             //初始化IIC的IO口
void 				IIC_Start				(void);				//发送IIC开始信号
void 				IIC_Stop				(void);	  			//发送IIC停止信号
void 				IIC_Send_Byte			(uint8_t Txd);		//IIC发送一个字节
uint8_t 			IIC_Read_Byte			(uint8_t ACK);		//IIC读取一个字节
uint8_t 			IIC_Wait_Ack			(void); 			//IIC等待ACK信号
void 				IIC_Ack					(void);				//IIC发送ACK信号
void 				IIC_NAck				(void);				//IIC不发送ACK信号


 
#endif
