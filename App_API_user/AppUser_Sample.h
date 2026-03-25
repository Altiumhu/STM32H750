#ifndef USRE_APP_USER_SAMPLE_H_
#define USRE_APP_USER_SAMPLE_H_
#include "head.h"
#include "system_Config.h"



#define V_CAL_Ka 0.000050354772f

// #define V_CAL_Ka 0.050354772f


#define CC_CAL_Ka 0.0003329f
#define CC_CAL_Kb -0.043701f


typedef struct
{
	uint8_t index;			 // 工步索引号
	uint8_t type;			 // 工步类型
	float currentStart; // 启动电流
	float voltLimit;	 // 限制电压
	float currentLimit; // 终止电流
	uint32_t timeLimit;	 // 终止时间
} RuningWorkSetup_t;


// 单通道数据数据结构
typedef struct
{

     float Cap_voltage;        // 输出电容电压
     uint16_t Cap_voltage_ADC; // 输出电容ADC

     float voltage_port;        // 端口电压	0.1mV
     uint16_t voltage_port_ADC; // 端口电压ADC

     float voltage;        // 电池电压0.1mV
     uint16_t voltage_ADC; // 电池电压ADC
     float OVP_voltage;    //

     float current;           // 电流 		0.1mA 平均
     float current_voltage;   // 电流        0.1mA
     uint16_t current_ADC;    // 电流        0.1mA
     uint16_t current_DC_ADC; // 放电电流ADC

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

     float Set_CC;    // 充电恒流值
     float Set_PreCC; // 预充设置充电恒流值

    float  Set_PreDC;

     float Set_CV;    // 恒压值
     float Set_PreCV; // 恒压值

     uint16_t SS_Timer;    // ADC跟新中断

     uint16_t GetPortTimer;    // ADC跟新中断

     uint16_t WorkeStartup;//运行启动工步
     uint32_t WorkeRunStartTimer;//开始工步运行时间
     uint32_t WorkeRunTimer;//工步运行时间
     RuningWorkSetup_t RunningWorkSetup;//运行设置




} ChannelInfo;

extern volatile ChannelInfo g_Channelinfo[BOARD_CHANNEL_NUM];


extern void AppUser_Sample_Debug(void);

extern  void sample_irq_handler(void);

extern void Get_Send_dev_Data(void);




#endif





