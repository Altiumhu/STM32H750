#include "crc.h"
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "message_handler.h"
#include "system_can_config.h"
// CRC16-CCITT多项式 (x^16 + x^12 + x^5 + 1)
#define CRC16_POLY 0x1021

// 计算CRC16校验和
uint16_t CRC16_Calculate(const uint8_t *data, uint32_t length) {
    uint16_t crc = 0xFFFF; // 初始值
    
    for (uint32_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;
        
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ CRC16_POLY;
            } else {
                crc <<= 1;
            }
        }
    }
    
    return crc;
}




