

/**
 **************************************************************************************************
 * @file        UserSlave.c
 * @brief
 *
 * @description:

 *
 * @hardware:   STM32F750VBT6
 * @author      Hu Weiping
 * @date        2023-05-09
 * @version     V1.0
 **************************************************************************************************
 */

#include "head.h"
#include "UserSlave.h"

#define TIME_SLAVEDISCONNECT 700
#define TIME_SLAVEPULSE 1500
#define TIMEOUT_WAITACK 100 // 等待返回数据时间超时

enum ESlaveSampleStatus_t
{
    ESlaveSampleStatus_Empty = 0,     // 空
    ESlaveSampleStatus_WaitSlaveData, // 已经向分机请求数据，正在等待分机返回数据
    ESlaveSampleStatus_Ready,         // 准备好数据
};

#pragma pack(4)

typedef struct
{
    SampleData_t sampleData[MaxNumChannel];
    uint8_t status;
} SlaveSampleData_t;

typedef struct
{
    SlaveSampleData_t sample;

    StartWorkeChanne SetChanneWorke; // 设置启动通道

    uint32_t sampleErrorStatus; // sampleError中是否有保存截止条件到
    uint32_t sampleWaitAck;

    // 每个通道的启动工步时间
    SystemTime_t timeStartWorkStep[MaxNumChannel];

    uint32_t timeLastRec;

    uint32_t error;

    uint16_t channelWorkStepSet; // 各通道是否已经建立工艺信息

    uint8_t status;
    uint8_t isLinked;
    uint8_t nBytesChannelBitIndex;

} SlaveS_t;

// typedef struct
// {
//     SampleData_t sampleNotReady;                     // 还没有准备好的数据状态
//     SampleData_t sampleCanError;                     // 未连接
//     SystemTime_t timeStampSample[MaxNumSamplesFifo]; // 采样时间
//     uint32_t clockStampSlaveQuest;                   // 当前请求的分机的请求时间
//     uint8_t indexSlaveQuest;                         // 当前请求数据的下位机索引

//     uint8_t flashSize; // 当前已经缓存个数
//     uint8_t headFifo;  // 写位置
//     uint8_t tailFifo;  // 读位置
// } SampleManage_t;

struct
{
    uint8_t canFrameData[NumMaxTpCanData]; // canFrameSend的数据部分
                                           // SlaveS_t slave[SLAVENUM_MAX];          // 各分机数据
    SlaveS_t slave;                        // 各分机数据

    uint32_t timeLastSendLink;
    uint32_t logicVoltMax;
    uint32_t logicCurMax;
    uint32_t UpGradeDataLen;
    uint32_t UpGradeDataCheck;
    uint32_t error;
    uint16_t softVersion;
    uint16_t logicChannelsPerSlave; // 一个下位机的逻辑通道数
    uint16_t channelNumTotal;       // 总通道数，该值由上位机下发
    uint8_t numLink;
    uint8_t slaveNum;
} UserSlave_Data;
#pragma pack()

int UserSlave_Init(void)
{
}

void UserSlave_SendLink(void)
{
}
void UserSlave_UpdateSlaveRec(void)
{
#if 1
    uint8_t index;
    CanFrame_t *pFrame;
    SlaveS_t *pSlave;
    pFrame = CanFr_GetFrame();
    if (pFrame)
    {
        printf("\r\n addrSlave  =%d ", pFrame->addrSlave);
        printf("\r\n cmd_CAN  =0x%X ", pFrame->cmd);
        index = pFrame->addrSlave - 1;
        if (index > 16 && pFrame->addrSlave != 0xFF)
        {
            printf("\r\n中位机发送地址错误=%d ", index);
            return;
        }
        UserSlave_Data.slave.isLinked = 1;
        UserSlave_Data.slave.timeLastRec = Timer_GetClock();
        pSlave = &(UserSlave_Data.slave);
        switch (pFrame->cmd)
        {
        case EMTOSCMD_Link: // 连接命令

            CanFr_SendData(BoardInfo_GetID(), EMTOSCMD_Link, GetBorad_Device(), 8);
            printf("\r\n Link ID=%d", BoardInfo_GetID());
            break;
        case EMTOSCMD_Setpar: // 设置参数
            break;

        case EMTOSCMD_SampleStart: // 开始采样

            break;
        case EMTOSCMD_SampleQuest: // 请求采样数据
            break;

        case EMTOSCMD_SendWorkStepInfo: // 下发工步信息
                    printf("\r\n SendWorkStepInfo_dataLen=%d ", pFrame->dataLen);
            break;

        case EMTOSCMD_StartWorkStep: // 启动工步
            printf("\r\n EMTOSCMD_dataLen=%d ", pFrame->dataLen);
            memcpy(&(pSlave->SetChanneWorke), pFrame->data, pFrame->dataLen);

            printf("\r\n Run_Cyc_indx =%d ", pSlave->SetChanneWorke.Run_Cyc_indx);

            break;
        case EMTOSCMD_StopWorkStep: // 停止工步
            break;
        case EMTOSCMD_ContinueWorkStep: // 继续工步
            break;

        case EMTOSCMD_JumpWorkStep: // 工步跳转

            break;

        ///////////////////////////////////以下是修调命令
        case EMTOSCMDCALIBRATE_SAMPLE:
            printf("\r\n CAN Calibrate ");
            // UserCalibrate_SendSample(index, pFrame->data[8], U8TOU32(pFrame->data), U8TOU32((pFrame->data + 4)));
            break;

        ////////////////////////////////// 以下是辅助工具命令 升级下位机回复中上位机
        case EMTOSCMDIAP_UpGradeInfo:
            // UserHelpTool_UpGradeSlaveInfoAck(1, index, pFrame->data[0]);
            break;
        case EMTOSCMDIAP_UpGradeData:
            //  UserHelpTool_UpGradeSlaveDataAck(1, index, U8TOU32(pFrame->data), pFrame->data[4]);
            break;
        default:
            break;
        }
    }
#endif
}

void UserSlave_Update(void)
{
    //  if(Encrypt_IsPass()==FALSE)
    //  {
    //      UserSlave_Pause(-1,0xff);
    //  }
    UserSlave_UpdateSlaveRec();
    // UserSlave_UpdateLink();
    // UserSlave_UpdateSample();
}
