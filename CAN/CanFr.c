/********************************Copyright (c)**********************************\

**----------------------------------文件信息------------------------------------

** 文档描述: Can总线FrameWork部分的接口实现
**  1、发送短包，不需要建立连接，可以直接发送，发送长包必须先建立连接才能发送
**  2、在发送端和接收端之间建立长包连接后（接收端设置发送端和广播两个过滤器），接收端只接收来自发送端的长包或者短包，或者来自其它分机的广播包，其它分机发送的端对端的长包被拒绝，
**  3、发送端主动在任意时刻可以断开连接
**  4、主机地址为0，分机有8bit地址（1~254），255为广播地址
**  5、全部使用扩展帧，数据帧
**  6、过滤器使用32bitIDmask模式
**  7、定义，can底层发送的最下单位，这里定义为包，用户封装的最小单位定义为帧，这主要是为了与其它通信在用户层兼容
**----------------------------------版本信息------------------------------------
** 版本代号: V0.1
** 版本说明: 初始版本
**
**------------------------------------------------------------------------------
\********************************End of Head************************************/

// #include "string.h"
// #include "./BSP/FDCAN/fdcan.h"
// #include "./SYSTEM/usart/uart.h"
// #include "./SYSTEM/delay/delay.h"
// #include "./BSP/LED/led.h"
// #include "Timer.h"
// #include "BoardInfo.h"
#include "fdcan.h"
#include "CanFr.h"
// #include "Utility.h"

#include "head.h"

#define canUsed hcan
#define BUFFERSIZE_REC 64  // 接收缓存大小
#define BUFFERSIZE_SEND 64 // 发送缓存大小

#define TIMEOUT_SENDONEPACK 500 // 发送单包超时时间，ms

#define QueueHeadForward(Queue, Size)    \
    do                                   \
    {                                    \
        (Queue).head++;                  \
        if ((Queue).head >= Size)        \
            (Queue).head = 0;            \
        (Queue).size++;                  \
        if ((Queue).size >= Size)        \
        {                                \
            (Queue).size = Size;         \
            (Queue).tail = (Queue).head; \
        }                                \
    } while (0)

#define QueueTailForward(Queue, Size) \
    do                                \
    {                                 \
        (Queue).tail++;               \
        if ((Queue).tail >= Size)     \
            (Queue).tail = 0;         \
        (Queue).size--;               \
    } while (0)

#pragma pack(4)
typedef union
{
    // 按照32位访问
    uint32_t dWordData; // 字操作定义
    // 按照16bit访问
    uint16_t wordData[2];
    // 按照8bit访问
    uint8_t byteData[4];
    // 按照具体的项目访问
    struct
    {
        // uint32_t RESERVE : 1; // 1bit 0
        // uint32_t RTR : 1;     // 1bit RTR
        // uint32_t IDE : 1;     // 1bit IDE
        uint32_t EXID : 29; // 11位STID（扩展帧的高11位）18bit EXID（扩展帧的低18位）
    } canItem;
    // 按照用户定义访问
    struct
    {
        // uint32_t RESERVE : 1; // 1bit 0
        // uint32_t RTR : 1;     // 1bit RTR
        // uint32_t IDE : 1;     // 1bit IDE
        // 一下为EXID部分，发送顺序是MSB，仲裁时，0（显性电平）优先
        uint32_t addrSlave : 8; // 分机地址，主机的地址固定为0，分机地址如果是0xff,表示广播
        uint32_t cmd : 8;       // 命令
        uint32_t userPar : 12;  // 用户参数，可以作为数据索引使用
        uint32_t direct : 1;    // 方向，主机向分机发送为0，分机向主机发送为1
    } userItemp;
} PackHeader_t;

typedef struct
{
    uint16_t userPar;            // 用户的参数
    uint8_t addrSlave;           // 分机地址, 对于分机来说，不需要该变量，因为就是自己的地址
    uint8_t dataLen;             // 数据长度
    uint8_t cmd;                 // 命令
    uint8_t data[NumMaxCanData]; // 数据指针
} CanShortFrame_t;

typedef struct
{
    CanFrame_t frame;       // 长帧数据
    uint16_t checkValueRec; // 接收到的校验值
    uint16_t checkValueCal; // 计算的校验值
    uint16_t lenIndex;      // 当前已经接收或者发送的长度
    uint8_t status;         // 当前状态，0：空闲，1，正在接收，2，接收完成
} TpProcessData;

