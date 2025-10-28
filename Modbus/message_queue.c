

#include "message_queue.h"
#include <string.h>

void MessageQueue_Init(MessageQueue *queue)
{
    queue->front = 0;
    queue->rear = 0;
    queue->count = 0;
}

Modbus_Status MessageQueue_Enqueue(MessageQueue *queue, ModbusMessage *msg)
{
    if (MessageQueue_IsFull(queue)) {
        return MODBUS_ERR_QUEUE_FULL;
    }
    
    memcpy(&queue->messages[queue->rear], msg, sizeof(ModbusMessage));
    queue->rear = (queue->rear + 1) % MESSAGE_QUEUE_SIZE;
    queue->count++;
    
    return MODBUS_OK;
}

Modbus_Status MessageQueue_Dequeue(MessageQueue *queue, ModbusMessage *msg)
{
    if (MessageQueue_IsEmpty(queue)) {
        return MODBUS_ERR_QUEUE_EMPTY;
    }
    
    memcpy(msg, &queue->messages[queue->front], sizeof(ModbusMessage));
    queue->front = (queue->front + 1) % MESSAGE_QUEUE_SIZE;
    queue->count--;
    
    return MODBUS_OK;
}

uint8_t MessageQueue_IsEmpty(MessageQueue *queue)
{
    return (queue->count == 0);
}

uint8_t MessageQueue_IsFull(MessageQueue *queue)
{
    return (queue->count == MESSAGE_QUEUE_SIZE);
}