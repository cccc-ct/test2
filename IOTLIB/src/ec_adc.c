#include "ec_adc.h"
#include "stm32f10x_dma.h"

// ADC转换值，通过DMA自动更新
__IO uint16_t EC_ADC_ConvertedValue;

/**
  * @brief  EC ADC GPIO初始化
  */
static void EC_ADC_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能ADC GPIO时钟
    EC_ADC_GPIO_APBxClock_FUN(EC_ADC_GPIO_CLK, ENABLE);
    
    // 配置ADC IO为模拟输入模式
    GPIO_InitStructure.GPIO_Pin = EC_ADC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(EC_ADC_PORT, &GPIO_InitStructure);
}

/**
  * @brief  配置ADC DMA模式
  */
static void EC_ADC_Mode_Config(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    
    // 使能DMA时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    // 使能ADC时钟
    EC_ADC_APBxClock_FUN(EC_ADC_CLK, ENABLE);
    
    // 复位DMA控制器
    DMA_DeInit(EC_ADC_DMA_CHANNEL);
    
    // 配置DMA初始化结构体
    // 外设地址为ADC数据寄存器地址
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(EC_ADCx->DR));
    
    // 存储器地址，实际上就是一个内部SRAM的变量
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&EC_ADC_ConvertedValue;
    
    // 数据源：外设到存储器
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    
    // 缓冲区大小为1，缓冲区大小应等于存储器的大小
    DMA_InitStructure.DMA_BufferSize = 1;
    
    // 外设寄存器地址固定
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    
    // 存储器地址固定
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    
    // 外设数据大小为半字，即16位
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    
    // 存储器数据大小也为半字，与外设数据大小相同
    DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_HalfWord;
    
    // 循环传输模式
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    
    // DMA通道优先级为高，当使用一个DMA通道时优先级设置不影响
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    
    // 禁止存储器到存储器模式，因为是外设到存储器
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    
    // 初始化DMA
    DMA_Init(EC_ADC_DMA_CHANNEL, &DMA_InitStructure);
    
    // 使能DMA通道
    DMA_Cmd(EC_ADC_DMA_CHANNEL, ENABLE);
    
    // ADC模式配置
    // 只使用一个ADC，属于独立模式
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    
    // 禁止扫描模式，多通道才要，单通道不需要
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    
    // 连续转换模式
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
    
    // 不使用外部触发转换，软件开启即可
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    
    // 转换结果右对齐
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    
    // 转换通道1个
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    
    // 初始化ADC
    ADC_Init(EC_ADCx, &ADC_InitStructure);
    
    // 配置ADC时钟为PCLK2的8分频，即9MHz
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);
    
    // 配置ADC通道转换顺序为1，第一个转换，采样时间为55.5个时钟周期
    ADC_RegularChannelConfig(EC_ADCx, EC_ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5);
    
    // 使能ADC DMA请求
    ADC_DMACmd(EC_ADCx, ENABLE);
    
    // 使能ADC
    ADC_Cmd(EC_ADCx, ENABLE);
    
    // 初始化ADC校准寄存器
    ADC_ResetCalibration(EC_ADCx);
    // 等待校准寄存器初始化完成
    while(ADC_GetResetCalibrationStatus(EC_ADCx));
    
    // ADC开始校准
    ADC_StartCalibration(EC_ADCx);
    // 等待校准完成
    while(ADC_GetCalibrationStatus(EC_ADCx));
    
    // 由于没有采用外部触发，所以使用软件触发ADC转换
    ADC_SoftwareStartConvCmd(EC_ADCx, ENABLE);
}

/**
  * @brief  EC ADC初始化
  */
void EC_ADC_Init(void)
{
    EC_ADC_GPIO_Config();
    EC_ADC_Mode_Config();
}

/**
  * @brief  获取ADC转换值
  * @return ADC转换值（12位，0-4095）
  */
uint16_t EC_ADC_GetValue(void)
{
    return EC_ADC_ConvertedValue;
}