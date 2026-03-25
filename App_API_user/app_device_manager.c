
/***********************************************************************
 * Copyright (C) 2022, AT Corporation.
 *
 * File Name: 	  app_device_manager.c
 * File Mark:
 * Description:
 * Others: version  2025-03-30, V2.1.0, firmware for GD32F30x
 * Version:
 * Author: Hu Weiping
 * Date:
 * History 1:
 *     Date:           2024-05-5
 *     Version:        V1.0
 *     Author:         Hu Weiping
 **********************************************************************/

#include "head.h"
#include "app_device_manager.h"

volatile Calibration_Device g_Calibration_Device[TYPE_MAX_NUM]; // 继电器高压侧输入电压
volatile DEVICE_State_Information g_DEVICE_Information;

volatile RED_Device_Data g_Handle_REC_Device; // 存储需要保存的数据


Borad_Device g_Borad_Device;

void InitBorad_Device(void)
{
    g_Borad_Device.byte.currMax=6;
    g_Borad_Device.byte.voltMax=0x05;

    g_Borad_Device.byte.softVersion=0x85;
    g_Borad_Device.byte.Channel=0x10;
}
uint8_t * GetBorad_Device(void)
{
    printf("\r\n currMax =%d voltMax=%d softVersion=%d Channel=%d", g_Borad_Device.byte.currMax,g_Borad_Device.byte.voltMax,g_Borad_Device.byte.softVersion, g_Borad_Device.byte.Channel);
    return (uint8_t *)&g_Borad_Device.data[0];
}



void AppUser_Write_Init_REC_Device(void);

void Calibration_DeviceInit(void)
{
    uint16_t i;
    for (i = 0; i < TYPE_MAX_NUM; i++)
    {
        g_Calibration_Device[i].ka = 1.0;
        g_Calibration_Device[i].kb = 1.0;
    }

    // 电压校准系数
    // g_Calibration_Device[Caltype_V_OUT].ka = 0.004282227f; // 0~4095 --->0~17.54V
    // g_Calibration_Device[Caltype_V_OUT].kb = 0.0f;
    g_Calibration_Device[Caltype_V_OUT].ka = g_Handle_REC_Device.V_OUT_REC_ka;
    g_Calibration_Device[Caltype_V_OUT].kb = g_Handle_REC_Device.V_OUT_REC_kb;

    // 电流校准系数 34.127x - 25.568     0.0279x - 25.338
    // g_Calibration_Device[Caltype_Current].ka = 0.0279f;
    // g_Calibration_Device[Caltype_Current].kb = -25.338f;
    g_Calibration_Device[Caltype_Current].ka = g_Handle_REC_Device.I_OUT_REC_ka;
    g_Calibration_Device[Caltype_Current].kb = g_Handle_REC_Device.I_OUT_REC_kb;
}

// 初始化REC数据结构
void AppUser_Init_REC_Device(void)
{
   
    memset(&g_Handle_REC_Device, 0, sizeof(g_Handle_REC_Device)); // 初始化存储数据结构体

    g_Handle_REC_Device.dev_id = 1;

//    Read_REC_Device_Date(FLASH_SAMPLE_READ_START_ADDR);
//    // AppUser_Write_Init_REC_Device();
//    Calibration_DeviceInit(); // 校准参数
//    if(g_Handle_REC_Device.CRC_Code!=0xAA)
//    {
//      AppUser_Write_Init_REC_Device();
//      
//    DBG_PRINTF(" IC NOT Init_REC_Device \r\n");
//    }


//    DBG_PRINTF("V_Ref_REC_Vaule=%f I_Ref_REC_Vaule=%f \r\n", g_Handle_REC_Device.V_Ref_REC_Vaule, g_Handle_REC_Device.I_Ref_REC_Vaule);

//    DBG_PRINTF("dev_id=%d\r\n", g_Handle_REC_Device.dev_id);
//    DBG_PRINTF("I ka=%f kb=%f\r\n", g_Calibration_Device[Caltype_Current].ka, g_Calibration_Device[Caltype_Current].kb);
//    DBG_PRINTF("V ka=%f kb=%f\r\n", g_Calibration_Device[Caltype_V_OUT].ka, g_Calibration_Device[Caltype_V_OUT].kb);


}

// 写入初始化数据
void AppUser_Write_Init_REC_Device(void)
{

    // 电压校准系数
    g_Calibration_Device[Caltype_V_OUT].ka = 0.004282227f; // 0~4095 --->0~17.54V
    g_Calibration_Device[Caltype_V_OUT].kb = 0.0f;
    g_Handle_REC_Device.V_OUT_REC_ka = g_Calibration_Device[Caltype_V_OUT].ka;
    g_Handle_REC_Device.V_OUT_REC_kb = g_Calibration_Device[Caltype_V_OUT].kb;

    // 电流校准系数 34.127x - 25.568     0.0279x - 25.338
    g_Calibration_Device[Caltype_Current].ka = 0.0279f;
    g_Calibration_Device[Caltype_Current].kb = -25.338f;
    g_Handle_REC_Device.I_OUT_REC_ka = g_Calibration_Device[Caltype_Current].ka;
    g_Handle_REC_Device.I_OUT_REC_kb = g_Calibration_Device[Caltype_Current].kb;

    g_Handle_REC_Device.dev_id = 1;
    g_Handle_REC_Device.V_Ref_REC_Vaule = 12.0f;
    g_Handle_REC_Device.I_Ref_REC_Vaule = 81.0f;
    g_Handle_REC_Device.CRC_Code = 0xAA; // 添加写入校准数据校验码
    g_Handle_REC_Device.FW_Code = 0;     // BOOT需要设备来跟新固件

    g_Handle_REC_Device.FW_Rev = FW_REV; // 版本号

//    FlashErase(FLASH_SAMPLE_ERASE_START_ADDR, FLASH_SAMPLE_ERASE_NUM); // 擦除数据
//    memcpy(g_tempData, &g_Handle_REC_Device, sizeof(g_Handle_REC_Device));

//    FlashWrite(FLASH_SAMPLE_WRITE_START_ADDR, sizeof(g_Handle_REC_Device));

    // DBG_PRINTF("dev_id=%d\r\n", g_Handle_REC_Device.dev_id);
    // DBG_PRINTF("ka=%f kb=%f\r\n", g_Calibration_Device[Caltype_Current].ka, g_Calibration_Device[Caltype_Current].kb);
}


uint16_t BoardInfo_GetID()
{


    return g_Handle_REC_Device.dev_id ;
}
