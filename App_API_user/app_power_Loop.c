/*****************************************************************************
 *
 *  All rights reserved.
 *
 *  @file     app_power_Loop.h
 *  @brief    环路控制
 *
 *  @author
 *  @email
 *  @version  1.0.0.1
 *  @date      2025-1-13
 *  @license  GNU General Public License (
 *****************************************************************************/
/*--------------------------------- Includes -------------------------------*/

#include "head.h"
#include "app_power_Loop.h"

void TIMER0CallbackFunction(void *handle);

void System_CloseLoop_Status(void);

/**********************************************************************
 * Function: 	 System_CloseLoop_Status
 * Description:   环路状态机
 * Input: 	      void
 * Output:
 * Return: 	    void
 * Others:
 * Modify Date:    Version:    Author:	      Modification:
 * -----------------------------------------------
 * 2025-06-05	  V1.0	      Hu Weiping
 **********************************************************************/

void System_CloseLoop_Status(void)
{
    uint16_t ch = 0;
    uint32_t tempdata;

    for (ch = 0; ch < BOARD_CHANNEL_NUM; ch++)
    {

        if (g_Channelinfo[ch].fault.all) // 判断故障
        {
            if (g_Channelinfo[ch].fault.bit.Worke_fish == 1 || g_Channelinfo[ch].fault.bit.Worke_Setup_OVER == 1)
            {
                g_Channelinfo[ch].WorkeStartup = 0xFF; // 启动工步
                g_Channelinfo[ch].status = 0x53;       // 无操作时=0x53
            }
            else
            {
                g_Channelinfo[ch].status = 0x53; // 无操作时=0x53
            }

            g_epwmHandle[ch].High_MOS_STA = 0;
            g_epwmHandle[ch].High_MOS_OpenFlag = 0;
            g_Channelinfo[ch].workMode = POWER_FAULT;

            // Init_RunningWorkSetup();
            // pwm_stop(ch); /// 出现故障就关闭pwm
        }

        switch (g_Channelinfo[ch].workMode)
        {
        case POWER_OFF:   // 关闭驱动
        case POWER_FAULT: // 关闭驱动
        {
            if (g_Channelinfo[ch].fault.all == 0) // 判断故障  没有故障进入正常启动程序
            {

                g_Channelinfo[ch].workMode = POWER_SET_PARAM;

                /// g_Channelinfo[ch].workMode = POWER_GET_V_PORT;

                g_Channelinfo[ch].GetPortTimer = 0;
            }
            else if (g_Channelinfo[ch].fault.bit.CV_Limit_OUT == 1 || g_Channelinfo[ch].fault.bit.CC_Limit_OUT == 1 || g_Channelinfo[ch].fault.bit.TIMER_OUT == 1)
            {
                g_Channelinfo[ch].workeDelayTimer++;
                if (g_Channelinfo[ch].workeDelayTimer >= 5100)
                {
                    g_Channelinfo[ch].workMode = POWER_SET_PARAM;
                    g_Channelinfo[ch].workeDelayTimer = 0;
                    g_Channelinfo[ch].GetPortTimer = 0;
                    g_Channelinfo[ch].fault.bit.CV_Limit_OUT = 0;  // 工步到达恒压值
                    g_Channelinfo[ch].fault.bit.CC_Limit_OUT = 0;  // 工步到达恒流设置值
                    g_Channelinfo[ch].fault.bit.TIMER_OUT = 0;     // 工步时间到
                    Set_PWM_Channel_CH595_EN(2, ch, EX_595_RESET); // 关闭PWM_EN
                }
            }
        }
        break;

        case POWER_SET_PARAM: // 设置工步参数

            if (g_Channelinfo[ch].WorkeStartup >= GetTotal_steps()) // 运行工步号大于总工步数
            {
                g_Channelinfo[ch].fault.bit.Worke_Setup_OVER = 1; // 工步大于工步数 整个工艺结束跳转故障
                                                                  // g_Channelinfo[ch].WorkeStartup= g_Channelinfo[ch].WorkeStartup+ 1; // 当前运行工步循环号，表示当前工步需要循环工作几次
            }
            // 后续在完善功能

            if (g_Channelinfo[ch].loopSn > g_SetChanneWorke.Run_Cyc_indx) // 运行工步号大于总工步数
            {
                g_Channelinfo[ch].loopSn = g_Channelinfo[ch].loopSn + 1;

                g_Channelinfo[ch].fault.bit.Worke_fish = 1; // 工艺完成
                break;
            }

            if (g_Channelinfo[ch].CH_StartFlag == 1) // 启动工步开始
            {
                switch (g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].type)
                {
                case WORKE_SETUP_IDLE: // 跳转搁置阶段 静置(D) 工步名称

                    g_Channelinfo[ch].workMode = POWER_IDLE; //  跳转搁置阶段
                    g_Channelinfo[ch].status = WORKE_SETUP_IDLE;
                    g_Channelinfo[ch].WorkeRunStartTimer = Timer_GetClock(); //  记录开始启动时间

                    break;
                case WORKE_SETUP_CC:    // 恒流恒压充电（K）
                case WORKE_SETUP_CC_CV: // 恒流恒压充电（A）

                    g_Channelinfo[ch].workMode = POWER_GET_V_PORT; // 获得端口电压状态

                    break;
                case WORKE_SETUP_DC: // 恒流放电(C) 工步名称

                    g_Channelinfo[ch].workMode = POWER_GET_V_PORT; // 获得端口电压状态

                    break;
                case WORKE_SETUP_LOOP: // 循环(R) 工步名称
                    // 起始工步：工步主参数1 ---电流值：工步主参数1
                    // 终止工步：工步主参数2---电压截止：工步主参数2
                    // 循环数：工步主参数3 -- 循环数：----终止电流
                    g_Channelinfo[ch].workMode = POWER_GET_V_PORT; // 获得端口电压状态

                    break;
                default:
                    break;
                }
            }

            break;
        case POWER_IDLE: // 待机状态 搁置阶段

            g_Channelinfo[ch].WorkeRunTimer = Timer_GetClock() - g_Channelinfo[ch].WorkeRunStartTimer;

            if (g_Channelinfo[ch].WorkeRunTimer >= g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].timeLimit) // 工步时间到
            {
                g_Channelinfo[ch].workeDelayTimer = 0;
                // g_Channelinfo[ch].workMode = POWER_SET_PARAM;
                g_Channelinfo[ch].WorkeStartup = g_Channelinfo[ch].WorkeStartup + 1;
                g_Channelinfo[ch].fault.bit.TIMER_OUT = 1;
                g_Channelinfo[ch].error = 0x03; // 0x03:  以时间条件结束
            }

            break;

        case POWER_GET_V_PORT: // 获得端口电压
            g_Channelinfo[ch].GetPortTimer++;
            if (g_Channelinfo[ch].GetPortTimer >= 12)
            {
                g_Channelinfo[ch].GetPortTimer = 0;
                g_Channelinfo[ch].workMode = POWER_INIT;
            }

            // 获得软启动电压目标值
            g_Channelinfo[ch].Set_SS_PreCV = g_Channelinfo[ch].voltage;
            break;

        case POWER_INIT: //
        {

          //  Set_PWM_Channel_CH595_EN(0, ch, EX_595_RESET); // 关闭
         //       Set_PWM_Channel_CH595_EN(0, ch, EX_595_SET); // 打开PRT
            PIDInit(ch);
            HAL_EPWM_Config(ch);
            // Set_Sample_Channel_VPortGPIO(AD_V_CAP_EN);
            g_Channelinfo[ch].workMode = POWER_PRECHARGE;

            g_Channelinfo[ch].run = 0x0;
            g_Channelinfo[ch].SS_Timer = 0;
            g_Channelinfo[ch].Limit_Timer = 0;
            pwm_start(ch, 0);
            Set_PWM_Channel_CH595_EN(2, ch, EX_595_SET); // 打开MOS驱动使能
            g_Channelinfo[ch].Cap_voltage = 0.0f;
        }
        break;
        case POWER_PRECHARGE: // 预充电
        {
            // 电流换
            gHandle_PID[ch].i_ref = 1.0f; // 设置给定值5A

            gHandle_PID[ch].i_fdb = g_Channelinfo[ch].current; // 设置反馈值
            pid_I_Loop_calc(&gHandle_PID[ch]);                 // 电流换

            // 电压环
            //  gHandle_PID[ch].v_ref = 2.5f;
            gHandle_PID[ch].v_ref = g_Channelinfo[ch].Set_SS_PreCV;
            gHandle_PID[ch].v_fdb = g_Channelinfo[ch].Cap_voltage; // 设置反馈值
            pid_V_Loop_calc(&gHandle_PID[ch]);

            if (gHandle_PID[ch].v_pid_out <= gHandle_PID[ch].i_pid_out)
            {
                gHandle_PID[ch].loop = V_LOOP; // 电压环
                g_epwmHandle[ch].High_MOS_DUTY = gHandle_PID[ch].v_pid_out;
            }
            else
            {
                gHandle_PID[ch].loop = I_LOOP; // 电流环
                g_epwmHandle[ch].High_MOS_DUTY = gHandle_PID[ch].i_pid_out;
            }

            if (g_Channelinfo[ch].Cap_voltage >= (g_Channelinfo[ch].Set_SS_PreCV - 0.01f))
            {
            
                g_Channelinfo[ch].SS_Timer++;
                if (g_Channelinfo[ch].SS_Timer >= 10) // 开机一瞬间误动作
                {
                    Set_PWM_Channel_CH595_EN(0, ch, EX_595_SET); // 打开PRT
                    g_Channelinfo[ch].SS_Timer = 0;
                    // ex_595_write(0, EX_595_PIN_0|EX_595_PIN_1, 1);

#if 1

                    //  g_Channelinfo[ch].Set_PreCV =4.2f;
                    if (g_Channelinfo[ch].CH_StartFlag == 1) // 启动工步开始
                    {
                        switch (g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].type)
                        {
                        case WORKE_SETUP_CC:
                            g_Channelinfo[ch].Set_PreCV = g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].voltLimit;
                            g_Channelinfo[ch].Set_PreCC = g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].currentStart;

                            g_Channelinfo[ch].WorkeRunStartTimer = Timer_GetClock(); //  记录开始启动时间
                            g_Channelinfo[ch].workMode = POWER_RUN_CHARGE;
                            g_Channelinfo[ch].status = WORKE_SETUP_CC;

                            break;
                        case WORKE_SETUP_CC_CV: // 恒流恒压充电（A）
                            g_Channelinfo[ch].Set_PreCV = g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].voltLimit;
                            g_Channelinfo[ch].Set_PreCC = g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].currentStart;

                            g_Channelinfo[ch].WorkeRunStartTimer = Timer_GetClock(); //  记录开始启动时间
                            g_Channelinfo[ch].workMode = POWER_RUN_CHARGE;
                            g_Channelinfo[ch].status = WORKE_SETUP_CC_CV;

                            break;
                        case WORKE_SETUP_DC: // 恒流放电(C) 工步名称
                                             //   g_Channelinfo[ch].Set_PreCV = g_Channelinfo[ch].RunningWorkSetup.voltLimit;
                            // g_Channelinfo[ch].Set_PreCV = 0.5f;
                            // g_Channelinfo[ch].Set_CC = 0.5f;
                            //  g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].voltLimit = 0.5f;
                            g_Channelinfo[ch].Set_PreDC = g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].currentStart;
                            g_Channelinfo[ch].workMode = POWER_RUN_DISCHARGE;
                            g_Channelinfo[ch].WorkeRunStartTimer = Timer_GetClock(); //  记录开始启动时间
                            g_Channelinfo[ch].status = WORKE_SETUP_DC;

                            g_Channelinfo[ch].Set_CC = 0.5f;

                            if (gHandle_PID[ch].loop == V_LOOP)
                            {
                                gHandle_PID[ch].i_err_sum = gHandle_PID[ch].v_err_sum;
                            }

                            //  gHandle_PID[ch].i_err_sum = 100;
                            break;
                        case 0x52: // 循环(R) 工步名称
                                   // 起始工步：工步主参数1 ---电流值：工步主参数1
                                   // 终止工步：工步主参数2---电压截止：工步主参数2
                                   // 循环数：工步主参数3 -- 循环数：----终止电流
                            g_Channelinfo[ch].workMode = POWER_RUN_CHARGE;
                            g_Channelinfo[ch].WorkeRunStartTimer = Timer_GetClock(); //  记录开始启动时间
                            break;
                        default:
                            break;
                        }
                    }
