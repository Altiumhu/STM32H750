/*
 * AppUser_Msg.c
 *
 *  Created on:
 *      Author: mornsun2
 */

#include "head.h"
#include "AppUser_Msg.h"
// #include "AppUser_pwm.h"
//  #define ApplicationAddress2 0x8015000 // app2代码入口地址 app1 0x8005000~0x8014FFF app2 0x8015000~0x8024FFF

// uint32_t FlashDestination = ApplicationAddress2; /* Flash user program offset */

_Master_Hand g_Master_Hand;

void Init_Master_Hand(void)
{
    g_Master_Hand.IO_INPUT_Data[0] = 0x0;
    g_Master_Hand.IO_INPUT_Data[1] = 0x0;
    g_Master_Hand.IO_INPUT_Data[2] = 0x0;
    g_Master_Hand.IO_INPUT_Data[3] = 0xFFFF;
    g_Master_Hand.fault.all = 0;
    // g_Master_Hand.IO_OUT_Data = GPIOD->IDR;
    g_Master_Hand.IO_OUT_Data = 0xFFFF;
}

//(uint8_t boxSn, uint8_t cmd, uint16_t startChannel, uint16_t numChannels)
#if 0
void UserRead_MasterState_pAck(tcpProtocol *frameRec)
{
    static uint32_t Timer = 0, sysFlag = 0;

    int16_t temp;
    uint16_t totalChannel = 0, i;
    uint8_t data[46 + 6];
    uint8_t dataIndex = 0;
    uint16 startChannel, numChannels;

    startChannel = U8TOU16(frameRec->data);

    numChannels = U8TOU16(frameRec->data + 2);

    frameRec->streamNum[0] = frameRec->data[frameRec->indx - 4]; // 流水号
    frameRec->streamNum[1] = frameRec->data[frameRec->indx - 3]; // 流水号
    frameRec->encrypt = frameRec->data[frameRec->indx - 2];      // 加密

    // 组合连接应答的数据
    data[dataIndex++] = frameRec->boxNum;
    data[dataIndex++] = frameRec->cmd;

    // 起始通道--预留
    data[dataIndex++] = startChannel & 0xff;
    data[dataIndex++] = startChannel >> 8;
    // 通道数--预留
    data[dataIndex++] = totalChannel & 0xff;
    data[dataIndex++] = totalChannel >> 8;

    g_Master_Hand.IO_INPUT_Data[0] = 0x0;
    g_Master_Hand.IO_INPUT_Data[1] = 0x0;
    g_Master_Hand.IO_INPUT_Data[2] = 0x0;
    // g_Master_Hand.IO_INPUT_Data[3] = GPIOE->IDR;

    // 设备状态
    if (sysFlag == 0)
    {
        sysFlag = 1;
        g_Master_Hand.dev_stat |= 1 << 0;
    }
    else
    {
        g_Master_Hand.dev_stat &= ~(1 << 0);
        sysFlag = 0;
    }
    data[dataIndex++] = g_Master_Hand.dev_stat & 0xff;
    data[dataIndex++] = g_Master_Hand.dev_stat >> 8;

    // 4组输入IO
    for (i = 0; i < 4; i++)
    {
        data[dataIndex++] = g_Master_Hand.IO_INPUT_Data[i] & 0xff;
        data[dataIndex++] = g_Master_Hand.IO_INPUT_Data[i] >> 8;
    }
    // 输出IO状态16IO
    data[dataIndex++] = g_Master_Hand.IO_OUT_Data & 0xff;
    data[dataIndex++] = g_Master_Hand.IO_OUT_Data >> 8;

    // 温度数据
    for (i = 0; i < 8; i++)
    {
        data[dataIndex++] = g_Master_Hand.temp_channe[i] & 0xff;
        data[dataIndex++] = g_Master_Hand.temp_channe[i] >> 8;
    }
    // 故障
    data[dataIndex++] = g_Master_Hand.fault.all & 0x000000FF;
    data[dataIndex++] = (g_Master_Hand.fault.all >> 8) & 0xFF;

    data[dataIndex++] = (g_Master_Hand.fault.all >> 16) & 0xFF;
    data[dataIndex++] = (g_Master_Hand.fault.all >> 24) & 0xFF;

    data[dataIndex++] = frameRec->streamNum[0]; // 流水号
    data[dataIndex++] = frameRec->streamNum[1]; // 流水号
    data[dataIndex++] = 0;                      // CRC
    memcpy(frameRec->data, data, dataIndex);
    frameRec->len = dataIndex;
    //    UserEthernet_SendFrame(data, dataIndex);

    printf("\r\n  PC_Read Stat=%d", Timer++);
}
#endif
/**********************************************************************
 * Function:     Modbus_ReadHoldingRegisters
 * Description: 03 (0x03) 读取保持寄存器（Read Holding Registers
 * Input:         void *p_arg
 * Output:
 * Return:      void
 * Others:
 * Modify Date:    Version:    Author:        Modification:
 * -----------------------------------------------
 * 2025-3-13     V1.0        Hu Weiping
 **********************************************************************/