struct
{
    struct
    {
        CanShortFrame_t frameBuffer[BUFFERSIZE_REC]; // 接收帧缓存地址空间
        uint32_t timeLastRec;                        // 单包接收时间
        PackHeader_t packHeader;
        FDCAN_RxHeaderTypeDef rxHeader;
        TpProcessData tpProcess;         // 长包处理数据
        uint8_t tpData[NumMaxTpCanData]; // 长包的数据部分
        CanFrame_t shortFrame;           // 返回给用户的短包
        uint8_t spData[NumMaxCanData];   // 短包的数据部分
        uint16_t head;
        uint16_t tail;
        uint16_t size;
    } recStruct;
    struct
    {
        CanShortFrame_t frameBuffer[BUFFERSIZE_SEND]; // 发送包缓存地址空间
        PackHeader_t packHeader;
        FDCAN_TxHeaderTypeDef txHeader;
        uint32_t timeLastSend; // 单包发送时间
        uint16_t head;
        uint16_t tail;
        uint16_t size;
        uint16_t isSending; // 正在发送状态，这个主要用于上次发送失败的标记，下次再次尝试发送
    } sendStruct;
    uint8_t addrSelf; // 本机地址，主机的地址是0
} Can_Data;
#pragma pack()

int Can_SetFilter(uint8_t addrSlave, bool direct)
{
    // 配置接收过滤器

    FDCAN_FilterTypeDef fdcan_filterconfig;

    PackHeader_t receiveFileter;

    if (addrSlave > 16)
    {
        printf("Can_SetFilter addrSlave error\r\n");
        return -1;
    }

    // 清零过滤器
    receiveFileter.dWordData = 0;
    receiveFileter.userItemp.addrSlave = addrSlave;
    if (direct)
        receiveFileter.userItemp.direct = 1;
    // 只接收扩展的数据帧
    /* 过滤器配置 --滤波器索引 只接收扩展的数据帧 */
    fdcan_filterconfig.IdType = FDCAN_EXTENDED_ID;             /* 标准扩展ID */
    fdcan_filterconfig.FilterIndex = 0;                        //* 滤波器索引 */
    fdcan_filterconfig.FilterType = FDCAN_FILTER_MASK;         /* 滤波器类型：传统位过滤 */
    fdcan_filterconfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; /* 过滤配置：当过滤匹配以后存储在Rx FIFO0中 */
    fdcan_filterconfig.FilterID1 = receiveFileter.dWordData;   /* 过滤ID1：29位标准ID */
    fdcan_filterconfig.FilterID2 = 0x0000007FF;                /* 过滤ID2：配置为传统位过滤，ID2是29位掩码
                                                                *   这里表示过滤接收和FilterID1完全一样的消息ID
                                                                */

    if (HAL_FDCAN_ConfigFilter(&hfdcan1, &fdcan_filterconfig) != HAL_OK)
    {
        printf("Can_SetFilter addrSlave error=%d\r\n", fdcan_filterconfig.FilterIndex);
        return -1;
    }

    // 只接收广播消息--0xFF
    /* 过滤器配置 */
    fdcan_filterconfig.IdType = FDCAN_EXTENDED_ID;             /* 标准ID */
    fdcan_filterconfig.FilterIndex = 1;                        //* 滤波器索引 */
    fdcan_filterconfig.FilterType = FDCAN_FILTER_MASK;         /* 滤波器类型：传统位过滤 */
    fdcan_filterconfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; /* 过滤配置：当过滤匹配以后存储在Rx FIFO0中 */
    fdcan_filterconfig.FilterID1 = 0x00000000FF;               /* 过滤ID1：29位标准ID 配置广播ID:0xFFF*/
    fdcan_filterconfig.FilterID2 = 0x00000000FF;               /* 过滤ID2：配置为传统位过滤，ID2是29位掩码
                                                                *   这里表示过滤接收和FilterID1完全一样的消息ID
                                                                */

    if (HAL_FDCAN_ConfigFilter(&hfdcan1, &fdcan_filterconfig) != HAL_OK)
    {
        printf("Can_SetFilter addrSlave error=%d\r\n", fdcan_filterconfig.FilterIndex);
        return -1;
    }

    return 0;
}

