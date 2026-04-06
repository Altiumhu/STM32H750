

#include "head.h" // head.h 头文件

#include "sysinitstartup.h"
// #include "app_power_Loop.h"

void Initdata_g_DEVICE_Information(void)
{

  //   g_DEVICE_Information.Err_State = 0;
  //   g_DEVICE_Information.workMode = POWER_SET_CHARGE;
}
/**
 * Enable DMA controller clock
 */
void BSP_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 1, 1);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA1_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 3, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
  /* DMA1_Stream7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream7_IRQn);

  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 4, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
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
  uint16_t ch, type;

  AppUser_Init_REC_Device();

  Initdata_g_DEVICE_Information();

  AppUuser_InitTempData(); // 温度采集数据初始化
  debug_init();



  for (ch = 0; ch < BOARD_CHANNEL_NUM; ch++)
  {
    memset(&g_Channelinfo[ch], 0, sizeof(ChannelInfo));
  }

  for (ch = 0; ch < BOARD_CHANNEL_NUM; ch++)
  {
    g_Channelinfo[ch].fault.all = 1;
    g_epwmHandle[ch].High_MOS_STA = 0;

    g_Channelinfo[ch].run = Worke_OFF;

    g_Channelinfo[ch].Set_PreCC = 1.0f; // 5.0A设置CC电5.0A

    g_Channelinfo[ch].Set_PreDC = -1.0f; // 设置放电5.0A

    g_Channelinfo[ch].Set_PreCV = BOARD_OUT_VOLT; // 5.0V 电压值
    g_Channelinfo[ch].Set_CV = 3.0f;              // 5.0V 电压值

    g_Channelinfo[ch].Set_CC = 1.0;

    g_Channelinfo[ch].Run_Cyc_indx =0;

    //       g_Channelinfo[ch].Set_DC = g_Channelinfo[ch].Set_CC*(-1.0f);
    //       g_Channelinfo[ch].Set_CV = g_Channelinfo[ch].Set_PreCV;

    HAL_EPWM_Config(ch);
    PIDInit(ch);
    pwm_stop(ch);
  }

  InitBorad_Device();
  CanFr_Init(); // 配置


    UserSlave_Init();
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
  App_GPIO_Init();
  //  MX_GPIO_Init();
  BSP_DMA_Init();

  EXTIO_HC165_Init();
  ex_595_init();
  //  norflash_init(); /* 初始化W25Q128 */
  //  printf("\r\n prvSetupHardware_Init");
  //  mpu_memory_protection();                               /* 保护相关存储区域 */
  //  sdram_init();                                          /* 初始化SDRAM */
  //  // key_init();                                            /* 初始化按键 */
  MCU_IO_OUT_Init(); /* 输出IO初始化 */
  //  fdcan_init(1, 8, 31, 8,   FDCAN_MODE_NORMAL); /* FDCAN初始化， 普通模式,波特率500kbps  */
  //  fdcan3_init(10, 8, 16, 5, FDCAN_MODE_NORMAL);          /* FDCAN初始化，普通模式,波特率500kbps */
  App_Drive_UASRT_Init();

  // 校准ADC
  HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
  // 启动ADC转换
  ADC_StartConversion();
  // 校准ADC
  HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
  // 启动ADC转换
  ADC3_StartConversion();

  Set_ULock_GPIO(); // 解除硬件保护

  for (uint16_t i = 0; i < 16; i++)
  {
    Set_PWM_Channel_CH595_EN(2, i, EX_595_RESET); // 打开MOS驱动使能

    Set_PWM_Channel_CH595_EN(0, i, EX_595_RESET); // 打开PRT
  }
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