INT32S Modbus_ReadHoldingRegisters(Protocol *pmodbusProtocol)
{
    uint16_t index = 0;
    switch (pmodbusProtocol->Reg)
    {
    case 0x0020: // 寄存器地址--可以把它当做用户指令寄存器使用
        //                            pmodbusProtocol->len =(pmodbusProtocol->Reg_Number)*2;
        //                          memcpy(&pmodbusProtocol->data[0], (uint16_t *)&Modbus_data.ModbusBase[0], sizeof(Modbus_data.ModbusBase));

        //      pmodbusProtocol->len = (pmodbusProtocol->Reg_Number) * 2;
        //      index = AppUser_uint16_CharTo_Big(Modbus_data.Byte.OutPut_Volt, &pmodbusProtocol->data[0]);
        //      index += AppUser_uint16_CharTo_Big(Modbus_data.Byte.OutPut_Curr, &pmodbusProtocol->data[index]);
        //      index += AppUser_uint16_CharTo_Big(Modbus_data.Byte.OutPut_Pow, &pmodbusProtocol->data[index]);
        //      index += AppUser_uint16_CharTo_Big(Modbus_data.Byte.Power_Generation, &pmodbusProtocol->data[index]);
        //      index += AppUser_uint16_CharTo_Big(Modbus_data.Byte.Equipment_Temperature, &pmodbusProtocol->data[index]);

        //      index += AppUser_uint16_CharTo_Big(Modbus_data.Byte.AC_Status, &pmodbusProtocol->data[index]);
        //      index += AppUser_uint16_CharTo_Big(Modbus_data.Byte.Error_Code, &pmodbusProtocol->data[index]);

        //      index += AppUser_uint16_CharTo_Big(Modbus_data.Byte.RES1, &pmodbusProtocol->data[index]);
        //      index += AppUser_uint16_CharTo_Big(Modbus_data.Byte.RES2, &pmodbusProtocol->data[index]);
        break;
    case 0x4000: // 寄存器地址--可以把它当做用户指令寄存器使用
        break;
    case 0x4010: // 寄存器地址
        break;
    case 0x4020: // 寄存器地址
        break;
    case 0x40B0: // 寄存器地址
        break;
    case 0x40D0: // 寄存器地址 读取脉宽
                 //      pmodbusProtocol->len = (pmodbusProtocol->Reg_Number) * 2;
                 //      index = AppUser_uint16_CharTo_Big(g_epwmHandle[0].High_MOS_DUTY, &pmodbusProtocol->data[0]);
        break;
    case 0x40D2: // 寄存器地址 读取低压侧脉宽
                 //      pmodbusProtocol->len = (pmodbusProtocol->Reg_Number) * 2;
                 //      index = AppUser_uint16_CharTo_Big(g_epwmHandle[0].Low_MOS_DUTY, &pmodbusProtocol->data[0]);
        break;
    case 0x40D4: // 寄存器地址 读取频率
                 //      pmodbusProtocol->len = (pmodbusProtocol->Reg_Number) * 2;
                 //      index = AppUser_uint16_CharTo_Big(g_epwmHandle[0].High_MOS_Timer_TBPRD, &pmodbusProtocol->data[0]);
        break;
    }
    return 0;
}

/**********************************************************************
 * Function:     Modbus_ReadInputRegisters
 * Description: ////读取输入寄存器（功能码04）
 * Input:         void *p_arg
 * Output:
 * Return:      void
 * Others:
 * Modify Date:    Version:    Author:        Modification:
 * -----------------------------------------------
 * 2025-3-13     V1.0        Hu Weiping
 **********************************************************************/
