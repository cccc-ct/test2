#include "WS2812B.h"
#include "delay.h"

// 像素数据缓存（存储每个LED的颜色）
Color_TypeDef PixelBuf[PIXEL_NUM];
u8 flag;
Color_TypeDef Color_Prar;

// 晶振216M, 1/216≈0.004629us=4.6ns（延时宏保持不变）
#define delay_4_6ns    __NOP()                          // 1个NOP=4.6ns（标准库用__NOP()）
#define delay_23ns     delay_4_6ns;delay_4_6ns;delay_4_6ns;delay_4_6ns;delay_4_6ns
#define delay_115ns    delay_23ns;delay_23ns;delay_23ns;delay_23ns;delay_23ns
#define delay_391ns    delay_115ns;delay_115ns;delay_115ns;delay_23ns;delay_23ns  // T0H
#define delay_851ns    delay_805ns;delay_23ns;delay_23ns                          // T0L
#define delay_460ns    delay_115ns;delay_115ns;delay_115ns;delay_115ns            // T1L
#define delay_805ns    delay_460ns;delay_115ns;delay_115ns;delay_115ns            // T1H

/*---------------------------------------------------------------------------
@Function   :WS2812B_Init
@Description:初始化WS2812B控制引脚（PB1）
@Note       :标准库GPIO配置：推挽输出、上拉、50MHz
----------------------------------------------------------------------------*/
void WS2812B_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    // 使能GPIOB时钟（标准库时钟使能函数）
    RCC_APB2PeriphClockCmd(WS2812B_RCC, ENABLE);
    
    // GPIO配置参数（标准库结构体成员）
    GPIO_Initure.GPIO_Pin   = WS2812B_PIN;          // 选择PB1引脚
    GPIO_Initure.GPIO_Mode  = GPIO_Mode_Out_PP;     // 推挽输出（标准库宏）
    GPIO_Initure.GPIO_Speed = GPIO_Speed_50MHz;     // 输出速度50MHz（标准库宏）
    
    // 初始化GPIO（标准库初始化函数）
    GPIO_Init(WS2812B_PORT, &GPIO_Initure);
    
    // 初始状态：引脚置高（WS2812B空闲状态）
    GPIO_SetBits(WS2812B_PORT, WS2812B_PIN);
}

/*---------------------------------------------------------------------------
@Function   :WS2812B_Reset
@Description:WS2812B复位（低电平≥50μs）
----------------------------------------------------------------------------*/
void WS2812B_Reset(void)
{
    WS2812B_Low();
    delay_us(300);  // 复位延时（足够长，确保稳定复位）
}

/*---------------------------------------------------------------------------
@Function   :WS2812B_WriteByte
@Description:向WS2812B写1个字节（高位先发送）
@Input      :dat：要发送的字节数据
----------------------------------------------------------------------------*/
void WS2812B_WriteByte(u8 dat)
{
    u8 i;
    for (i = 0; i < 8; i++)
    {
        if (dat & 0x80)  // 发送高位（1bit）
        {
            WS2812B_Hi();
            delay_805ns;  // T1H：805ns高电平
            WS2812B_Low();
            delay_460ns;  // T1L：460ns低电平
        }
        else  // 发送0bit
        {
            WS2812B_Hi();
            delay_391ns;  // T0H：391ns高电平
            WS2812B_Low();
            delay_851ns;  // T0L：851ns低电平
        }
        dat <<= 1;  // 左移，准备发送下一位
    }
}

/*---------------------------------------------------------------------------
@Function   :RGB_LED_Write_24Bits
@Description:写入1个24bit颜色数据（G→R→B顺序）
@Input      :green：绿色分量（0~255）；red：红色分量；blue：蓝色分量
----------------------------------------------------------------------------*/
void RGB_LED_Write_24Bits(u8 green, u8 red, u8 blue)
{
    WS2812B_WriteByte(green);
    WS2812B_WriteByte(red);
    WS2812B_WriteByte(blue);
    WS2812B_Reset();
}

/*---------------------------------------------------------------------------
@Function   :RGB_LED_Red
@Description:点亮1个红色LED
----------------------------------------------------------------------------*/
void RGB_LED_Red(void)
{
    u8 i;
    for (i = 0; i < 1; i++)
    {
        RGB_LED_Write_24Bits(0, 0xFF, 0);  // G=0, R=255, B=0
    }
}

/*---------------------------------------------------------------------------
@Function   :RGB_LED_Green
@Description:点亮1个绿色LED
----------------------------------------------------------------------------*/
void RGB_LED_Green(void)
{
    u8 i;
    for (i = 0; i < 1; i++)
    {
        RGB_LED_Write_24Bits(0xFF, 0, 0);  // G=255, R=0, B=0
    }
}

