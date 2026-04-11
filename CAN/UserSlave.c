

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

WorkStepInfoStream_t g_WorkStepInfoStream[BOARD_CHANNEL_NUM][MAX_SETUP_WORKE]; // 工步信息最大32个

StartWorkeChanne g_SetChanneWorke; // 设置启动通道

uint8_t canFrameData[NumMaxTpCanData]; // canFrameSend的数据部分

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

    uint32_t sampleErrorStatus; // sampleError中是否有保存截止条件到
    uint8_t Total_steps;        // 总工步数

    uint32_t timeLastRec;

    uint32_t error;

    uint16_t channelWorkStepSet; // 各通道是否已经建立工艺信息

    uint8_t status;
    uint8_t isLinked;
    uint8_t nBytesChannelBitIndex;

} SlaveS_t;

struct
{

    SlaveS_t slave; // 各分机数据

    uint32_t timeLastSendLink;
    uint32_t logicVoltMax;
    uint32_t logicCurMax;
    uint32_t UpGradeDataLen;
    uint32_t UpGradeDataCheck;
    uint32_t error;
    uint16_t softVersion;

    uint8_t numLink;
    uint8_t slaveNum;
} UserSlave_Data;
#pragma pack()

// 下位机数据初始化
int UserSlave_Init(void)
{
    memset(&UserSlave_Data, 0, sizeof(UserSlave_Data));
    for (uint16_t i = 0; i < 16; i++)
    {
        memset(&g_WorkStepInfoStream[i], 0, sizeof(WorkStepInfoStream_t));
    }

    memset(&g_SetChanneWorke.chnum[0], 0xFF, 32);
    g_SetChanneWorke.Run_Cyc_indx = 1;
    g_SetChanneWorke.runWorke_setup = 0xFF;

    // 初始化工步信息
    for (uint16_t ch = 0; ch < 16; ch++)
    {
        g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].currentStart = 1.0f;
        g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].voltLimit = 4.2f;

        g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].timeLimit = 160000;
        g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].currentLimit = 0.0010f;
        g_Channelinfo[ch].WorkeStartup = 1;

        // g_Channelinfo[ch].RunningWorkSetup.type = WORKE_SETUP_CC_CV;
        g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].type = WORKE_SETUP_DC;
    }

    Init_RunningWorkSetup();
}

void Init_RunningWorkSetup(void)
{

    // 初始化工步信息
    for (uint16_t ch = 0; ch < 16; ch++)
    {
        g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].currentStart = 1.0f;
        g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].voltLimit = 4.2f;

        g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].timeLimit = 160000;
        g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].currentLimit = 0.0010f;

        g_Channelinfo[ch].WorkeStartup = 0xFF; // 运行的工步号 无操作时= 0xff。
        // 工步号+循环号同时为0xff时，表示无效数据
        g_Channelinfo[ch].loopSn = 0xFF; // 运行的循环号 无操作时=0xff,起始循环号为1。

        g_Channelinfo[ch].status = 0x53; // 无操作时=0x53
        g_Channelinfo[ch].error = 0;     // 错误

        // g_Channelinfo[ch].RunningWorkSetup.type = WORKE_SETUP_CC_CV;
        g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].type = WORKE_SETUP_IDLE;
    }
}

void UserSlave_SendLink(void)
{
}

uint8_t GetTotal_steps(void)
{

    return UserSlave_Data.slave.Total_steps;
}

