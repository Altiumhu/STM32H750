/* CD4052 模拟多路复用器驱动 (简化版)
 * 4个CD4052的A、B控制引脚并联
 * 只需PB12(A)、PB15(B)两个控制引脚
 */

#include "cd4052.h"
#include "main.h"

// ========== 硬件定义 ==========

// CD4052控制引脚 (所有芯片并联)
#define CD4052_A_GPIO_PORT  GPIOB
#define CD4052_A_PIN        GPIO_PIN_12
#define CD4052_B_GPIO_PORT  GPIOB
#define CD4052_B_PIN        GPIO_PIN_15

// ========== 全局变量 ==========

// 64路ADC采集结果
// [0-15] 电压通道1-16
// [16-31] 电流通道1-16  
// [32-63] 预留
volatile uint16_t cd4052_adc_results[64] = {0};

// 当前通道索引 (0-3)
static uint8_t cd4052_current_channel = 0;

// ========== 函数实现 ==========

/**
 * @brief 初始化CD4052控制引脚
 */
void CD4052_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // 使能GPIOB时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    // 配置控制引脚为输出
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    
    GPIO_InitStruct.Pin = CD4052_A_PIN;
    HAL_GPIO_Init(CD4052_A_GPIO_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = CD4052_B_PIN;
    HAL_GPIO_Init(CD4052_B_GPIO_PORT, &GPIO_InitStruct);
    
    // 初始选择通道0
    CD4052_SetChannel(0);
}

/**
 * @brief 设置CD4052通道 (A、B引脚控制)
 * @param channel: 0-3
 */
void CD4052_SetChannel(uint8_t channel)
{
    if (channel > 3) channel = 3;
    cd4052_current_channel = channel;
    
    // A = channel bit0, B = channel bit1
    HAL_GPIO_WritePin(CD4052_A_GPIO_PORT, CD4052_A_PIN, 
                      (GPIO_PinState)(channel & 0x01));
    HAL_GPIO_WritePin(CD4052_B_GPIO_PORT, CD4052_B_PIN, 
                      (GPIO_PinState)((channel >> 1) & 0x01));
}

/**
 * @brief 获取当前通道
 */
uint8_t CD4052_GetChannel(void)
{
    return cd4052_current_channel;
}

/**
 * @brief 扫描所有64路ADC通道 (阻塞式)
 * @note 调用前确保ADC已启动DMA采集
 *       函数内会切换通道并等待ADC数据更新
 */
void CD4052_ScanAllChannels(void)
{
    uint8_t ch;
    
    // ========== 电压采集 (CD4052 #1 和 #2) ==========
    // CD4052_V1: X→PA1(ADC1_IN17), Y→PA0(ADC2_IN16) → 1-8通道
    // CD4052_V2: X→PA3(ADC1_IN15), Y→PA2(ADC2_IN14) → 9-16通道
    
    for (ch = 0; ch < 4; ch++)
    {
        // 切换通道
        CD4052_SetChannel(ch);
        
        // 等待模拟开关切换稳定 + ADC转换完成
        HAL_Delay(1);
        
        // 读取电压ADC值 (根据你的硬件连接)
        // 电压通道 1-4 (Y: CD4052_V1)
        cd4052_adc_results[ch * 4 + 0] = adc_values[0];  // PA0
        // 电压通道 5-8 (X: CD4052_V1)
        cd4052_adc_results[ch * 4 + 1] = adc_values[1];  // PA1
        // 电压通道 9-12 (Y: CD4052_V2)
        cd4052_adc_results[ch * 4 + 2] = adc_values[2];  // PA2
        // 电压通道 13-16 (X: CD4052_V2)
        cd4052_adc_results[ch * 4 + 3] = adc_values[3];  // PA3
    }
    
    // ========== 电流采集 (CD4052 #3 和 #4) ==========
    // CD4052_I1: X→PA5(ADC1_IN19), Y→PA4(ADC2_IN18) → 17-24通道
    // CD4052_I2: X→PA7(ADC1_IN7), Y→PA6(ADC2_IN3) → 25-32通道
    
    for (ch = 0; ch < 4; ch++)
    {
        // 切换通道
        CD4052_SetChannel(ch);
        
        // 等待稳定
        HAL_Delay(1);
        
        // 读取电流ADC值
        // 电流通道 1-4 (Y: CD4052_I1)
        cd4052_adc_results[16 + ch * 4 + 0] = adc_values[4];  // PA4
        // 电流通道 5-8 (X: CD4052_I1)
        cd4052_adc_results[16 + ch * 4 + 1] = adc_values[5];  // PA5
        // 电流通道 9-12 (Y: CD4052_I2)
        cd4052_adc_results[16 + ch * 4 + 2] = adc_values[6];  // PA6
        // 电流通道 13-16 (X: CD4052_I2)
        cd4052_adc_results[16 + ch * 4 + 3] = adc_values[7];  // PA7
    }
}

