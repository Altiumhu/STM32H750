/* CD4052 模拟多路复用器驱动头文件
 * 4个CD4052并联控制，只用2个GPIO
 */

#ifndef __CD4052_H
#define __CD4052_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

// ADC原始值缓存 (从adc.c引用)
extern volatile uint16_t adc_values[14];

// ========== API函数 ==========

/**
 * @brief 初始化CD4052控制引脚
 */
void CD4052_Init(void);

/**
 * @brief 设置CD4052通道 (A、B引脚控制)
 * @param channel: 0-3
 */
void CD4052_SetChannel(uint8_t channel);

/**
 * @brief 获取当前通道
 */
uint8_t CD4052_GetChannel(void);

/**
 * @brief 扫描所有64路ADC通道 (阻塞式)
 */
void CD4052_ScanAllChannels(void);

/**
 * @brief 扫描电压16路
 */
void CD4052_ScanVoltageChannels(void);

/**
 * @brief 扫描电流16路  
 */
void CD4052_ScanCurrentChannels(void);

/**
 * @brief 非阻塞式处理 - 每次调用处理一个通道
 */
void CD4052_ProcessScan(void);

/**
 * @brief 获取指定通道的ADC值
 * @param channel: 0-63
 */
uint16_t CD4052_GetADCValue(uint8_t channel);

/**
 * @brief 获取电压通道值
 * @param channel: 1-16
 */
uint16_t CD4052_GetVoltage(uint8_t channel);

/**
 * @brief 获取电流通道值
 * @param channel: 1-16
 */
uint16_t CD4052_GetCurrent(uint8_t channel);

// ========== 全局变量 ==========

// 64路ADC采集结果
extern volatile uint16_t cd4052_adc_results[64];

// 通道映射:
// [0-15]   电压通道1-16
// [16-31]  电流通道1-16  
// [32-47]  预留
// [48-63]  预留

#ifdef __cplusplus
}
#endif

#endif /* __CD4052_H */
