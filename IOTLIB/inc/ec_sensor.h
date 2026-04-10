#ifndef __EC_SENSOR_H
#define __EC_SENSOR_H

#include "stm32f10x.h"

// 硬件参数定义
#define EC_RES2    820.0f    // 上拉电阻值（Ω）
#define EC_ECREF   200.0f    // 参考电压值

// EC传感器数据结构
typedef struct {
    float voltage;          // 电压值（mV）
    float rawEC;            // 原始EC值
    float ecValue;          // 最终EC值（mS/cm）
    float temperature;      // 温度值（℃）
    float kValue;           // 当前使用的校准系数
    float kValue_Low;       // 低范围校准系数（0-2 mS/cm）
    float kValue_High;      // 高范围校准系数（2-20 mS/cm）
} EC_Sensor_TypeDef;

// 函数声明
void EC_Sensor_Init(void);
void EC_Sensor_Update(float temperature);
float EC_Sensor_GetECValue(void);
float EC_Sensor_GetVoltage(void);
void EC_Sensor_SetKValue_Low(float kValue);
void EC_Sensor_SetKValue_High(float kValue);
EC_Sensor_TypeDef* EC_Sensor_GetData(void);

#endif /* __EC_SENSOR_H */