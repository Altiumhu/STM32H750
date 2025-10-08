
#include "head.h" // head.h 头文件
#include "AppUser_protection.h"

volatile uint16_t g_StartTimer = 0;

volatile power_fault_define g_Handle_Fault[CH_MAX];
volatile system_fault_define g_systemHandle_Fault;


 void Init_Protect_handle(void)
 {
     uint16_t ch;
        for(ch =0;ch<CH_MAX;ch++)
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
     if(g_StartTimer>=200)
     {
         g_StartTimer =200;
         AppUser_OUT_OCP();//过流电流保护
         AppUser_Voltage_OUT_VP();//电池电压保护

     }
}