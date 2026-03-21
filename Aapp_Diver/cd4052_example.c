/* CD4052 测试和示例代码
 * 在 main.c 的主循环中调用
 */

#include "cd4052.h"
#include "adc.h"
#include "stdio.h"

// 示例: 在定时器中断或主循环中调用
void CD4052_Test(void)
{
    // 方式1: 在主循环中定期调用 (推荐)
    CD4052_ScanAllChannels();  // 阻塞式扫描所有64路
    
    // 方式2: 非阻塞方式 - 每次ADC完成回调后处理一个通道
    // CD4052_ProcessScan();
    
    // 获取结果示例:
    uint16_t v1 = CD4052_GetVoltage(1);  // 电压通道1
    uint16_t c1 = CD4052_GetCurrent(1);   // 电流通道1
    
    printf("V1=%d, V2=%d, V3=%d, V4=%d\r\n", 
        CD4052_GetVoltage(1), CD4052_GetVoltage(2),
        CD4052_GetVoltage(3), CD4052_GetVoltage(4));
        
    printf("I1=%d, I2=%d, I3=%d, I4=%d\r\n", 
        CD4052_GetCurrent(1), CD4052_GetCurrent(2),
        CD4052_GetCurrent(3), CD4052_GetCurrent(4));
}

// 使用建议:
// 1. 在 sysinitstartup.c 的 AppUser_prvSetupHardware() 中调用 CD4052_Init()
// 2. 在定时器回调或主循环中调用 CD4052_ScanAllChannels() 或 CD4052_ProcessScan()
// 3. 使用 CD4052_GetVoltage(ch) 和 CD4052_GetCurrent(ch) 获取ADC值