#endif
                }
                g_epwmHandle[ch].High_MOS_STA = 1;
            }
            break;
        }

        case POWER_RUN_CHARGE: // 06
        {
            // 充电切换占空比设置
            g_epwmHandle[ch].High_MOS_DUTY_MAX = TIMER_DUTY_MAX;         // 36.40% 97750
            g_epwmHandle[ch].High_MOS_DUTY_MIN = 100;                    // 5200
            gHandle_PID[ch].i_up = (g_epwmHandle[ch].High_MOS_DUTY_MAX); //
            gHandle_PID[ch].i_ui = (g_epwmHandle[ch].High_MOS_DUTY_MIN); //
            gHandle_PID[ch].i_max_out_value = gHandle_PID[ch].i_up;      /* 最大脉宽*/
            gHandle_PID[ch].i_min_out_value = gHandle_PID[ch].i_ui;      /* 最小脉宽*/

            gHandle_PID[ch].v_up = (g_epwmHandle[ch].High_MOS_DUTY_MAX); //
            gHandle_PID[ch].v_ui = (g_epwmHandle[ch].High_MOS_DUTY_MIN); //
            gHandle_PID[ch].v_max_out_value = gHandle_PID[ch].v_up;      /* 最大脉宽*/
            gHandle_PID[ch].v_min_out_value = gHandle_PID[ch].v_ui;      /* 最小脉宽*/

            gHandle_PID[ch].v_kp = gHandle_PID[ch].v_kp + 0.1;
            if (gHandle_PID[ch].v_kp >= 10.1f)
                gHandle_PID[ch].v_kp = 10.1f;
            gHandle_PID[ch].v_ki = gHandle_PID[ch].v_ki + 0.1;
            if (gHandle_PID[ch].v_ki >= 1.5f)
                gHandle_PID[ch].v_ki = 1.5f;

            // 电压环
            g_Channelinfo[ch].Set_CV = g_Channelinfo[ch].Set_CV + 0.01f;
            if (g_Channelinfo[ch].Set_CV >= g_Channelinfo[ch].Set_PreCV)
            {
                g_Channelinfo[ch].Set_CV = g_Channelinfo[ch].Set_PreCV;
            }

            gHandle_PID[ch].v_ref = g_Channelinfo[ch].Set_CV;

            gHandle_PID[ch].v_fdb = g_Channelinfo[ch].voltage; // 设置反馈值
            pid_V_Loop_calc(&gHandle_PID[ch]);

            // 电流环
            gHandle_PID[ch].i_ref += 0.1f;
            if (gHandle_PID[ch].i_ref >= g_Channelinfo[ch].Set_PreCC)
            {
                gHandle_PID[ch].i_ref = g_Channelinfo[ch].Set_PreCC;
            }

            gHandle_PID[ch].i_fdb = g_Channelinfo[ch].current; // 设置反馈值
            pid_I_Loop_calc(&gHandle_PID[ch]);

            if (gHandle_PID[ch].v_pid_out < gHandle_PID[ch].i_pid_out) // 双环竞争
            {
                gHandle_PID[ch].loop = V_LOOP; // 电压环
                g_epwmHandle[ch].High_MOS_DUTY = gHandle_PID[ch].v_pid_out;
            }
            else
            {
                gHandle_PID[ch].loop = I_LOOP; // 电流环
                g_epwmHandle[ch].High_MOS_DUTY = gHandle_PID[ch].i_pid_out;
            }

            // if (g_Channelinfo[ch].current >= gHandle_PID[ch].i_ref)
            // {
            //     gHandle_PID[ch].i_err_sum = g_epwmHandle[ch].High_MOS_DUTY;
            // }

            // 1. 工步时间到
            g_Channelinfo[ch].WorkeRunTimer = Timer_GetClock() - g_Channelinfo[ch].WorkeRunStartTimer;

            if (g_Channelinfo[ch].WorkeRunTimer >= g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].timeLimit) // 工步时间到
            {
                g_Channelinfo[ch].workeDelayTimer = 0;
                g_Channelinfo[ch].error = 0x03; // 0x03:  以时间条件结束
                g_Channelinfo[ch].WorkeStartup = g_Channelinfo[ch].WorkeStartup + 1;
                g_Channelinfo[ch].fault.bit.TIMER_OUT = 1; // 工步时间到
            }
