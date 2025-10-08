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

FDCAN_TxHeaderTypeDef     g_fdcanx_txheade;            /* ������Ϣ */
FDCAN_RxHeaderTypeDef     g_fdcanx_rxheade;            /* ������Ϣ */
/* USER CODE END 0 */

FDCAN_HandleTypeDef hfdcan1;

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
  hfdcan1.Init.TxFifoQueueElmtsNbr = 10;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN1_Init 2 */
    /* ���������� */
    fdcan_filterconfig.IdType = FDCAN_EXTENDED_ID;                   /* ��׼ID */
    fdcan_filterconfig.FilterIndex = 0;                              /* �˲������� */
    fdcan_filterconfig.FilterType = FDCAN_FILTER_MASK;               /* �˲������ͣ���ͳλ���� */
    fdcan_filterconfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;       /* �������ã�������ƥ���Ժ�洢��Rx FIFO0�� */
    fdcan_filterconfig.FilterID1 = 0x123;                            /* ����ID1��11λ��׼ID */
    fdcan_filterconfig.FilterID2 = 0x0;                            /* ����ID2������Ϊ��ͳλ���ˣ�ID2��11λ����
                                                                      * �����ʾ���˽��պ�FilterID1��ȫһ������ϢID
                                                                      */

    if (HAL_FDCAN_ConfigFilter(&hfdcan1, &fdcan_filterconfig) != HAL_OK)
    {
        return ;
    }

    /* ����ȫ�ֹ��������������в�ƥ���֡ */
    HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);

    /* ����FDCAN */
    if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
    {
        return ;
    }

    /* ʹ�ܽ���FIFO 1����Ϣ�ж� */
    HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
    HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_TX_COMPLETE, 0);
  /* USER CODE END FDCAN1_Init 2 */

}
<<<<<<< HEAD
=======
/* FDCAN2 init function */
void MX_FDCAN2_Init(void)
{

  /* USER CODE BEGIN FDCAN2_Init 0 */
    FDCAN_FilterTypeDef fdcan_filterconfig;
    HAL_FDCAN_DeInit(&hfdcan2);
  /* USER CODE END FDCAN2_Init 0 */

  /* USER CODE BEGIN FDCAN2_Init 1 */

  /* USER CODE END FDCAN2_Init 1 */
  hfdcan2.Instance = FDCAN2;
  hfdcan2.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan2.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan2.Init.AutoRetransmission = DISABLE;
  hfdcan2.Init.TransmitPause = ENABLE;
  hfdcan2.Init.ProtocolException = DISABLE;
  hfdcan2.Init.NominalPrescaler = 20;
  hfdcan2.Init.NominalSyncJumpWidth = 1;
  hfdcan2.Init.NominalTimeSeg1 = 10;
  hfdcan2.Init.NominalTimeSeg2 = 1;
  hfdcan2.Init.DataPrescaler = 1;
  hfdcan2.Init.DataSyncJumpWidth = 1;
  hfdcan2.Init.DataTimeSeg1 = 1;
  hfdcan2.Init.DataTimeSeg2 = 1;
  hfdcan2.Init.MessageRAMOffset = 0;
  hfdcan2.Init.StdFiltersNbr = 0;
  hfdcan2.Init.ExtFiltersNbr = 1;
  hfdcan2.Init.RxFifo0ElmtsNbr = 1;
  hfdcan2.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
  hfdcan2.Init.RxFifo1ElmtsNbr = 1;
  hfdcan2.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
  hfdcan2.Init.RxBuffersNbr = 1;
  hfdcan2.Init.RxBufferSize = FDCAN_DATA_BYTES_8;
  hfdcan2.Init.TxEventsNbr = 0;
  hfdcan2.Init.TxBuffersNbr = 0;
  hfdcan2.Init.TxFifoQueueElmtsNbr = 1;
  hfdcan2.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  hfdcan2.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
  if (HAL_FDCAN_Init(&hfdcan2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN2_Init 2 */
    /* ���������� */
    fdcan_filterconfig.IdType = FDCAN_EXTENDED_ID;                   /* ��׼ID */
    fdcan_filterconfig.FilterIndex = 0;                              /* �˲������� */
    fdcan_filterconfig.FilterType = FDCAN_FILTER_MASK;               /* �˲������ͣ���ͳλ���� */
    fdcan_filterconfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;       /* �������ã�������ƥ���Ժ�洢��Rx FIFO0�� */
    fdcan_filterconfig.FilterID1 = 0x1123;                            /* ����ID1��11λ��׼ID */
    fdcan_filterconfig.FilterID2 = 0x0;                            /* ����ID2������Ϊ��ͳλ���ˣ�ID2��11λ����
                                                                      * �����ʾ���˽��պ�FilterID1��ȫһ������ϢID
                                                                      */

    if (HAL_FDCAN_ConfigFilter(&hfdcan2, &fdcan_filterconfig) != HAL_OK)
    {
        return ;
    }

    /* ����ȫ�ֹ��������������в�ƥ���֡ */
    HAL_FDCAN_ConfigGlobalFilter(&hfdcan2, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);

    /* ����FDCAN */
    if (HAL_FDCAN_Start(&hfdcan2) != HAL_OK)
    {
        return ;
    }

    /* ʹ�ܽ���FIFO 1����Ϣ�ж� */
    HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);


  /* USER CODE END FDCAN2_Init 2 */

}

static uint32_t HAL_RCC_FDCAN_CLK_ENABLED=0;
>>>>>>> d776bc83130b40cacfe172a11eb11aaef0ef179f

void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef* fdcanHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(fdcanHandle->Instance==FDCAN1)
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
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
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

void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef* fdcanHandle)
{

  if(fdcanHandle->Instance==FDCAN1)
  {
  /* USER CODE BEGIN FDCAN1_MspDeInit 0 */

  /* USER CODE END FDCAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_FDCAN_CLK_DISABLE();

    /**FDCAN1 GPIO Configuration
    PD0     ------> FDCAN1_RX
    PD1     ------> FDCAN1_TX
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0|GPIO_PIN_1);

    /* FDCAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(FDCAN1_IT0_IRQn);
    HAL_NVIC_DisableIRQ(FDCAN1_IT1_IRQn);
  /* USER CODE BEGIN FDCAN1_MspDeInit 1 */

  /* USER CODE END FDCAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/**
 * @brief       FDCAN ����һ������
 * @note        ���͸�ʽ�̶�Ϊ: ��׼ID, ����֡
 * @param       len     :���ݳ��ȣ�ȡֵ��Χ��FDCAN_DLC_BYTES_0 ~ FDCAN_DLC_BYTES_64
 * @param       msg     :����ָ�룬���Ϊ64���ֽ�
 * @retval      ����״̬ 0, �ɹ�; 1, ʧ��;
 */
uint8_t fdcan_send_msg(uint8_t *msg, uint32_t len)
{
    g_fdcanx_txheade.Identifier = 0x123;                             /* ֡ID */
    g_fdcanx_txheade.IdType = FDCAN_EXTENDED_ID;                     /* 11λ��׼ID */
    g_fdcanx_txheade.TxFrameType = FDCAN_DATA_FRAME;                 /* ��������֡ */
    g_fdcanx_txheade.DataLength = len;                               /* ���ݳ��� */
    g_fdcanx_txheade.ErrorStateIndicator = FDCAN_ESI_ACTIVE;         /* ����ڵ����Դ��� */
    g_fdcanx_txheade.BitRateSwitch = FDCAN_BRS_ON;                   /* ���������л� */
    g_fdcanx_txheade.FDFormat = FDCAN_CLASSIC_CAN;                        /* FDCAN֡��ʽ */
    g_fdcanx_txheade.TxEventFifoControl = FDCAN_NO_TX_EVENTS;        /* ���洢�����¼� */
    g_fdcanx_txheade.MessageMarker = 0;                              /* ��Ϣ���� */

    /* �����Ϣ������FIFO�������Ӧ�ķ������� */
    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &g_fdcanx_txheade, msg) != HAL_OK)  /* ������Ϣ */
    {
        return 1;
    }

    return 0;
		
	
}


