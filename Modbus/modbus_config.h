#ifndef __MODBUS_CONFIG_H
#define __MODBUS_CONFIG_H

#include "stm32h7xx.h"
#include "core_cm7.h"
#include "stm32h7xx_hal.h"

// Modbus配置参数
#define MODBUS_SLAVE_ADDR          0x01    // 默认从站地址
#define MODBUS_READ_HOLDING_REGS   0x03    // 读保持寄存器
#define MODBUS_WRITE_SINGLE_REG    0x06    // 写单个寄存器
#define MODBUS_WRITE_MULTI_REGS    0x10    // 写多个寄存器

// RS485配置
#define RS485_USART                USART1
#define RS485_DE_PORT              GPIOD
#define RS485_DE_PIN               GPIO_PIN_8

// 缓冲区大小
#define MODBUS_RX_BUFFER_SIZE      256
#define MODBUS_TX_BUFFER_SIZE      256
#define MESSAGE_QUEUE_SIZE         20

// 超时设置
#define MODBUS_RESPONSE_TIMEOUT    1000    // 响应超时(ms)
#define MODBUS_BYTE_TIMEOUT        5       // 字节间超时(ms)


#define MODBUS_FRAME_DELIMITER     3       // 帧间最小间隔(ms)
#define MODBUS_MAX_FRAME_LENGTH    256     // 最大帧长度

// 错误码
typedef enum {
    MODBUS_OK = 0,
    MODBUS_ERR_CRC,
    MODBUS_ERR_TIMEOUT,
    MODBUS_ERR_QUEUE_FULL,
    MODBUS_ERR_QUEUE_EMPTY,
    MODBUS_ERR_INVALID_RESPONSE
} Modbus_Status;

#endif