#if 1
            if (gHandle_PID[ch].loop == I_LOOP)
            {
                // 2. 到达设置充电截止电流
                if (g_Channelinfo[ch].current <= g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].currentLimit)
                {
                    if (g_Channelinfo[ch].SS_Timer >= 50) // 一瞬间误动作
                    {
                        g_Channelinfo[ch].error = 0x02; // 0x02:  以电流条件结束
                        g_Channelinfo[ch].SS_Timer = 0;
                        g_Channelinfo[ch].WorkeStartup = g_Channelinfo[ch].WorkeStartup + 1;
                        g_Channelinfo[ch].fault.bit.CC_Limit_OUT = 1; // 工步到达恒流设置值
                    }
                }
            }
            else if (gHandle_PID[ch].loop == V_LOOP)
            {
                // 3. 到达设置充电电压
                if (g_Channelinfo[ch].voltage >= g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].voltLimit)
                {
                    g_Channelinfo[ch].error = 0x01; // 0x01：以电压条件结束
                    g_Channelinfo[ch].WorkeStartup = g_Channelinfo[ch].WorkeStartup + 1;
                    g_Channelinfo[ch].fault.bit.CV_Limit_OUT = 1; // 工步到达恒压值
                }
            }
#endif
        }
        break;

        case POWER_RUN_DISCHARGE: // 放电模式
        {

            // DC放电切换占空比设置
            g_epwmHandle[ch].High_MOS_DUTY_MAX = TIMER_DC_DUTY_MAX;      // 36.40% 97750
            g_epwmHandle[ch].High_MOS_DUTY_MIN = 100;                    // 5200
            gHandle_PID[ch].i_up = (g_epwmHandle[ch].High_MOS_DUTY_MAX); //
            gHandle_PID[ch].i_ui = (g_epwmHandle[ch].High_MOS_DUTY_MIN); //
            gHandle_PID[ch].i_max_out_value = gHandle_PID[ch].i_up;      /* 最大脉宽*/
            gHandle_PID[ch].i_min_out_value = gHandle_PID[ch].i_ui;      /* 最小脉宽*/

            gHandle_PID[ch].v_up = (g_epwmHandle[ch].High_MOS_DUTY_MAX); //
            gHandle_PID[ch].v_ui = (g_epwmHandle[ch].High_MOS_DUTY_MIN); //
            gHandle_PID[ch].v_max_out_value = gHandle_PID[ch].v_up;      /* 最大脉宽*/
            gHandle_PID[ch].v_min_out_value = gHandle_PID[ch].v_ui;      /* 最小脉宽*/

            gHandle_PID[ch].v_kp = gHandle_PID[ch].v_kp + 1.1;
            if (gHandle_PID[ch].v_kp >= 10.1f)
                gHandle_PID[ch].v_kp = 10.1f;
            gHandle_PID[ch].v_ki = gHandle_PID[ch].v_ki + 1.1;
            if (gHandle_PID[ch].v_ki >= 1.1f)
                gHandle_PID[ch].v_ki = 1.1f;

            // 电流环g_Channelinfo[ch].Set_PreDC
            g_Channelinfo[ch].Set_CC += 0.1f;
            if (g_Channelinfo[ch].Set_CC >= g_Channelinfo[ch].Set_PreDC)
            {
                g_Channelinfo[ch].Set_CC = g_Channelinfo[ch].Set_PreDC;
            }

            gHandle_PID[ch].i_ref = g_Channelinfo[ch].Set_CC * (-1.0f);

            gHandle_PID[ch].i_fdb = g_Channelinfo[ch].current * (-1.0f); // 设置反馈值
            pid_I_Loop_calc(&gHandle_PID[ch]);

            gHandle_PID[ch].loop = I_LOOP; // 电流环
            g_epwmHandle[ch].High_MOS_DUTY = gHandle_PID[ch].i_pid_out;

            // if (g_Channelinfo[ch].current >=  g_Channelinfo[ch].Set_CC )
            // {
            //     gHandle_PID[ch].i_err_sum = g_epwmHandle[ch].High_MOS_DUTY;
            // }

            // 1. 工步时间到
            g_Channelinfo[ch].WorkeRunTimer = Timer_GetClock() - g_Channelinfo[ch].WorkeRunStartTimer;

            if (g_Channelinfo[ch].WorkeRunTimer >= g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].timeLimit) // 工步时间到
            {
                // g_Channelinfo[ch].workMode = POWER_SET_PARAM;
                g_Channelinfo[ch].WorkeStartup = g_Channelinfo[ch].WorkeStartup + 1;
                g_Channelinfo[ch].fault.bit.TIMER_OUT = 1; // 工步时间到
            }

            // 2. 到达设置充电截止电流
            if (g_Channelinfo[ch].current <= g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].currentLimit)
            {
                g_Channelinfo[ch].SS_Timer++;
                if (g_Channelinfo[ch].SS_Timer >= 500) // 一瞬间误动作
                {
                    g_Channelinfo[ch].SS_Timer = 0;
                    g_Channelinfo[ch].WorkeStartup = g_Channelinfo[ch].WorkeStartup + 1;
                    g_Channelinfo[ch].fault.bit.CC_Limit_OUT = 1; // 工步到达恒流设置值
                }
            }
            else
            {
                g_Channelinfo[ch].SS_Timer = 0;
            }