/*---------------------------------------------------------------------------
@Function   :RGB_LED_Blue
@Description:点亮1个蓝色LED
----------------------------------------------------------------------------*/
void RGB_LED_Blue(void)
{
    u8 i;
    for (i = 0; i < 1; i++)
    {
        RGB_LED_Write_24Bits(0, 0, 0xFF);  // G=0, R=0, B=255
    }
}

/*---------------------------------------------------------------------------
@Function   :WS2812B_WriteColor
@Description:写入1个颜色结构体数据（G→R→B顺序）
@Input      :pColor：颜色结构体指针
----------------------------------------------------------------------------*/
void WS2812B_WriteColor(Color_TypeDef *pColor)
{
    WS2812B_WriteByte(pColor->G);
    WS2812B_WriteByte(pColor->R);
    WS2812B_WriteByte(pColor->B);
}

/*---------------------------------------------------------------------------
@Function   :WS2812B_RefreshPixel
@Description:更新所有LED显示（发送PixelBuf中的颜色数据）
----------------------------------------------------------------------------*/
void WS2812B_RefreshPixel(void)
{
    u8 i;
    for (i = 0; i < PIXEL_NUM; i++)
    {
        WS2812B_WriteColor(&PixelBuf[i]);
    }
    WS2812B_Reset();  // 所有数据发送完毕后复位，确保灯带同步点亮
}

/*---------------------------------------------------------------------------
@Function   :Copy_Color
@Description:复制颜色结构体数据
@Input      :pDst：目标结构体指针；pScr：源结构体指针
----------------------------------------------------------------------------*/
void Copy_Color(Color_TypeDef *pDst, Color_TypeDef *pScr)
{
    pDst->R = pScr->R;
    pDst->G = pScr->G;
    pDst->B = pScr->B;
}

/*---------------------------------------------------------------------------
@Function   :WS2812B_FillColor
@Description:填充指定范围LED的颜色
@Input      :start：开始索引；end：结束索引；pColor：颜色结构体指针
----------------------------------------------------------------------------*/
void WS2812B_FillColor(u16 start, u16 end, Color_TypeDef *pColor)
{
    if (start > end)  // 交换起始和结束索引（避免参数错误）
    {
        u16 temp = start;
        start = end;
        end = temp;
    }
    
    // 边界检查（避免数组越界）
    if (start >= PIXEL_NUM) return;
    if (end >= PIXEL_NUM) end = PIXEL_NUM - 1;
    
    // 填充颜色到缓存
    while (start <= end)
    {
        Copy_Color(&PixelBuf[start], pColor);
        start++;
    }
}

/*---------------------------------------------------------------------------
@Function   :WS2812B_All_Blue
@Description:点亮所有LED为蓝色
@Input      :num：LED个数（建议传入PIXEL_NUM）
----------------------------------------------------------------------------*/
void WS2812B_All_Blue(u8 num)
{
    Color_Prar.G = 0x00;
    Color_Prar.R = 0x00;
    Color_Prar.B = 0xFF;
    WS2812B_FillColor(0, num - 1, &Color_Prar);  // 0~num-1：避免越界
}

/*---------------------------------------------------------------------------
@Function   :WS2812B_All_Red
@Description:点亮所有LED为红色
----------------------------------------------------------------------------*/
void WS2812B_All_Red(u8 num)
{
    Color_Prar.G = 0x00;
    Color_Prar.R = 0xFF;
    Color_Prar.B = 0x00;
    WS2812B_FillColor(0, num - 1, &Color_Prar);
}

/*---------------------------------------------------------------------------
@Function   :WS2812B_All_Green
@Description:点亮所有LED为绿色
----------------------------------------------------------------------------*/
void WS2812B_All_Green(u8 num)
{
    Color_Prar.G = 0xFF;
    Color_Prar.R = 0x00;
    Color_Prar.B = 0x00;
    WS2812B_FillColor(0, num - 1, &Color_Prar);
}

/*---------------------------------------------------------------------------
@Function   :WS2812B_All_Yellow
@Description:点亮所有LED为黄色（R+G）
----------------------------------------------------------------------------*/
void WS2812B_All_Yellow(u8 num)
{
    Color_Prar.G = 0xFF;
    Color_Prar.R = 0xFF;
    Color_Prar.B = 0x00;
    WS2812B_FillColor(0, num - 1, &Color_Prar);
}

/*---------------------------------------------------------------------------
@Function   :WS2812B_All_White
@Description:点亮所有LED为白色（R+G+B）
----------------------------------------------------------------------------*/
void WS2812B_All_White(u8 num)
{
    Color_Prar.G = 0xFF;  // 绿色满值
    Color_Prar.R = 0xFF;  // 红色满值
    Color_Prar.B = 0xFF;  // 蓝色满值
    WS2812B_FillColor(0, num - 1, &Color_Prar);  // 0~num-1：避免越界
}
