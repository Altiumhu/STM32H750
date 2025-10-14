/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "fdcan.h"
#include "i2c.h"
#include "memorymap.h"
#include "quadspi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "message_handler.h"
#include "system_can_config.h"
#include "large_data_transfer.h"
#include "crc.h"
#include "message_handler.h"
#include "system_can_config.h"

#include "head.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// #define ADC_BUFFER_SIZE 4  // ??????
uint16_t adcBuffer[ADC_BUFFER_SIZE]; // ADC???????
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */
// 传输完成回调
void OnTransferComplete(TransferStatus status);

// 数据接收回调
void OnDataReceived(uint16_t src_did, uint16_t session_id,
                    uint8_t *data, uint32_t size);

// 传输失败回调
void OnTransferFailed(uint16_t session_id, TransferStatus reason);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

LargeDataFrame gLargeDataFrame;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  MX_ADC1_Init();
  MX_FDCAN1_Init();
  MX_TIM2_Init();
  MX_TIM8_Init();
  MX_ADC3_Init();
  MX_QUADSPI_Init();
  MX_TIM4_Init();
  MX_UART7_Init();
  MX_I2C4_Init();
  MX_TIM5_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */

  AppUser_prvSetupHardware();
  Display_PeriphCLKFreq();

  // printf("\r\nSTM32F750=%f LargeDataFrame=%d", votlag,sizeof(LargeDataFrame));
  HAL_TIM_Base_Start_IT(&htim3);
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adcBuffer, ADC_BUFFER_SIZE);

  RS485_ModbusCmdTask("CmdTask"); // RS485通讯解初始化

  AppUser_Device_InitData();

  log_init(&huart1, LOG_OUTPUT_UART); // 初始化日志系统 (使用UART1输出)
  cli_init(&huart1);                  // 初始化CLI系统
  User_CLI_Cmd();                     // 注册命令

  // 校准ADC
  HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
  // 启动ADC转换
  ADC_StartConversion();
  printf("\r\nV 50 ");
  // TEST_Flash();

  // 发送初始消息 (设备上线通知)
  uint8_t init_msg[4] = {0xAA, 0x55, 0x01, 0x23};
  FDCAN_SendMessage(FC_BROADCAST, BROADCAST_DEVICE_ID, 0x000, init_msg, sizeof(init_msg));
  CAN1_Send_TEST();
  LargeDataTransfer_InitSender();
  LargeDataTransfer_InitReceiver();

  // 设置回调
  LargeDataTransfer_SetCompleteCallback(OnTransferComplete);
  LargeDataTransfer_SetReceivedCallback(OnDataReceived);
  LargeDataTransfer_SetFailedCallback(OnTransferFailed);

  while (1)
  {

    //        User_Update();
    AppUser_PortocolRecv(); // MCU通讯协议包

    AppDebug_vTask();
    //        AppUser_temp_sample();

#if 0
        // 主循环 - 发送定期状态更新
        static uint32_t last_tick = 0;
        if (HAL_GetTick() - last_tick > 1000)
        {
            last_tick = HAL_GetTick();

            // 发送状态更新
            uint8_t status[8] = {0};
            // 填充状态数据...
            FDCAN_SendMessage(FC_DIAGNOSTICS, 0x000, 0x000, status, sizeof(status));
        }
#else

    // 处理发送超时
    LargeDataTransfer_TimeoutHandler();

    // 处理接收会话超时
    LargeDataTransfer_SessionTimeoutHandler();
#endif

    //        // 其他应用逻辑...
    //        HAL_Delay(10);
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  //    while (1)
  //    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  //        /* USER CODE BEGIN 5 */
  //
  //        HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, 1);
  //        /* USER CODE END 5 */

  //    }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInitStruct.PLL2.PLL2M = 2;
  PeriphClkInitStruct.PLL2.PLL2N = 12;
  PeriphClkInitStruct.PLL2.PLL2P = 2;
  PeriphClkInitStruct.PLL2.PLL2Q = 2;
  PeriphClkInitStruct.PLL2.PLL2R = 2;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOMEDIUM;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
// uint8_t rx_data;
// HAL_UART_Receive_IT(&huart1, &rx_data, 1);

#if 0



extern void CAN1_Send_TEST(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {

        Timer3Cunt++;
        if (Timer3Cunt < 10)
        {
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, 0);
        }
        else if (Timer3Cunt < 20)
        {
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, 1);
        }
        else
        {
            Timer3Cunt = 0;
        }
        CAN1_Send_TEST();
        // printf("\r\n  ADC1 %d  %d  %d  %d ", adcBuffer[0], adcBuffer[1], adcBuffer[2], adcBuffer[3]);

    }
}
#endif
// void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
//{
//     if (hadc->Instance == ADC1)
//     {
//         // ??ADC??
//         uint16_t ch0_value = adcBuffer[0];  // PA0???
//         uint16_t ch1_value = adcBuffer[1];  // PA1???
//         uint16_t ch4_value = adcBuffer[2];  // PA4???

//        // ??:??ADC?(???UART)
////         printf("ADC: CH0=%u, CH1=%u, CH4=%u\n",
////               ch0_value, ch1_value, ch4_value);
//    }
//}

// 传输完成回调
void OnTransferComplete(TransferStatus status)
{
  if (status == TRANSFER_COMPLETE)
  {
    // 大数据传输成功
  }
  else
  {
    // 处理错误
  }
}

// 数据接收回调
void OnDataReceived(uint16_t src_did, uint16_t session_id,
                    uint8_t *data, uint32_t size)
{
  // 处理接收到的完整数据
  // 注意: 数据指针在会话结束后可能失效，需要立即复制数据
}

// 传输失败回调
void OnTransferFailed(uint16_t session_id, TransferStatus reason)
{
  // 处理传输失败
}

/* USER CODE END 4 */

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