#if 1
            // 3. 到达设置充电电压
            if (g_Channelinfo[ch].voltage <= g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].voltLimit)
            {
                g_Channelinfo[ch].Limit_Timer++;
                if (g_Channelinfo[ch].Limit_Timer >= 5000) // 一瞬间误动作
                {
                    g_Channelinfo[ch].Limit_Timer = 0;
                    g_Channelinfo[ch].WorkeStartup = g_Channelinfo[ch].WorkeStartup + 1;
                    g_Channelinfo[ch].fault.bit.CV_Limit_OUT = 1; // 工步到达恒压值
                }
            }
            else
            {
                g_Channelinfo[ch].Limit_Timer = 0;
            }

#endif
        }
        break;

        case POWER_TEST: // 开环测试
        {
            // g_epwmHandle[i].Low_MOS_DUTY+=1;
            //  if(g_epwmHandle[i].Low_MOS_DUTY>g_epwmHandle[i].Low_MOS_Timer_TBPRD) g_epwmHandle[i].Low_MOS_DUTY =g_epwmHandle[i].Low_MOS_Timer_TBPRD;
        }
        break;
        }
    }
}

/**********************************************************************
 * Function: 	 TIMER0CallbackFunction
 * Description:  定时器0中断 20us
 * Input: 	      void
 * Output:
 * Return: 	    void
 * Others:
 * Modify Date:    Version:    Author:	      Modification:
 * -----------------------------------------------
 * 2025-06-05	  V1.0	      Hu Weiping
 **********************************************************************/