// FDCAN���ͺ���
//FDCAN_StatusTypeDef FDCAN_SendMessage(uint8_t fc, uint16_t dest_did, uint16_t sub, uint8_t* data, uint8_t size) {
//  FDCAN_TxHeaderTypeDef TxHeader;
//  uint32_t id = BUILD_CAN_ID(fc, dest_did, sub);
//
//  TxHeader.Identifier = id;
//  TxHeader.IdType = FDCAN_EXTENDED_ID;
//  TxHeader.TxFrameType = FDCAN_DATA_FRAME;
//  TxHeader.DataLength = size << 16; // ת��ΪDLC
//  TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
//  TxHeader.BitRateSwitch = (fc == FC_DATA_TRANSFER) ? FDCAN_BRS_ON : FDCAN_BRS_OFF;
//  TxHeader.FDFormat = FDCAN_FD_CAN;
//  TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
//  TxHeader.MessageMarker = 0;
//
//  return HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, data);
//}

/**
 * @brief       FDCAN�������ݲ�ѯ
 * @param       buf:���ݻ�����
 * @retval      0, û�н��յ�����;
 *              ����, ���յ������ݳ���;
 */
uint8_t fdcan_receive_msg(uint8_t *buf)
{
    /* ��RX FIFO0�������յ���Ϣ */
    if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &g_fdcanx_rxheade, buf) != HAL_OK)  /* �������� */
    {
        return 0;
    }

    return g_fdcanx_rxheade.DataLength;
}

