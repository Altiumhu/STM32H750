
#include "head.h"
#include "large_data_transfer.h"
//#include "./BSP/FDCAN/fdcan.h"
//#include "fdcan.h"

#include <string.h>
#include "user_crc_data.h"
// 发送状态机
typedef enum {
    SENDER_IDLE,
    SENDING_START,
    SENDING_DATA,
    SENDING_END,
    WAITING_ACK
} SenderState;

static struct {
    SenderState state;
    uint16_t session_id;
    uint16_t total_packets;
    uint16_t current_packet;
    uint8_t *data_ptr;
    uint32_t data_size;
    uint32_t timeout;
    uint8_t retry_count;
} sender_ctx;


uint32_t transfer_complete_cb =0;
// 初始化大数据发送
void LargeDataTransfer_InitSender(void) {
    memset(&sender_ctx, 0, sizeof(sender_ctx));
    sender_ctx.state = SENDER_IDLE;
}

// 开始大数据传输
int LargeDataTransfer_Send(uint16_t dest_did, uint8_t *data, uint32_t size) {
    if (sender_ctx.state != SENDER_IDLE) {
        return TRANSFER_BUSY;
    }
    
    if (size > MAX_LARGE_DATA_SIZE) {
        return TRANSFER_TOO_LARGE;
    }
    
    // 计算总包数 (每包最大60字节)
    uint16_t total_packets = (size + MAX_PAYLOAD - 1) / MAX_PAYLOAD;
    
    // 初始化发送上下文
    sender_ctx.session_id = (uint16_t)HAL_GetTick(); // 使用时间戳作为会话ID
    sender_ctx.total_packets = total_packets;
    sender_ctx.current_packet = 0;
    sender_ctx.data_ptr = data;
    sender_ctx.data_size = size;
    sender_ctx.state = SENDING_START;
    sender_ctx.retry_count = 0;
    sender_ctx.timeout = HAL_GetTick();
    
    // 发送起始帧
    return SendStartFrame(dest_did);
}

// 发送起始帧
 int SendStartFrame(uint16_t dest_did) {
    LargeDataFrame frame;
    frame.frame_type = START_FRAME;
    frame.session_id = sender_ctx.session_id;
    frame.packet_index = sender_ctx.total_packets; // 在起始帧中存储总包数
    frame.payload_length = 0;
    
    // 计算校验和
    frame.checksum = CRC16_Calculate((uint8_t*)&frame, sizeof(frame) - sizeof(frame.checksum));
#if DEBUG_CAN   
    // 发送帧
    if (FDCAN_SendMessage(FC_DATA_TRANSFER, dest_did, 0, 
                         (uint8_t*)&frame, sizeof(frame)) != HAL_OK) {
        return TRANSFER_ERROR;
    }
#endif
    sender_ctx.state = WAITING_ACK;
    sender_ctx.timeout = HAL_GetTick();
    return TRANSFER_OK;
}

// 发送数据帧
static int SendDataFrame(uint16_t dest_did) {
    LargeDataFrame frame;
    frame.frame_type = DATA_FRAME;
    frame.session_id = sender_ctx.session_id;
    frame.packet_index = sender_ctx.current_packet;
    
    // 计算当前包的有效载荷
    uint32_t offset = sender_ctx.current_packet * MAX_PAYLOAD;
    uint8_t payload_len = MAX_PAYLOAD;
    
    // 最后一包可能不足60字节
    if (offset + MAX_PAYLOAD > sender_ctx.data_size) {
        payload_len = sender_ctx.data_size - offset;
    }
    
    frame.payload_length = payload_len;
    memcpy(frame.payload, sender_ctx.data_ptr + offset, payload_len);
    
    // 计算校验和
    frame.checksum = CRC16_Calculate((uint8_t*)&frame, sizeof(frame) - sizeof(frame.checksum));
#if DEBUG_CAN   
    // 发送帧
    if (FDCAN_SendMessage(FC_DATA_TRANSFER, dest_did, 0, 
                         (uint8_t*)&frame, sizeof(frame)) != HAL_OK) {
        return TRANSFER_ERROR;
    }
#endif   
    sender_ctx.state = WAITING_ACK;
    sender_ctx.timeout = HAL_GetTick();
    return TRANSFER_OK;
}