INT32S Modbus_ReadInputRegisters(Protocol *pmodbusProtocol)
{
    static uint16_t sysFlag = 0;
    uint16_t index = 0, i, IO_Data = 0;

    switch (pmodbusProtocol->Reg)
    {
    case 0x0000: // 寄存器地址--可以把它当做用户指令寄存器使用
                 //      pmodbusProtocol->len = (pmodbusProtocol->Reg_Number) * 2;
                 //      index = AppUser_uint16_CharTo_Big(LLC_Modbus_data.Byte.OutPut_Volt, &pmodbusProtocol->data[0]);
                 //      index += AppUser_uint16_CharTo_Big(LLC_Modbus_data.Byte.OutPut_Curr, &pmodbusProtocol->data[index]);
                 //      index += AppUser_uint16_CharTo_Big(LLC_Modbus_data.Byte.Input_Vbus, &pmodbusProtocol->data[index]);
                 //      index += AppUser_uint16_CharTo_Big(LLC_Modbus_data.Byte.Equipment_Temperature, &pmodbusProtocol->data[index]);
                 //      index += AppUser_uint16_CharTo_Big(LLC_Modbus_data.Byte.V_Ref, &pmodbusProtocol->data[index]);
                 //      index += AppUser_uint16_CharTo_Big(LLC_Modbus_data.Byte.I_Ref, &pmodbusProtocol->data[index]);
                 //      index += AppUser_uint16_CharTo_Big(LLC_Modbus_data.Byte.Error_Code, &pmodbusProtocol->data[index]);
                 //      index += AppUser_uint16_CharTo_Big(LLC_Modbus_data.Byte.LLC_Status, &pmodbusProtocol->data[index]);
                 //      index += AppUser_uint16_CharTo_Big(LLC_Modbus_data.Byte.RES1, &pmodbusProtocol->data[index]);
        // memcpy(&pmodbusProtocol->data[0], (uint16_t *)&LLC_Modbus_data.ModbusBase[0], sizeof(LLC_Modbus_data.ModbusBase));
        break;
    case 0x0002: // 读取ID寄存器地址
                 //      pmodbusProtocol->len = (pmodbusProtocol->Reg_Number) * 2;
                 //      index = AppUser_uint16_CharTo_Big(g_Handle_REC_Device.dev_id, &pmodbusProtocol->data[0]);
        break;
    case 0x0005: // 读取固件版本号
//        pmodbusProtocol->len = (pmodbusProtocol->Reg_Number) * 2;
//        index = AppUser_uint16_CharTo_Big(g_Handle_REC_Device.FW_Rev, &pmodbusProtocol->data[0]);

//        for (i = 0; i < sizeof(g_Handle_REC_Device.FW_CRC32_Code); i++)
//            pmodbusProtocol->data[index + i] = g_Handle_REC_Device.FW_CRC32_Code[i];
//        index += sizeof(g_Handle_REC_Device.FW_CRC32_Code);
		
//		        pmodbusProtocol->len = (pmodbusProtocol->Reg_Number) * 2;
//        index = AppUser_uint16_CharTo_Big(BoardInfo_GetSoftVersion(SelfBoard), &pmodbusProtocol->data[0]);
//        index += AppUser_uint16_CharTo_Big(0xAA55, &pmodbusProtocol->data[2]);
//        index += AppUser_uint16_CharTo_Big(0xABDC, &pmodbusProtocol->data[4]);
//        for (i = 0; i < sizeof(g_Handle_REC_Device.FW_CRC32_Code); i++)
//            pmodbusProtocol->data[index + i] = g_Handle_REC_Device.FW_CRC32_Code[i];
//        index += sizeof(g_Handle_REC_Device.FW_CRC32_Code);

        break;

    case 0x0020: // 寄存器地址--可以把它当做用户指令寄存器使用
                 //      pmodbusProtocol->len = (pmodbusProtocol->Reg_Number) * 2;
                 //      index = AppUser_uint16_CharTo_Big(Modbus_data.Byte.OutPut_Volt, &pmodbusProtocol->data[0]);
                 //      index += AppUser_uint16_CharTo_Big(Modbus_data.Byte.OutPut_Curr, &pmodbusProtocol->data[index]);
                 //      index += AppUser_uint16_CharTo_Big(Modbus_data.Byte.OutPut_Pow, &pmodbusProtocol->data[index]);
                 //      index += AppUser_uint16_CharTo_Big(Modbus_data.Byte.Power_Generation, &pmodbusProtocol->data[index]);
                 //      index += AppUser_uint16_CharTo_Big(Modbus_data.Byte.Equipment_Temperature, &pmodbusProtocol->data[index]);

        //      index += AppUser_uint16_CharTo_Big(Modbus_data.Byte.AC_Status, &pmodbusProtocol->data[index]);
        //      index += AppUser_uint16_CharTo_Big(Modbus_data.Byte.Error_Code, &pmodbusProtocol->data[index]);

        //      index += AppUser_uint16_CharTo_Big(Modbus_data.Byte.RES1, &pmodbusProtocol->data[index]);
        //      index += AppUser_uint16_CharTo_Big(Modbus_data.Byte.RES2, &pmodbusProtocol->data[index]);
        break;
    case 0x0070: // 寄存器地址--上传校准电压值和ADC
        // pmodbusProtocol->len = (pmodbusProtocol->Reg_Number) * 2;
        // index = AppUser_uint16_CharTo_Big((INT16U)(OutPut_Volt * 10.0f), &pmodbusProtocol->data[0]);
        // index += AppUser_uint16_CharTo_Big(AdcData.Vout_det, &pmodbusProtocol->data[index]);
        break;
    case 0x0080: // 寄存器地址--上传校准电流值和ADC
        pmodbusProtocol->len = (pmodbusProtocol->Reg_Number) * 2;
        // index = AppUser_uint16_CharTo_Big((INT16U)(OutPut_Curr * 10.0f), &pmodbusProtocol->data[0]);
        // index += AppUser_uint16_CharTo_Big(AdcData.Iout_det, &pmodbusProtocol->data[index]);

        break;

    case 0x4000: // 寄存器地址--可以把它当做用户指令寄存器使用
        break;
    case 0x4010: // 寄存器地址
        break;
    case 0x4020: // 寄存器地址
        break;
    case 0x40B0: // 寄存器地址
        break;
    case 0x40D0: // 寄存器地址 读取脉宽
                 //      pmodbusProtocol->len = (pmodbusProtocol->Reg_Number) * 2;
                 //      index = AppUser_uint16_CharTo_Big(g_epwmHandle[0].High_MOS_DUTY, &pmodbusProtocol->data[0]);
        break;
    case 0x40D2: // 寄存器地址 读取低压侧脉宽
                 //      pmodbusProtocol->len = (pmodbusProtocol->Reg_Number) * 2;
                 //      index = AppUser_uint16_CharTo_Big(g_epwmHandle[0].Low_MOS_DUTY, &pmodbusProtocol->data[0]);
        break;
    case 0x40D4: // 寄存器地址 读取频率
                 //      pmodbusProtocol->len = (pmodbusProtocol->Reg_Number) * 2;
                 //      index = AppUser_uint16_CharTo_Big(g_epwmHandle[0].High_MOS_Timer_TBPRD, &pmodbusProtocol->data[0]);
        break;
    case 0x4090: // 寄存器地址读取IO信息
        pmodbusProtocol->len = (pmodbusProtocol->Reg_Number) * 2;
        // 输入IO口64个
        g_Master_Hand.IO_INPUT_Data[0] = 0x0;
        g_Master_Hand.IO_INPUT_Data[1] = 0x0;
        g_Master_Hand.IO_INPUT_Data[2] = 0x0;
        // g_Master_Hand.IO_INPUT_Data[3] = GPIOE->IDR;
        // 设备状态
        if (sysFlag == 0)
        {
            sysFlag = 1;
            g_Master_Hand.dev_stat |= 1 << 0;
        }
        else
        {
            g_Master_Hand.dev_stat &= ~(1 << 0);
            sysFlag = 0;
        }
        //  index = g_Master_Hand.dev_stat; //设备状态
        index = AppUser_uint16_CharTo_Big(g_Master_Hand.dev_stat, &pmodbusProtocol->data[0]);
        index += AppUser_uint16_CharTo_Big(g_Master_Hand.IO_INPUT_Data[0], &pmodbusProtocol->data[index]);
        index += AppUser_uint16_CharTo_Big(g_Master_Hand.IO_INPUT_Data[1], &pmodbusProtocol->data[index]);
        index += AppUser_uint16_CharTo_Big(g_Master_Hand.IO_INPUT_Data[2], &pmodbusProtocol->data[index]);
        index += AppUser_uint16_CharTo_Big(g_Master_Hand.IO_INPUT_Data[3], &pmodbusProtocol->data[index]);

        // 输出IO状态16IO
        index += AppUser_uint16_CharTo_Big(g_Master_Hand.IO_OUT_Data, &pmodbusProtocol->data[index]);
        // 温度数据
        for (i = 0; i < 8; i++)
            index += AppUser_uint16_CharTo_Big(g_Master_Hand.temp_channe[i], &pmodbusProtocol->data[index]);
        // 故障
        index += AppUser_uint16_CharTo_Big((g_Master_Hand.fault.all & 0xFF00) >> 8, &pmodbusProtocol->data[index]);
        index += AppUser_uint16_CharTo_Big(g_Master_Hand.fault.all & 0x00FF, &pmodbusProtocol->data[index]);

        break;
    case 0x40E0: // 寄存器地址
        break;
    case 0x40F0: // 寄存器地址
        break;
    }

    return 0;
}
/**********************************************************************
 * Function:     Modbus_WriteSingleRegister
 * Description: //6 预置单寄存器（功能码06）
 * Input:         void *p_arg
 * Output:
 * Return:      void
 * Others:
 * Modify Date:    Version:    Author:        Modification:
 * -----------------------------------------------
 * 2025-3-13     V1.0        Hu Weiping
 **********************************************************************/
