/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    fdcan.c
 * @brief   This file provides code for the configuration
 *          of the FDCAN instances.
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
#include "fdcan.h"

/* USER CODE BEGIN 0 */

// ss
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "stdio.h"
#include "math.h"
#include "string.h"
#include <stdarg.h>

#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "system_can_config.h"

#include "message_handler.h"
#include "system_can_config.h"
#include "large_data_transfer.h"
#include "crc.h"
#include "message_handler.h"
#include "head.h"
FDCAN_TxHeaderTypeDef g_fdcanx_txheade;
FDCAN_RxHeaderTypeDef g_fdcanx_rxheade;


Can_Data g_Can_DataHandler;//
/* USER CODE END 0 */

FDCAN_HandleTypeDef hfdcan1;

int Can_SetFilter(uint8_t addrSlave, bool direct)
{
	//配置接收过滤器

    FDCAN_FilterTypeDef fdcan_filterconfig;



	PackHeader_t receiveFileter;

    if(addrSlave>16)
    {
      printf("Can_SetFilter addrSlave error\r\n");
      return -1;
    }

	
	//清零过滤器
	receiveFileter.dWordData=0;
	receiveFileter.userItemp.addrSlave = addrSlave;
	if(direct)
		receiveFileter.userItemp.direct = 1;
	//只接收扩展的数据帧
   /* 过滤器配置 --滤波器索引 只接收扩展的数据帧 */
    fdcan_filterconfig.IdType = FDCAN_EXTENDED_ID;             /* 标准扩展ID */
    fdcan_filterconfig.FilterIndex = 0;                        //* 滤波器索引 */
    fdcan_filterconfig.FilterType = FDCAN_FILTER_MASK;         /* 滤波器类型：传统位过滤 */
    fdcan_filterconfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; /* 过滤配置：当过滤匹配以后存储在Rx FIFO0中 */
    fdcan_filterconfig.FilterID1 = receiveFileter.dWordData;   /* 过滤ID1：29位标准ID */
    fdcan_filterconfig.FilterID2 = 0x0000007FF;                 /* 过滤ID2：配置为传统位过滤，ID2是29位掩码
                                                                *   这里表示过滤接收和FilterID1完全一样的消息ID
                                                                */

    if (HAL_FDCAN_ConfigFilter(&hfdcan1, &fdcan_filterconfig) != HAL_OK)
    {
         printf("Can_SetFilter addrSlave error=%d\r\n",fdcan_filterconfig.FilterIndex);
        return -1;
    }
	
   //只接收广播消息--0xFF
    /* 过滤器配置 */
    fdcan_filterconfig.IdType = FDCAN_EXTENDED_ID;             /* 标准ID */
    fdcan_filterconfig.FilterIndex = 1;                        //* 滤波器索引 */
    fdcan_filterconfig.FilterType = FDCAN_FILTER_MASK;         /* 滤波器类型：传统位过滤 */
    fdcan_filterconfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; /* 过滤配置：当过滤匹配以后存储在Rx FIFO0中 */
    fdcan_filterconfig.FilterID1 = 0x00000000FF;                      /* 过滤ID1：29位标准ID 配置广播ID:0xFFF*/
    fdcan_filterconfig.FilterID2 = 0x00000000FF;                 /* 过滤ID2：配置为传统位过滤，ID2是29位掩码
                                                                *   这里表示过滤接收和FilterID1完全一样的消息ID
                                                                */

    if (HAL_FDCAN_ConfigFilter(&hfdcan1, &fdcan_filterconfig) != HAL_OK)
    {
         printf("Can_SetFilter addrSlave error=%d\r\n",fdcan_filterconfig.FilterIndex);
        return -1;
    }
	
	return 0;
}
int CanFr_Init(void)
{
	
	memset(&g_Can_DataHandler,0 ,sizeof(Can_Data));	

	g_Can_DataHandler.recStruct.shortFrame.data = g_Can_DataHandler.recStruct.spData;
	
	#if CANMASTER
	Can_Data.addrSelf = 0;							//设置本机地址为0，表示主机	
	//主机只接收来自分机的信息，分机地址不过滤
	Can_SetFilter(0,1);
	#else
	g_Can_DataHandler.addrSelf = BoardInfo_GetID();							
	//分机只接收来自主机的广播信息和只发送给本机的信息
	Can_SetFilter(g_Can_DataHandler.addrSelf,0);

	#endif
    /* 配置全局过滤器，拒收所有不匹配的帧和远程帧 */
    HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);

    /* 开启FDCAN */
    if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
    {
       // printf("CanFr_Init error\r\n");
       // return -1;
    }

    /* 使能接收FIFO 0新消息中断 */
    HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
    HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_TX_COMPLETE, 0);
    return 0;
}

