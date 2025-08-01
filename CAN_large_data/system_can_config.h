#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

// FDCAN配置
#define FDCAN_CLOCK_SOURCE       80000000  // 80 MHz FDCAN时钟
#define FDCAN_NOMINAL_BAUDRATE   1000000   // 1 Mbps 标称波特率
#define FDCAN_DATA_BAUDRATE      2000000   // 2 Mbps 数据波特率 (可选)

// 设备ID定义
#define NODE_DEVICE_ID           0x123     // 本设备ID
#define BROADCAST_DEVICE_ID      0x7FF     // 广播地址

// 功能码定义
#define FC_SYSTEM_CONTROL        0x0
#define FC_DATA_TRANSFER         0x1
#define FC_CONFIGURATION         0x2
#define FC_DIAGNOSTICS           0x3
#define FC_BROADCAST             0xF

// ID位分配 (29位扩展ID)
#define ID_FC_POS                25        // 功能码位置 [28:25]
#define ID_DID_POS               12        // 设备ID位置 [24:12]
#define ID_SUB_POS               0         // 子地址位置 [11:0]

// 消息ID构建宏
#define BUILD_CAN_ID(fc, did, sub) (((fc & 0x0F) << ID_FC_POS) | \
                                   ((did & 0x1FFF) << ID_DID_POS) | \
                                   (sub & 0x0FFF))

#endif // SYSTEM_CONFIG_H