#if FDCAN1_RX0_INT_ENABLE           /* ʹ��FDCAN1 RX0�ж� */
/**
 * @brief       FDCAN1�ж���0�жϷ�����
 * @param       ��
 * @retval      ��;
 */
//void FDCAN1_IT0_IRQHandler(void)
//{
//    HAL_FDCAN_IRQHandler(&hfdcan1);
//}


/**
 * @brief       ����FIFO0�ص�����
 * @param       hfdcan:FDCAN���
 * @param       RxFifo0ITs:����FIFO 0�ж�״̬
 * @retval      ��;
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    uint8_t i = 0;
#if  0

    uint8_t rxdata[8];

    if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)     /* ����FIFO0����Ϣ�ж� */
    {
        /* ��ȡFIFO0�н��յ������� */
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
            if (RxHeader.IdType == FDCAN_EXTENDED_ID)//接收扩展帧
            {
                uint32_t id = RxHeader.Identifier;
                uint8_t fc = (id >> ID_FC_POS) & 0x0F;        // 功能码位置 [28:25]
                uint16_t did = (id >> ID_DID_POS) & 0x1FFF; //设备ID位置 [24:12]
                uint16_t sub = id & 0x0FFF; //// 子地址位置 [11:0]

                printf("\r\nDataLength= %d ", RxHeader.DataLength);

                // 处理大数据传输
                if ((fc == FC_DATA_TRANSFER) && (RxHeader.DataLength == sizeof(LargeDataFrame)))
                {
                    printf(" big_code \r\n");
                    LargeDataFrame *frame = (LargeDataFrame *)RxData;

                    if (frame->frame_type == ACK_FRAME)
                    {
                        // 发送端处理ACK
                        LargeDataTransfer_HandleACK(did, frame);
                    }
                    else
                    {
                        // 接收端处理数据帧
                        LargeDataTransfer_HandleFrame(did, frame);
                    }
                }
                else
                {
                    printf(" shortcode \r\n");

                    for (i = 0; i < 8; i++)
                    {
                        printf("rxdata[%d]:%d\r\n", i, RxData[i]);
                    }
                    // 其他功能码处理
                    ProcessCANMessage(fc, did, sub, RxData, RxHeader.DataLength);
                }
            }
        }
        HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
    }



#endif

}

#endif


uint8_t  g_CAN_TX_MSG[8] = {0xAA, 0x55, 0x66, 0X77, 0x88, 0x99, 0xCC, 0x11};

void CAN1_Send_TEST(void)
{

    fdcan_send_msg(g_CAN_TX_MSG, 8);

}


HAL_StatusTypeDef FDCAN_SendMessage(uint8_t fc, uint16_t dest_did, uint16_t sub, uint8_t* data, uint8_t size)
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