// 发送结束帧
static int SendEndFrame(uint16_t dest_did) {
    LargeDataFrame frame;
    frame.frame_type = END_FRAME;
    frame.session_id = sender_ctx.session_id;
    frame.packet_index = 0;
    frame.payload_length = 0;
    
    // 计算校验和
    frame.checksum = CRC16_Calculate((uint8_t*)&frame, sizeof(frame) - sizeof(frame.checksum));
#if DEBUG_CAN     
    // 发送帧
    if (FDCAN_SendMessage(FC_DATA_TRANSFER, dest_did, 0, 
                         (uint8_t*)&frame, sizeof(frame)) != HAL_OK) {
        return TRANSFER_ERROR;
    }
#endif      
    sender_ctx.state = WAITING_ACK;
    sender_ctx.timeout = HAL_GetTick();
    return TRANSFER_OK;
}

// 处理ACK
void LargeDataTransfer_HandleACK(uint16_t src_did, LargeDataFrame *ack_frame) {
    if (sender_ctx.state != WAITING_ACK) {
        return;
    }
    
    // 验证会话ID
    if (ack_frame->session_id != sender_ctx.session_id) {
        return;
    }
    
    // 验证帧类型
    if (ack_frame->frame_type != ACK_FRAME) {
        return;
    }
    
    // 根据当前状态处理ACK
    switch (sender_ctx.state) {
        case WAITING_ACK:
            if (ack_frame->packet_index == ACK_START_OK) {
                // 起始帧ACK，开始发送数据
                sender_ctx.current_packet = 0;
                sender_ctx.state = SENDING_DATA;
                SendDataFrame(src_did);
            }
            else if (ack_frame->packet_index == ACK_DATA_OK) {
                // 数据帧ACK，发送下一包
                sender_ctx.current_packet++;
                
                if (sender_ctx.current_packet < sender_ctx.total_packets) {
                    SendDataFrame(src_did);
                } else {
                    // 所有数据包发送完成，发送结束帧
                    sender_ctx.state = SENDING_END;
                    SendEndFrame(src_did);
                }
            }
            else if (ack_frame->packet_index == ACK_END_OK) {
                // 结束帧ACK，传输完成
                sender_ctx.state = SENDER_IDLE;
#if DEBUG_CAN
                // 通知应用层传输完成
                if (transfer_complete_cb) {
                    transfer_complete_cb(TRANSFER_COMPLETE);
                }
#endif							
            }
            break;
            
        default:
            break;
    }
}

// 超时处理 (在主循环中调用)
void LargeDataTransfer_TimeoutHandler(void) {
	
    if (sender_ctx.state == WAITING_ACK && 
        (HAL_GetTick() - sender_ctx.timeout) > ACK_TIMEOUT) {
        
        sender_ctx.retry_count++;
#if DEBUG_CAN        
        if (sender_ctx.retry_count > MAX_RETRY_COUNT) {
            // 超过最大重试次数，放弃传输
            sender_ctx.state = SENDER_IDLE;
            if (transfer_complete_cb) {
                transfer_complete_cb(TRANSFER_TIMEOUT);
            }
            return;
        }
#endif        
        // 重试当前状态
        switch (sender_ctx.state) {
            case WAITING_ACK:
                if (sender_ctx.current_packet == 0) {
                    // 重发起始帧
                    sender_ctx.state = SENDING_START;
                    SendStartFrame(0); // 目标设备ID需要根据实际情况设置
                } else {
                    // 重发数据帧
                    sender_ctx.state = SENDING_DATA;
                    SendDataFrame(0); // 目标设备ID需要根据实际情况设置
                }
                break;
                
            default:
                break;
        }
    }
}