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

    for (ch = 0; ch < BOARD_CHANNEL_NUM; ch++)
    {

        if (g_Channelinfo[ch].fault.all) // 判断故障
        {
            g_Channelinfo[ch].workMode = POWER_FAULT;
            // pwm_stop(ch); /// 出现故障就关闭pwm
        }

        switch (g_Channelinfo[ch].workMode)
        {
        case POWER_OFF: // 关闭驱动    
        case POWER_FAULT: // 关闭驱动
        {
            if (g_Channelinfo[ch].fault.all == 0) // 判断故障  没有故障进入正常启动程序
            {
                if (g_DEVICE_Information.workMode == POWER_SET_CHARGE)
                {
                }
                g_Channelinfo[ch].workMode = POWER_SET_PARAM;

                g_Channelinfo[ch].GetPortTimer = 0;
            }
            else
            { // 说明有故障，关闭 PRT管 输出低关闭
            }
        }
        break;

        case POWER_SET_PARAM://设置工步参数

            g_Channelinfo[ch].workMode = POWER_GET_V_PORT;

            break;

        case POWER_GET_V_PORT: //
            g_Channelinfo[ch].GetPortTimer++;
            if (g_Channelinfo[ch].GetPortTimer >= 12)
            {
                g_Channelinfo[ch].GetPortTimer = 0;
                g_Channelinfo[ch].workMode = POWER_INIT;
            }

            break;

        case POWER_INIT: //
        {

            PIDInit(ch);
            HAL_EPWM_Config(ch);
            // Set_Sample_Channel_VPortGPIO(AD_V_CAP_EN);

            g_Channelinfo[ch].workMode = POWER_PRECHARGE;
            //            if ((g_Channelinfo[i].voltage_port - g_Channelinfo[i].Cap_voltage) >= -0.05f) //端口电压-电容电压》=0.05V 防止开机电容有电
            //
            //            {
            //                g_Channelinfo[i].workMode = POWER_PRECHARGE;
            //            }
            //            else
            //            {
            //                g_Channelinfo[i].fault.bit.CAP_BAT = 1; //容压大于电池电压
            //            }
            g_Channelinfo[ch].run = 0x0;
            g_Channelinfo[ch].SS_Timer = 0;

            pwm_start(ch, 0);
            Set_PWM_Channel_CH595_EN(2, ch, EX_595_SET); // 打开MOS驱动使能
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
            gHandle_PID[ch].v_ref = g_Channelinfo[ch].Set_CV;

            // gHandle_PID[i].v_ref = gHandle_PID[i].v_ref + 0.001f;
            // 软启功率输出电容电压作为反馈值，电池端电压作为给定值
            // gHandle_PID[i].v_ref = fmin(gHandle_PID[i].v_ref, g_Channelinfo[i].voltage);

            gHandle_PID[ch].v_fdb = g_Channelinfo[ch].Cap_voltage; // 设置反馈值
            pid_V_Loop_calc(&gHandle_PID[ch]);

            if (gHandle_PID[ch].v_pid_out <= gHandle_PID[ch].i_pid_out)
            {
                g_epwmHandle[ch].High_MOS_DUTY = gHandle_PID[ch].v_pid_out;
            }
            else
            {
                g_epwmHandle[ch].High_MOS_DUTY = gHandle_PID[ch].i_pid_out;
            }

            g_epwmHandle[ch].High_MOS_STA = 1;

            if (g_Channelinfo[ch].Cap_voltage >= 3.0f)

            {
                g_Channelinfo[ch].SS_Timer++;
                if (g_Channelinfo[ch].SS_Timer >= 10) // 开机一瞬间误动作
                {
                    g_Channelinfo[ch].SS_Timer = 0;
                    // ex_595_write(0, EX_595_PIN_0|EX_595_PIN_1, 1);
                    Set_PWM_Channel_CH595_EN(0, ch, EX_595_SET); // 打开PRT
                    g_Channelinfo[ch].workMode = POWER_RUN_CHARGE;
                }
            }

            //		 ex_595_write(2, EX_595_PIN_0|EX_595_PIN_1, 1);
        }
        break;
        case POWER_SoftStart: // 软启动
        {
        }
        break;

        case POWER_RUN_CHARGE: // 06
        {

            gHandle_PID[ch].v_kp = gHandle_PID[ch].v_kp + 0.1;
            if (gHandle_PID[ch].v_kp >= 1.1f)
                gHandle_PID[ch].v_kp = 1.1f;
            gHandle_PID[ch].v_ki = gHandle_PID[ch].v_ki + 0.1;
            if (gHandle_PID[ch].v_ki >= 0.1f)
                gHandle_PID[ch].v_ki = 0.1f;

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
            gHandle_PID[ch].i_ref += 0.01f;
            if (gHandle_PID[ch].i_ref >= g_Channelinfo[ch].Set_PreCC)
            {
                gHandle_PID[ch].i_ref = g_Channelinfo[ch].Set_PreCC;
            }

            gHandle_PID[ch].i_fdb = g_Channelinfo[ch].current; // 设置反馈值
            pid_I_Loop_calc(&gHandle_PID[ch]);

            if (gHandle_PID[ch].v_pid_out < gHandle_PID[ch].i_pid_out) // 双环竞争
            {
                g_epwmHandle[ch].High_MOS_DUTY = gHandle_PID[ch].v_pid_out;
            }
            else
            {
                g_epwmHandle[ch].High_MOS_DUTY = gHandle_PID[ch].i_pid_out;
            }

            if (g_Channelinfo[ch].current >= gHandle_PID[ch].i_ref)
            {
                gHandle_PID[ch].i_err_sum = g_epwmHandle[ch].High_MOS_DUTY;
            }
        }
        break;
        case POWER_RUN_DISCHARGE: // 放电模式
        {
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
