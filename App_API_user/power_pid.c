/*
 * power_pid.c
 *
 *  Created on: 2024年1月11日
 *      Author: eng985
 */
#include "head.h"
#include "power_pid.h"

 float First_v_err_sum;

 power_pid_define gHandle_PID[BOARD_CELL_NUM];


 ZPK_Controller g_ZPK_Handle;

 float ZPK_A[5] = {1, 1.4586f, -0.4586f, 0.0f}; // A1      A2       A3
 float ZPK_B[5] = {0.0f, 601.03f, -548.4399f};  // B0   B1      B2       B3

 float ZPK_Y[5] = {2000.0f, 2000.0f, 2000.0f, 2000.0f};
 float ZPK_U[5] = {0.0f, 0.0f, 0.0f, 0.0f};

void pid_V_Loop_calc(power_pid_define *v);
void pid_I_Loop_calc(power_pid_define *I);
void VLimitCtlPI(power_pid_define *v); // 限压环

inline void ZPK_Handle_Update(float REF, float FB);
// void Voltage_Current_Loop_Position_PI(power_pid_define *pmVotage_Current,power_parm_define *pmParm);
void ZPK_Handle_Init(uint16_t ch);

void ZPK_Handle_Init(uint16_t ch)
{

    g_ZPK_Handle.A1 = ZPK_A[0];
    g_ZPK_Handle.A2 = ZPK_A[1];
    g_ZPK_Handle.A3 = ZPK_A[2];

    g_ZPK_Handle.B0 = ZPK_B[0];
    g_ZPK_Handle.B1 = ZPK_B[1];
    g_ZPK_Handle.B2 = ZPK_B[2];

    g_ZPK_Handle.y3 = ZPK_Y[0];
    g_ZPK_Handle.y2 = ZPK_Y[1];
    g_ZPK_Handle.y1 = ZPK_Y[2];
    g_ZPK_Handle.y = ZPK_Y[3];

    g_ZPK_Handle.u = ZPK_U[0];
    g_ZPK_Handle.u1 = ZPK_U[1];
    g_ZPK_Handle.u2 = ZPK_U[2];
    g_ZPK_Handle.u3 = ZPK_U[3];
}
void PIDInit(uint16_t ch)
{

    volatile float init_Duty;

    ZPK_Handle_Init(ch);
#if 1
    // 初始化占空比前馈

    // 电压环的参数初始化通道1
    gHandle_PID[ch].v_fdb = 0;                              // 反馈值
    gHandle_PID[ch].v_err = 0.0f;                           // 误差
    gHandle_PID[ch].v_kp =1.0f;                            // 比例系数
    gHandle_PID[ch].v_ki = 0.5f;                            // 积分系数
    gHandle_PID[ch].v_ref = 3000.0f;                          // 电压环基准复位，进行软起动
    gHandle_PID[ch].v_err_sum = (200);         //
    gHandle_PID[ch].v_up = (3000);               //
    gHandle_PID[ch].v_ui = (0);              //
    gHandle_PID[ch].v_max_out_value = gHandle_PID[ch].v_up; /* 最大脉宽*/
    gHandle_PID[ch].v_min_out_value = gHandle_PID[ch].v_ui; /* 最小脉宽*/
    gHandle_PID[ch].v_pid_out = (200);

    First_v_err_sum = gHandle_PID[ch].v_err_sum;

    gHandle_PID[ch].i_fdb = 0.0f;              // 反馈值
    gHandle_PID[ch].i_err = 0.0f;              // 误差
    gHandle_PID[ch].i_kp = 0.1f;               // 比例系数500
    gHandle_PID[ch].i_ki = 0.055f;               // 积分系数100-30a
    gHandle_PID[ch].i_ref = 0.5f;              // 电压环基准复位，进行软起动
    gHandle_PID[ch].i_up = (3000);  //
    gHandle_PID[ch].i_ui = (0); //
    gHandle_PID[ch].i_err_sum = 200;
    gHandle_PID[ch].i_max_out_value = gHandle_PID[ch].i_up; /* 最大脉宽*/
    gHandle_PID[ch].i_min_out_value = gHandle_PID[ch].i_ui; /* 最小脉宽*/
    gHandle_PID[ch].i_pid_out = 200;




    // LLCPIDCtrl.V_loop_calc      = (void (*)(long))pid_V_Loop_calc;
    // LLCPIDCtrl.I_Loop_calc      = (void (*)(long))pid_I_Loop_calc;
    // LLCPIDCtrl.V_LimitCtl_Loop_calc      = (void (*)(long))VLimitCtlPI;
    //  LLCPIDCtrl.Voltage_Current_Loop_Position_PI_calc      = (void (*)(long))Voltage_Current_Loop_Position_PI;
#endif
}

