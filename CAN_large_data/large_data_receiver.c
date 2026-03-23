#include "large_data_transfer.h"
#include "fdcan.h"
#include "crc.h"
#include <stdlib.h>
#include <string.h>
#include "message_handler.h"
#include "system_can_config.h"


// 接收状态机
typedef enum
{
    RECEIVER_IDLE,
    RECEIVING_START,
    RECEIVING_DATA,
    RECEIVING_END
} ReceiverState;

// 接收会话结构
typedef struct
{
    uint16_t session_id;  // 会话ID (0-65535)
    uint16_t total_packets;
    uint16_t received_packets;
    uint8_t *buffer;
    uint32_t buffer_size;
    uint32_t last_receive_time;
    uint8_t active;
} ReceiveSession;

#define MAX_SESSIONS 3


static ReceiveSession sessions[MAX_SESSIONS];
static ReceiverState receiver_state = RECEIVER_IDLE;


void HandleStartFrame(uint16_t src_did, LargeDataFrame *frame);

void HandleDataFrame(uint16_t src_did, LargeDataFrame *frame);


void HandleEndFrame(uint16_t src_did, LargeDataFrame *frame) ;


// 发送ACK
void SendACK(uint16_t dest_did, uint16_t session_id, uint16_t ack_code);
// 查找会话
 ReceiveSession *FindSession(uint16_t session_id);
// 初始化大数据接收
void LargeDataTransfer_InitReceiver(void)
{
    for (int i = 0; i < MAX_SESSIONS; i++)
    {
        sessions[i].active = 0;
        sessions[i].buffer = NULL;
    }
    receiver_state = RECEIVER_IDLE;
}

// 处理接收到的数据帧
void LargeDataTransfer_HandleFrame(uint16_t src_did, LargeDataFrame *frame)
{
    // 验证校验和
    uint16_t calc_checksum = CRC16_Calculate((uint8_t*)frame,
                             sizeof(LargeDataFrame) - sizeof(frame->checksum));

    if (calc_checksum != frame->checksum)
    {
        // 校验失败，发送NAK
        SendACK(src_did, frame->session_id, ACK_CHECKSUM_ERROR);
        return;
    }

    // 根据帧类型处理
    switch (frame->frame_type)
    {
    case START_FRAME://起始帧
        HandleStartFrame(src_did, frame);
        break;

    case DATA_FRAME:// 数据帧
        HandleDataFrame(src_did, frame);
        break;

    case END_FRAME://END_FRAME
        HandleEndFrame(src_did, frame);
        break;

    case ACK_FRAME:
        // ACK帧由发送端处理
        break;

    default:
        // 未知帧类型
        break;
    }
}

// 处理起始帧
void HandleStartFrame(uint16_t src_did, LargeDataFrame *frame)
{
    // 检查是否已有相同会话
    for (int i = 0; i < MAX_SESSIONS; i++)
    {
        if (sessions[i].active && sessions[i].session_id == frame->session_id)
        {
            // 会话已存在，发送NAK
            SendACK(src_did, frame->session_id, ACK_SESSION_EXISTS);
            return;
        }
    }

    // 查找空闲会话槽
    int session_index = -1;
    for (int i = 0; i < MAX_SESSIONS; i++)
    {
        if (!sessions[i].active)
        {
            session_index = i;
            break;
        }
    }

    if (session_index == -1)
    {
        // 无可用会话槽
        SendACK(src_did, frame->session_id, ACK_NO_RESOURCES);
        return;
    }

    // 计算所需缓冲区大小
    uint32_t buffer_size = frame->packet_index * MAX_PAYLOAD; // packet_index存储总包数

    // 分配缓冲区
    uint8_t *buffer = malloc(buffer_size);
    if (!buffer)
    {
        // 内存分配失败
        SendACK(src_did, frame->session_id, ACK_NO_MEMORY);
        return;
    }

    // 初始化会话
    ReceiveSession *session = &sessions[session_index];
    session->active = 1;
    session->session_id = frame->session_id;
    session->total_packets = frame->packet_index;
    session->received_packets = 0;
    session->buffer = buffer;
    session->buffer_size = buffer_size;
    session->last_receive_time = HAL_GetTick();

    // 发送ACK
    SendACK(src_did, frame->session_id, ACK_START_OK);
}

