#ifndef USRE_APP_USER_SAMPLE_H_
#define USRE_APP_USER_SAMPLE_H_
#include "head.h"
#include "system_Config.h"
// 单通道数据数据结构
typedef struct
{

     float LLC_IPeak; // 峰值电流
    
     float Cap_voltage; // 输出电容电压

     float voltage;    // 输出电压0.1mV
     float OVP_voltage; //

     float current;    // 电流 		0.1mA 平均
     float current2; // 电流        0.1mA 峰值

     float current_VABU_12V; // 电流        0.1mA

     float voltage_port;    // 端口电压	0.1mV
     float Sense_VO_det1; // 端口电压  0.1mV
     float Sense_VO_det2; // 端口电压  0.1mV

     float power;              // 功率
     float res;                // 电阻
     float temp_channe;        // 温度通道0.1°C
     float temp_pre;           // 开始按下的温度值
     float temp_Clam;          // 温度补偿值
     uint16_t run;             // 通道状态
     uint32_t err;             // 错误
     power_fault_define fault; // 故障
     uint16_t workMode;        // 充电放电
     uint16_t prt_en;          // 使能和关闭PRT
     uint16_t Turn_offFlag;    // 关闭驱动软关标记

     float current_ADC; // 电流        0.1mA

     float Set_CC;    // 充电恒流值
     float Set_PreCC; // 预充设置充电恒流值

     float Set_CV;    // 恒压值
     float Set_PreCV; // 恒压值

} ChannelInfo;

extern volatile ChannelInfo g_Channelinfo[BOARD_CHANNEL_NUM];


extern void AppUser_Sample_Debug(void);

extern  void sample_irq_handler(void);

extern void Get_Send_dev_Data(void);




#endif





