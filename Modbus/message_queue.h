#ifndef __MESSAGE_QUEUE_H
#define __MESSAGE_QUEUE_H

#include "modbus_config.h"

// 消息类型
typedef enum {
    MSG_READ_HOLDING_REGISTERS,
    MSG_WRITE_SINGLE_REGISTER,
    MSG_WRITE_MULTIPLE_REGISTERS
} MessageType;

// Modbus消息结构
typedef struct {
    MessageType type;
    uint8_t slave_addr;
    uint16_t start_addr;
    uint16_t quantity;
    uint16_t *data;
    uint16_t data_count;
    uint32_t timeout;
    void (*callback)(uint8_t, Modbus_Status, uint16_t*, uint16_t);
} ModbusMessage;

// 消息队列结构
typedef struct {
    ModbusMessage messages[MESSAGE_QUEUE_SIZE];
    uint8_t front;
    uint8_t rear;
    uint8_t count;
} MessageQueue;

// 函数声明
void MessageQueue_Init(MessageQueue *queue);
Modbus_Status MessageQueue_Enqueue(MessageQueue *queue, ModbusMessage *msg);
Modbus_Status MessageQueue_Dequeue(MessageQueue *queue, ModbusMessage *msg);
uint8_t MessageQueue_IsEmpty(MessageQueue *queue);
uint8_t MessageQueue_IsFull(MessageQueue *queue);

#endif