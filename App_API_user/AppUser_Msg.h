/*
 * AppUser_Msg.h
 *
 *  Created on:
 *      Author: mornsun2
 */

#ifndef USRE_APP_APPUSER_MSG_H_
#define USRE_APP_APPUSER_MSG_H_

#include "head.h"
#include "Rs485Task.h"
#include "AppUser_protection.h"
#define CMD_0x01 0x01
#define CMD_0x02 0x02
#define CMD_0x03 0x03
#define CMD_0x04 0x04
#define CMD_0x05 0x05
#define CMD_0x06 0x06
#define CMD_0x10 0x10

#define CMD_0x0400 0x0400
#define CMD_0x0401 0x0401
#define CMD_0x0402 0x0402
#define CMD_0x0403 0x0403

#define CMD_0x0050 0x0050 // 发送给副边数据

// 中位机向上位机上传数据
typedef struct
{
    uint16_t dev_stat;         // 状态  2
    uint16_t IO_INPUT_Data[4]; // IO输入预留64个输入检测 10
    uint16_t IO_OUT_Data;      // 16路输入控制 12
    uint16_t temp_channe[8];   // 温度通道 28
    power_fault_define fault;  // 32
} _Master_Hand;

extern _Master_Hand g_Master_Hand;

extern void Init_Master_Hand(void);

INT32S AppUser_Msg_CmdAnalys(Protocol *pmodbusProtocol);

//extern void UserRead_MasterState_pAck(tcpProtocol *frameRec);

#endif /* USRE_APP_APPUSER_MSG_H_ */
