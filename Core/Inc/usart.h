/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "stm32h7xx.h"
//#include "stm32_types.h"
#include "stm32h7xx_hal.h"
/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN Private defines */
#define UART_LSR_RDR  (1<<5)    //������ɱ�־
#define UART_LSR_THRE (1<<6)    //������ɱ�־

#define SEND_BUF1 (USART1->TDR)
#define RECV_BUF1 (USART1->RDR)
#define USART1_SR (USART1->ISR)

#define SEND_BUF2 (USART2->TDR)
#define RECV_BUF2 (USART2->RDR)
#define USART2_SR (USART2->ISR)

#define SEND_BUF3 (USART3->TDR)
#define RECV_BUF3 (USART3->RDR)
#define USART3_SR (USART3->ISR)

#define SEND_BUF6 (USART6->TDR)
#define RECV_BUF6 (USART6->RDR)
#define USART6_SR (USART6->ISR)

#define SEND_BUF4 (UART4->TDR)
#define RECV_BUF4 (UART4->RDR)
#define UART4_SR  (UART4->ISR)

#define SEND_BUF5 (UART5->TDR)
#define RECV_BUF5 (UART5->RDR)
#define UART5_SR  (UART5->ISR)
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

