/*
 * AppUser_protection.h
 *
 *  Created on: 2023年5月19日
 *      Author: admin
 */

#ifndef APPAPI_APPUSER_PROTECTION_H_
#define APPAPI_APPUSER_PROTECTION_H_
#include "head.h" // head.h 头文件
#define CH_MAX 1
// 电源闭环控制控制状态机状态定义枚举
typedef enum _Fault_CH_NUM_
{
    Fault_CH0 = 0U, // 故障通道1
    Fault_CH1,      // 故障通道2
} _Fault_CH_NUM_;

/* 错误状态信息体   */
typedef union power_fault_define
{
    uint32_t all;
    struct fault_bit
    {
        uint32_t ctrlonoff : 1;
        uint32_t stop : 1;  // 急停故障
        uint32_t H_OVP : 1; // 输入过压
        uint32_t TIMER_OUT : 1;   // 工步到达恒流设置值

        uint32_t DC_OCP : 1; // 欠压压标志位
        uint32_t L_OVP : 1;  //  电池电压保护
        uint32_t L_UVP : 1;  //  电池电压保护
        uint32_t FAN : 1;    //  电容电压保护

        uint32_t OTP1 : 1;    // 副边温度过温
        uint32_t OTP2 : 1;    // 谐振电流负载峰值保护/过流标志位
        uint32_t BAT_OUT_ERR : 1;    // 副边温度过温
        uint32_t CAP_BAT : 1; // 容压大于池压

        uint32_t CMD_ERR : 1; // 指令错误
        uint32_t CTR_ERR : 1; // 继电器
        uint32_t CV_Limit_OUT : 1;   // 工步到达恒压值
        uint32_t CC_Limit_OUT : 1; // 工步到达恒流设置值

        uint32_t Worke_Setup_OVER : 1; // 运行工步号大于总工步数
        uint32_t FAN_ERR : 1;   // 风机故障
        uint32_t Worke_fish : 1; // 运行工步号大于总工步数
                                //        uint16_t rsvd2:4;  // 保留位
    } bit;
} power_fault_define;

/* 错误状态信息体   */
typedef union system_fault_define
{
    uint32_t all;
    struct system_bit
    {
        uint32_t Net : 1;//网络故障
        uint32_t stop : 1;   // 
        uint32_t SMOKE : 1;  // 
        uint32_t CC_OCP : 1; // 

        uint32_t DC_OCP : 1;  // 欠压压标志位
        uint32_t L_OVP : 1;   //  电池电压保护
        uint32_t L_UVP : 1;   //  电池电压保护
        uint32_t Cap_OVP : 1; //  电容电压保护
        uint32_t CanID_ERR : 1;   //CANID错误

    } bit;
} system_fault_define;

extern volatile system_fault_define g_systemHandle_Fault;



extern volatile power_fault_define g_Handle_Fault[CH_MAX];

extern void Scan_System_Fault(void);

extern void Debug_Scan_System_Fault(void);

extern void Init_Protect_handle(void);

extern void WriteInit_Protect_handle(void);

extern void Debug_ScanHead_System_Fault(void);

extern void Scan_SysteHeand_Fault(void);

#endif /* APPAPI_APPUSER_PROTECTION_H_ */
