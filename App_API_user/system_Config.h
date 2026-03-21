/*
 * system_Config.h
 *
 *  Created on: 2023年9月3日
 *      Author: Administrator
 */

#ifndef USRE_APP_SYSTEM_CONFIG_H_
#define USRE_APP_SYSTEM_CONFIG_H_

#define Bsp_InitSCIBFIFO_EN 0 //
#define Bsp_Init_LIN_EN 1

#define Q16 16 // 2^16=;
#define Q15 15 // 2^15=0;
#define Q14 14 // 2^14=;
#define Q13 13 // 2^16=;
#define Q12 12 // 2^16=;
#define Q11 11 // 2^16=;
#define Q10 10 // 2^16=;

#define TMS32F28335X 0
#define TMS32F28003X 1
#define GD32A50X 1

/*************主板配置*************/
#define BOARD_CHANNEL_NUM 16// 主板通道数
#define BOARD_CELL_NUM 16    // 主板电池数
#define BOARD_TEMP_NUM 1     // 主板温度数

#define SS_START_MODE 0 //  软启动1--启动轨迹控制 0---表示正常高频启动


#define  BOARD_OUT_VOLT   4.2f    //输出电压

#define DBUG_EN 1 // 调试使能-----

#define CLOOS_LOOP_MODE 1 // 调试使能  0是开环  1是闭环

// FDCAN配置
#define FDCAN_CLOCK_SOURCE 80000000    // 80 MHz FDCAN时钟
#define FDCAN_NOMINAL_BAUDRATE 1000000 // 1 Mbps 标称波特率
#define FDCAN_DATA_BAUDRATE 2000000    // 2 Mbps 数据波特率 (可选)

// 设备ID定义
#define NODE_DEVICE_ID 0x123      // 本设备ID
#define BROADCAST_DEVICE_ID 0x7FF // 广播地址

// 功能码定义
#define FC_SYSTEM_CONTROL 0x0
#define FC_DATA_TRANSFER 0x1
#define FC_CONFIGURATION 0x2
#define FC_DIAGNOSTICS 0x3
#define FC_BROADCAST 0xF

// ID位分配 (29位扩展ID)
#define ID_FC_POS 25  // 功能码位置 [28:25]
#define ID_DID_POS 12 // 设备ID位置 [24:12]
#define ID_SUB_POS 0  // 子地址位置 [11:0]

// 消息ID构建宏
#define BUILD_CAN_ID(fc, did, sub) (((fc & 0x0F) << ID_FC_POS) |     \
                                    ((did & 0x1FFF) << ID_DID_POS) | \
                                    (sub & 0x0FFF))

#endif /* USRE_APP_SYSTEM_CONFIG_H_ */