INT32S Modbus_WriteSingleRegister(Protocol *pmodbusProtocol)
{
    volatile uint16_t Temp_Data, IO_Data = 0;
    volatile uint8_t ret, i;
    switch (pmodbusProtocol->Reg)
    {
    case 0x0001: // 开关机寄存器地址--
        Temp_Data = pmodbusProtocol->Reg_Number;
        if (Temp_Data == 0x0001) // 开机
        {
            // Power_PreChareg(); //预充电s
            //          g_Channelinfo[0].fault.all = 0;
            //  pwm_start(0, 0);
            //  pwm_start(1, 0);
            // DBG_PRINTF("\r\nStarting up=%d\r\n", Temp_Data);
        }
        else if (Temp_Data == 0x0000) // 关机
        {
            //   pwm_stop(0);
            //   pwm_stop(1);
            // FaultState.bit.Sys_On_OFF = 1;
            //          g_Channelinfo[0].fault.bit.ctrlonoff =1;
            //           g_Channelinfo[0].workMode = POWER_FAULT;
        }
        else if (Temp_Data == 0x0002)
        {
            // NVIC_SystemReset(); //系统复位
            ret = 0xFE;
            return ret;
        }
        break;
    case 0x0002: // 设置ID寄存器地址--
                 //      Temp_Data = pmodbusProtocol->Reg_Number;
                 //      Temp_Data = fmin(Temp_Data, 16); // 限制ID
                 //      Temp_Data = fmax(Temp_Data, 1);
                 //      g_Handle_REC_Device.dev_id = Temp_Data;
                 //       g_Calibration_MSG = 3; // 通知后台消息记录数据
        break;
    case 0x0004: // 设置打印信息命令(内部使用):
        Temp_Data = pmodbusProtocol->Reg_Number;
        // g_CAL_EN = (Temp_Data & 0xFF00) >> 8;     // //校准使能发送数据
        // g_DebugPrintf_EN = (Temp_Data & 0xFF00); //// 打开调试信息
        break;

    case 0x0010: // 设置输出电压指令 寄存器地址--可以把它当做用户指令寄存器使用
                 //      Temp_Data = fmin(Temp_Data, 120); // 限制最大电压：58.0V
                 //      Temp_Data = fmax(Temp_Data, 0);
                 //      g_Handle_REC_Device.V_Ref_REC_Vaule = (float)Temp_Data * 0.1f;
                 //       g_Calibration_MSG = 3; // 通知后台消息记录数据
        break;
    case 0x0011: // 设置输出电流指令 寄存器地址--可以把它当做用户指令寄存器使用
                 //      Temp_Data = fmin(Temp_Data, 81); // 限制最大电流107A
                 //      Temp_Data = fmax(Temp_Data, 0);
                 //      g_Handle_REC_Device.I_Ref_REC_Vaule = (float)(Temp_Data * 0.1f);
                 //       g_Calibration_MSG = 3; // 通知后台消息记录数据
        break;

    case 0xD0: // 寄存器地址
        //   g_Calibration_MSG =2; //数据初始化

        //      GF_CONTROL2_HIGH_LEVEL;
        //      DBG_PRINTF("\r\nGF_CONTROL2_HIGH_LEVELr\n");

        break;
    case 0x4020: // 寄存器地址
        break;

    case 0x40C0: // LLC后级----启动升级指令
        //  printf("\r\n0x40C0LLC");
        //  g_Calibration_MSG = 3;              // 通知后台写入升级标记
        //      g_Handle_REC_Device.FW_Code = 0xAA; // 添加写入LLC升级数据校验码

        break;
    case 0x40C2: // MPPT前进---启动升级指令
        //                           printf("\r\n0x40C2MPPT");

        break;
    case 0x40D0: // 寄存器地址
        Temp_Data = pmodbusProtocol->Reg_Number;
        // 限制范围
        //      Temp_Data = FIND_MIN(Temp_Data, (g_epwmHandle[0].High_MOS_Timer_TBPRD >> 1));
        //      Temp_Data = FIND_MAX(Temp_Data, g_epwmHandle[0].High_MOS_DTF);
        //      g_epwmHandle[0].High_MOS_DUTY = Temp_Data;

        break;
    case 0x40D2: // 寄存器地址 侧脉宽
        Temp_Data = pmodbusProtocol->Reg_Number;
        // 限制范围
        //      Temp_Data = FIND_MIN(Temp_Data, (g_epwmHandle[0].High_MOS_Timer_TBPRD >> 1));
        //      Temp_Data = FIND_MAX(Temp_Data, g_epwmHandle[0].High_MOS_DTF);
        //      g_epwmHandle[0].Low_MOS_DUTY = Temp_Data;
        break;
    case 0x40D4: // 寄存器地址 频率
        Temp_Data = pmodbusProtocol->Reg_Number;
        // 限制范围
        //      Temp_Data = FIND_MIN(Temp_Data, 4000);
        //      Temp_Data = FIND_MAX(Temp_Data, 1000);
        //      g_epwmHandle[0].High_MOS_Timer_TBPRD = Temp_Data;
        break;
    case 0x40D6: // 寄存器地址 高压侧mos
        Temp_Data = pmodbusProtocol->Reg_Number;
        //
        if (Temp_Data == 0)
        {
            //       pwm_stop(0);
        }
        else
        {
            //         pwm_start(0, 0);
        }

        break;
    case 0x40D8: // 寄存器地址 同步整流
        Temp_Data = pmodbusProtocol->Reg_Number;
        // 限制范围
        if (Temp_Data == 0)
        {
            //       pwm_stop(1);
        }
        else
        {
            //         pwm_start(1, 0);
        }
        break;
    case 0x4091: // 寄存器地址--控制输出IO
        IO_Data = pmodbusProtocol->Reg_Number;
        g_Master_Hand.IO_OUT_Data = IO_Data;
        for (i = 0; i < 12; i++)
        {
            if (IO_Data & 0x0001)
            {
               // HAL_GPIO_WritePin(MCU_IO_OUT[i].port, MCU_IO_OUT[i].pin, GPIO_PIN_SET); // 设置高电平
            }
            else
            {
                //HAL_GPIO_WritePin(MCU_IO_OUT[i].port, MCU_IO_OUT[i].pin, GPIO_PIN_RESET); // 设置低电平
            }
            IO_Data = IO_Data >> 1;
        }
        // printf("\r\n IIO_Set=0x%X,",pmodbusProtocol->Reg_Number);
        break;
    case 0x4092: // 寄存器地址-
//        Temp_Data = pmodbusProtocol->Reg_Number;
//        Set_FAN_Duty(TIM_CHANNEL_1, (Temp_Data & 0xFF00) >> 8);
//        Set_FAN_Duty(TIM_CHANNEL_2, Temp_Data);
        // printf("\r\n FAN_Set=0x%X  ",Temp_Data);

        break;
    case 0x40DA: // 寄存器地址
        break;
    case 0x40E0: // 寄存器地址
        break;
    case 0x40F0: // 寄存器地址
        break;
    }

    return 1;
}