/**
 * @brief 只扫描电压16路
 */
void CD4052_ScanVoltageChannels(void)
{
    uint8_t ch;
    
    for (ch = 0; ch < 4; ch++)
    {
        CD4052_SetChannel(ch);
        HAL_Delay(1);
        
        cd4052_adc_results[ch * 4 + 0] = adc_values[0];  // 1-4
        cd4052_adc_results[ch * 4 + 1] = adc_values[1];  // 5-8
        cd4052_adc_results[ch * 4 + 2] = adc_values[2];  // 9-12
        cd4052_adc_results[ch * 4 + 3] = adc_values[3];  // 13-16
    }
}

/**
 * @brief 只扫描电流16路
 */
void CD4052_ScanCurrentChannels(void)
{
    uint8_t ch;
    
    for (ch = 0; ch < 4; ch++)
    {
        CD4052_SetChannel(ch);
        HAL_Delay(1);
        
        cd4052_adc_results[16 + ch * 4 + 0] = adc_values[4];
        cd4052_adc_results[16 + ch * 4 + 1] = adc_values[5];
        cd4052_adc_results[16 + ch * 4 + 2] = adc_values[6];
        cd4052_adc_results[16 + ch * 4 + 3] = adc_values[7];
    }
}

/**
 * @brief 非阻塞式处理 - 每次调用处理一个通道
 * @note 需要在ADC转换完成回调中或定时器中循环调用
 */
void CD4052_ProcessScan(void)
{
    static uint8_t state = 0;  // 0-3: 电压扫描, 4-7: 电流扫描
    
    if (state < 4)
    {
        // 电压扫描阶段
        CD4052_SetChannel(state);
        HAL_Delay(1);
        
        cd4052_adc_results[state * 4 + 0] = adc_values[0];
        cd4052_adc_results[state * 4 + 1] = adc_values[1];
        cd4052_adc_results[state * 4 + 2] = adc_values[2];
        cd4052_adc_results[state * 4 + 3] = adc_values[3];
        
        state++;
    }
    else if (state < 8)
    {
        // 电流扫描阶段
        uint8_t ch = state - 4;
        CD4052_SetChannel(ch);
        HAL_Delay(1);
        
        cd4052_adc_results[16 + ch * 4 + 0] = adc_values[4];
        cd4052_adc_results[16 + ch * 4 + 1] = adc_values[5];
        cd4052_adc_results[16 + ch * 4 + 2] = adc_values[6];
        cd4052_adc_results[16 + ch * 4 + 3] = adc_values[7];
        
        state++;
    }
    else
    {
        // 完成一轮扫描
        state = 0;
    }
}

/**
 * @brief 获取指定通道的ADC值
 * @param channel: 0-63
 */
uint16_t CD4052_GetADCValue(uint8_t channel)
{
    if (channel >= 64) return 0;
    return cd4052_adc_results[channel];
}

/**
 * @brief 获取电压通道值
 * @param channel: 1-16
 */
uint16_t CD4052_GetVoltage(uint8_t channel)
{
    if (channel < 1 || channel > 16) return 0;
    return cd4052_adc_results[channel - 1];
}

/**
 * @brief 获取电流通道值
 * @param channel: 1-16
 */
uint16_t CD4052_GetCurrent(uint8_t channel)
{
    if (channel < 1 || channel > 16) return 0;
    return cd4052_adc_results[16 + channel - 1];
}