float Limit_ZPK_out(float u, float lower, float upper)
{
    float sat_out;
    if (u > upper)
        sat_out = upper;
    else if (u < lower)
        sat_out = lower;
    else
        sat_out = u;

    return sat_out;
}
//Power_PID_Updata
inline void ZPK_Handle_Update(float REF, float FB)
{

#if 1
    // 将 LLCTP3 的 y 和 u 值进行移位，保持历史值，进行计算
    g_ZPK_Handle.y3 = g_ZPK_Handle.y2; // 将 y2 的值赋给 y3，保存前一个输出
    g_ZPK_Handle.y2 = g_ZPK_Handle.y1; // 将 y1 的值赋给 y2，保存上上个输出
    g_ZPK_Handle.y1 = g_ZPK_Handle.y;  // 将当前 y 的值赋给 y1，保存上一个输出
    g_ZPK_Handle.u3 = g_ZPK_Handle.u2; // 将 u2 的值赋给 u3，保存前一个输入
    g_ZPK_Handle.u2 = g_ZPK_Handle.u1; // 将 u1 的值赋给 u2，保存上上个输入
    g_ZPK_Handle.u1 = g_ZPK_Handle.u;  // 将当前 u 的值赋给 u1，保存上一个输入
    g_ZPK_Handle.u = FB - REF;         // 计算当前输入信号 u，等于输出电压 Vo 与参考电压 REF 的差值
    g_ZPK_Handle.u = Limit_ZPK_out(g_ZPK_Handle.u, -5.0f, 5.0f);
    // 计算控制输出 y
    g_ZPK_Handle.y = g_ZPK_Handle.B0 * g_ZPK_Handle.u +  // 当前输入 u 的加权值
                     g_ZPK_Handle.B1 * g_ZPK_Handle.u1 + // 上一个输入 u1 的加权值
                     g_ZPK_Handle.B2 * g_ZPK_Handle.u2 + // 上上个输入 u2 的加权值
                     g_ZPK_Handle.B3 * g_ZPK_Handle.u3 + // 上上上个输入 u3 的加权值
                     g_ZPK_Handle.A1 * g_ZPK_Handle.y1 + // 上一个输出 y1 的加权值
                     g_ZPK_Handle.A2 * g_ZPK_Handle.y2 + // 上上个输出 y2 的加权值
                     g_ZPK_Handle.A3 * g_ZPK_Handle.y3;  // 上上上个输出 y3 的加权值

    // 将输出 y 限制在指定范围 [88600.0f, 180000.0f] 之间
    g_ZPK_Handle.y = Limit_ZPK_out(g_ZPK_Handle.y, 3500.0f, 130000.0f);

    // 计算开关频率 PRD，使用与 y 相关的公式
    g_ZPK_Handle.OUT = 200e7f / g_ZPK_Handle.y; // 将 y 代入公式，得到PRD的值。200e7f 是HRTIM的主频200MHz，在main.c的HRTIM_Config()中配置所得

#endif
}

/**********************************************************************
 * Function:      pid_V_Loop_calc
 * Description:   电压环函数
 * Input:          void
 * Output:
 * Return:       void
 * Others:
 * Modify Date:    Version:    Author:         Modification:
 * -----------------------------------------------
 * 2023-05-31     V1.0        Hu Weiping
 **********************************************************************/

void pid_V_Loop_calc(power_pid_define *v)
{
#if 1
    // 第一步：误差计算 // 计算电压误差量，当参考电压大于输出电压，频率往小调，周期量往大调，输出量增加
    v->v_err = v->v_ref - v->v_fdb;

    v->v_err = __fmin(v->v_err, 10.0f);  // 误差限幅
    v->v_err = __fmax(v->v_err, -10.0f); // 误差限幅

    v->v_err_sum = v->v_err_sum + v->v_ki * v->v_err; // 正常情况下积分计算

    v->v_pid_out = v->v_kp * v->v_err + v->v_err_sum;

    v->v_err_sum = __fmin(v->v_err_sum, v->v_up); // 积分输出值上限
    v->v_err_sum = __fmax(v->v_err_sum, v->v_ui); // 积分输出值上限

    v->v_pid_out = __fmin(v->v_pid_out, v->v_max_out_value); // 限幅 65Khz--7690
    v->v_pid_out = __fmax(v->v_pid_out, v->v_min_out_value); // 限幅
#endif
}

/*
** ===================================================================
**     Funtion Name : void VLimitCtlPI(power_pid_define *v)
**     Description :  限压环
**     当输出电压小于限压值时，限压环输出为零
**     当输出电压大于限压值时，限压环输出为正
**     其输出叠加在电流环的电流参考值上面，通过降低电流环参考值，降低输出电流的方式从而限制输出电压
**     Parameters  :无
**     Returns     :无
** ===================================================================
*/

