

#include "head.h"
#include "app_drive_adc.h"

uint16_t adc1Result[ADC_1_CH_NUM_MAX];
uint16_t adc2Result[ADC_2_CH_NUM_MAX];


SampleADC g_SampleADC; //



short adc3_get_temperature(void)
{


    return 0;
}

void  GetADC_Driver_Result(void)
{
    // 启动转换前刷新Cache
    SCB_CleanDCache_by_Addr((uint32_t *)adc_values, sizeof(adc_values));

    // 主循环中读取ADC值
    adc1Result[0] = ADC_GetValue(1); // PA0
    adc1Result[1] = ADC_GetValue(2); // PA1
    adc1Result[2] = ADC_GetValue(3); // PA2
    adc1Result[3] = ADC_GetValue(4); // PA3
    adc1Result[4] = ADC_GetValue(5); // PA4
    adc1Result[5] = ADC_GetValue(6); // PA5
    adc1Result[6] = ADC_GetValue(7); // PA6
    adc1Result[7] = ADC_GetValue(8); // PA7
    adc1Result[8] = ADC_GetValue(9); // PC0

    // 读取后使Cache无效
    SCB_InvalidateDCache_by_Addr((uint32_t *)adc_values, sizeof(adc_values));
}