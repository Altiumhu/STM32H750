/*
 * AppUser_pwm.h
 *
 *  Created on: 2023年5月17日
 *      Author: admin
 */

#ifndef APPAPI_APPUSER_PWM_H_
#define APPAPI_APPUSER_PWM_H_
#include "head.h"


#define EPWM_CH_NUM 16

// 电源闭环控制控制状态机状态定义枚举
typedef enum EPWM_Num_define
{
    CH_NUM0 = 0U, // 通道0
    CH_NUM1,      // 通道1

} EPWM_Num_define;

// PWM控制量信息体
typedef struct
{
     uint16_t High_MOS_Timer_TBPRD;
     uint16_t High_MOS_Timer_TBPRD_MAX;
     uint16_t High_MOS_Timer_TBPRD_MIN;
     uint16_t High_MOS_PHS;      // 高压侧相位
     uint16_t High_MOS_DUTY;     // 高压侧上管占空比
     uint16_t High_MOS_LOW_DUTY; // 高压侧上管占空比

     uint16_t High_MOS_DTF;      // 高压侧下管前死区
     uint16_t High_MOS_DTB;      // 高压侧下管后死区
     uint16_t High_MOS_DUTY_MAX; // buck上管占空比最大值
     uint16_t High_MOS_DUTY_MIN; // buck上管占空比最大值
     uint8_t High_MOS_STA;       // 高压侧下管使能
     uint16_t High_MOS_OpenFlag; // 打开标记



    uint32_t  Low_MOS_Timer_TBPRD;
    uint32_t  Low_MOS_PHS;   //低压侧相位
    uint32_t  Low_MOS_DUTY; //低压侧上管占空比
    uint32_t  Low_MOS_DTF;  //低压侧下管前死区
    uint32_t  Low_MOS_DTB;  //低压侧下管后死区
    uint32_t  Low_MOS_DUTY_MAX; //buck上管占空比最大值
    uint32_t   Low_MOS_DUTY_MIN;
    uint16_t   Low_MOS_STA;  //低压侧下管使能
    uint16_t   Low_MOS_OpenFlag;  //打开标记
		
} EPWM_Handle;

// 单通道数据数据结构
typedef struct
{
     uint32_t FAN_cunt;
     uint32_t FAN_TBPRD;
     uint32_t FAN_Duty;
} FAN_Handle;

extern volatile FAN_Handle gFAN_Handle[EPWM_CH_NUM];

extern volatile EPWM_Handle g_epwmHandle[EPWM_CH_NUM];

extern inline void pwm_start(uint16_t channel, uint16_t mode);
extern inline void pwm_stop(uint16_t channel);


void HAL_EPWM_Config(uint16_t ch);

void Debug_PWM(void);

void Updata_EPWM_Handle(void);

#endif /* APPAPI_APPUSER_PWM_H_ */
