#ifndef USRE_APP_POWER_LOOP_H_
#define USRE_APP_POWER_LOOP_H_

#include "head.h"
#include "AppUser_protection.h"
#include "system_Config.h"



#if SS_START_MODE
#define PWM_SOTC_CNT 5 //  轨迹控制软启动
#else
#define PWM_SOTC_CNT 3 //  高频软启动

#define PWM_BURST_CNT 3 //  高频软启动
#endif



// 电源闭环控制控制状态机状态定义枚举
typedef enum power_state_define
{
    POWER_OFF = 0U,      // 关闭驱动状态1
    POWER_FAULT,         // 故障状态1
    POWER_INIT,          // 电源初始化状态2
    POWER_PRECHARGE,     // 预充电状态3
    POWER_PRE_DISCHARGE, // 放电预处理4
    POWER_RUN_DISCHARGE, // 放电运行5
    POWER_RUN_CHARGE,    // 充电运行6
    POWER_RUN,           // 运行状态7
    POWER_BURST,         // 打嗝模式8
    POWER_Mode_Change,   // 9
    POWER_TEST,          // 10

    POWER_SoftStart, // 软启动 11
    POWER_SET_CHARGE,    // 设置充电12

} power_state_define;

// 电源闭环控制控制状态机状态定义枚举
typedef enum power_Worke_define
{
    Worke_OFF = 0U, // 待机

    Worke_CV,    // 恒压
    Worke_CC,    // 恒流
    Worke_CC_CV, // 恒压

} power_Worke_define;





typedef struct
{
    float     debugNUmer1;
    float     temp;
    uint16_t  debug1;
    uint16_t  debug2;
    uint16_t  debug3;

}debugNUmer;

extern debugNUmer g_debugNUmer;

extern void APT0TimerCallback(void *aptHandle);

extern void TIMER0CallbackFunction(void *handle);

 extern void Debug_Loop(void);


#endif




