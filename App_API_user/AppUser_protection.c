
#include "head.h" // head.h 头文件
#include "AppUser_protection.h"

volatile uint16_t g_StartTimer = 0;

volatile power_fault_define g_Handle_Fault[CH_MAX];

void Init_Protect_handle(void)
{
    uint16_t ch;
    for (ch = 0; ch < CH_MAX; ch++)
    {
    }
}

void AppUser_OUT_OCP(void)
{
}

void AppUser_Voltage_OUT_VP(void)
{
}

void Scan_System_Fault(void)
{
    g_StartTimer++;
    if (g_StartTimer >= 200)
    {
        g_StartTimer = 200;
        Sys_Power_INPUT_OVP();    // 输入过压保护
        AppUser_OUT_OCP();        // 过流电流保护
        AppUser_Voltage_OUT_VP(); // 电池电压保护
    }
}



void AppUser_Debug_Protect(void)
{
    uint16_t ch;
    for (ch  = 0; ch < 16; ch++)
    {
        
        if(g_Channelinfo[ch].fault.all>1)
        {
              printf(" \r\n  ch%d: fault=0x%X", ch+1,g_Channelinfo[ch].fault.all);
              if(g_Channelinfo[ch].fault.bit.TIMER_OUT)
              {
                printf("\r\n fault TIMER_OUT 工步到达设置完成");
              }
              if(g_Channelinfo[ch].fault.bit.CV_Limit_OUT)
              {
                printf("\r\n fault CV_Limit_OUT 工步到达恒压值 ");
              }
               if(g_Channelinfo[ch].fault.bit.CC_Limit_OUT)
              {
                printf("\r\n fault CC_Limit_OUT 工步到达恒流设置值 ");
              }

              if(g_Channelinfo[ch].fault.bit.Worke_Setup_OVER)
              {
                printf("\r\n fault  Worke_Setup_OVER 工艺流程结束 ");
              }
              if(g_Channelinfo[ch].fault.bit.FAN_ERR)
              {
                printf("\r\n fault  风机故障 ");
              }
        }
      
    }
    

}