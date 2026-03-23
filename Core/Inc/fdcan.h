/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    fdcan.h
  * @brief   This file contains all the function prototypes for
  *          the fdcan.c file
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
#ifndef __FDCAN_H__
#define __FDCAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern FDCAN_HandleTypeDef hfdcan1;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_FDCAN1_Init(void);

/* USER CODE BEGIN Prototypes */


/* FDCAN 引脚 定义 */

#define FDCAN_RX_GPIO_PORT                GPIOA
#define FDCAN_RX_GPIO_PIN                 GPIO_PIN_11
#define FDCAN_RX_GPIO_AF                  GPIO_AF9_FDCAN1
#define FDCAN_RX_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)              /* PA口时钟使能 */

#define FDCAN_TX_GPIO_PORT                GPIOA
#define FDCAN_TX_GPIO_PIN                 GPIO_PIN_12
#define FDCAN_TX_GPIO_AF                  GPIO_AF9_FDCAN1
#define FDCAN_TX_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)              /* PA口时钟使能 */

/* FDCAN1接收RX0中断使能 */
#define FDCAN1_RX0_INT_ENABLE   1         /* 0,不使能;1,使能. */

/******************************************************************************************/

uint8_t fdcan_init(uint16_t presc, uint8_t tsjw, uint16_t ntseg1, uint8_t ntseg2, uint32_t mode);  /* FDCAN初始化 */
uint8_t fdcan_send_msg(uint8_t *msg, uint32_t len);                                                /* FDCAN发送数据 */
uint8_t fdcan_receive_msg(uint8_t *buf);                                                           /* FDCAN接收数据 */


/*****************************FDCAN3*************************************************************/

#define FDCAN3_RX_GPIO_PORT                GPIOG
#define FDCAN3_RX_GPIO_PIN                 GPIO_PIN_10
#define FDCAN3_RX_GPIO_AF                  GPIO_AF2_FDCAN3
#define FDCAN3_RX_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)              /* PA口时钟使能 */

#define FDCAN3_TX_GPIO_PORT                GPIOG
#define FDCAN3_TX_GPIO_PIN                 GPIO_PIN_9
#define FDCAN3_TX_GPIO_AF                  GPIO_AF2_FDCAN3
#define FDCAN3_TX_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)              /* PA口时钟使能 */

/* FDCAN1接收RX0中断使能 */
#define FDCAN3_RX0_INT_ENABLE   1         /* 0,不使能;1,使能. */

/******************************************************************************************/
extern FDCAN_HandleTypeDef       g_fdcan3_handle;             /* FDCANx句柄 */
extern FDCAN_TxHeaderTypeDef     g_fdcan3_txheade;            /* 发送消息 */
extern FDCAN_RxHeaderTypeDef     g_fdcan3_rxheade;            /* 接收消息 */
uint8_t fdcan3_init(uint16_t presc, uint8_t tsjw, uint16_t ntseg1, uint8_t ntseg2, uint32_t mode);  /* FDCAN初始化 */
uint8_t fdcan3_send_msg(uint8_t *msg, uint32_t len);                                                /* FDCAN发送数据 */
uint8_t fdcan3_receive_msg(uint8_t *buf);                                                           /* FDCAN接收数据 */


void CAN1_Send_TEST(void);//初始化测试CAN 不知道为啥需要发送，才能发送 后续解决





extern HAL_StatusTypeDef FDCAN_SendMessage(uint8_t fc, uint16_t dest_did, uint16_t sub, uint8_t* data, uint8_t size);




/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __FDCAN_H__ */

