

#include "head.h"
#include "AppUser_Sample.h"



volatile ChannelInfo g_Channelinfo[BOARD_CHANNEL_NUM]; // 通道信息
void AppUser_ChannelInfo_Debug(void);

void ADC_Filter(void);
/**********************************************************************
 * Function:      ADC_Filter
 * Description:   ADC滤波
 * Input:
 * Output:
 * Return:       void
 * Others:
 * Modify Date:    Version:    Author:         Modification:
 * -----------------------------------------------
 * 2023-5-9   V1.0        Hu Weiping
 **********************************************************************/

void ADC_Filter(void)
{
#if 1
    uint16_t ch;

    //ADC_A 通道滤波
    for (ch = 0; ch < ADC_1_CH_NUM_MAX; ch++)
    {
        g_SampleADC.ADC_1_CH_Sum[ch] = adc1Result[ch] + g_SampleADC.ADC_1_CH_Sum[ch] - (g_SampleADC.ADC_1_CH_Sum[ch] >> FILTER_NUM);
        g_SampleADC.ADC_1_CH_Num_Filter[ch] = g_SampleADC.ADC_1_CH_Sum[ch] >> FILTER_NUM;
    }
//    //ADC_C 通道滤波
    for (ch = 0; ch < ADC_2_CH_NUM_MAX; ch++)
    {
        g_SampleADC.ADC_2_CH_Sum[ch] = (adc2Result[ch] + g_SampleADC.ADC_2_CH_Sum[ch]) - (g_SampleADC.ADC_2_CH_Sum[ch] >> FILTER_NUM);
        g_SampleADC.ADC_2_CH_Num_Filter[ch] = g_SampleADC.ADC_2_CH_Sum[ch] >> FILTER_NUM;
    }
#endif
}

/**********************************************************************
 * Function:      ADC_Convert_Vaule
 * Description:   ADC转换化实际数据
 * Input:
 * Output:
 * Return:       void
 * Others:
 * Modify Date:    Version:    Author:         Modification:
 * -----------------------------------------------
 * 2023-5-9   V1.0        Hu Weiping
 **********************************************************************/

void ADC_Convert_Vaule(void)
{
    float adcvoltage;

}

void sample_irq_handler(void)
{
    GetADC_Driver_Result();
    ADC_Filter();        // ADC滤波
    ADC_Convert_Vaule(); // ADC转换化实际数据
}

void AppUser_Sample_Debug(void)
{

     printf("ADC1_A0=%d\r\n", adc1Result[ADC1_A0]);
     printf("ADC1_A1=%d\r\n", adc1Result[ADC1_A1]);
     printf("ADC1_A2=%d\r\n", adc1Result[ADC1_A2]);
     printf("ADC1_A3=%d\r\n", adc1Result[ADC1_A3]);

     printf("ADC1_A4=%d\r\n", adc1Result[ADC1_A4]);
     printf("ADC1_A5=%d\r\n", adc1Result[ADC1_A5]);

     printf("ADC1_A6=%d\r\n", adc1Result[ADC1_A6]);
     printf("ADC1_A7=%d\r\n", adc1Result[ADC1_A7]);
     printf("PC4 =%d\r\n", adc1Result[ADC1_A8]);

     printf("PC5 =%d\r\n", adc1Result[9]);

     printf("PB0 %d\r\n", adc1Result[10]);
     printf("PB1=%d\r\n", adc1Result[11]);
	
	
     printf("PC0 %d\r\n", adc1Result[12]);
     printf("PC1=%d\r\n", adc1Result[13]);
    AppUser_ChannelInfo_Debug();
}

void AppUser_ChannelInfo_Debug(void)
{

    // 峰值电流
//  DBG_PRINTF("LLC_IPeak =%f ADC1_A1=%d\r\n", g_Channelinfo[0].LLC_IPeak, adc1Result[ADC1_A8]);

//  // 采集输出电容电压
//  DBG_PRINTF("Cap_voltage =%f ADC1_A6=%d\r\n", g_Channelinfo[0].Cap_voltage, adc1Result[ADC1_A6]);
//  DBG_PRINTF("voltage =%f ADC1_A5=%d\r\n", g_Channelinfo[0].voltage, adc1Result[ADC1_A5]);
//  // 电流
//  DBG_PRINTF("+12V_IO_det1 =%f ADC1_A2=%d current_ADC=%fV\r\n", g_Channelinfo[0].current, adc1Result[ADC1_A2],g_Channelinfo[0].current_ADC );
//  DBG_PRINTF("+12V_IO_det2 =%f ADC1_A3=%d\r\n", g_Channelinfo[0].current2, adc1Result[ADC1_A3]);
//  // 12V电流
//  DBG_PRINTF("current_VABU_12V =%f ADC1_A4=%d\r\n", g_Channelinfo[0].current_VABU_12V, adc1Result[ADC1_A4]);

    // DBG_PRINTF("+Sense_VO_det1 =%f ADC2_A7=%d\r\n", g_Channelinfo[0].Sense_VO_det1, adc1Result[ADC2_A7]);
    // DBG_PRINTF("+Sense_VO_det2 =%f ADC1_B4=%d\r\n", g_Channelinfo[0].Sense_VO_det2, adc1Result[ADC1_B4]);


}



/*-----------------------------------------------------------
*  函数介绍    :    获取设备信息状态
*  入口参数    :    无
*  出口参数    :    无
*   备注       :
-----------------------------------------------------------*/
void Get_Send_dev_Data(void)
{

}