#ifndef LARGE_DATA_TRANSFER_H
#define LARGE_DATA_TRANSFER_H

#include <stdint.h>

// 配置参数
#define MAX_PAYLOAD          60     // 每帧最大有效载荷
#define MAX_LARGE_DATA_SIZE  65535  // 最大支持64KB数据
#define ACK_TIMEOUT          100    // ACK超时时间(ms)
#define SESSION_TIMEOUT      5000   // 会话超时时间(ms)
#define MAX_RETRY_COUNT      5      // 最大重试次数


 #define CAN_MSG_BUG 0

// 传输状态码
typedef enum {
    TRANSFER_OK,
    TRANSFER_BUSY,
    TRANSFER_ERROR,
    TRANSFER_TIMEOUT,
    TRANSFER_TOO_LARGE,
    TRANSFER_COMPLETE
} TransferStatus;

// ACK状态码
typedef enum {
    ACK_START_OK = 0x0001,
    ACK_DATA_OK  = 0x0002,
    ACK_END_OK   = 0x0004,
    ACK_CHECKSUM_ERROR   = 0x1000,
    ACK_SESSION_EXISTS   = 0x1001,
    ACK_NO_RESOURCES     = 0x1002,
    ACK_NO_MEMORY        = 0x1003,
    ACK_SESSION_INVALID  = 0x1004,
    ACK_INVALID_INDEX    = 0x1005,
    ACK_BUFFER_OVERFLOW  = 0x1006,
    ACK_MISSING_PACKETS  = 0x1007
} ACKCode;

// 大数据帧结构
typedef struct {
    uint16_t session_id;
    uint16_t packet_index;
    uint8_t  frame_type;
    uint8_t  payload_length;
    uint8_t  payload[60];
    uint16_t checksum;
} LargeDataFrame;

// 回调函数类型
typedef void (*TransferCompleteCallback)(TransferStatus status);
typedef void (*DataReceivedCallback)(uint16_t src_did, uint16_t session_id, 
                                    uint8_t *data, uint32_t size);
typedef void (*TransferFailedCallback)(uint16_t session_id, TransferStatus reason);

// 公共接口
void LargeDataTransfer_InitSender(void);
void LargeDataTransfer_InitReceiver(void);
int LargeDataTransfer_Send(uint16_t dest_did, uint8_t *data, uint32_t size);
void LargeDataTransfer_HandleFrame(uint16_t src_did, LargeDataFrame *frame);
void LargeDataTransfer_HandleACK(uint16_t src_did, LargeDataFrame *ack_frame);
void LargeDataTransfer_TimeoutHandler(void);
void LargeDataTransfer_SessionTimeoutHandler(void);

// 设置回调函数
void LargeDataTransfer_SetCompleteCallback(TransferCompleteCallback cb);
void LargeDataTransfer_SetReceivedCallback(DataReceivedCallback cb);
void LargeDataTransfer_SetFailedCallback(TransferFailedCallback cb);

// CRC函数
uint16_t CRC16_Calculate(const uint8_t *data, uint32_t length);



#endif // LARGE_DATA_TRANSFER_H

