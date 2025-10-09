

#include "head.h" // head.h 头文件

#include "sysinitstartup.h"
// #include "app_power_Loop.h"

void Initdata_g_DEVICE_Information(void)
{

  //   g_DEVICE_Information.Err_State = 0;
  //   g_DEVICE_Information.workMode = POWER_SET_CHARGE;
}

/**********************************************************************
 * Function:     AppUser_InitData
 * Description:
 * Input:
 * Output:
 * Return:      void
 * Others:
 * Modify Date:    Version:    Author:        Modification:
 * -----------------------------------------------
 * 2022-12-15     V1.0        Hu Weiping
 **********************************************************************/
void AppUser_Device_InitData(void)
{
  volatile uint16_t ch;
  //  memset( &g_Handle_Fault,0,sizeof(power_fault_define));
  //   g_Handle_Fault[0].all = 0x0;

  //    Initdata_g_DEVICE_Information();

//  AppUuser_InitTempData(); // 温度采集数据初始化
//  debug_init();
//  Init_Master_Hand();

//  Smoke_Init_Data();
  //     Init_Protect_handle();
  // memset( &g_DEVICE_Information,0,sizeof(DEVICE_State_Information));  //  这个编译器使用memset会有问题

  // memset(&g_Channelinfo, 0, sizeof(ChannelInfo));

  // device_data_init();
  //  Init_Calibration_Device();

  //    Init_Protect_handle();
}

/**********************************************************************
 * Function:      AppUser_prvSetupHardware
 * Description:
 * Input:          void
 * Output:
 * Return:       void
 * Others:
 * Modify Date:    Version:    Author:         Modification:
 * -----------------------------------------------
 * 2022-05-31     V1.0        Hu Weiping
 **********************************************************************/
void AppUser_prvSetupHardware(void)
{

//  MX_GPIO_Init();
//  MX_DMA_Init();
//  MX_TIM7_Init();
//  MX_USART1_UART_Init();
//  MX_USART3_UART_Init();
//  MX_UART7_Init();
//  MX_SPI2_Init();
//  MX_TIM6_Init();
//  MX_TIM8_Init();
//  MX_ADC1_Init();
  EXTIO_HC165_Init();
//  norflash_init(); /* 初始化W25Q128 */
//  printf("\r\n prvSetupHardware_Init");
//  mpu_memory_protection();                               /* 保护相关存储区域 */
//  sdram_init();                                          /* 初始化SDRAM */
//  // key_init();                                            /* 初始化按键 */
//  MCU_IO_OUT_Init();                                     /* 输出IO初始化 */
//  fdcan_init(1, 8, 31, 8,   FDCAN_MODE_NORMAL); /* FDCAN初始化， 普通模式,波特率500kbps  */
//  fdcan3_init(10, 8, 16, 5, FDCAN_MODE_NORMAL);          /* FDCAN初始化，普通模式,波特率500kbps */
  App_Drive_UASRT_Init();

//  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1); // 关键启动代码
//  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2); // 关键启动代码
     App_Drive_InitTimer_7();                  // 100ms
}
void Display_PeriphCLKFreq(void)
{

  printf("\n\rfdcan_init ");
  printf("\n\rFDCAN 时钟: %d Hz", HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_FDCAN));
  printf("\n\rSPI2 时钟: %d Hz", HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SPI2));
  printf("\n\rFMC 时钟: %d Hz", HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_FMC));
  printf("\n\rSDMMC 时钟: %d Hz", HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_SDMMC));
  printf("\n\rADC时钟: %d Hz\n\r", HAL_RCCEx_GetPeriphCLKFreq(RCC_PERIPHCLK_ADC));
}
