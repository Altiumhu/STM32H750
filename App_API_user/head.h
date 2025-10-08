/***********************************************************************
* Copyright (C) 2024, xx Corporation.
*
* File Name: 	  head.h
* File Mark:
* Description: STM32H7
* Others: version  2025-06-13, V2.1.0, firmware for STM32H7x 使用STM32H723ZGT6 
* Version: BootCode V1.0
* Author: Hu Weiping
* Date:
* History 1:
*     Date:           2024-03-13
*     Version:        V1.0
*     Author:         Hu Weiping
**********************************************************************/
#ifndef BSP_HEAD_H
#define BSP_HEAD_H


/*********************************************************************************************************
  类型重定义
*********************************************************************************************************/
typedef unsigned char  BOOLEAN;
typedef unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef signed   char  INT8S;                    /* Signed    8 bit quantity                           */
typedef unsigned short INT16U;                   /* Unsigned 16 bit quantity                           */
typedef signed   short INT16S;                   /* Signed   16 bit quantity                           */
typedef unsigned int   INT32U;                   /* Unsigned 32 bit quantity                           */
typedef signed   int   INT32S;                   /* Signed   32 bit quantity                           */
typedef float          FP32;                     /* Single precision floating point                    */
typedef double         FP64;                     /* Double precision floating point                    */
typedef long long	     INT64;
typedef unsigned long long INT64U;

typedef unsigned int   uint32_t;                   /* Unsigned 32 bit quantity                           */
typedef unsigned short uint16_t;                   /* Unsigned 16 bit quantity                           */
typedef unsigned char  uint8_t;                    /* Unsigned  8 bit quantity                           */

typedef signed int   int32_t;                   /* Unsigned 32 bit quantity                           */
typedef signed short int16_t;                   /* Unsigned 16 bit quantity                           */
typedef signed char  int8_t;                    /* Unsigned  8 bit quantity                           */

#define	bool	_Bool



#ifndef FALSE
#define	FALSE					0
#define	false					0
#endif

#ifndef TRUE

#define	TRUE					1
#define	true					1
	
#endif


#ifdef __cplusplus
}
#endif      /*  __cplusplus                 */


// ss
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "stdio.h"
#include "math.h"
#include "string.h"
#include <stdarg.h>


#include "sys.h"
#include "delay.h"


#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "main.h"
//#include "spi.h"


//#include "./SYSTEM/sys/sys.h"
//#include "./SYSTEM/usart/uart.h"
//#include "./SYSTEM/delay/delay.h"
//#include "./BSP/LED/led.h"
//#include "./BSP/MPU/mpu.h"
//#include "./BSP/SDRAM/sdram.h"
//#include "./USMART/usmart.h"
//#include "./BSP/KEY/key.h"
//#include "./BSP/FDCAN/fdcan.h"

//#include "./BSP/PWR/pwr.h"
//#include "./BSP/RTC/rtc.h"


//#include "./BSP/NORFLASH/norflash.h"
//#include "./BSP/SDMMC/sdmmc_sdcard.h"
//#include "./FATFS/exfuns/exfuns.h"






#include "app_drive_adc.h"
#include "app_drive_gpio.h"
#include "app_drive_timer1.h"
#include "app_drive_uasrt.h"
#include "app_drive_flash.h"
//#include "app_drive_apt.h"
//#include "app_drive_fdcan.h"

#include "HC165D.h"
#include "power_pid.h"

#include "Rxqueue.h"
#include "Rs485Task.h"
#include "app_debug.h"
//#include "App_MCU_Queue.h"

#include "app_device_manager.h"
#include "app_api_timer1.h"
#include "app_power_Loop.h"


#include "AppUser_Msg.h"
#include "AppUser_Sample.h"
#include "user_crc_data.h"
#include "AppUser_protection.h"
#include "AppUser_pwm.h"
#include "AppUuser_Temper.h"
#include "sysinitstartup.h"   
#include "system_Config.h"
#include "appUser_Lowpass.h"
#include "app_api_cal.h"

#include "app_api_gpio.h"  
//// #include "Appuser_WatchDog.h"
 #include "AppUser_Flash.h"
//// #include "AppUser_DAC.h"


// 
// 

// 
//#include "Framework.h"
//#include "UserEthernet.h"
//#include "UserUpMchine.h"
//#include "UserExtFlash.h"
//#include "UserSlave.h"
//#include "UserCalibrate.h"
//#include "UserHelpTool.h"
//#include "UserRGBTower.h"
//#include "UserRs485.h"
//#include "UserTempCap.h"
//#include "UserMain.h"

//#include "./MALLOC/malloc.h"

//#include "log_system.h"
//#include "cli_system.h"
//#include "cli_cfg.h"


//#include "large_data_transfer.h"

//#include "UserNorFlash_data.h"
//#include "UserFATFS_data.h"


#define  fmin(a,b) ((a)<(b)?(a):(b))
#define  fmax(a,b) ((a)>(b)?(a):(b))

#define  __fmin(a,b) ((a)<(b)?(a):(b))
#define  __fmax(a,b) ((a)>(b)?(a):(b))


#endif

