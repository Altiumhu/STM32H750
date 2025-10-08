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
/**********************************************************************
 * Function:     debug_show_en
 * Description:  调试输出使能
 * Input:        flag：0.关闭输出
 * Output:
 * Return:      void
 * Others:      Modbus-RTU
 * Modify Date:    Version:    Author:        Modification:
 * -----------------------------------------------
 * 2022-12-05     V1.0        Hu Weiping
 **********************************************************************/
void debug_show_en(uint16_t en)
{

    debug.out = en;
    if (debug.out == 1)
    {
        printf("\033[2J");   // 清屏
        printf("\033[0;0H"); // 设置光标
    }
}



void debug_show(void)
{
    static uint32_t poll_time = 0;
    if (debug.msg == 0)
        return;

    printf("\r\n********第%d次****************\r\n", poll_time++);

}
extern void CAN3_Send_TEST(void);

extern void UserFlash_WriteUserData(void);
void AppDebug_vTask(void)
{
    static uint32_t poll_time = 0;

    if (debug.msg == 0)
        return;

    printf("\r\nDebug Run=%d\r\n", poll_time++);


       //  UserSlave_SendLink();
//         UserFlash_WriteUserData();
 //     AppUserDebug_TempVaule();
    // DebugLED_LOW_LEVEL ;
    // DBG_PRINTF("\r\ng_debugNUmer.debug1 %d \r\n",  g_debugNUmer.debug1);
   // AppUser_Sample_Debug();
//    Debug_HandlePID();

//     Debug_PWM();

//    AppUserDebug_TempVaule();
		//  CAN3_Send_TEST();
    Debug_Clear_Msg();
// DebugLED_HIGH_LEVEL ;

}