void VLimitCtlPI(power_pid_define *v)
{
#if 0
    //第一步：误差计算 // 计算电压误差量，当参考电压大于输出电压，频率往小调，周期量往大调，输出量增加
    v->v_e =  v->v_fdb -v->v_ref ;

    //补偿值
    v->v_up = v->v_kp * v->v_e;

    //第二步：积分量=积分量+KI*误差量
    v->v_ui = v->v_ui + v->v_e * v->v_ki;

    //积分防饱和
    if (v->v_ui >= USER_POWER_IOUT) //140.0A
    {
        //只减不加
        v->v_ui = USER_POWER_IOUT;
    }
    else if (v->v_ui <= 0)
    {
        //只加不减
        v->v_ui = 0;
    }

    //第三步：电压环路输出=KP*误差量+积分量
    v->v_pid_out = v->v_up + v->v_ui;
    //最大最小限制
    if (v->v_pid_out >= USER_POWER_IOUT)
    {
        v->v_pid_out = USER_POWER_IOUT;
    }
    else if (v->v_pid_out <= 0)
    {
        v->v_pid_out = 0;
    }

#endif
}

void pid_I_Loop_calc(power_pid_define *I)
{
    I->i_err = I->i_ref - I->i_fdb;

    I->i_err = __fmin(I->i_err, 1.0f);  // 误差限幅
    I->i_err = __fmax(I->i_err, -1.0f); // 误差限幅

    I->i_err_sum = I->i_err_sum + I->i_ki * I->i_err; // 正常情况下积分计算

    I->i_pid_out = I->i_kp * I->i_err + I->i_err_sum; // 积分量+比例量

    I->i_err_sum = __fmin(I->i_err_sum, I->i_up); // 限幅 上限
    I->i_err_sum = __fmax(I->i_err_sum, I->i_ui); // 限幅 下限

    I->i_pid_out = __fmin(I->i_pid_out, I->i_max_out_value); // 限幅 65Khz--7690
    I->i_pid_out = __fmax(I->i_pid_out, I->i_min_out_value); // 限幅
}

#if 0
//变量声明
float ui_max = 1350.0f;
float ui_min = 0.0f;
//函数定义
void pid_calc(power_pid_define *v)
{
    v->e = v->pid_ref - v->pid_fdb; //误差计算

    //补偿值
    v->up = v->Kp0 * v->e;
    v->ui = v->ui + v->e * v->Ki0;

    //积分防饱和
    if (v->ui >= ui_max)
    {
        //只减不加
        v->ui = ui_max;
    }
    else if (v->ui <= ui_min)
    {
        //只加不减
        v->ui = ui_min;
    }

    //最大最小限制
    v->pid_out = v->up + v->ui;
    if (v->pid_out >= v->pid_out_max)
    {
        v->pid_out = v->pid_out_max;
    }
    else if (v->pid_out <= v->pid_out_min)
    {
        v->pid_out = v->pid_out_min;
    }
}

#endif

#if 0


