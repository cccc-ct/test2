#include "gy30.h"

//软件I2C延时，标准时序，无需修改
void GY30_I2C_Delay(void)
{
    delay_us(2);
}

//软件I2C起始信号
void GY30_I2C_Start(void)
{
    GY30_SDA_H;
    GY30_SCL_H;
    GY30_I2C_Delay();
    GY30_SDA_L;
    GY30_I2C_Delay();
    GY30_SCL_L;
}

//软件I2C停止信号
void GY30_I2C_Stop(void)
{
    GY30_SDA_L;
    GY30_SCL_L;
    GY30_I2C_Delay();
    GY30_SCL_H;
    GY30_I2C_Delay();
    GY30_SDA_H;
}

//软件I2C等待应答，返回0=有应答，返回1=无应答/超时【修复版，增加超时防卡死】
u8 GY30_I2C_Wait_Ack(void)
{
    u8 ack=1;
    u16 timeout=0;
    GY30_SDA_H;
    GY30_I2C_Delay();
    GY30_SCL_H;
    GY30_I2C_Delay();
    //超时等待，防止程序卡死，最大200us
    while(GY30_SDA_READ && timeout<200)
    {
        timeout++;
        GY30_I2C_Delay();
    }
    if(timeout>=200)
    {
        GY30_I2C_Stop();
        return 1;
    }
    else
    {
        ack=0; //有应答，正常
    }
    GY30_SCL_L;
    GY30_I2C_Delay();
    return ack;
}

//主机发送应答信号(ACK) 【新增必备函数】
void GY30_I2C_Ack(void)
{
    GY30_SDA_L;
    GY30_I2C_Delay();
    GY30_SCL_H;
    GY30_I2C_Delay();
    GY30_SCL_L;
    GY30_I2C_Delay();
    GY30_SDA_H;
}

//主机发送非应答信号(NACK) 【新增必备函数】
void GY30_I2C_NAck(void)
{
    GY30_SDA_H;
    GY30_I2C_Delay();
    GY30_SCL_H;
    GY30_I2C_Delay();
    GY30_SCL_L;
    GY30_I2C_Delay();
}

//软件I2C发送一个字节
void GY30_I2C_Send_Byte(u8 dat)
{
    u8 i;
    for(i=0;i<8;i++)
    {
        GY30_SCL_L;
        GY30_I2C_Delay();
        if(dat&0x80)GY30_SDA_H;
        else GY30_SDA_L;
        dat<<=1;
        GY30_I2C_Delay();
        GY30_SCL_H;
        GY30_I2C_Delay();
    }
    GY30_SCL_L;
}

//软件I2C读取一个字节，ack=1 发应答(继续读)，ack=0 发非应答(读完停止)【完全修复版】
u8 GY30_I2C_Read_Byte(u8 ack)
{
    u8 i,dat=0;
    GY30_SDA_H;  //释放SDA总线，准备读取数据
    for(i=0;i<8;i++)
    {
        GY30_SCL_L;
        GY30_I2C_Delay();
        GY30_SCL_H;
        GY30_I2C_Delay();
        dat<<=1;
        if(GY30_SDA_READ)dat|=0x01;
    }
    GY30_SCL_L;
    if(ack)
    {
        GY30_I2C_Ack();  //发应答，继续读取下一个字节
    }
    else
    {
        GY30_I2C_NAck(); //发非应答，结束读取
    }
    GY30_I2C_Delay();
    return dat;
}

//写指令到GY30
u8 GY30_Write_Cmd(u8 cmd)
{
    GY30_I2C_Start();
    GY30_I2C_Send_Byte(GY30_ADDR<<1);
    if(GY30_I2C_Wait_Ack()){GY30_I2C_Stop();return 1;}
    GY30_I2C_Send_Byte(cmd);
    if(GY30_I2C_Wait_Ack()){GY30_I2C_Stop();return 1;}
    GY30_I2C_Stop();
    return 0;
}

//GY30初始化（仅初始化GPIO，时钟已在System_Clock_Init开启，无需重复开启）
void GY30_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;    //开漏输出 软件I2C必用
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_InitStructure.GPIO_Pin = GY30_SCL_PIN;
    GPIO_Init(GY30_SCL_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GY30_SDA_PIN;
    GPIO_Init(GY30_SDA_PORT, &GPIO_InitStructure);
    
    GY30_SCL_H;
    GY30_SDA_H;
    GY30_Write_Cmd(GY30_POWER_ON);    //GY30上电
    GY30_Write_Cmd(GY30_CONT_H_RES);  //设置连续高分辨率模式，配置一次永久生效
}

//读取光照值 核心函数  返回值uint16_t 直接赋值给light_lux【修复核心错误，删除重复指令】
uint16_t GY30_Read_Lux(void)
{
    u16 lux=0;
    u8 buf[2];
    delay_ms(120);  //BH1750高分辨率模式必须的采集等待时间，不能删除
    GY30_I2C_Start();
    GY30_I2C_Send_Byte((GY30_ADDR<<1)|1);  //发送读地址
    if(GY30_I2C_Wait_Ack()){GY30_I2C_Stop();return 0;}
    buf[0] = GY30_I2C_Read_Byte(1);  //读取高8位，发送应答，继续读低位
    buf[1] = GY30_I2C_Read_Byte(0);  //读取低8位，发送非应答，结束读取
    GY30_I2C_Stop();                 //新增停止信号，时序更完整
    lux = (buf[0]<<8)+buf[1];
    return (uint16_t)(lux/1.2);      //转换为实际光照值 LUX，公式正确
}