/**********************************************************************
 * Function:     Modbus_WriteMultipleCoils
 * Description:  //0x10 预置单寄存器（功能码0x10）
 * Input:         void *p_arg
 * Output:
 * Return:      void
 * Others:
 * Modify Date:    Version:    Author:        Modification:
 * -----------------------------------------------
 * 2025-3-13     V1.0        Hu Weiping
 **********************************************************************/
INT32S Modbus_WriteMultipleCoils(Protocol *pmodbusProtocol)
{
    INT16U index = 0, i;
    uint8_t writeFlash[130];
    uint32_t RreadData_Add;
    uint32_t packet_length;
    uint16_t Length = 0, crc_data;
    uint8_t ret;
    __attribute__((aligned(4))) static uint8_t data_temp[256];
    static uint32_t data_size = 0;

    // if (FaultState.bit.MPPT_FW == 1)
    //  return -1; // 启动透传功能 LLC不解析就退出

    memset(writeFlash, 0, sizeof(writeFlash)); // 写入writeFlash初始化
    packet_length = pmodbusProtocol->data[0];
    // 数据长度是128个字节限制
    packet_length = fmin(packet_length, 128);
    packet_length = fmax(packet_length, 0);
    switch (pmodbusProtocol->Reg)
    {
    case 0x0005: // 固件版本和固件校验码CRC32
        for (i = 0; i < 4; i++)
        {
            //          g_Handle_REC_Device.FW_CRC32_Code[i] = pmodbusProtocol->data[i + 1];
        }
        //       g_Calibration_MSG = 3; /// 通知后台写入数据
        // DBG_PRINTF("\r\nFW_CRC32_Code=%X =%X",g_Handle_REC_Device.FW_CRC32_Code[0],g_Handle_REC_Device.FW_CRC32_Code[1]);
        break;
    case 0x0050: // 保存电压校准数据寄存器地址-

        //      g_Calibration_Device[Caltype_V_OUT].ka = AppUser_CharToFloat_Samll(&pmodbusProtocol->data[1]);
        //      g_Calibration_Device[Caltype_V_OUT].kb = AppUser_CharToFloat_Samll(&pmodbusProtocol->data[5]);

        //      g_Handle_REC_Device.V_OUT_REC_ka = g_Calibration_Device[Caltype_V_OUT].ka ;
        //        g_Handle_REC_Device.V_OUT_REC_kb = g_Calibration_Device[Caltype_V_OUT].kb ;
        //      g_Calibration_MSG = 1; /// 通知后台写入数据
        // DBG_PRINTF("\r\nV ka=%f kb=%f ",g_Calibration_Device[Caltype_V_OUT].ka,g_Calibration_Device[Caltype_V_OUT].kb);
        break;
    case 0x0060: // 保存电流校准数据寄存器地址-

        //      g_Calibration_Device[Caltype_Current].ka = AppUser_CharToFloat_Samll(&pmodbusProtocol->data[1]);
        //      g_Calibration_Device[Caltype_Current].kb = AppUser_CharToFloat_Samll(&pmodbusProtocol->data[5]);

        //      g_Handle_REC_Device.I_OUT_REC_ka = g_Calibration_Device[Caltype_Current].ka ;
        //        g_Handle_REC_Device.I_OUT_REC_kb = g_Calibration_Device[Caltype_Current].kb ;
        //      g_Calibration_MSG = 1; /// 通知后台写入数据
        // DBG_PRINTF("\r\nC ka=%f kb=%f ",g_Calibration_Device[Caltype_Current].ka,g_Calibration_Device[Caltype_Current].kb);
        break;

    case 0x4000: // 寄存器地址--接收Boost

        //      g_DEVICE_Information.Boost_VBUS = AppUser_CharToFloat_Samll(&pmodbusProtocol->data[1]);
        //      g_DEVICE_Information.Boost_Curr = AppUser_CharToFloat_Samll(&pmodbusProtocol->data[5]);
        //      g_DEVICE_Information.Boost_State = AppUser_CharToU32_Samll(&pmodbusProtocol->data[6]);

        //      DBG_PRINTF("\r\nBoost_VBUS=%f Boost_Curr=%f Boost_State = 0x%X\r\n", g_DEVICE_Information.Boost_VBUS ,g_DEVICE_Information.Boost_Curr ,g_DEVICE_Information.Boost_State);
        //
        break;
    case 0x4010: // 寄存器地址
        break;
    case 0x4020: // 寄存器地址
        break;
    case 0x4030: // 寄存器地址
        break;
    case 0x40C0: // LLC后级----开始吸入数据
        //                       //拷贝需要的Flash数据
        //      memcpy(writeFlash, &pmodbusProtocol->data[1], packet_length);

        // ret = FlashWrite(packet_length, writeFlash, FlashDestination); // 写入APP1数据
        // if (ret == FMC_READY)
        // {
        //  memcpy(data_temp, writeFlash, packet_length);
        //  crc_data = crc16_ccitt((const unsigned char *)(data_temp), packet_length);

        //  // 再次对写入Flash中的数据进行CRC校验，确保写入Flash的数据无误
        //  if (crc_data != crc16_ccitt((const unsigned char *)(FlashDestination), packet_length))
        //  {
        //      printf("\r\nwriteFlash CRC ERR 2 写入数据错误");
        //      while (1)
        //          ;
        //  }
        // }
        // else
        // {
        //  printf("\r\nwriteFlash ERR 1 写入数据错误");
        //  while (1)
        //      ;
        // }
        // FlashDestination += packet_length; // 计算Flah偏移地址
        //                                 //  printf("\r\n0x40C0LLC_Write_OK Length =%d",Data_Length);
        break;
    case 0x40C2: // MPPT前进---启动升级指令
        //                           printf("\r\n0x40C2MPPT");
        // Set_MPPT_FW(TRUE);

        break;
    case 0x40D0: // 测试读取数据

        break;
    case 0x40E0: // 寄存器地址
        break;
    case 0x40F0: // 寄存器地址
        // memcpy(writeFlash, (uint32_t *)(RreadData_Add + Length), 128);
        // for (i = 0; i < 128; i++)
        // {
        //  DBG_PRINTF("\r\ni=%d data=%X", i, writeFlash[i]);
        // }
        // DBG_PRINTF("\r\n0x40C0LLC_Rread_OK ");

        break;
    }

    pmodbusProtocol->len = index;
    return 1;
}

