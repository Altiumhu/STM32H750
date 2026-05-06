
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

uint16_t OUT_V_ERR_CUNT [16]={0};
uint16_t OUT_V_CUNT [16]={0};
  float  Vouterr[16]={0.0f};
void AppUser_Voltage_OUT_VP(void)
{

  uint16_t ch = 0;
  uint32_t tempdata;



  for (ch = 0; ch < BOARD_CHANNEL_NUM; ch++)
  {

    switch (g_Channelinfo[ch].workMode)
    {
          case POWER_OFF:   // 关闭驱动
          case POWER_FAULT: // 关闭驱动
            if (g_Channelinfo[ch].voltage <= 0.5f && g_Channelinfo[ch].error==0)
            {
              g_Channelinfo[ch].fault.bit.L_UVP = 1;
              g_Channelinfo[ch].error =  EChannelError_NoBattery; //  //电池接反
            }

            break;
         
          case POWER_GET_V_PORT:
          case POWER_IDLE:  // 

            if (g_Channelinfo[ch].voltage <= 0.5f)
            {
              g_Channelinfo[ch].fault.bit.L_UVP = 1;
              g_Channelinfo[ch].error = EChannelError_NoBattery; //  // 无电池
            }

            break;


          case POWER_RUN_CHARGE:  // 
           


              // if (g_Channelinfo[ch].voltage_port< g_Channelinfo[ch].voltage )
              // {
              //   OUT_V_CUNT[ch]++;
              //   if( OUT_V_CUNT[ch]>=10)
              //   {
              //      OUT_V_CUNT[ch] =10;
              //     g_Channelinfo[ch].fault.bit.BAT_OUT_ERR = 1;
              //     g_Channelinfo[ch].error = EChannelError_SLAVE_OUT_ERR_0x89; // 电压接触不良
              //   }

              // }
              // else
              // {
              //    OUT_V_CUNT[ch] =0;
               
              // }

          //  if (g_Channelinfo[ch].voltage <= 0.5f)
          //   {
          //     g_Channelinfo[ch].fault.bit.L_UVP = 1;
          //     g_Channelinfo[ch].error = EChannelError_SLAVE_OUT_ERR_0x89; //  // 无电池
          //   }

          //  if (g_Channelinfo[ch].voltage >4.2f)
          //   {
              
          //    g_Channelinfo[ch].fault.bit.L_OVP = 1;
          //     g_Channelinfo[ch].error = EChannelError_SLAVE_OUT_ERR_0x89; //  // 无电池
          //   }
 

            break;



          case POWER_RUN_DISCHARGE:  // 放电模式
           

              // if (g_Channelinfo[ch].voltage_port> g_Channelinfo[ch].voltage )
              // {
              //   OUT_V_ERR_CUNT[ch]++;
              //   if( OUT_V_ERR_CUNT[ch]>=10)
              //   {
              //      OUT_V_ERR_CUNT[ch]=10;
              //     g_Channelinfo[ch].fault.bit.BAT_OUT_ERR = 1;
              //     g_Channelinfo[ch].error = EChannelError_SLAVE_OUT_ERR_0x89; // 电压接触不良
              //   }

              // }
              // else
              // {
              //    OUT_V_ERR_CUNT[ch] =0;
               
              // }

           if (g_Channelinfo[ch].voltage <= 0.5f)
            {
              g_Channelinfo[ch].fault.bit.L_UVP = 1;
              g_Channelinfo[ch].error = EChannelError_SLAVE_OUT_ERR_0x89; //  // 无电池
            }

           if (g_Channelinfo[ch].voltage >4.2f)
            {
              
              g_Channelinfo[ch].fault.bit.L_OVP = 1;
              g_Channelinfo[ch].error = EChannelError_SLAVE_OUT_ERR_0x89; //  // 无电池
            }
    

      

            break;

    }



  }
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
  for (ch = 0; ch < 16; ch++)
  {

    if (g_Channelinfo[ch].fault.all > 1)
    {
      printf(" \r\n  ch%d: fault=0x%X", ch + 1, g_Channelinfo[ch].fault.all);
      if (g_Channelinfo[ch].fault.bit.TIMER_OUT)
      {
        printf("\r\n fault TIMER_OUT 工步到达设置完成 %d ms ", g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].timeLimit);
      }
      if (g_Channelinfo[ch].fault.bit.CV_Limit_OUT)
      {
        printf("\r\n fault CV_Limit_OUT 工步到达恒压值=%f ", g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].voltLimit);
      }
      if (g_Channelinfo[ch].fault.bit.CC_Limit_OUT)
      {
        printf("\r\n fault CC_Limit_OUT 工步到达恒流设置值 =%f ", g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].currentLimit);
      }

      if (g_Channelinfo[ch].fault.bit.Worke_Setup_OVER)
      {
        printf("\r\n fault  Worke_Setup_OVER 工艺流程结束 ");
      }
      if (g_Channelinfo[ch].fault.bit.FAN_ERR)
      {
        printf("\r\n fault  风机故障 ");
      }
    }
  }
}