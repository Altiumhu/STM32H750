#include "modbus_master.h"
#include "rs485_driver.h"
#include <string.h>

static ModbusMaster g_modbus_master;
static uint8_t tx_buffer[MODBUS_TX_BUFFER_SIZE];
static uint8_t rx_buffer[MODBUS_RX_BUFFER_SIZE];
static uint16_t rx_index = 0;

void ModbusMaster_Init(ModbusMaster *master, UART_HandleTypeDef *huart)
{
    memset(master, 0, sizeof(ModbusMaster));
    MessageQueue_Init(&master->tx_queue);
    MessageQueue_Init(&master->rx_queue);
    master->huart = huart;
    master->is_busy = 0;

    RS485_Init(huart);
}

Modbus_Status Modbus_ReadHoldingRegisters(ModbusMaster *master, uint8_t slave_addr,
                                          uint16_t start_addr, uint16_t quantity,
                                          uint16_t *data, uint32_t timeout,
                                          void (*callback)(uint8_t, Modbus_Status, uint16_t *, uint16_t))
{
    ModbusMessage msg;

    if (MessageQueue_IsFull(&master->tx_queue))
    {
        return MODBUS_ERR_QUEUE_FULL;
    }

    msg.type = MSG_READ_HOLDING_REGISTERS;
    msg.slave_addr = slave_addr;
    msg.start_addr = start_addr;
    msg.quantity = quantity;
    msg.data = data;
    msg.data_count = 0;
    msg.timeout = timeout;
    msg.callback = callback;

    return MessageQueue_Enqueue(&master->tx_queue, &msg);
}

Modbus_Status Modbus_WriteSingleRegister(ModbusMaster *master, uint8_t slave_addr,
                                         uint16_t reg_addr, uint16_t value,
                                         uint32_t timeout,
                                         void (*callback)(uint8_t, Modbus_Status, uint16_t *, uint16_t))
{
    ModbusMessage msg;
    static uint16_t temp_data;

    if (MessageQueue_IsFull(&master->tx_queue))
    {
        return MODBUS_ERR_QUEUE_FULL;
    }

    temp_data = value;

    msg.type = MSG_WRITE_SINGLE_REGISTER;
    msg.slave_addr = slave_addr;
    msg.start_addr = reg_addr;
    msg.quantity = 1;
    msg.data = &temp_data;
    msg.data_count = 1;
    msg.timeout = timeout;
    msg.callback = callback;

    return MessageQueue_Enqueue(&master->tx_queue, &msg);
}

Modbus_Status Modbus_WriteMultipleRegisters(ModbusMaster *master, uint8_t slave_addr,
                                            uint16_t start_addr, uint16_t *values,
                                            uint16_t count, uint32_t timeout,
                                            void (*callback)(uint8_t, Modbus_Status, uint16_t *, uint16_t))
{
    ModbusMessage msg;

    if (MessageQueue_IsFull(&master->tx_queue))
    {
        return MODBUS_ERR_QUEUE_FULL;
    }

    msg.type = MSG_WRITE_MULTIPLE_REGISTERS;
    msg.slave_addr = slave_addr;
    msg.start_addr = start_addr;
    msg.quantity = count;
    msg.data = values;
    msg.data_count = count;
    msg.timeout = timeout;
    msg.callback = callback;

    return MessageQueue_Enqueue(&master->tx_queue, &msg);
}

static void SendModbusRequest(ModbusMaster *master, ModbusMessage *msg)
{
    uint16_t crc;
    uint8_t *p = tx_buffer;

    switch (msg->type)
    {
    case MSG_READ_HOLDING_REGISTERS:
        *p++ = msg->slave_addr;
        *p++ = MODBUS_READ_HOLDING_REGS;
        *p++ = (msg->start_addr >> 8) & 0xFF;
        *p++ = msg->start_addr & 0xFF;
        *p++ = (msg->quantity >> 8) & 0xFF;
        *p++ = msg->quantity & 0xFF;
        break;

    case MSG_WRITE_SINGLE_REGISTER:
        *p++ = msg->slave_addr;
        *p++ = MODBUS_WRITE_SINGLE_REG;
        *p++ = (msg->start_addr >> 8) & 0xFF;
        *p++ = msg->start_addr & 0xFF;
        *p++ = (msg->data[0] >> 8) & 0xFF;
        *p++ = msg->data[0] & 0xFF;
        break;

    case MSG_WRITE_MULTIPLE_REGISTERS:
        *p++ = msg->slave_addr;
        *p++ = MODBUS_WRITE_MULTI_REGS;
        *p++ = (msg->start_addr >> 8) & 0xFF;
        *p++ = msg->start_addr & 0xFF;
        *p++ = (msg->quantity >> 8) & 0xFF;
        *p++ = msg->quantity & 0xFF;
        *p++ = msg->quantity * 2;
        for (int i = 0; i < msg->quantity; i++)
        {
            *p++ = (msg->data[i] >> 8) & 0xFF;
            *p++ = msg->data[i] & 0xFF;
        }
        break;
    }

    // 计算CRC
    crc = Modbus_CRC16(tx_buffer, p - tx_buffer);
    *p++ = crc & 0xFF;
    *p++ = (crc >> 8) & 0xFF;

    // 发送数据
    RS485_SendData(tx_buffer, p - tx_buffer);
    master->is_busy = 1;
    master->last_byte_time = HAL_GetTick();
    rx_index = 0;
}

