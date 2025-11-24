/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define FAN_GPIO_Pin GPIO_PIN_3
#define FAN_GPIO_GPIO_Port GPIOE
#define LED_595_LD_Pin GPIO_PIN_4
#define LED_595_LD_GPIO_Port GPIOE
#define LED_595_CLK_Pin GPIO_PIN_5
#define LED_595_CLK_GPIO_Port GPIOE
#define LED_595_DATA_Pin GPIO_PIN_6
#define LED_595_DATA_GPIO_Port GPIOE
#define SYS_LED1_Pin GPIO_PIN_14
#define SYS_LED1_GPIO_Port GPIOC
#define SYS_LED2_Pin GPIO_PIN_15
#define SYS_LED2_GPIO_Port GPIOC
#define EXIO_165_LD_Pin GPIO_PIN_10
#define EXIO_165_LD_GPIO_Port GPIOE
#define EXIO_165_CLK_Pin GPIO_PIN_12
#define EXIO_165_CLK_GPIO_Port GPIOE
#define EXIO_165_DATA_Pin GPIO_PIN_15
#define EXIO_165_DATA_GPIO_Port GPIOE
#define CD4052_A_Pin GPIO_PIN_12
#define CD4052_A_GPIO_Port GPIOB
#define CD4052_B_Pin GPIO_PIN_15
#define CD4052_B_GPIO_Port GPIOB
#define AD_VD_EN_Pin GPIO_PIN_9
#define AD_VD_EN_GPIO_Port GPIOD
#define AD_V_PORT_EN_Pin GPIO_PIN_10
#define AD_V_PORT_EN_GPIO_Port GPIOD
#define EXIO_595_LD_Pin GPIO_PIN_8
#define EXIO_595_LD_GPIO_Port GPIOA
#define EXIO_595_CLK_Pin GPIO_PIN_11
#define EXIO_595_CLK_GPIO_Port GPIOA
#define EXIO_595_DATA_Pin GPIO_PIN_12
#define EXIO_595_DATA_GPIO_Port GPIOA
#define ADD1_Pin GPIO_PIN_2
#define ADD1_GPIO_Port GPIOD
#define ADD2_Pin GPIO_PIN_3
#define ADD2_GPIO_Port GPIOD
#define ADD3_Pin GPIO_PIN_4
#define ADD3_GPIO_Port GPIOD
#define ADD4_Pin GPIO_PIN_5
#define ADD4_GPIO_Port GPIOD
#define ADD5_Pin GPIO_PIN_6
#define ADD5_GPIO_Port GPIOD
#define ADD6_Pin GPIO_PIN_7
#define ADD6_GPIO_Port GPIOD
#define BUS_OVP_Pin GPIO_PIN_7
#define BUS_OVP_GPIO_Port GPIOB
#define UN_OVP_Pin GPIO_PIN_0
#define UN_OVP_GPIO_Port GPIOE
#define UN_OCP_Pin GPIO_PIN_1
#define UN_OCP_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
