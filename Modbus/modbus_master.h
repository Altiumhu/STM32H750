#ifndef __MODBUS_MASTER_H
#define __MODBUS_MASTER_H

#include "message_queue.h"

// Modbus主站结构
typedef struct {
    MessageQueue tx_queue;
    MessageQueue rx_queue;
    UART_HandleTypeDef *huart;
    uint8_t is_busy;
    uint32_t last_byte_time;
    uint16_t timeout_counter;
} ModbusMaster;

// 函数声明
void ModbusMaster_Init(ModbusMaster *master, UART_HandleTypeDef *huart);
Modbus_Status Modbus_ReadHoldingRegisters(ModbusMaster *master, uint8_t slave_addr, 
                                         uint16_t start_addr, uint16_t quantity,
                                         uint16_t *data, uint32_t timeout,
                                         void (*callback)(uint8_t, Modbus_Status, uint16_t*, uint16_t));
Modbus_Status Modbus_WriteSingleRegister(ModbusMaster *master, uint8_t slave_addr,
                                        uint16_t reg_addr, uint16_t value,
                                        uint32_t timeout,
                                        void (*callback)(uint8_t, Modbus_Status, uint16_t*, uint16_t));
Modbus_Status Modbus_WriteMultipleRegisters(ModbusMaster *master, uint8_t slave_addr,
                                           uint16_t start_addr, uint16_t *values,
                                           uint16_t count, uint32_t timeout,
                                           void (*callback)(uint8_t, Modbus_Status, uint16_t*, uint16_t));
void ModbusMaster_Process(ModbusMaster *master);
void ModbusMaster_RxCompleteCallback(ModbusMaster *master);
void ModbusMaster_TimeoutHandler(ModbusMaster *master);

#endif