int CanFr_Init(void)
{

    memset(&Can_Data, 0, sizeof(Can_Data));

    Can_Data.recStruct.shortFrame.data = Can_Data.recStruct.spData;

#if CANMASTER
    Can_Data.addrSelf = 0; // 设置本机地址为0，表示主机
    // 主机只接收来自分机的信息，分机地址不过滤
    Can_SetFilter(0, 1);
#else
    Can_Data.addrSelf = BoardInfo_GetID();
    // 分机只接收来自主机的广播信息和只发送给本机的信息
    Can_SetFilter(Can_Data.addrSelf, 0);

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
// 发送数据更新
void Send_Update(void)
{
    uint32_t TxMailbox;
    CanShortFrame_t *pFrame;
    FDCAN_TxHeaderTypeDef *pHeader;
    PackHeader_t *pPack;

#if 1
    if (Can_Data.sendStruct.size > 0)
    {
        // 发送箱有空余
        //        if (HAL_CAN_GetTxMailboxesFreeLevel(&hfdcan1) > 0)
        if (HAL_FDCAN_GetLatestTxFifoQRequestBuffer(&hfdcan1) > 0)
        {
            pFrame = &(Can_Data.sendStruct.frameBuffer[Can_Data.sendStruct.tail]);
            // 上次发送失败
            if (Can_Data.sendStruct.isSending)
            {
                // 如果还是发送失败
                //   if (HAL_CAN_AddTxMessage(&hfdcan1, &(Can_Data.sendStruct.txHeader), pFrame->data, &TxMailbox) != HAL_OK)
                /* 添加消息到发送FIFO并添加相应的发送请求 */
                if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &Can_Data.sendStruct.txHeader, pFrame->data) != HAL_OK) /* 发送消息 */
                {
                    // 如果没有超时继续等待
                    if (Timer_GetClock() - Can_Data.sendStruct.timeLastSend < TIMEOUT_SENDONEPACK)
                    {
                        return;
                    }
                }
                QueueTailForward(Can_Data.sendStruct, BUFFERSIZE_SEND);
                Can_Data.sendStruct.isSending = 0;
            }
            while (Can_Data.sendStruct.size > 0)
            {
                pFrame = &(Can_Data.sendStruct.frameBuffer[Can_Data.sendStruct.tail]);
                pHeader = &(Can_Data.sendStruct.txHeader);
                pPack = &(Can_Data.sendStruct.packHeader);

                pPack->dWordData = 0;
#if CANMASTER
                pPack->userItemp.addrSlave = pFrame->addrSlave;
                pPack->userItemp.direct = 0;
#else
                pPack->userItemp.addrSlave = Can_Data.addrSelf;
                pPack->userItemp.direct = 1;
#endif
                pPack->userItemp.cmd = pFrame->cmd;
                pPack->userItemp.userPar = pFrame->userPar;
                // pHeader->IdType = FDCAN_EXTENDED_ID;  //扩展帧
                // pHeader->TxFrameType = FDCAN_DATA_FRAME;   //数据帧
                // pHeader->Identifier = pPack->canItem.EXID;//标识符
                if (pFrame->dataLen > 8)
                    pFrame->dataLen = 8;

                //  pHeader->DataLength = pFrame->dataLen;

                pHeader->Identifier = pPack->canItem.EXID;        /* 帧ID 标识符*/
                pHeader->IdType = FDCAN_EXTENDED_ID;              /* 29位标准ID */
                pHeader->TxFrameType = FDCAN_DATA_FRAME;          /* 发送数据帧 */
                pHeader->DataLength = pFrame->dataLen;            /* 数据长度 */
                pHeader->ErrorStateIndicator = FDCAN_ESI_ACTIVE;  /* 传输节点显性错误 */
                pHeader->BitRateSwitch = FDCAN_BRS_OFF;           /* 关闭速率切换 */
                pHeader->FDFormat = FDCAN_CLASSIC_CAN;            /* 标准帧格式 */
                pHeader->TxEventFifoControl = FDCAN_NO_TX_EVENTS; /* 不存储发送事件 */
                pHeader->MessageMarker = 0;                       /* 消息掩码 */

                Can_Data.sendStruct.timeLastSend = Timer_GetClock();
                //     if (HAL_CAN_AddTxMessage(&hfdcan1, pHeader, pFrame->data, &TxMailbox) != HAL_OK)
                if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, pHeader, pFrame->data) != HAL_OK)
                {
                    Can_Data.sendStruct.isSending = 1;
                    return;
                }
                else
                {
                    QueueTailForward(Can_Data.sendStruct, BUFFERSIZE_SEND);
                    Can_Data.sendStruct.isSending = 0;
                }
            }
        }
    }
