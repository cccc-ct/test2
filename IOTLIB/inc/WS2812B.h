#ifndef __WS2812B_H
#define	__WS2812B_H
#include "stm32f10x.h"  // 标准库核心头文件（根据你的MCU型号调整，如stm32f407x.h）

// WS2812B IO 定义（标准库格式）
#define WS2812B_PORT    GPIOB
#define WS2812B_PIN     GPIO_Pin_1  // 标准库引脚宏：GPIO_Pin_X（X=0~15）
#define WS2812B_RCC     RCC_APB2Periph_GPIOB  // GPIOB 时钟使能宏

// 高低电平控制（标准库函数）
#define WS2812B_Hi()    GPIO_SetBits(WS2812B_PORT, WS2812B_PIN)
#define WS2812B_Low()   GPIO_ResetBits(WS2812B_PORT, WS2812B_PIN)

// 颜色结构体（保持不变）
typedef struct {
    u8 G;  // 绿色通道（WS2812B 颜色顺序：G→R→B）
    u8 R;  // 红色通道
    u8 B;  // 蓝色通道
} Color_TypeDef;

// LED 灯珠个数（根据实际修改！）
#define PIXEL_NUM       30  

// 函数声明
void WS2812B_Init(void);                  // 初始化WS2812B引脚
void WS2812B_Reset(void);                 // 复位WS2812B
void WS2812B_WriteByte(u8 dat);           // 向WS2812B写1个字节
void RGB_LED_Write_24Bits(u8 green, u8 red, u8 blue);  // 写1个24bit颜色数据
void RGB_LED_Red(void);                   // 点亮单个红色LED
void RGB_LED_Green(void);                 // 点亮单个绿色LED
void RGB_LED_Blue(void);                  // 点亮单个蓝色LED
void WS2812B_WriteColor(Color_TypeDef *pColor);  // 写1个颜色结构体
void WS2812B_RefreshPixel(void);          // 更新所有LED显示
void Copy_Color(Color_TypeDef *pDst, Color_TypeDef *pScr);  // 复制颜色
void WS2812B_FillColor(u16 start, u16 end, Color_TypeDef *pColor);  // 填充颜色
// 全色点亮函数
void WS2812B_All_Blue(u8 num);
void WS2812B_All_Red(u8 num);
void WS2812B_All_Green(u8 num);
void WS2812B_All_Yellow(u8 num);
void WS2812B_All_White(u8 num);

#endif