void UserSlave_UpdateSlaveRec(void)
{
#if 1
    uint32_t tempdata;
    uint16_t tmep[2];
    uint16_t SetCh_Activity = 0; // 设置有效的通道工步参数

    uint8_t devid = 1, loopSn;
    uint8_t index, setindex, ch;
    CanFrame_t *pFrame;
    SlaveS_t *pSlave;
    pFrame = CanFr_GetFrame();
    if (pFrame)
    {
        // printf("\r\n addrSlave  =%d ", pFrame->addrSlave);
        // printf("\r\n cmd_CAN  =0x%X ", pFrame->cmd);
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
            // printf("\r\n Link ID=%d", BoardInfo_GetID());
            break;
        case EMTOSCMD_Setpar: // 设置参数

            break;

        case EMTOSCMD_SampleStart: // 开始采样

            break;
        case EMTOSCMD_SampleQuest: // 请求采样数据
            index = 0;
            for (ch = 0; ch < BOARD_CHANNEL_NUM; ch++) // 获取通道数据
            {
                // 电流
                tempdata = (uint32_t)(g_Channelinfo[ch].current * 10000.0f);
                tempdata = 15000;
                index += AppUser_uint32_CharTo_Samll(tempdata, &canFrameData[index]);
                // 电压
                g_Channelinfo[ch].voltage = 3.5164f;
                index += AppUser_uint16_CharTo_Samll((uint16_t)(g_Channelinfo[ch].voltage * 10000.0f), &canFrameData[index]);
                // 温度
                index += AppUser_uint16_CharTo_Samll(250, &canFrameData[index]);
                // 工步索引号 运行的工步号step
                // canFrameData[index++] = 0xFF; // 工步索引号
                // // // 通道状态

                // canFrameData[index++] = 0x53; // 通道的工作在哪个工步中  g_Channelinfo[ch].status
                // // // 错误状态
                //  canFrameData[index++] = 0;//
                // // // 当前运行工步循环号
                //  canFrameData[index++] = 0;//

                canFrameData[index++] = g_Channelinfo[ch].WorkeStartup; // 工步索引号
                // 通道状态
                canFrameData[index++] = g_Channelinfo[ch].status; // 通道的工作在哪个工步中类型
                // 错误状态
                canFrameData[index++] = g_Channelinfo[ch].error; //
                // 当前运行工步循环号
                canFrameData[index++] = g_Channelinfo[ch].loopSn; //
            }

            CanFr_SendData(BoardInfo_GetID(), EMTOSCMD_SampleQuest, canFrameData, index);

            printf("\r\n 请求采样数据 =%d ", index);

            break;

        case EMTOSCMD_SendWorkStepInfo: // 下发工步信息
            printf("\r\n 下发工步信息 _dataLen=%d ", pFrame->dataLen);
            devid = BoardInfo_GetID();
            pSlave->Total_steps = pFrame->data[32]; // 总工步数
            printf("\r\n 总工步数 =%d  %d", pSlave->Total_steps, pFrame->data[33]);
            if (pSlave->Total_steps > MAX_SETUP_WORKE)
            {
                printf("\r\n Total_steps ERR!!!=%d ", pSlave->Total_steps);
                return;
            }
            //  g_Channelinfo[ch].SendWorkStepIndx = pFrame->data[33]; // 工步索引号 工步索引号，从0工步开始

            tmep[0] = pFrame->data[(devid - 1) * 2];
            tmep[1] = pFrame->data[(devid - 1) * 2 + 1];
            SetCh_Activity = tmep[0] | (tmep[1] << 8);
            printf("\r\n 设置启动通道值=0x %X ", SetCh_Activity);

            for (ch = 0; ch < BOARD_CHANNEL_NUM; ch++)
            {
                for (setindex = 0; setindex < pSlave->Total_steps; setindex++) // 判断哪个通道被激活 通道工步数据// 每个通道工步数量
                {

                    if (SetCh_Activity >> ch & 0x0001)
                    {

                        memcpy(&(g_WorkStepInfoStream[ch][setindex]), pFrame->data + 33 + 16 * setindex, 16);

                        //  printf("\r\n ch=%d 设置工作类型=0x%X ", ch + 1, g_WorkStepInfoStream[ch][setindex].type);

                        // 设置工作启动电流
                        tempdata = U8TOU32(g_WorkStepInfoStream[ch][setindex].currentStart);
                        g_Channelinfo[ch].RunningWorkSetup[setindex].currentStart = (float)tempdata;
                        g_Channelinfo[ch].RunningWorkSetup[setindex].currentStart = g_Channelinfo[ch].RunningWorkSetup[setindex].currentStart * 0.0001f; // 10000mA=10.0A
                                                                                                                                     //  printf("\r\nch=%d 启动电流=%f A ", ch + 1, g_Channelinfo[ch].RunningWorkSetup.currentStart );

                        // 设置截止电压
                        tempdata = U8TOU16(g_WorkStepInfoStream[ch][setindex].voltLimit);
                        g_Channelinfo[ch].RunningWorkSetup[setindex].voltLimit = (float)tempdata;
                        g_Channelinfo[ch].RunningWorkSetup[setindex].voltLimit = g_Channelinfo[ch].RunningWorkSetup[setindex].voltLimit * 0.0001f; // 1500mV=1.5V
                                                                                                                               // printf("\r\nch=%d 截止电流=%fA ", ch + 1, g_Channelinfo[ch].RunningWorkSetup.voltLimit);
                                                                                                                               //  printf("\r\n ch=%d 设置截止电压 =%fV ", ch + 1, g_Channelinfo[ch].RunningWorkSetup.voltLimit );
                        // 设置工作截止电流
                        tempdata = U8TOU32(g_WorkStepInfoStream[ch][setindex].currentLimit);
                        g_Channelinfo[ch].RunningWorkSetup[setindex].currentLimit = (float)tempdata;
                        g_Channelinfo[ch].RunningWorkSetup[setindex].currentLimit = g_Channelinfo[ch].RunningWorkSetup[setindex].currentLimit * 0.0001f; // 10000mA=10.0A
                                                                                                                                     // printf("\r\nch=%d 设置工作截止电流 =%fA ", ch + 1, g_Channelinfo[ch].RunningWorkSetup.currentLimit);
                        // 设置工作截止时间
                        tempdata = U8TOU32(g_WorkStepInfoStream[ch][setindex].timeLimit);
                        g_Channelinfo[ch].RunningWorkSetup[setindex].timeLimit = tempdata;
                        // printf("\r\n ch=%d 设置工作截止时间=%d ", ch + 1, g_Channelinfo[ch].RunningWorkSetup.timeLimit);
                    }
                    printf("\r\n setindex=%d 启动电流=%fA ", setindex, g_Channelinfo[0].RunningWorkSetup[setindex].currentStart);
                    printf("\r\n setindex=%d 截止电压=%fV ", setindex, g_Channelinfo[0].RunningWorkSetup[setindex].voltLimit);
                    printf("\r\n setindex=%d 设置工作截止电流 =%fA ", setindex, g_Channelinfo[0].RunningWorkSetup[setindex].currentLimit);
                    printf("\r\n setindex=%d 设置工作截止时间=%d ", setindex, g_Channelinfo[0].RunningWorkSetup[setindex].timeLimit);

                }
            }



            break;

        case EMTOSCMD_StartWorkStep: // 启动工步
            printf("\r\n 启动工步数据长度=%d ", pFrame->dataLen);
            memcpy(&(g_SetChanneWorke), pFrame->data, pFrame->dataLen);

            devid = BoardInfo_GetID();

            printf("\r\n 运行工步号 =%d ", g_SetChanneWorke.runWorke_setup);
            if (g_SetChanneWorke.runWorke_setup > MAX_SETUP_WORKE)
            {
                printf("\r\n runWorke_setup=%d ", g_SetChanneWorke.runWorke_setup);
                return;
            }
            g_SetChanneWorke.Run_Cyc_indx = pFrame->data[33]; // 运行循环号
            printf("\r\n 运行循环号 默认是 1 当前设置=%d ", g_SetChanneWorke.Run_Cyc_indx);

            if (g_SetChanneWorke.runWorke_setup > MAX_SETUP_WORKE)
            {
                printf("\r\n Run_Cyc_indxx=%d ", g_SetChanneWorke.Run_Cyc_indx);
                return;
            }
            for (ch = 0; ch < BOARD_CHANNEL_NUM; ch++) //
            {
                g_Channelinfo[ch].loopSn = 0; //  初始化工步循环号 循环号，表示工艺流程，重复执行几次
            }

            tmep[0] = pFrame->data[(devid - 1) * 2];
            tmep[1] = pFrame->data[(devid - 1) * 2 + 1];
            SetCh_Activity = tmep[0] | (tmep[1] << 8);
            printf("\r\n SetCh_Activity=0x%X ", SetCh_Activity);

            for (ch = 0; ch < BOARD_CHANNEL_NUM; ch++) // 判断哪个通道被激活 通道工步数据
            {
                if ((SetCh_Activity >> ch) & 0x0001)
                {

                   // g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].index = g_SetChanneWorke.runWorke_setup; // 运行工步号
                    // printf("\r\n ch=%d ", ch);
                    g_Channelinfo[ch].WorkeStartup = 1; // 启动工步
                    g_Channelinfo[ch].fault.all = 0;    // 清除故障
                    ch++;
                }
                else
                {
                    g_Channelinfo[ch].WorkeStartup = 0; // 停止工步
                    g_Channelinfo[ch].fault.all = 1;
                }
            }

            // for (setindex = 0; setindex < 0; setindex++) // 判断哪个通道被激活 通道工步数据// 每个通道工步数量
            // {

            //     for (ch = 0; ch < BOARD_CHANNEL_NUM; ch++)
            //     {
            //         if (SetCh_Activity >> ch & 0x0001)
            //         {

            //             // 设置工作启动电流
            //             // printf("\r\nch=%d 启动电流=%fA ", ch + 1, g_Channelinfo[ch].RunningWorkSetup.currentStart);

            //             g_Channelinfo[ch].RunningWorkSetup.voltLimit = g_Channelinfo[ch].RunningWorkSetup.voltLimit * 0.001f; // 1500mV=1.5V
            //             // printf("\r\nch=%d 截止电流=%fA ", ch + 1, g_Channelinfo[ch].RunningWorkSetup.voltLimit);

            //             // // 设置工作截止电流
            //             // printf("\r\nch=%d currentLimit=%fA ", ch + 1, g_Channelinfo[ch].RunningWorkSetup.currentLimit);
            //             // // 设置工作截止时间

            //             // printf("\r\nch=%d timeLimit=%d ", ch + 1, g_Channelinfo[ch].RunningWorkSetup.timeLimit);
            //         }
            //     }
            // }

            ch = 0;
            // 设置工作启动电流
            printf("\r\n ch=%d 启动电流=%f A ", ch + 1, g_Channelinfo[ch].RunningWorkSetup[0].currentStart);

            printf("\r\n ch=%d 截止电流=%f  A ", ch + 1, g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].voltLimit);

            // 设置工作截止电流
            printf("\r\n ch=%d currentLimit=%f A ", ch + 1, g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].currentLimit);
            // 设置工作截止时间

            printf("\r\n ch=%d timeLimit=%d  ", ch + 1, g_Channelinfo[ch].RunningWorkSetup[g_Channelinfo[ch].WorkeStartup].timeLimit);

            printf("\r\n 启动工步=%d  运行循环号 =%d  ", g_SetChanneWorke.runWorke_setup, g_SetChanneWorke.Run_Cyc_indx);

            break;
        case EMTOSCMD_StopWorkStep: // 停止工步

            printf("\r\n 收到托盘停止命令=0x%X ", SetCh_Activity);
            printf("\r\n EMTOSCMD_StopWorkStep=%d ", pFrame->dataLen);
            memcpy(&(g_SetChanneWorke), pFrame->data, pFrame->dataLen);

            devid = BoardInfo_GetID();

            tmep[0] = pFrame->data[(devid - 1) * 2];
            tmep[1] = pFrame->data[(devid - 1) * 2 + 1];
            SetCh_Activity = tmep[0] | (tmep[1] << 8);
            printf("\r\n 收到托盘停止命令=0x%X ", SetCh_Activity);

            for (ch = 0; ch < BOARD_CHANNEL_NUM;) // 判断哪个通道被激活 通道工步数据
            {
                if ((SetCh_Activity >> ch) & 0x0001)
                {
                    g_Channelinfo[ch].status = 0x53; // 无操作时=0x53
                    g_Channelinfo[ch].error = 0;     // 错误0x04: 用户强制停止
                    // printf("\r\n ch=%d ", ch);
                    g_Channelinfo[ch].WorkeStartup = 0xFF; // 启动工步
                    g_Channelinfo[ch].loopSn = 0xFF;       // 启动工步
                    g_Channelinfo[ch].fault.all = 1;       // 停止工步
                    ch++;
                }
            }
            break;

        case EMTOSCMD_StartSomeChannelWorkStep: //// 部分通道启动工步

            printf("\r\n 部分通道启动工步 =%d ", pFrame->dataLen);

            break;
        case EMTOSCMD_ContinueWorkStep: // 继续工步

            printf("\r\n 继续工步 =%d ", pFrame->dataLen);

            //            uint16_t channelBitSelect;
            //            uint8_t indexWorkStep; // 续接的工步号
            //            uint8_t snLoop;
            //            uint32_t timeLeft; // 运行的通道剩余时间  单位：ms

            //            tmep[0] = pFrame->data[0];
            //            tmep[1] = pFrame->data[1];
            //            indexWorkStep = pFrame->data[2];

            //            snLoop = pFrame->data[3]; // 续接的循环号

            //            timeLeft = U8TOU32(pFrame->data[4]);

            //            channelBitSelect = tmep[0] | (tmep[1] << 8); // 续接的通道号

            //            printf("\r\n 续接的通道号 =%d ", channelBitSelect);
            //            printf("\r\n 续接的工步号 =%d ", indexWorkStep);
            //            printf("\r\n 续接的循环号=%d ", snLoop);
            //            printf("\r\n 运行的通道剩余时间 =%d ms", timeLeft);
            //            for (ch = 0; ch < BOARD_CHANNEL_NUM;) // 判断哪个通道被续接
            //            {
            //                if ((channelBitSelect >> ch) & 0x0001)
            //                {
            //                    g_Channelinfo[ch].RunningWorkSetup.index = indexWorkStep; // 修改续接的工步号
            //                    //  g_SetChanneWorke.runWorke_setup =indexWorkStep;; // 续接的工步号
            //                    g_Channelinfo[ch].Run_Cyc_indx = snLoop; // 续接的循环号
            //                    g_Channelinfo[ch].RunningWorkSetup.timeLimit = timeLeft;
            //                    // printf("\r\n ch=%d ", ch);
            //                    g_Channelinfo[ch].WorkeStartup = 1; // 启动工步
            //                    g_Channelinfo[ch].fault.all = 0;    // 清除故障
            //                }
            //            }

            // 续接的工步号

            break;

        case EMTOSCMD_JumpWorkStep: // 工步跳转

            break;
        case EMTOSCMD_StopWorkStepw: // 部分通道停止停止工步  托盘通道单点停止的命令。

            tmep[0] = pFrame->data[0];
            tmep[1] = pFrame->data[1];
            SetCh_Activity = tmep[0] | (tmep[1] << 8);
            printf("\r\n 部分通道停止停止工步 =%d ", pFrame->dataLen);
            printf("\r\n SetCh_Activity=0x%X ", SetCh_Activity);

            for (ch = 0; ch < BOARD_CHANNEL_NUM;) // 判断哪个通道被激活 通道工步数据
            {
                if ((SetCh_Activity >> ch) & 0x0001)
                {
                    g_Channelinfo[ch].status = 0x53; // 无操作时=0x53
                    g_Channelinfo[ch].error = 4;     // 错误0x04: 用户强制停止
                    // printf("\r\n ch=%d ", ch);
                    g_Channelinfo[ch].WorkeStartup = 0; // 启动工步
                    g_Channelinfo[ch].fault.all = 1;    // 停止工步
                    ch++;
                }
            }
            break;

        case EMTOSCMD_SampleQuestAck: // // 发送采样数据确认
            // printf("\r\n 发送采样数据确认 ");

            break;
        case EMTOSCMD_LEDControl: // //
            printf("\r\n 设置指示灯控制模式命令 ");

            break;

        case EMTOSCMD_LEDStatus: // //
            printf("\r\n 指示灯状态命令 ");

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