/* FDCAN1 init function */
void MX_FDCAN1_Init(void)
{

    /* USER CODE BEGIN FDCAN1_Init 0 */
    FDCAN_FilterTypeDef fdcan_filterconfig;
    HAL_FDCAN_DeInit(&hfdcan1);
    /* USER CODE END FDCAN1_Init 0 */

    /* USER CODE BEGIN FDCAN1_Init 1 */

    /* USER CODE END FDCAN1_Init 1 */
    hfdcan1.Instance = FDCAN1;
    hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
    hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
    hfdcan1.Init.AutoRetransmission = ENABLE;
    hfdcan1.Init.TransmitPause = DISABLE;
    hfdcan1.Init.ProtocolException = DISABLE;
    hfdcan1.Init.NominalPrescaler = 20;
    hfdcan1.Init.NominalSyncJumpWidth = 1;
    hfdcan1.Init.NominalTimeSeg1 = 10;
    hfdcan1.Init.NominalTimeSeg2 = 1;
    hfdcan1.Init.DataPrescaler = 1;
    hfdcan1.Init.DataSyncJumpWidth = 1;
    hfdcan1.Init.DataTimeSeg1 = 1;
    hfdcan1.Init.DataTimeSeg2 = 1;
    hfdcan1.Init.MessageRAMOffset = 0;
    hfdcan1.Init.StdFiltersNbr = 0;
    hfdcan1.Init.ExtFiltersNbr = 10;
    hfdcan1.Init.RxFifo0ElmtsNbr = 16;
    hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
    hfdcan1.Init.RxFifo1ElmtsNbr = 16;
    hfdcan1.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
    hfdcan1.Init.RxBuffersNbr = 8;
    hfdcan1.Init.RxBufferSize = FDCAN_DATA_BYTES_8;
    hfdcan1.Init.TxEventsNbr = 10;
    hfdcan1.Init.TxBuffersNbr = 10;
    hfdcan1.Init.TxFifoQueueElmtsNbr = 1;
    hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
    hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
    if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN FDCAN1_Init 2 */
    /* 过滤器配置 */
    fdcan_filterconfig.IdType = FDCAN_EXTENDED_ID;             /* 标准ID */
    fdcan_filterconfig.FilterIndex = 0;                        //* 滤波器索引 */
    fdcan_filterconfig.FilterType = FDCAN_FILTER_MASK;         /* 滤波器类型：传统位过滤 */
    fdcan_filterconfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; /* 过滤配置：当过滤匹配以后存储在Rx FIFO0中 */
    fdcan_filterconfig.FilterID1 = 0x00000004;                      /* 过滤ID1：29位标准ID */
    fdcan_filterconfig.FilterID2 = 0x0000007FF;                 /* 过滤ID2：配置为传统位过滤，ID2是29位掩码
                                                                *   这里表示过滤接收和FilterID1完全一样的消息ID
                                                                */

    if (HAL_FDCAN_ConfigFilter(&hfdcan1, &fdcan_filterconfig) != HAL_OK)
    {
        return;
    }

        /* USER CODE BEGIN FDCAN1_Init 2 */
    /* 过滤器配置 */
    fdcan_filterconfig.IdType = FDCAN_EXTENDED_ID;             /* 标准ID */
    fdcan_filterconfig.FilterIndex = 1;                        //* 滤波器索引 */
    fdcan_filterconfig.FilterType = FDCAN_FILTER_MASK;         /* 滤波器类型：传统位过滤 */
    fdcan_filterconfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; /* 过滤配置：当过滤匹配以后存储在Rx FIFO0中 */
    fdcan_filterconfig.FilterID1 = 0x00000000FF;                      /* 过滤ID1：29位标准ID 配置广播ID:0xFFF*/
    fdcan_filterconfig.FilterID2 = 0x000000007FF;                 /* 过滤ID2：配置为传统位过滤，ID2是29位掩码
                                                                *   这里表示过滤接收和FilterID1完全一样的消息ID
                                                                */

    if (HAL_FDCAN_ConfigFilter(&hfdcan1, &fdcan_filterconfig) != HAL_OK)
    {
        return;
    }


    /* 配置全局过滤器，拒收所有不匹配的帧和远程帧 */
    HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);

    /* 开启FDCAN */
    if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
    {
        return;
    }

    /* 使能接收FIFO 0新消息中断 */
    HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
    HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_TX_COMPLETE, 0);
    /* USER CODE END FDCAN1_Init 2 */
}

