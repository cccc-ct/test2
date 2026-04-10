

/*
本程序所占用的GPIO接口PA13、PA14上电后为JTAG功能，
需要在RCC程序里启动AFIO时钟，再在RELAY_Init函数里加入：
GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
// 改变指定管脚的映射,完全禁用JTAG+SW-DP才能将JATG接口重定义为GPIO

*/

#include "relay.h"

void RELAY_Init(void){ //继电器的接口初始化
	GPIO_InitTypeDef  GPIO_InitStructure; 	
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE); //APB2外设时钟使能 
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); 	
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);//启动AFIO重映射功能时钟  
	
  //初始化RELAY1 (PA8)
  GPIO_InitStructure.GPIO_Pin = RELAY1;                    	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //选择IO接口工作方式       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //设置IO接口速度（2/10/50MHz）    
	GPIO_Init(RELAYPORT1, &GPIO_InitStructure);
	GPIO_SetBits(RELAYPORT1,RELAY1 ); //都为高电平 初始为关继电器
	
	//初始化RELAY2 (PA9)
	GPIO_InitStructure.GPIO_Pin = RELAY2;                       
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(RELAYPORT2, &GPIO_InitStructure);
	GPIO_SetBits(RELAYPORT2,RELAY2 ); 
	
	//初始化RELAY3 (PA10)
	GPIO_InitStructure.GPIO_Pin = RELAY3;                       
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(RELAYPORT3, &GPIO_InitStructure);
	GPIO_SetBits(RELAYPORT3,RELAY3 ); 
	
		//初始化RELAY4 (PB8)
	GPIO_InitStructure.GPIO_Pin = RELAY4;                       
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(RELAYPORT4, &GPIO_InitStructure);
	GPIO_SetBits(RELAYPORT4,RELAY4 ); 
	
		//初始化RELAY5 (PB9)
	GPIO_InitStructure.GPIO_Pin = RELAY5;                       
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_Init(RELAYPORT5, &GPIO_InitStructure);
	GPIO_SetBits(RELAYPORT5,RELAY5 ); 
}

void RELAY_1(u8 c){ //继电器1的控制程序（c=0继电器放开，c=1继电器吸合）
	GPIO_WriteBit(RELAYPORT1,RELAY1,(BitAction)(c));//通过参数值写入接口
}
void RELAY_2(u8 c){ //继电器2的控制程序（c=0继电器放开，c=1继电器吸合）
	GPIO_WriteBit(RELAYPORT2,RELAY2,(BitAction)(c));//通过参数值写入接口
}
void RELAY_3(u8 c){ //继电器3的控制程序（c=0继电器放开，c=1继电器吸合）
	GPIO_WriteBit(RELAYPORT3,RELAY3,(BitAction)(c));//通过参数值写入接口
}
void RELAY_4(u8 c){ //继电器4的控制程序（c=0继电器放开，c=1继电器吸合）
	GPIO_WriteBit(RELAYPORT4,RELAY4,(BitAction)(c));//通过参数值写入接口
}
void RELAY_5(u8 c){ //继电器5的控制程序（c=0继电器放开，c=1继电器吸合）
	GPIO_WriteBit(RELAYPORT5,RELAY5,(BitAction)(c));//通过参数值写入接口
}




