#include "rs485_driver.h"
#include "head.h"

// RS485方向控制引脚初始化在main.c中完成

void RS485_SetTxMode(void)
{
    HAL_GPIO_WritePin(RS485_DE_PORT, RS485_DE_PIN, GPIO_PIN_SET);
    HAL_Delay(1); // 确保状态稳定
}

void RS485_SetRxMode(void)
{
    HAL_Delay(1); // 确保发送完成
    HAL_GPIO_WritePin(RS485_DE_PORT, RS485_DE_PIN, GPIO_PIN_RESET);
}

void RS485_SendData(uint8_t *data, uint16_t size)
{
    RS485_SetTxMode();
    HAL_UART_Transmit(&huart1, data, size, 1000);
    RS485_SetRxMode();
}

uint16_t RS485_ReceiveData(uint8_t *buffer, uint16_t size)
{
    // 使用HAL_UART_Receive或在中断中处理
    return 0;
}

// Modbus CRC16计算
uint16_t Modbus_CRC16(uint8_t *data, uint16_t length)
{
    uint16_t crc = 0xFFFF;
    uint16_t i, j;
    
    for (i = 0; i < length; i++) {
        crc ^= data[i];
        for (j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc = crc >> 1;
            }
        }
    }
    
    return crc;
}