static Modbus_Status ParseModbusResponse(ModbusMessage *req, uint8_t *data, uint16_t length)
{
    uint16_t crc_calc, crc_received;

    if (length < 5)
    { // 最小响应长度
        return MODBUS_ERR_INVALID_RESPONSE;
    }

    // 检查CRC
    crc_calc = Modbus_CRC16(data, length - 2);
    crc_received = data[length - 2] | (data[length - 1] << 8);

    if (crc_calc != crc_received)
    {
        return MODBUS_ERR_CRC;
    }

    // 检查从站地址和功能码
    if (data[0] != req->slave_addr)
    {
        return MODBUS_ERR_INVALID_RESPONSE;
    }

    // 检查异常响应
    if (data[1] & 0x80)
    {
        return MODBUS_ERR_INVALID_RESPONSE;
    }

    // 解析响应数据
    switch (req->type)
    {
    case MSG_READ_HOLDING_REGISTERS:
        if (data[1] != MODBUS_READ_HOLDING_REGS)
        {
            return MODBUS_ERR_INVALID_RESPONSE;
        }
        // 提取寄存器数据
        for (int i = 0; i < req->quantity; i++)
        {
            req->data[i] = (data[3 + i * 2] << 8) | data[4 + i * 2];
        }
        break;

    case MSG_WRITE_SINGLE_REGISTER:
        if (data[1] != MODBUS_WRITE_SINGLE_REG)
        {
            return MODBUS_ERR_INVALID_RESPONSE;
        }
        break;

    case MSG_WRITE_MULTIPLE_REGISTERS:
        if (data[1] != MODBUS_WRITE_MULTI_REGS)
        {
            return MODBUS_ERR_INVALID_RESPONSE;
        }
        break;
    }

    return MODBUS_OK;
}

void ModbusMaster_Process(ModbusMaster *master)
{
    ModbusMessage msg;

    // 如果当前不忙且队列中有消息，发送下一条
    if (!master->is_busy && !MessageQueue_IsEmpty(&master->tx_queue))
    {
        if (MessageQueue_Dequeue(&master->tx_queue, &msg) == MODBUS_OK)
        {
            SendModbusRequest(master, &msg);
            // 将消息添加到响应等待队列
            MessageQueue_Enqueue(&master->rx_queue, &msg);
        }
    }

    // 处理超时
    ModbusMaster_TimeoutHandler(master);
}

void ModbusMaster_RxCompleteCallback(ModbusMaster *master)
{
    ModbusMessage req_msg;
    Modbus_Status status;

    if (MessageQueue_IsEmpty(&master->rx_queue))
    {
        return;
    }

    // 获取等待响应的消息
    if (MessageQueue_Dequeue(&master->rx_queue, &req_msg) == MODBUS_OK)
    {
        status = ParseModbusResponse(&req_msg, rx_buffer, rx_index);

        // 调用回调函数
        if (req_msg.callback)
        {
            req_msg.callback(req_msg.slave_addr, status, req_msg.data, req_msg.quantity);
        }
    }

    master->is_busy = 0;
    rx_index = 0;
}

void ModbusMaster_TimeoutHandler(ModbusMaster *master)
{
    if (master->is_busy)
    {
        if ((HAL_GetTick() - master->last_byte_time) > MODBUS_RESPONSE_TIMEOUT)
        {
            // 超时处理
            ModbusMessage req_msg;
            if (MessageQueue_Dequeue(&master->rx_queue, &req_msg) == MODBUS_OK)
            {
                if (req_msg.callback)
                {
                    req_msg.callback(req_msg.slave_addr, MODBUS_ERR_TIMEOUT, NULL, 0);
                }
            }
            master->is_busy = 0;
            rx_index = 0;
        }
    }
}

// UART接收中断回调
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == RS485_USART)
    {
        g_modbus_master.last_byte_time = HAL_GetTick();

        // 这里需要实现数据接收逻辑
        // 通常使用DMA或中断接收

        // 检查是否收到完整帧（通过帧间超时）
        // 如果收到完整帧，调用 ModbusMaster_RxCompleteCallback(&g_modbus_master);
    }
}