/**********************************************************************
 * Function:     AppUser_Msg_CmdAnalys
 * Description:  MCU内部通讯
 * Input:         void *p_arg
 * Output:
 * Return:      void
 * Others:      Modbus-RTU
 * Modify Date:    Version:    Author:        Modification:
 * -----------------------------------------------
 * 2022-12-05     V1.0        Hu Weiping
 **********************************************************************/
INT32S AppUser_Msg_CmdAnalys(Protocol *pmodbusProtocol)
{
    volatile INT8U uRstFalg = 0, indx = 0, i;
//  DBG_PRINTF("\r\n CMD_0x%X \r\n", pmodbusProtocol->cmd);
#if 1
    switch (pmodbusProtocol->cmd) // Modbus功能码
    {

    case CMD_0x01: //
    {
        switch (pmodbusProtocol->data[0])
        {
            //			  case 0: //充电电压
            //					     HAL_GPIO_WritePin(GPIOD, CV_GPIO_Pin, GPIO_PIN_SET); //设置高电平 PD4
            //							 HAL_GPIO_WritePin(GPIOD, DCC_GPIO_Pin, GPIO_PIN_RESET); //设置低电平 PD2
            //							 HAL_GPIO_WritePin(GPIOD, CCC_GPIO_Pin, GPIO_PIN_RESET); //设置低电平 PD0
            //					     break;
            //			  case 1: //充电电流
            //							 HAL_GPIO_WritePin(GPIOD, CV_GPIO_Pin, GPIO_PIN_RESET); //设置高电平 PD4
            //							 HAL_GPIO_WritePin(GPIOD, DCC_GPIO_Pin, GPIO_PIN_RESET); //设置低电平 PD2
            //							 HAL_GPIO_WritePin(GPIOD, CCC_GPIO_Pin, GPIO_PIN_SET); //设置高电平 PD0
            //					     break;
            //			  case 2: //放电电流
            //					    	HAL_GPIO_WritePin(GPIOD, CV_GPIO_Pin, GPIO_PIN_RESET); //设置高电平 PD4
            //							 HAL_GPIO_WritePin(GPIOD, DCC_GPIO_Pin, GPIO_PIN_SET); //设置低电平 PD2
            //							 HAL_GPIO_WritePin(GPIOD, CCC_GPIO_Pin, GPIO_PIN_RESET); //设置低电平 PD0
            //					     break;
        }
    }
    break;
    case CMD_0x02: //
    {

        indx = pmodbusProtocol->data[1];
        pmodbusProtocol->len = 3;
        // printf("\r\n  ch=%d " ,indx);
        if (indx >= 0 && indx < 32)
        {
            for (i = 0; i < 32; i++)
            {
                if (indx == i) // 打开通道
                {
                   /// HAL_GPIO_WritePin(MCU_IO_OUT[indx].port, MCU_IO_OUT[indx].pin, GPIO_PIN_SET); // 设置高电平
                }
                else
                {
                    //HAL_GPIO_WritePin(MCU_IO_OUT[indx].port, MCU_IO_OUT[indx].pin, GPIO_PIN_RESET); // 设置低电平
                }
            }
        }
        else
        {
            // printf("\r\n ERR ch=%d " ,indx);
        }
    }
    break;
    case CMD_0x03: //
    {
        Modbus_ReadHoldingRegisters(pmodbusProtocol);
    }
    break;
    case CMD_0x04: //
    {
        Modbus_ReadInputRegisters(pmodbusProtocol);
    }
    break;
    case CMD_0x05: //
    {
    }
    break;
    case CMD_0x06: //
    {
        uRstFalg = Modbus_WriteSingleRegister(pmodbusProtocol);
    }
    break;

    case CMD_0x10: // 预置多寄存器（功能码0x10）
    {
        Modbus_WriteMultipleCoils(pmodbusProtocol);
    }
    break;

    case CMD_0x0050: //
    {
    }
    break;
    default:
    {
    }
    break;
    }

//    USART1_TX_EN();
//    AppUser_Rs485Send_Data(pmodbusProtocol); /// LOGD("\r\nRS485 发送数据 模式\r\n");
//    USART1_RX_EN();

    //  if (pmodbusProtocol->sendMOde == RS485_Send_MODE && pmodbusProtocol->sendEnbl == 1)
    //  {
    //
    //      AppUser_Rs485Send_Data(pmodbusProtocol);/// LOGD("\r\nRS485 发送数据 模式\r\n");
    //  }
    //  else if (pmodbusProtocol->sendMOde == CAN_Send_MODE && pmodbusProtocol->sendEnbl == 1)
    //  {
    //      //AppUser_CANSendModbus_Data(pmodbusProtocol);// LOGD("TODO CAN 发送数据 模式\r\n");
    //  }
    //  else if (pmodbusProtocol->sendMOde == RS485_To_TCP_MODE && pmodbusProtocol->sendEnbl == 1)
    //  {
    //      //AppUser_Rs485_TO_TCP_Send_Data(pmodbusProtocol);// LOGD("RS485 to TCP 发送数据 模式\r\n");
    //  }

    if (uRstFalg == 0xFE) // 复位指令
    {

         NVIC_SystemReset(); // 系统复位
    }

#endif
    return 1;
}

void Send_Cmd_Hand(void)
{
}
