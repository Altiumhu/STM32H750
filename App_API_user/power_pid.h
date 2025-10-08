/*
 * power_pid.h
 *
 *  Created on: 2024年1月11日
 *      Author: eng985
 */

#ifndef APP_POWER_POWER_PID_H_
#define APP_POWER_POWER_PID_H_
#include "head.h"
 #include "system_Config.h"

// 电源闭环控制控制状态机状态定义枚举
typedef enum loop_state_define
{
    I_LOOP = 0U, // 电流环
    V_LOOP,      // 电压环
} loop_state;

// pid数据信息体
typedef struct power_pid_define
{
     bool softrun;

     float v_fdb;           /* 输入电压反馈值  */
     float v_err;           /* Variable: Error    */
     float v_kp;            // 比例系数
     float v_ki;            // 微分系数
     float v_ref;           // 期望值
     float v_err_sum;       // 积分和
     float v_up;            /* Variable: Proportional output  */
     float v_ui;            /* Variable: Integral output    */
     float v_min_out_value; // 最小输出值限定值
     float v_max_out_value; // 最大输出值限定值
     float v_pid_out;       /* Output: PID output    */

     float i_fdb;           /* 输入电流反馈值  */
     float i_err;           /* Variable: Error    */
     float i_kp;            // 比例系数
     float i_ki;            // 微分系数
     float i_ref;           // 期望值
     float i_err_ref1;      // 稳态误差值
     float i_err_sum;       // 积分和
     float i_up;            /* Variable: Proportional output  */
     float i_ui;            /* Variable: Integral output    */
     float i_min_out_value; // 最小输出值限定值
     float i_max_out_value; // 最大输出值限定值
     float i_pid_out;       /* Output: PID output    */

     loop_state loop;

    void (*V_loop_calc)();                           /*电压环 */
    void (*I_Loop_calc)();                           /* 电流换 */
    void (*V_LimitCtl_Loop_calc)();                  /* 限压环 */
    void (*Voltage_Current_Loop_Position_PI_calc)(); /* 电流电压双环竞争 */

} power_pid_define;


typedef struct ZPK_Controller  
{
	float u;
	float u1;
	float u2;
	float u3;
	float y3;
	float y2;
	float y1;
	float y;
	float A1;
	float A2;
	float A3;
	float B0;
	float B1;
	float B2;
	float B3;
	float REF;
	float OUT;

} ZPK_Controller;

extern volatile ZPK_Controller g_ZPK_Handle;

extern void PIDInit(uint16_t ch);
extern void ZPK_Handle_Init(uint16_t ch);

void pid_V_Loop_calc(power_pid_define *v);
void pid_I_Loop_calc(power_pid_define *I);

extern volatile power_pid_define gHandle_PID[BOARD_CELL_NUM];

extern volatile power_pid_define gHandle_Burst_PID[BOARD_CELL_NUM];

extern void Debug_HandlePID(void);

extern  void ZPK_Handle_Update(float REF ,float FB);


#endif /* APP_POWER_POWER_PID_H_ */