#endif
}


// 用户实现示例（在自定义文件中）
void HAL_FDCAN_TxFifoEmptyCallback(FDCAN_HandleTypeDef *hfdcan)
{
    // 1. 填充新数据到发送队列
    // 2. 重启数据传输
    // 3. 更新发送状态标志

    if (hfdcan->Instance == FDCAN1)
    {

        Send_Update();
    }
}

void HAL_FDCAN_TxBufferCompleteCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndexes)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(hfdcan);
    UNUSED(BufferIndexes);
    if (hfdcan->Instance == FDCAN1)
    {

        Send_Update();
    }
    /* NOTE: This function Should not be modified, when the callback is needed,
              the HAL_FDCAN_TxBufferCompleteCallback could be implemented in the user file
     */
}
#if 1

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    uint8_t i = 0;

    CanShortFrame_t *pFrame;
    FDCAN_RxHeaderTypeDef *pHeader; // CAN接收数据结构体
    PackHeader_t *pPack;

    if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != 0)
    {
        FDCAN_RxHeaderTypeDef RxHeader;
        uint8_t RxData[64];

        if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET) /* 接收FIFO0新消息中断 */
        {

            if (hfdcan->Instance == FDCAN1)
            {
                pFrame = &(Can_Data.recStruct.frameBuffer[Can_Data.recStruct.head]);

                pHeader = &(Can_Data.recStruct.rxHeader);
                pPack = &(Can_Data.recStruct.packHeader);

                /* 提取FIFO0中接收到的数据 */
                if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &(Can_Data.recStruct.rxHeader), pFrame->data) == HAL_OK)
                {

                    // printf("id:%#x\r\n", pHeader->Identifier);
                    // printf("len:%d\r\n", pHeader->DataLength);
                    // for (i = 0; i < 8; i++)
                    // {
                    //     printf("rxdata[%d]:0x%X\r\n", i, pFrame->data[i]);
                    // }

                    pPack->canItem.EXID = pHeader->Identifier; // 扩展帧还是标准帧
                    pFrame->dataLen = pHeader->DataLength;
                    pFrame->cmd = pPack->userItemp.cmd;
                    pFrame->addrSlave = pPack->userItemp.addrSlave;
                    pFrame->userPar = pPack->userItemp.userPar;

                    //  printf("\r\nuserPar 0x%x  addSlave=%d ",pFrame->userPar,pPack->userItemp.addrSlave);
                    // 队列头向前移动
                    QueueHeadForward(Can_Data.recStruct, BUFFERSIZE_REC);

                    // fdcan3_send_msg((uint8_t *)pFrame->data, 8);

                    HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
                }
            }
        }

        HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
    }
}

#else

/**
 * @brief       接收FIFO1回调函数
 * @param       hfdcan:FDCAN句柄
 * @param       RxFifo0ITs:接收FIFO 0中断状态
 * @retval      无;
 */