// 处理数据帧
void HandleDataFrame(uint16_t src_did, LargeDataFrame *frame)
{
    // 查找会话
    ReceiveSession *session = FindSession(frame->session_id);
    if (!session || !session->active)
    {
        // 会话不存在
        SendACK(src_did, frame->session_id, ACK_SESSION_INVALID);
        return;
    }

    // 检查包序号是否有效
    if (frame->packet_index >= session->total_packets)
    {
        // 无效包序号
        SendACK(src_did, frame->session_id, ACK_INVALID_INDEX);
        return;
    }

    // 检查是否已接收过该包
    if (session->received_packets > frame->packet_index)
    {
        // 包已接收，可能是重复包
        SendACK(src_did, frame->session_id, ACK_DATA_OK);
        return;
    }

    // 计算数据偏移
    uint32_t offset = frame->packet_index * MAX_PAYLOAD;

    // 检查缓冲区边界
    if (offset + frame->payload_length > session->buffer_size)
    {
        // 数据溢出
        SendACK(src_did, frame->session_id, ACK_BUFFER_OVERFLOW);
        return;
    }

    // 复制数据
    memcpy(session->buffer + offset, frame->payload, frame->payload_length);
    session->received_packets = frame->packet_index + 1;
    session->last_receive_time = HAL_GetTick();

    // 发送ACK
    SendACK(src_did, frame->session_id, ACK_DATA_OK);
}

// 处理结束帧
void HandleEndFrame(uint16_t src_did, LargeDataFrame *frame)
{
    // 查找会话
    ReceiveSession *session = FindSession(frame->session_id);
    if (!session || !session->active)
    {
        // 会话不存在
        SendACK(src_did, frame->session_id, ACK_SESSION_INVALID);
        return;
    }

    // 检查是否接收完所有包
    if (session->received_packets != session->total_packets)
    {
        // 缺少数据包
        SendACK(src_did, frame->session_id, ACK_MISSING_PACKETS);
        return;
    }
#if CAN_MSG_BUG	
    // 通知应用层数据接收完成
    if (data_received_cb)
    {
        data_received_cb(src_did, session->session_id,
                         session->buffer, session->buffer_size);
    }
#endif
    // 释放资源
    free(session->buffer);
    session->buffer = NULL;
    session->active = 0;

    // 发送最终ACK
    SendACK(src_did, frame->session_id, ACK_END_OK);
}

// 发送ACK
void SendACK(uint16_t dest_did, uint16_t session_id, uint16_t ack_code)
{
    LargeDataFrame frame;
    frame.frame_type = ACK_FRAME;
    frame.session_id = session_id;
    frame.packet_index = ack_code; // 使用packet_index字段传输ACK状态码
    frame.payload_length = 0;

    // 计算校验和
    frame.checksum = CRC16_Calculate((uint8_t*)&frame, sizeof(frame) - sizeof(frame.checksum));

    // 发送ACK
    FDCAN_SendMessage(FC_DATA_TRANSFER, dest_did, 0,
                      (uint8_t*)&frame, sizeof(frame));
}

// 查找会话
 ReceiveSession *FindSession(uint16_t session_id)
{
    for (int i = 0; i < MAX_SESSIONS; i++)
    {
        if (sessions[i].active && sessions[i].session_id == session_id)
        {
            return &sessions[i];
        }
    }
    return NULL;
}

// 超时处理 (在主循环中调用)
void LargeDataTransfer_SessionTimeoutHandler(void)
{
    uint32_t current_time = HAL_GetTick();

    for (int i = 0; i < MAX_SESSIONS; i++)
    {
        if (sessions[i].active && (current_time - sessions[i].last_receive_time) > SESSION_TIMEOUT)
               
        {

            // 会话超时，清理资源
            if (sessions[i].buffer)
            {
                free(sessions[i].buffer);
            }
            sessions[i].buffer = NULL;
            sessions[i].active = 0;
#if CAN_MSG_BUG	
            // 通知应用层传输失败
            if (transfer_failed_cb)
            {
                transfer_failed_cb(sessions[i].session_id, TRANSFER_TIMEOUT);
            }
#endif						
        }
    }
}

