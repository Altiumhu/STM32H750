/*
 * AppUuser_Temper.h
 *
 *  Created on: 2023年5月20日
 *      Author: admin
 */

#ifndef APPAPI_APPUUSER_TEMPER_H_
#define APPAPI_APPUUSER_TEMPER_H_


#include "head.h"     // head.h 头文件

#define TEMP_SCALE 150 //最大温度校准点数
#define MINTEMP    0   //最小温度 0.1℃
#define MAXTEMP    1500 //最大温度 0.1℃

#define TEMP_CHANNEL_NUM 8       //通道温度

typedef struct
{
    int32_t adc[TEMP_CHANNEL_NUM];
    float value[TEMP_CHANNEL_NUM];
    float current[TEMP_CHANNEL_NUM];
} Structural_Body_TEMP;

typedef  struct Tempdataformat
{
    uint16_t  *Resistance;//NTC RK5的查表参数
    uint16_t  *EnvResistance;//NTC RK10的查表参数
//    _TempOffSetFormat OffSet; //偏差补偿
    uint16_t  Scan_enable; //扫描模式
}_Tempdataformat;

extern _Tempdataformat TempFormat;
typedef enum
{
    PT100_Sensor = 0U,
    PT1000_Sensor,
    NTC_5K,
    NTC_10K

} Sensor_typedef_enum;

extern void AppUuser_InitTempData(void);
extern float AppUser_Get_Channel_Temp(int ch, uint16_t adc_Sample,Sensor_typedef_enum u_PTSensor_Mode);

extern void AppUserDebug_TempVaule(void);
extern void AppUserRead_TempVaule(void);

extern void AppUser_temp_sample(void);

uint16_t Get_Master_TempVaule(uint16_t ch);


#endif /* APPAPI_APPUUSER_TEMPER_H_ */