void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs)
{
    uint8_t i = 0;
    //    uint8_t rxdata[8];

    CanShortFrame_t *pFrame;
    FDCAN_RxHeaderTypeDef *pHeader; // CAN接收数据结构体
    PackHeader_t *pPack;

    //    if (hfdcan->Instance != FDCAN3)
    //        return;

    if ((RxFifo1ITs & FDCAN_IT_RX_FIFO1_NEW_MESSAGE) != RESET) /* 接收FIFO0新消息中断 */
    {
        if (hfdcan->Instance == FDCAN1)
        {
            pFrame = &(Can_Data.recStruct.frameBuffer[Can_Data.recStruct.head]);

            pHeader = &(Can_Data.recStruct.rxHeader);
            pPack = &(Can_Data.recStruct.packHeader);

            /* 提取FIFO0中接收到的数据 */
            if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO1, &(Can_Data.recStruct.rxHeader), pFrame->data) == HAL_OK)
            {

                printf("id:%#x\r\n", pHeader->Identifier);
                printf("len:%d\r\n", pHeader->DataLength);
                for (i = 0; i < 8; i++)
                {
                    printf("rxdata[%d]:0x%X\r\n", i, pFrame->data[i]);
                }

                pPack->canItem.EXID = pHeader->Identifier; // 扩展帧还是标准帧
                pFrame->dataLen = pHeader->DataLength;
                pFrame->cmd = pPack->userItemp.cmd;
                pFrame->addrSlave = pPack->userItemp.addrSlave;
                pFrame->userPar = pPack->userItemp.userPar;

                //  printf("\r\nuserPar 0x%x  addSlave=%d ",pFrame->userPar,pPack->userItemp.addrSlave);
                // 队列头向前移动
                QueueHeadForward(Can_Data.recStruct, BUFFERSIZE_REC);

                // fdcan3_send_msg((uint8_t *)pFrame->data, 8);

                HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0);
            }
        }
    }
}