/*
** ===================================================================
**     Funtion Name :void BUCK_Voltage_Current_Position_PI(power_pid_define *pmVotage_Current,power_parm_define *pmParm)
**     Description : 双环竞争 输出恒压恒流控制，位置式离散PI算法
**     Parameters  :
**     int32_t vRef：电压目标值
**     int32_t vReal：电压当前值
**     int32_t iRef：电流目标值
**     int32_t iReal：电流当前值
**     Returns     :  pmVotage_Current-> out_value
** ===================================================================
*/
// __IO int32_t verr=0;//电压当前误差
// __IO int32_t ierr=0;//电流当前误差
// __IO int32_t vprop=0;//电压环比例量
// __IO int32_t iprop=0;//电流环比例量
// __IO int32_t inte=0;//积分量
// __IO int32_t v0=0;//电压环路输出
// __IO int32_t i0=0;//电流环路输出
// __IO int32_t u0=0;//环路输出
#pragma CODE_SECTION(Voltage_Current_Loop_Position_PI, ".TI.ramfunc");
 void Voltage_Current_Loop_Position_PI(power_pid_define *pmVotage_Current,power_parm_define *pmParm)
{
//     static int32_t vkp=100,vki=10;//位置式电压PI环路的P,I，参数放大Q8倍
//     static int32_t ikp=20,iki=2;//位置式电流PI环路的P,I，参数放大Q8倍
//     static int32_t maxinte=0,mininte=0;//最大积分量，最小积分量，位置式PI的积分量即为工作稳态时的输出点


  //  verr = vRef - vReal;//误差=目标-当前
  //  ierr = iRef - iReal;//误差=目标-当前
    //误差=目标-当前
    pmVotage_Current->v_e = pmVotage_Current->v_ref - pmVotage_Current->v_fdb ;
    pmVotage_Current->i_e = pmVotage_Current->i_ref - pmVotage_Current->i_fdb ;

//    vprop = verr*vkp;//比例量=比例系数*误差
//    iprop = ierr*ikp;//比例量=比例系数*误差
    pmVotage_Current->v_up =   pmVotage_Current->v_e * pmVotage_Current->v_kp ;//比例量=比例系数*误差
    pmVotage_Current->i_up =   pmVotage_Current->i_e * pmVotage_Current->i_kp ;//比例量=比例系数*误差

//    v0= inte + vprop;//电压环输出，位置式PI控制输出=积分量+比例量
//    i0= inte + iprop;//电流环输出，位置式PI控制输出=积分量+比例量

    //电压环输出，位置式PI控制输出=积分量+比例量
    pmVotage_Current->v_pid_out = pmVotage_Current-> err_sum + pmVotage_Current->v_up;
    //电流环输出，位置式PI控制输出=积分量+比例量
    pmVotage_Current->i_pid_out = pmVotage_Current-> err_sum + pmVotage_Current->i_up;

    //取两者中小的一方作为最终控制
    if(pmVotage_Current->v_pid_out>pmVotage_Current->i_pid_out)//取两者中小的一方作为最终控制
    {
        pmVotage_Current-> out_value = pmVotage_Current->i_pid_out;
        //（积分量累积）PI的当前积分量=上一时刻积分量+KI*误差量
        pmVotage_Current-> err_sum =  pmVotage_Current-> err_sum + pmVotage_Current->i_e*pmVotage_Current->i_ki;
    }
    else
    {
        //u0 = v0>>8;
       // inte = inte + verr*vki;//（积分量累积）PI的当前积分量=上一时刻积分量+KI*误差量
        pmVotage_Current-> out_value = pmVotage_Current->v_pid_out;
        pmVotage_Current-> err_sum =  pmVotage_Current-> err_sum + pmVotage_Current->v_e*pmVotage_Current->v_ki;
    }



//        if(pmVotage_Current-> out_value > pmParm->PWM_LLC_MAX_ON_TIME)//积分量最大限制
//            pmVotage_Current-> out_value = pmParm->PWM_LLC_MAX_ON_TIME;
//       else if(pmVotage_Current-> out_value < pmParm->PWM_LLC_MIN_ON_TIME)//积分量最小限制
//           pmVotage_Current-> out_value = pmParm->PWM_LLC_MIN_ON_TIME;
}

#endif

void Debug_HandlePID(void)
{
    uint16_t ch;

#if 1

    for (ch = 0; ch < BOARD_CHANNEL_NUM; ch++)
    {
        printf("\r\nCH[%d] I_Set=%fmA I_FB=%fmA i_pid_out=%f ", ch + 1, gHandle_PID[ch].i_ref, gHandle_PID[ch].i_fdb, gHandle_PID[ch].i_pid_out);
        printf("\r\nCH[%d]Set=%fV FB=%fV pid_out=%f ", ch + 1, gHandle_PID[ch].v_ref, gHandle_PID[ch].v_fdb, gHandle_PID[ch].v_pid_out);
        printf("\r\n  HOMSduty=%d low=%d \r\n", g_epwmHandle[ch].High_MOS_DUTY, g_epwmHandle[ch].Low_MOS_DUTY);

      // printf("\r\n v_max_out_value=%d  v_up=%d ", gHandle_PID[ch].v_max_out_value, gHandle_PID[0].v_up);
    }

#endif
}

float LV_100us_lowpass(float input, uint32_t reinitialize)
{
    static struct LOWPASS
    {
        float u;
        float u1;
        float y1;
        float y;
        float A1;
        float B1;
    } LV_lowpass = {0.0f, 0.0f, 5.0f, 5.0f, 0.6065f, 0.3935f}; // y1和y设置为sotc刚刚介入时候的电压
    if (reinitialize)
    {
        LV_lowpass.u = 0.0f;
        LV_lowpass.u1 = 0.0f;
        LV_lowpass.y1 = 5.0f;
        LV_lowpass.y = 5.0f;
    }
    else
    {
        LV_lowpass.y1 = LV_lowpass.y;
        LV_lowpass.u1 = LV_lowpass.u;
        LV_lowpass.u = input;
        // LV_lowpass.y= LV_lowpass.B0 * LV_lowpass.u + LV_lowpass.B1 * LV_lowpass.u1 + LV_lowpass.A1 * LV_lowpass.y1;由于B0为0
        LV_lowpass.y = LV_lowpass.B1 * LV_lowpass.u1 + LV_lowpass.A1 * LV_lowpass.y1;
    }
    return LV_lowpass.y;
}


