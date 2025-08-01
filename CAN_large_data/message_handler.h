#ifndef _MESSAGE_HANDLER_H
#define _MESSAGE_HANDLER_H

#include <stdint.h>
//// 控制帧类型定义
typedef enum {
    START_FRAME = 0x01,      // 起始帧
    DATA_FRAME  = 0x02,      // 数据帧
    END_FRAME   = 0x04,      // 结束帧
    ACK_FRAME   = 0x08       // 确认帧
} FrameType;

//// 数据帧结构 (最大60字节有效载荷)
//#pragma pack(push, 1)
//typedef struct {
//    uint16_t session_id;     // 会话ID (0-65535)
//    uint16_t packet_index;   // 包序号 (0-1092)
//    uint8_t  frame_type;     // 帧类型 (FrameType)
//    uint8_t  payload_length; // 有效载荷长度 (0-60)
//    uint8_t  payload[60];    // 有效载荷
//    uint16_t checksum;       // CRC16校验和
//} LargeDataFrame;
//#pragma pack(pop)


extern void ProcessCANMessage(uint8_t fc, uint16_t did, uint16_t sub, uint8_t* data, uint8_t size);

#endif