#endif
CanFrame_t *CanFr_GetFrame(void)
{
    CanShortFrame_t *pFrame;
    while (Can_Data.recStruct.size > 0)
    {
        // printf("\r\n CAN_Size=%d ",Can_Data.recStruct.size );
        pFrame = &(Can_Data.recStruct.frameBuffer[Can_Data.recStruct.tail]);
        // return &(Can_Data.recStruct.frameBuffer[Can_Data.recStruct.tail]);
        // 收到的是短包，直接将数据放在缓存中然后返回
        if (Can_Data.recStruct.frameBuffer[Can_Data.recStruct.tail].userPar == CanFrtameType_Short)
        {
            Can_Data.recStruct.shortFrame.addrSlave = pFrame->addrSlave;
            Can_Data.recStruct.shortFrame.cmd = pFrame->cmd;
            Can_Data.recStruct.shortFrame.dataLen = pFrame->dataLen;
            for (int i = 0; i < pFrame->dataLen; i++)
            {
                Can_Data.recStruct.shortFrame.data[i] = pFrame->data[i];
            }
            //          HAL_NVIC_DisableIRQ(FDCAN3_IT0_IRQn);
            // 4. 临时关闭接收中断
            HAL_FDCAN_DeactivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO1_NEW_MESSAGE);
            QueueTailForward(Can_Data.recStruct, BUFFERSIZE_REC);
            //          HAL_NVIC_EnableIRQ(FDCAN3_IT0_IRQn);
            // 5. 重新启用接收中断
            HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0);
            return &(Can_Data.recStruct.shortFrame);
        }
        // 如果接收到的是长包首帧，为长包的信息帧
        else if (Can_Data.recStruct.frameBuffer[Can_Data.recStruct.tail].userPar == CanFrtameType_TpStart)
        {
            Can_Data.recStruct.tpProcess.frame.cmd = pFrame->cmd;
            Can_Data.recStruct.tpProcess.frame.addrSlave = pFrame->addrSlave;
            Can_Data.recStruct.tpProcess.frame.dataLen = U8TOU16(pFrame->data);
            Can_Data.recStruct.tpProcess.checkValueRec = U8TOU16((pFrame->data + 2));
            Can_Data.recStruct.tpProcess.checkValueCal = 0;
            Can_Data.recStruct.tpProcess.lenIndex = 0;
            for (int i = 4; i < pFrame->dataLen; i++)
            {
                // 拷贝数据到接收位置
                Can_Data.recStruct.tpProcess.frame.data[Can_Data.recStruct.tpProcess.lenIndex] = pFrame->data[i];
                // 计算校验值
                Can_Data.recStruct.tpProcess.checkValueCal += pFrame->data[i] ^ (Can_Data.recStruct.tpProcess.lenIndex);
                Can_Data.recStruct.tpProcess.lenIndex++;
            }
            Can_Data.recStruct.tpProcess.status = 1;
            goto forwardAndContinue;
        }
        // 接收到的是长包中间帧
        else if (Can_Data.recStruct.frameBuffer[Can_Data.recStruct.tail].userPar < CanFrtameType_Short)
        {
            if (Can_Data.recStruct.tpProcess.status != 1)
            {
                Can_Data.recStruct.tpProcess.status = 0;
                goto forwardAndContinue;
            }
            if (Can_Data.recStruct.tpProcess.frame.cmd != pFrame->cmd)
            {
                Can_Data.recStruct.tpProcess.status = 0;
                goto forwardAndContinue;
            }
            if (Can_Data.recStruct.tpProcess.frame.addrSlave != pFrame->addrSlave)
            {
                Can_Data.recStruct.tpProcess.status = 0;
                goto forwardAndContinue;
            }
            if ((pFrame->userPar + pFrame->dataLen) > Can_Data.recStruct.tpProcess.frame.dataLen)
            {
                Can_Data.recStruct.tpProcess.status = 0;
                goto forwardAndContinue;
            }
            if (Can_Data.recStruct.tpProcess.lenIndex != pFrame->userPar)
            {
                Can_Data.recStruct.tpProcess.status = 0;
                ;
            }
            for (int i = 0; i < pFrame->dataLen; i++)
            {
                // 拷贝数据到接收位置
                Can_Data.recStruct.tpProcess.frame.data[Can_Data.recStruct.tpProcess.lenIndex] = pFrame->data[i];
                // 计算校验值
                Can_Data.recStruct.tpProcess.checkValueCal += pFrame->data[i] ^ (Can_Data.recStruct.tpProcess.lenIndex);
                Can_Data.recStruct.tpProcess.lenIndex++;
            }
            goto forwardAndContinue;
        }
        // 长包尾帧
        else if (Can_Data.recStruct.frameBuffer[Can_Data.recStruct.tail].userPar == CanFrtameType_TpEnd)
        {
            if ((Can_Data.recStruct.tpProcess.lenIndex + pFrame->dataLen) != Can_Data.recStruct.tpProcess.frame.dataLen)
            {
                Can_Data.recStruct.tpProcess.status = 0;
                goto forwardAndContinue;
            }
            for (int i = 0; i < pFrame->dataLen; i++)
            {
                // 拷贝数据到接收位置
                Can_Data.recStruct.tpProcess.frame.data[Can_Data.recStruct.tpProcess.lenIndex] = pFrame->data[i];
                // 计算校验值
                Can_Data.recStruct.tpProcess.checkValueCal += pFrame->data[i] ^ (Can_Data.recStruct.tpProcess.lenIndex);
                Can_Data.recStruct.tpProcess.lenIndex++;
            }
            if (Can_Data.recStruct.tpProcess.checkValueCal != Can_Data.recStruct.tpProcess.checkValueRec)
            {
                Can_Data.recStruct.tpProcess.status = 0;
                goto forwardAndContinue;
            }
            else
            {
                Can_Data.recStruct.tpProcess.status = 2;
                // HAL_NVIC_DisableIRQ(FDCAN3_IT0_IRQn);
                HAL_FDCAN_DeactivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO1_NEW_MESSAGE);
                QueueTailForward(Can_Data.recStruct, BUFFERSIZE_REC);
                //              HAL_NVIC_EnableIRQ(FDCAN3_IT0_IRQn);
                // 5. 重新启用接收中断
                HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0);
                return &(Can_Data.recStruct.tpProcess.frame);
            }
        }
    forwardAndContinue:
        // HAL_NVIC_DisableIRQ(FDCAN3_IT0_IRQn);
        HAL_FDCAN_DeactivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO1_NEW_MESSAGE);
        QueueTailForward(Can_Data.recStruct, BUFFERSIZE_REC);
        //      HAL_NVIC_EnableIRQ(FDCAN3_IT0_IRQn);
        // 5. 重新启用接收中断
        HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0);
    }
    return NULL;
}

