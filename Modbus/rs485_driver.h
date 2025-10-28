#ifndef __RS485_DRIVER_H
#define __RS485_DRIVER_H

#include "modbus_config.h"

// RS485驱动函数
void RS485_Init(UART_HandleTypeDef *huart);
void RS485_SetTxMode(void);
void RS485_SetRxMode(void);
void RS485_SendData(uint8_t *data, uint16_t size);
uint16_t RS485_ReceiveData(uint8_t *buffer, uint16_t size);

// CRC计算
uint16_t Modbus_CRC16(uint8_t *data, uint16_t length);

#endif