void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef *fdcanHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    if (fdcanHandle->Instance == FDCAN1)
    {
        /* USER CODE BEGIN FDCAN1_MspInit 0 */

        /* USER CODE END FDCAN1_MspInit 0 */

        /** Initializes the peripherals clock
         */
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
        PeriphClkInitStruct.FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
        {
            Error_Handler();
        }

        /* FDCAN1 clock enable */
        __HAL_RCC_FDCAN_CLK_ENABLE();

        __HAL_RCC_GPIOD_CLK_ENABLE();
        /**FDCAN1 GPIO Configuration
        PD0     ------> FDCAN1_RX
        PD1     ------> FDCAN1_TX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

        /* FDCAN1 interrupt Init */
        HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
				
        HAL_NVIC_SetPriority(FDCAN1_IT1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(FDCAN1_IT1_IRQn);
        /* USER CODE BEGIN FDCAN1_MspInit 1 */

        /* USER CODE END FDCAN1_MspInit 1 */
    }
}

void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef *fdcanHandle)
{

    if (fdcanHandle->Instance == FDCAN1)
    {
        /* USER CODE BEGIN FDCAN1_MspDeInit 0 */

        /* USER CODE END FDCAN1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_FDCAN_CLK_DISABLE();

        /**FDCAN1 GPIO Configuration
        PD0     ------> FDCAN1_RX
        PD1     ------> FDCAN1_TX
        */
        HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0 | GPIO_PIN_1);

        /* FDCAN1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(FDCAN1_IT0_IRQn);
        HAL_NVIC_DisableIRQ(FDCAN1_IT1_IRQn);
        /* USER CODE BEGIN FDCAN1_MspDeInit 1 */

        /* USER CODE END FDCAN1_MspDeInit 1 */
    }
}

uint8_t fdcan_send_msg(uint8_t *msg, uint32_t len)
{
    g_fdcanx_txheade.Identifier = 0x123;
    g_fdcanx_txheade.IdType = FDCAN_EXTENDED_ID;
    g_fdcanx_txheade.TxFrameType = FDCAN_DATA_FRAME;
    g_fdcanx_txheade.DataLength = len;
    g_fdcanx_txheade.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    g_fdcanx_txheade.BitRateSwitch = FDCAN_BRS_ON;
    g_fdcanx_txheade.FDFormat = FDCAN_CLASSIC_CAN;
    g_fdcanx_txheade.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    g_fdcanx_txheade.MessageMarker = 0;

    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &g_fdcanx_txheade, msg) != HAL_OK)
    {
        return 1;
    }

    return 0;
}