int CanFr_SendData(uint8_t addrSlave, uint8_t cmd, uint8_t *data, uint16_t dataLen)
{
    CanShortFrame_t *pStFrame;
    uint16_t checkValue = 0;
    uint16_t lenIndexSend = 0;

    uint32_t timeStart = Timer_GetClock();

    // 短包
    if (dataLen <= NumMaxCanData)
    {
        // 发送队列满，等待
        while (Can_Data.sendStruct.size >= BUFFERSIZE_SEND)
        {
            // 超时直接退出
            if (Timer_GetClock() - timeStart > TIMEOUT_SENDONEPACK)
                return -1;
        }

        //  HAL_NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);

        HAL_FDCAN_DeactivateNotification(&hfdcan1, FDCAN_IT_TX_COMPLETE);

        pStFrame = &(Can_Data.sendStruct.frameBuffer[Can_Data.sendStruct.head]);
        pStFrame->addrSlave = addrSlave;
        pStFrame->cmd = cmd;
        pStFrame->dataLen = dataLen;
        pStFrame->userPar = CanFrtameType_Short;
        for (int i = 0; i < dataLen; i++)
        {
            pStFrame->data[i] = data[i];
        }
        QueueHeadForward(Can_Data.sendStruct, BUFFERSIZE_SEND);
        Send_Update();
        //      HAL_NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);

        HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_TX_COMPLETE, 0);
    }
    else
    {
        while (lenIndexSend < dataLen)
        {
            // 发送队列满，等待
            while (Can_Data.sendStruct.size >= BUFFERSIZE_SEND)
            {
                // 超时直接退出
                if (Timer_GetClock() - timeStart > TIMEOUT_SENDONEPACK)
                    return -1;
            }
            if (lenIndexSend == 0)
            {
                for (int i = 0; i < dataLen; i++)
                {
                    checkValue += data[i] ^ i;
                }
                //  HAL_NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);
                HAL_FDCAN_DeactivateNotification(&hfdcan1, FDCAN_IT_TX_COMPLETE);
                pStFrame = &(Can_Data.sendStruct.frameBuffer[Can_Data.sendStruct.head]);
                pStFrame->addrSlave = addrSlave;
                pStFrame->cmd = cmd;
                pStFrame->dataLen = 8;
                pStFrame->userPar = CanFrtameType_TpStart;
                U16TOU8(dataLen, (pStFrame->data));
                U16TOU8(checkValue, (pStFrame->data + 2));
                for (int i = 4; i < 8; i++)
                {
                    pStFrame->data[i] = data[lenIndexSend++];
                }
                QueueHeadForward(Can_Data.sendStruct, BUFFERSIZE_SEND);
                Send_Update();
                // HAL_NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
                HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_TX_COMPLETE, 0);
            }
            else if (lenIndexSend + 8 < dataLen)
            {
                HAL_FDCAN_DeactivateNotification(&hfdcan1, FDCAN_IT_TX_COMPLETE);
                //  HAL_NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);
                pStFrame = &(Can_Data.sendStruct.frameBuffer[Can_Data.sendStruct.head]);
                pStFrame->addrSlave = addrSlave;
                pStFrame->cmd = cmd;
                pStFrame->dataLen = 8;
                pStFrame->userPar = lenIndexSend;
                for (int i = 0; i < pStFrame->dataLen; i++)
                {
                    pStFrame->data[i] = data[lenIndexSend++];
                }
                QueueHeadForward(Can_Data.sendStruct, BUFFERSIZE_SEND);
                Send_Update();
                // HAL_NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
                HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_TX_COMPLETE, 0);
            }
            else
            {
                // HAL_NVIC_DisableIRQ(USB_HP_CAN1_TX_IRQn);
                HAL_FDCAN_DeactivateNotification(&hfdcan1, FDCAN_IT_TX_COMPLETE);
                pStFrame = &(Can_Data.sendStruct.frameBuffer[Can_Data.sendStruct.head]);
                pStFrame->addrSlave = addrSlave;
                pStFrame->cmd = cmd;
                pStFrame->dataLen = dataLen - lenIndexSend;
                pStFrame->userPar = CanFrtameType_TpEnd;
                for (int i = 0; i < pStFrame->dataLen; i++)
                {
                    pStFrame->data[i] = data[lenIndexSend++];
                }
                QueueHeadForward(Can_Data.sendStruct, BUFFERSIZE_SEND);
                Send_Update();
                //  HAL_NVIC_EnableIRQ(USB_HP_CAN1_TX_IRQn);
                HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_TX_COMPLETE, 0);
            }
        }
    }

    return 0;
}

int CanFr_SendFrame(CanFrame_t *pFrame)
{
    return CanFr_SendData(pFrame->addrSlave, pFrame->cmd, pFrame->data, pFrame->dataLen);
}
/********************************End of File************************************/
