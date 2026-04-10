#include "ec_sensor.h"
#include "ec_adc.h"
#include <math.h>

// EC传感器数据结构
static EC_Sensor_TypeDef EC_Sensor;

/**
  * @brief  EC传感器初始化
  */
void EC_Sensor_Init(void)
{
    // 初始化ADC
    EC_ADC_Init();
    
    // 初始化数据结构
    EC_Sensor.voltage = 0.0f;
    EC_Sensor.rawEC = 0.0f;
    EC_Sensor.ecValue = 0.0f;
    EC_Sensor.temperature = 25.0f;  // 默认25℃
    EC_Sensor.kValue = 1.0f;
    EC_Sensor.kValue_Low = 1.0f;    // 默认校准系数
    EC_Sensor.kValue_High = 1.0f;   // 默认校准系数
}

/**
  * @brief  更新EC值计算（需要提供当前温度）
  * @param  temperature: 当前温度值（℃）
  */
void EC_Sensor_Update(float temperature)
{
    float EC_voltage;
    float EC_valueTemp;
    float compensationCoefficient;
    
    // 保存温度值
    EC_Sensor.temperature = temperature;
    
    // 读取ADC值并转换为电压（mV）
    // ADC是12位，参考电压3.3V = 3300mV
    EC_voltage = (float)EC_ADC_GetValue() / 4096.0f * 3300.0f;
    EC_Sensor.voltage = EC_voltage;
    
    // 计算原始EC值
    // 公式：EC = 1000 * V / RES2 / ECREF
    EC_Sensor.rawEC = 1000.0f * EC_voltage / EC_RES2 / EC_ECREF;
    EC_valueTemp = EC_Sensor.rawEC * EC_Sensor.kValue;
    
    // 根据EC值范围选择校准系数
    // 第一范围：(0, 2] mS/cm
    // 第二范围：(2, 20] mS/cm
    if(EC_valueTemp > 2.0f)
    {
        EC_Sensor.kValue = EC_Sensor.kValue_High;
    }
    else
    {
        EC_Sensor.kValue = EC_Sensor.kValue_Low;
    }
    
    // 应用校准系数
    EC_Sensor.ecValue = EC_Sensor.rawEC * EC_Sensor.kValue;
    
    // 温度补偿
    // 补偿系数公式：1.0 + 0.0185 * (T - 25.0)
    compensationCoefficient = 1.0f + 0.0185f * (temperature - 25.0f);
    EC_Sensor.ecValue = EC_Sensor.ecValue / compensationCoefficient;
    
    // 限制EC值范围 [0, 20] mS/cm
    if(EC_Sensor.ecValue <= 0.0f)
    {
        EC_Sensor.ecValue = 0.0f;
    }
    if(EC_Sensor.ecValue > 20.0f)
    {
        EC_Sensor.ecValue = 20.0f;
    }
}

/**
  * @brief  获取EC值（mS/cm）
  * @return EC值
  */
float EC_Sensor_GetECValue(void)
{
    return EC_Sensor.ecValue;
}

/**
  * @brief  获取电压值（mV）
  * @return 电压值
  */
float EC_Sensor_GetVoltage(void)
{
    return EC_Sensor.voltage;
}

/**
  * @brief  设置低范围校准系数
  * @param  kValue: 校准系数
  */
void EC_Sensor_SetKValue_Low(float kValue)
{
    EC_Sensor.kValue_Low = kValue;
}

/**
  * @brief  设置高范围校准系数
  * @param  kValue: 校准系数
  */
void EC_Sensor_SetKValue_High(float kValue)
{
    EC_Sensor.kValue_High = kValue;
}

/**
  * @brief  获取EC传感器数据结构指针
  * @return 数据结构指针
  */
EC_Sensor_TypeDef* EC_Sensor_GetData(void)
{
    return &EC_Sensor;
}