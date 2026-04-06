#include "head.h"
#include "app_debug.h"

Debug debug;

void debug_init(void)
{
    // memset(&debug, 0, sizeof(debug));
    debug.out = 0;
    debug.time = 0;
    debug.echo = 0;
    debug.poll = 0;
    debug.msg = 0;

    debug.out = DEBUG_STATR;
}

inline void Debug_Timer_1ms(void)
{
    if (debug.out == 0)
    {
        debug.time = debug.time + 1;
        if (debug.time >= DEBUG_TIME_GAP)
        {
            debug.time = 0;
            debug.msg = 1;
            // g_Sys_State.start_RunFlag =1;
        }
    }
}
void Debug_Clear_Msg(void)
{
    debug.msg = 0;
}

/**********************************************************************
 * Function:     debug_show_mode
 * Description:  设置调试输出模式
 * Input:        echo：1.回显输出
 * Output:
 * Return:      void
 * Others:      Modbus-RTU
 * Modify Date:    Version:    Author:        Modification:
 * -----------------------------------------------
 * 2022-12-05     V1.0        Hu Weiping
 **********************************************************************/
void debug_show_mode(uint16_t echo)
{
    debug.echo = echo;
}

void debug_show_en(int argc, char *argv[])
{
    int num[16];
    int channel, i;

    if (argc < 2)
    {
        printf("Usage: 输入非法指令\r\n");
        return;
    }

    // 参数个数（不含命令本身）
    int num_params = argc - 1;
    // 字符串转整型
    num[0] = atoi(argv[1]);
    num[1] = atoi(argv[2]);
    debug.out = num[0];
    printf("\r\n  debug.out =%d\r\n", debug.out);

    if (debug.out == 1)
    {
        printf("\033[2J");   // 清屏
        printf("\033[0;0H"); // 设置光标
    }
}
void debug_show(void)
{
    static uint32_t poll_time = 0;
    debug.msg = 0;
    if (debug.msg == 0)
        return;

    printf("\r\n********第%d次****************\r\n", poll_time++);
}
extern void CAN1_Send_TEST(void);

extern void UserFlash_WriteUserData(void);

void debug_show_workMode(void)
{
}
void AppDebug_vTask(void)
{
    static uint32_t poll_time = 0;
    static uint16_t chnum = 1;

    if (debug.msg == 0)
        return;

    printf("\r\nDebug Run=%d\r\n", poll_time++);


    printf("\r\nTimer_GetClock =%d \r\n", Timer_GetClock());
    //
    //  ex_595_write(0, EX_595_PIN_0, 1);
    //  ex_595_write(2, EX_595_PIN_0, 1);

    //  g_Channelinfo[chnum].fault.all = 0;
    //  pwm_start(0, 0);
    //  Set_PWM_Channel_CH595_EN(2, chnum, EX_595_SET); // 打开MOS驱动使能

    ///   Set_PWM_Channel_CH595_EN(0, chnum, EX_595_SET); // 打开PRT

    //		  g_Channelinfo[0].fault.all= 0;
    //  UserSlave_SendLink();
    //         UserFlash_WriteUserData();
    //     AppUserDebug_TempVaule();
    // DebugLED_LOW_LEVEL ;
    // DBG_PRINTF("\r\ng_debugNUmer.debug1 %d \r\n",  g_debugNUmer.debug1);
    AppUser_Sample_Debug();
    AppUser_Debug_Protect();
    // Debug_HandlePID();
//    CAN1_Send_TEST();
    //     Debug_PWM();
    Debug_Loop();

    //    AppUserDebug_TempVaule();

    Debug_Clear_Msg();
    // DebugLED_HIGH_LEVEL ;
}