void TIMER0CallbackFunction(void *handle)
{
    // System_LED1_HIGH_LEVEL;
    // System_DBUGGPIO_HIGH_LEVEL;

    GetADC_Driver_Result();
    sample_irq_handler(); // 采集数据转换

#if CLOOS_LOOP_MODE      // 闭环开启保护
    Scan_System_Fault(); // 保护
#else
                          //  OpenLoopDebugPwm();
#endif

    System_CloseLoop_Status();

    Updata_EPWM_Handle();

    // System_DBUGGPIO_LOW_LEVEL;
    // System_LED1_LOW_LEVEL;
    /* USER CODE END TIMER1 ITCallBackFunc */
}

/**********************************************************************
 * Function: 	 Power_PID_Updata
 * Description:  调试pid参数
 * Input: 	      void
 * Output:
 * Return: 	    void
 * Others:
 * Modify Date:    Version:    Author:	      Modification:
 * -----------------------------------------------
 * 2025-06-05	  V1.0	      Hu Weiping
 **********************************************************************/
void Debug_Loop(void)
{
    // printf("\r\nworkMode= %d", g_Channelinfo[0].workMode);
    // printf("\r\nFAULT= 0x%X", g_Channelinfo[0].fault.all);
    // printf("\r\nSR_STA= 0x%d  LowFlag%d", g_epwmHandle[0].Low_MOS_STA, g_epwmHandle[0].Low_MOS_OpenFlag);

    printf("\r\nloop=%d", gHandle_PID[0].loop);
    // printf("\r\nlBurstout=%f", gHandle_Burst_PID[0].v_pid_out);

    //  printf("\r\nHigh_MOS_STA= 0x%d  High_MOS_OpenFlag=%d", g_epwmHandle[0].High_MOS_STA, g_epwmHandle[0].High_MOS_OpenFlag);
}