// FDCAN_StatusTypeDef FDCAN_SendMessage(uint8_t fc, uint16_t dest_did, uint16_t sub, uint8_t* data, uint8_t size) {
//   FDCAN_TxHeaderTypeDef TxHeader;
//   uint32_t id = BUILD_CAN_ID(fc, dest_did, sub);
//
//   TxHeader.Identifier = id;
//   TxHeader.IdType = FDCAN_EXTENDED_ID;
//   TxHeader.TxFrameType = FDCAN_DATA_FRAME;
//   TxHeader.DataLength = size << 16; // ת��ΪDLC
//   TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
//   TxHeader.BitRateSwitch = (fc == FC_DATA_TRANSFER) ? FDCAN_BRS_ON : FDCAN_BRS_OFF;
//   TxHeader.FDFormat = FDCAN_FD_CAN;
//   TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
//   TxHeader.MessageMarker = 0;
//
//   return HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, data);
// }

uint8_t fdcan_receive_msg(uint8_t *buf)
{

    if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &g_fdcanx_rxheade, buf) != HAL_OK)
    {
        return 0;
    }

    return g_fdcanx_rxheade.DataLength;
}

#if FDCAN1_RX0_INT_ENABLE

// void FDCAN1_IT0_IRQHandler(void)
//{
//     HAL_FDCAN_IRQHandler(&hfdcan1);
// }

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    uint8_t i = 0;
#if 0

    uint8_t rxdata[8];

    if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)    
    {
       
        HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &g_fdcanx_rxheade, rxdata);
        printf("id:%#x\r\n", g_fdcanx_rxheade.Identifier);
        printf("len:%d\r\n", g_fdcanx_rxheade.DataLength);
        for (i = 0; i < 8; i++)
        {
            printf("rxdata[%d]:%d\r\n", i, rxdata[i]);
        }

        HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
    }
#else

    if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0)
    {
        FDCAN_RxHeaderTypeDef RxHeader;
        uint8_t RxData[64];

        if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
        {
            if (RxHeader.IdType == FDCAN_EXTENDED_ID) // 接收扩展帧
            {
                uint32_t id = RxHeader.Identifier;
                uint8_t fc = (id >> 25) & 0x0F;      // 功能码位置 [28:25]
                uint16_t did = (id >> 12) & 0x1FFF; // 设备ID位置 [24:12]
                uint16_t sub = id & 0x0FFF;                 //// 子地址位置 [11:0]

                printf("\r\n Identifier= 0x%X DataLength= %d  \r\n",RxHeader.Identifier , RxHeader.DataLength);


            }
        }
        HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
    }

#endif
}

#endif

uint8_t g_CAN_TX_MSG[8] = {0xAA, 0x55, 0x66, 0X77, 0x88, 0x99, 0xCC, 0x11};

void CAN1_Send_TEST(void)
{

    fdcan_send_msg(g_CAN_TX_MSG, 8);
}

HAL_StatusTypeDef FDCAN_SendMessage(uint8_t fc, uint16_t dest_did, uint16_t sub, uint8_t *data, uint8_t size)
{
    FDCAN_TxHeaderTypeDef TxHeader;
    uint32_t id = BUILD_CAN_ID(fc, dest_did, sub);

    TxHeader.Identifier = id;
    TxHeader.IdType = FDCAN_EXTENDED_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    // TxHeader.DataLength = size << 16; // 转换为DLC
    TxHeader.DataLength = size; // 转换为DLC
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = (fc == FC_DATA_TRANSFER) ? FDCAN_BRS_ON : FDCAN_BRS_OFF;
    TxHeader.FDFormat = FDCAN_FD_CAN;
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;

    return HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, data);
}

// 用户实现示例（在自定义文件中）
void HAL_FDCAN_TxFifoEmptyCallback(FDCAN_HandleTypeDef *hfdcan)
{
    // 1. 填充新数据到发送队列
    // 2. 重启数据传输
    // 3. 更新发送状态标志
    printf("\r\nFDCAN1=  ");
    if (hfdcan->Instance == FDCAN1)
    {
    }
}

void HAL_FDCAN_TxBufferCompleteCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndexes)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hfdcan);
    UNUSED(BufferIndexes);
    printf("\r\n123  FDCAN1=  ");
    if (hfdcan->Instance == FDCAN1)
    {
    }
    /* NOTE: This function Should not be modified, when the callback is needed,
              the HAL_FDCAN_TxBufferCompleteCallback could be implemented in the user file
     */
}

/* USER CODE END 1 */
