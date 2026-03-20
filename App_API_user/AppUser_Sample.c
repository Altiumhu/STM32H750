/**
 **************************************************************************************************
 * @file        AppUser_Sample.c
 * @brief       ADC采样数据处理模块
 *
 * @description:
 *              - ADC1: 14通道DMA连续采集(电压/电流/电容电压)
 *              - ADC3: 2通道DMA连续采集(温度传感器等)
 *              - CD4052多路复用器控制实现16通道轮询
 *              - 支持数据滤波和实际值转换
 *
 * @hardware:   STM32F750VBT6
 * @author      Hu Weiping
 * @date        2023-05-09
 * @version     V1.0
 **************************************************************************************************
 */

#include "head.h"
#include "AppUser_Sample.h"

/*=================================================================================================
 * 全局变量定义
 *================================================================================================*/
volatile ChannelInfo g_Channelinfo[BOARD_CHANNEL_NUM]; // 通道信息结构体数组

/*=================================================================================================
 * 函数声明
 *================================================================================================*/
void AppUser_ChannelInfo_Debug(void);
void ADC_Filter(void);

/*=================================================================================================
 * ADC通道映射表 (adc_values[] -> adc1Result[])
 *================================================================================================*
 * adc_values[]: DMA缓冲区原始数据 (14通道)
 * adc1Result[]: 映射后的应用数据
 *
 * | adc1Result索引 | adc_values索引 | 引脚  | 描述           |
 * |----------------|----------------|-------|----------------|
 * |      0        |       0        | PA0   | 电压ADC        |
 * |      1        |       1        | PA1   | 电压ADC        |
 * |      2        |       2        | PA2   | 电压ADC        |
 * |      3        |       3        | PA3   | 电压ADC        |
 * |      4        |       4        | PA4   | 电流ADC        |
 * |      5        |       5        | PA5   | 电流ADC        |
 * |      6        |       6        | PA6   | 电流ADC        |
 * |      7        |       7        | PA7   | 电流ADC        |
 * |      8        |       8        | PC4   | 直流电流ADC    |
 * |      9        |       9        | PC5   | 直流电流ADC    |
 * |     10        |      10        | PB0   | 直流电流ADC    |
 * |     11        |      11        | PB1   | 直流电流ADC    |
 * |     12        |      12        | PC0   | 电容/端口电压  |
 * |     13        |      13        | PC1   | 电容/端口电压  |
 *================================================================================================*/

/*=================================================================================================
 * 函数名称:  GetADC_Driver_Result
 * 功能描述:  从DMA缓冲区读取ADC数据并映射到应用结构体
 * 说明:      - 每10次调用切换一次CD4052多路复用器通道
 *            - 实现16通道(4组x4通道)的轮询采样
 *            - 需要在DMA中断中调用或主循环中轮询
 *================================================================================================*/
void GetADC_Driver_Result(void)
{
    /* 静态变量: 通道切换控制 */
    static uint16_t CD4052_Chnum = 0;                    // CD4052通道编号 (0-3)
    static uint16_t Samp_ChnumTimer = 0, delayTimer = 0; // 通道切换计时器

    /* DMA传输后清理D-Cache,确保读取到最新数据 */
    SCB_CleanDCache_by_Addr((uint32_t *)adc_values, sizeof(adc_values));

    /* 根据CD4052通道号读取对应的ADC数据到g_Channelinfo */
    switch (CD4052_Chnum)
    {
    /*-------------------------- CD4052 通道0: 通道1,5,9,13 --------------------------*/
    case 0:
        Samp_ChnumTimer++;
        if (Samp_ChnumTimer >= 5)
        {
            CD4052_Chnum = 1;
            Samp_ChnumTimer = 0;
        }

        delayTimer++;
        if (delayTimer > 1) // 延时一个开关周期 等待CD4052开关稳定
        {
            delayTimer = 2;
            /* 电压采样 */
            g_Channelinfo[0].voltage_ADC = adc1Result[0];  // 通道1 电压
            g_Channelinfo[4].voltage_ADC = adc1Result[1];  // 通道5 电压
            g_Channelinfo[8].voltage_ADC = adc1Result[2];  // 通道9 电压
            g_Channelinfo[12].voltage_ADC = adc1Result[3]; // 通道13 电压
            /* 通道1电压: 分压系数2.2 */
            g_Channelinfo[0].voltage = g_Channelinfo[0].voltage_ADC * 0.050354772f * 2.2f;
            g_Channelinfo[4].voltage = g_Channelinfo[4].voltage_ADC * 0.050354772f * 2.2f;
            g_Channelinfo[8].voltage = g_Channelinfo[8].voltage_ADC * 0.050354772f * 2.2f;
            g_Channelinfo[13].voltage = g_Channelinfo[13].voltage_ADC * 0.050354772f * 2.2f;

            /* 充电电流采样 */
            g_Channelinfo[0].current_ADC = adc1Result[4];  // 通道1 电流
            g_Channelinfo[4].current_ADC = adc1Result[5];  // 通道5 电流
            g_Channelinfo[8].current_ADC = adc1Result[6];  // 通道9 电流
            g_Channelinfo[12].current_ADC = adc1Result[7]; // 通道13 电流

            /* 放电流电流采样 */
            g_Channelinfo[0].current_DC_ADC = adc1Result[8];   // 通道1 直流电流
            g_Channelinfo[4].current_DC_ADC = adc1Result[9];   // 通道5 直流电流
            g_Channelinfo[8].current_DC_ADC = adc1Result[10];  // 通道9 直流电流
            g_Channelinfo[12].current_DC_ADC = adc1Result[11]; // 通道13 直流电流

            /* 通道1电流: 采样电阻0.1Ω, 放大倍数 */
            if (g_Channelinfo[0].workMode == POWER_RUN_DISCHARGE)
            {
                g_Channelinfo[0].current = g_Channelinfo[0].current_DC_ADC * 0.000335088f;
                g_Channelinfo[4].current = g_Channelinfo[4].current_DC_ADC * 0.000335088f;
                g_Channelinfo[8].current = g_Channelinfo[8].current_DC_ADC * 0.000335088f;
                g_Channelinfo[12].current = g_Channelinfo[12].current_DC_ADC * 0.000335088f;
            }
            else
            {
                g_Channelinfo[0].current = g_Channelinfo[0].current_ADC * 0.000335088f;
                g_Channelinfo[4].current = g_Channelinfo[4].current_ADC * 0.000335088f;
                g_Channelinfo[8].current = g_Channelinfo[8].current_ADC * 0.000335088f;
                g_Channelinfo[12].current = g_Channelinfo[12].current_ADC * 0.000335088f;
            }
        }

        /* 电容端电压/端口电压采样 */
        if (g_Channelinfo[0].workMode == POWER_PRECHARGE || g_Channelinfo[0].workMode == POWER_INIT)
        {
            Set_Sample_Channel_VPortGPIO(AD_V_CAP_EN);         // 切换到电容电压采样
            g_Channelinfo[0].Cap_voltage_ADC = adc1Result[12]; // 通道1 电容电压
            g_Channelinfo[4].Cap_voltage_ADC = adc1Result[13]; // 通道5 电容电压
                                                               /* 通道1电容电压: 分压系数3.033333 */
            g_Channelinfo[0].Cap_voltage = g_Channelinfo[0].Cap_voltage_ADC * 0.050354772f * 3.033333f;
            g_Channelinfo[4].Cap_voltage = g_Channelinfo[4].Cap_voltage_ADC * 0.050354772f * 3.033333f;
        }
        else if (g_Channelinfo[0].workMode == POWER_FAULT || g_Channelinfo[0].workMode == POWER_RUN_CHARGE)
        {
            Set_Sample_Channel_VPortGPIO(AD_V_PORT);            // 切换到端口电压采样
            g_Channelinfo[0].voltage_port_ADC = adc1Result[12]; // 通道1 端口电压
            g_Channelinfo[4].voltage_port_ADC = adc1Result[13]; // 通道5 端口电压
            g_Channelinfo[0].Cap_voltage = g_Channelinfo[0].Cap_voltage_ADC * 0.050354772f * 3.033333f;
            g_Channelinfo[4].Cap_voltage = g_Channelinfo[4].Cap_voltage_ADC * 0.050354772f * 3.033333f;
        }

        SetCD4052(CD4052_Chnum);
        break;

    /*-------------------------- CD4052 通道1: 通道2,6,10,14 -------------------------*/
    case 1:
        Samp_ChnumTimer++;
        if (Samp_ChnumTimer >= 10)
        {
            CD4052_Chnum = 2;
            Samp_ChnumTimer = 0;
        }

        /* 电压采样 */
        g_Channelinfo[1].voltage_ADC = adc1Result[0];  // 通道1 电压
        g_Channelinfo[5].voltage_ADC = adc1Result[1];  // 通道5 电压
        g_Channelinfo[9].voltage_ADC = adc1Result[2];  // 通道9 电压
        g_Channelinfo[13].voltage_ADC = adc1Result[3]; // 通道13 电压
        /* 通道1电压: 分压系数2.2 */
        g_Channelinfo[1].voltage = g_Channelinfo[1].voltage_ADC * 0.050354772f * 2.2f;
        g_Channelinfo[5].voltage = g_Channelinfo[5].voltage_ADC * 0.050354772f * 2.2f;
        g_Channelinfo[9].voltage = g_Channelinfo[9].voltage_ADC * 0.050354772f * 2.2f;
        g_Channelinfo[13].voltage = g_Channelinfo[13].voltage_ADC * 0.050354772f * 2.2f;

        /* 充电电流采样 */
        g_Channelinfo[1].current_ADC = adc1Result[4];  // 通道2 电流
        g_Channelinfo[5].current_ADC = adc1Result[5];  // 通道6 电流
        g_Channelinfo[9].current_ADC = adc1Result[6];  // 通道10 电流
        g_Channelinfo[13].current_ADC = adc1Result[7]; // 通道14 电流

        /* 放电流电流采样 */
        g_Channelinfo[1].current_DC_ADC = adc1Result[8];   // 通道2 直流电流
        g_Channelinfo[5].current_DC_ADC = adc1Result[9];   // 通道6 直流电流
        g_Channelinfo[9].current_DC_ADC = adc1Result[10];  // 通道10 直流电流
        g_Channelinfo[13].current_DC_ADC = adc1Result[11]; // 通道14 直流电流

        /* 通道1电流: 采样电阻0.1Ω, 放大倍数 */
        if (g_Channelinfo[0].workMode == POWER_RUN_DISCHARGE)
        {
            g_Channelinfo[1].current = g_Channelinfo[1].current_DC_ADC * 0.000335088f;
            g_Channelinfo[5].current = g_Channelinfo[5].current_DC_ADC * 0.000335088f;
            g_Channelinfo[9].current = g_Channelinfo[9].current_DC_ADC * 0.000335088f;
            g_Channelinfo[13].current = g_Channelinfo[13].current_DC_ADC * 0.000335088f;
        }
        else
        {
            g_Channelinfo[1].current = g_Channelinfo[1].current_ADC * 0.000335088f;
            g_Channelinfo[5].current = g_Channelinfo[5].current_ADC * 0.000335088f;
            g_Channelinfo[9].current = g_Channelinfo[9].current_ADC * 0.000335088f;
            g_Channelinfo[13].current = g_Channelinfo[13].current_ADC * 0.000335088f;
        }

        /* 电容端电压/端口电压采样 */
        if (g_Channelinfo[0].workMode == POWER_PRECHARGE || g_Channelinfo[0].workMode == POWER_INIT)
        {
            Set_Sample_Channel_VPortGPIO(AD_V_CAP_EN);
            g_Channelinfo[1].Cap_voltage_ADC = adc1Result[12]; // 通道2 电容电压
            g_Channelinfo[5].Cap_voltage_ADC = adc1Result[13]; // 通道6 电容电压
            g_Channelinfo[1].Cap_voltage = g_Channelinfo[1].Cap_voltage_ADC * 0.050354772f * 3.033333f;
            g_Channelinfo[5].Cap_voltage = g_Channelinfo[5].Cap_voltage_ADC * 0.050354772f * 3.033333f;
        }
        else if (g_Channelinfo[0].workMode == POWER_FAULT || g_Channelinfo[0].workMode == POWER_RUN_CHARGE)
        {
            Set_Sample_Channel_VPortGPIO(AD_V_PORT);
            g_Channelinfo[1].voltage_port_ADC = adc1Result[12]; // 通道2 端口电压
            g_Channelinfo[5].voltage_port_ADC = adc1Result[13]; // 通道6 端口电压
            g_Channelinfo[1].Cap_voltage = g_Channelinfo[1].Cap_voltage_ADC * 0.050354772f * 3.033333f;
            g_Channelinfo[5].Cap_voltage = g_Channelinfo[5].Cap_voltage_ADC * 0.050354772f * 3.033333f;
        }
        SetCD4052(CD4052_Chnum); // 切换通道采样
        break;

    /*-------------------------- CD4052 通道2: 通道3,7,11,15 -------------------------*/
    case 2:
        Samp_ChnumTimer++;
        if (Samp_ChnumTimer >= 10)
        {
            CD4052_Chnum = 3;
            Samp_ChnumTimer = 0;
        }
        SetCD4052(CD4052_Chnum);

        /* 电压采样 */
        g_Channelinfo[2].voltage_ADC = adc1Result[0];  // 通道3 电压
        g_Channelinfo[6].voltage_ADC = adc1Result[1];  // 通道7 电压
        g_Channelinfo[10].voltage_ADC = adc1Result[2]; // 通道11 电压
        g_Channelinfo[14].voltage_ADC = adc1Result[3]; // 通道15 电压
        /* 通道1电压: 分压系数2.2 */
        g_Channelinfo[2].voltage = g_Channelinfo[2].voltage_ADC * 0.050354772f * 2.2f;
        g_Channelinfo[6].voltage = g_Channelinfo[6].voltage_ADC * 0.050354772f * 2.2f;
        g_Channelinfo[10].voltage = g_Channelinfo[10].voltage_ADC * 0.050354772f * 2.2f;
        g_Channelinfo[14].voltage = g_Channelinfo[14].voltage_ADC * 0.050354772f * 2.2f;

        /* 充电电流采样 */
        g_Channelinfo[2].current_ADC = adc1Result[4];  // 通道1 电流
        g_Channelinfo[6].current_ADC = adc1Result[5];  // 通道5 电流
        g_Channelinfo[10].current_ADC = adc1Result[6]; // 通道9 电流
        g_Channelinfo[14].current_ADC = adc1Result[7]; // 通道13 电流

        // /* 直流电流采样 */
        g_Channelinfo[2].current_DC_ADC = adc1Result[8];   // 通道3 直流电流
        g_Channelinfo[6].current_DC_ADC = adc1Result[9];   // 通道7 直流电流
        g_Channelinfo[9].current_DC_ADC = adc1Result[10];  // 通道11 直流电流
        g_Channelinfo[14].current_DC_ADC = adc1Result[11]; // 通道15 直流电流

        /* 通道1电流: 采样电阻0.1Ω, 放大倍数 */
        if (g_Channelinfo[0].workMode == POWER_RUN_DISCHARGE)
        {
            g_Channelinfo[2].current = g_Channelinfo[2].current_DC_ADC * 0.000335088f;
            g_Channelinfo[6].current = g_Channelinfo[6].current_DC_ADC * 0.000335088f;
            g_Channelinfo[10].current = g_Channelinfo[10].current_DC_ADC * 0.000335088f;
            g_Channelinfo[14].current = g_Channelinfo[14].current_DC_ADC * 0.000335088f;
        }
        else
        {
            g_Channelinfo[2].current = g_Channelinfo[2].current_ADC * 0.000335088f;
            g_Channelinfo[6].current = g_Channelinfo[6].current_ADC * 0.000335088f;
            g_Channelinfo[10].current = g_Channelinfo[10].current_ADC * 0.000335088f;
            g_Channelinfo[14].current = g_Channelinfo[14].current_ADC * 0.000335088f;
        }

        /* 电容端电压/端口电压采样 */
        if (g_Channelinfo[0].workMode == POWER_PRECHARGE || g_Channelinfo[0].workMode == POWER_INIT)
        {
            Set_Sample_Channel_VPortGPIO(AD_V_CAP_EN);
            g_Channelinfo[2].Cap_voltage_ADC = adc1Result[12]; // 通道3 电容电压
            g_Channelinfo[6].Cap_voltage_ADC = adc1Result[13]; // 通道7 电容电压
        }
        else if (g_Channelinfo[0].workMode == POWER_FAULT || g_Channelinfo[0].workMode == POWER_RUN_CHARGE)
        {
            Set_Sample_Channel_VPortGPIO(AD_V_PORT);
            g_Channelinfo[2].voltage_port_ADC = adc1Result[12]; // 通道3 端口电压
            g_Channelinfo[6].voltage_port_ADC = adc1Result[13]; // 通道7 端口电压
        }
        break;

    /*-------------------------- CD4052 通道3: 通道4,8,12,16 -------------------------*/
    case 3:
        Samp_ChnumTimer++;
        if (Samp_ChnumTimer >= 10)
        {
            CD4052_Chnum = 0; // 循环回到通道0
            Samp_ChnumTimer = 0;
        }
        SetCD4052(CD4052_Chnum);

        /* 电压采样 */
        g_Channelinfo[3].voltage_ADC = adc1Result[0];  // 通道4 电压
        g_Channelinfo[7].voltage_ADC = adc1Result[1];  // 通道8 电压
        g_Channelinfo[11].voltage_ADC = adc1Result[2]; // 通道12 电压
        g_Channelinfo[15].voltage_ADC = adc1Result[3]; // 通道16 电压
        /* 通道1电压: 分压系数2.2 */
        g_Channelinfo[3].voltage = g_Channelinfo[3].voltage_ADC * 0.050354772f * 2.2f;
        g_Channelinfo[7].voltage = g_Channelinfo[7].voltage_ADC * 0.050354772f * 2.2f;
        g_Channelinfo[11].voltage = g_Channelinfo[11].voltage_ADC * 0.050354772f * 2.2f;
        g_Channelinfo[15].voltage = g_Channelinfo[15].voltage_ADC * 0.050354772f * 2.2f;

        /* 充电电流采样 */
        /* 电流采样 */
        g_Channelinfo[3].current_ADC = adc1Result[4];  // 通道4 电流
        g_Channelinfo[7].current_ADC = adc1Result[5];  // 通道8 电流
        g_Channelinfo[11].current_ADC = adc1Result[6]; // 通道12 电流
        g_Channelinfo[15].current_ADC = adc1Result[7]; // 通道16 电流

        /* 直流电流采样 */
        g_Channelinfo[3].current_DC_ADC = adc1Result[8];   // 通道4 直流电流
        g_Channelinfo[7].current_DC_ADC = adc1Result[9];   // 通道8 直流电流
        g_Channelinfo[11].current_DC_ADC = adc1Result[10]; // 通道12 直流电流
        g_Channelinfo[15].current_DC_ADC = adc1Result[11]; // 通道16 直流电流
                                                           /* 通道1电流: 采样电阻0.1Ω, 放大倍数 */
        if (g_Channelinfo[0].workMode == POWER_RUN_DISCHARGE)
        {
            g_Channelinfo[3].current = g_Channelinfo[3].current_DC_ADC * 0.000335088f;
            g_Channelinfo[7].current = g_Channelinfo[7].current_DC_ADC * 0.000335088f;
            g_Channelinfo[11].current = g_Channelinfo[11].current_DC_ADC * 0.000335088f;
            g_Channelinfo[15].current = g_Channelinfo[15].current_DC_ADC * 0.000335088f;
        }
        else
        {
            g_Channelinfo[3].current = g_Channelinfo[3].current_ADC * 0.000335088f;
            g_Channelinfo[7].current = g_Channelinfo[7].current_ADC * 0.000335088f;
            g_Channelinfo[11].current = g_Channelinfo[11].current_ADC * 0.000335088f;
            g_Channelinfo[15].current = g_Channelinfo[15].current_ADC * 0.000335088f;
        }

        /* 电容端电压/端口电压采样 */
        if (g_Channelinfo[0].workMode == POWER_PRECHARGE || g_Channelinfo[0].workMode == POWER_INIT)
        {
            Set_Sample_Channel_VPortGPIO(AD_V_CAP_EN);
            g_Channelinfo[3].Cap_voltage_ADC = adc1Result[12]; // 通道3 电容电压
            g_Channelinfo[7].Cap_voltage_ADC = adc1Result[13]; // 通道7 电容电压
        }
        else if (g_Channelinfo[0].workMode == POWER_FAULT || g_Channelinfo[0].workMode == POWER_RUN_CHARGE)
        {
            Set_Sample_Channel_VPortGPIO(AD_V_PORT);
            g_Channelinfo[3].voltage_port_ADC = adc1Result[12]; // 通道3 端口电压
            g_Channelinfo[7].voltage_port_ADC = adc1Result[13]; // 通道7 端口电压
        }
        break;

    default:
        CD4052_Chnum = 0;
        break;
    }

    /*-------------------------- ADC通道数据映射 (DMA缓冲区 -> 应用缓冲区) -------------------------*
     * 说明: 将DMA缓冲区adc_values[]映射到应用缓冲区adc1Result[]
     *       确保数据一致性: 先CleanCache读取, 处理完后InvalidateCache释放
     *------------------------------------------------------------------------------------------------*/
    adc1Result[0] = adc_values[0];   // PA0 - 电压通道1
    adc1Result[1] = adc_values[1];   // PA1 - 电压通道2
    adc1Result[2] = adc_values[2];   // PA2 - 电压通道3
    adc1Result[3] = adc_values[3];   // PA3 - 电压通道4
    adc1Result[4] = adc_values[4];   // PA4 - 电流通道1
    adc1Result[5] = adc_values[5];   // PA5 - 电流通道2
    adc1Result[6] = adc_values[6];   // PA6 - 电流通道3
    adc1Result[7] = adc_values[7];   // PA7 - 电流通道4
    adc1Result[8] = adc_values[8];   // PC4 - 直流电流1
    adc1Result[9] = adc_values[9];   // PC5 - 直流电流2
    adc1Result[10] = adc_values[10]; // PB0 - 直流电流3
    adc1Result[11] = adc_values[11]; // PB1 - 直流电流4
    adc1Result[12] = adc_values[12]; // PC0 - 电容/端口电压1
    adc1Result[13] = adc_values[13]; // PC1 - 电容/端口电压2

    /* DMA读取完成后使D-Cache失效,避免后续DMA写入时产生缓存一致性冲突 */
    SCB_InvalidateDCache_by_Addr((uint32_t *)adc_values, sizeof(adc_values));
}

/*=================================================================================================
 * 函数名称:  ADC_Filter
 * 功能描述:  ADC数据滑动平均滤波
 * 说明:      使用移位实现除法,提高运算效率
 *================================================================================================*/
void ADC_Filter(void)
{
#if 0 // 滤波功能已禁用,如需启用请将0改为1
    uint16_t ch;

    // ADC1通道滤波
    for (ch = 0; ch < ADC_1_CH_NUM_MAX; ch++)
    {
        g_SampleADC.ADC_1_CH_Sum[ch] = adc1Result[ch] + g_SampleADC.ADC_1_CH_Sum[ch] - 
                                       (g_SampleADC.ADC_1_CH_Sum[ch] >> FILTER_NUM);
        g_SampleADC.ADC_1_CH_Num_Filter[ch] = g_SampleADC.ADC_1_CH_Sum[ch] >> FILTER_NUM;
    }

    // ADC2通道滤波
    for (ch = 0; ch < ADC_2_CH_NUM_MAX; ch++)
    {
        g_SampleADC.ADC_2_CH_Sum[ch] = (adc2Result[ch] + g_SampleADC.ADC_2_CH_Sum[ch]) - 
                                       (g_SampleADC.ADC_2_CH_Sum[ch] >> FILTER_NUM);
        g_SampleADC.ADC_2_CH_Num_Filter[ch] = g_SampleADC.ADC_2_CH_Sum[ch] >> FILTER_NUM;
    }
#endif
}

/*=================================================================================================
 * 函数名称:  ADC_Convert_Vaule
 * 功能描述:  ADC原始值转换为实际物理量(电压/电流)
 * 说明:      根据采样电阻和分压电阻计算实际值
 *================================================================================================*/
void ADC_Convert_Vaule(void)
{
    uint16_t i;
    /* ADC转换系数 (基于16位ADC, 3.3V参考电压)
     * 16位ADC分辨率: 3300mV / 65535 = 5.03548E-05 V/LSB
     */
    for (i = 0; i < BOARD_CHANNEL_NUM; i++)
    {
        /* 通道1电压: 分压系数2.2 */
        // g_Channelinfo[i].voltage = g_Channelinfo[i].voltage_ADC * 0.050354772f * 2.2f;

        // /* 通道1电流: 采样电阻0.1Ω, 放大倍数 */
        // g_Channelinfo[i].current = g_Channelinfo[i].current_ADC * 0.000335088f;

        /* 通道1电流采样电压 */
        g_Channelinfo[i].current_voltage = g_Channelinfo[i].current_ADC * 0.050354772f;

        /* 通道1电容电压: 分压系数3.033333 */
         g_Channelinfo[i].Cap_voltage = g_Channelinfo[i].Cap_voltage_ADC * 0.050354772f * 3.033333f;
    }
}

/*=================================================================================================
 * 函数名称:  sample_irq_handler
 * 功能描述:  采样定时器中断处理函数
 * 说明:      在定时器中断中调用,执行滤波和物理量转换
 *================================================================================================*/
void sample_irq_handler(void)
{
    ADC_Filter();        // ADC滤波处理
    ADC_Convert_Vaule(); // ADC原始值转换为实际物理量
}

/*=================================================================================================
 * 函数名称:  AppUser_Sample_Debug
 * 功能描述:  ADC采样数据调试打印
 * 说明:      输出所有ADC通道的原始值(通过串口)
 *================================================================================================*/
void AppUser_Sample_Debug(void)
{
    /* ADC1 电压/电流通道 (PA0-PA7) */
    printf("ADC1_A0=%d\r\n", adc_values[0]); // PA0
    printf("ADC1_A1=%d\r\n", adc_values[1]); // PA1
    printf("ADC1_A2=%d\r\n", adc_values[2]); // PA2
    printf("ADC1_A3=%d\r\n", adc_values[3]); // PA3
    printf("ADC1_A4=%d\r\n", adc_values[4]); // PA4
    printf("ADC1_A5=%d\r\n", adc_values[5]); // PA5
    printf("ADC1_A6=%d\r\n", adc_values[6]); // PA6
    printf("ADC1_A7=%d\r\n", adc_values[7]); // PA7

    /* ADC1 直流电流通道 (PC4, PC5, PB0, PB1) */
    printf("PC4 =%d\r\n", adc_values[8]);  // PC4
    printf("PC5 =%d\r\n", adc_values[9]);  // PC5
    printf("PB0 =%d\r\n", adc_values[10]); // PB0
    printf("PB1=%d\r\n", adc_values[11]);  // PB1

    /* ADC1 电容/端口电压通道 (PC0, PC1) */
    printf("PC0 =%d\r\n", adc_values[12]); // PC0
    printf("PC1=%d\r\n", adc_values[13]);  // PC1

    /* ADC3 通道 (PC2, PC3) - 温度传感器等 */
    printf("PC2 =%d\r\n", adc3_values[0]); // PC2 (ADC3_CH0)
    printf("PC3=%d\r\n", adc3_values[1]);  // PC3 (ADC3_CH1)

    /* 输出通道信息调试 */
    AppUser_ChannelInfo_Debug();
}

/*=================================================================================================
 * 函数名称:  AppUser_ChannelInfo_Debug
 * 功能描述:  通道信息调试打印
 * 说明:      输出转换后的实际物理量
 *================================================================================================*/
void AppUser_ChannelInfo_Debug(void)
{
    uint32_t ch;
    /* 电容电压 */
    printf("Cap_voltage =%f ADC1_A10=%d\r\n", g_Channelinfo[0].Cap_voltage, adc_values[12]);

    /* 电压 */
    printf("voltage =%f adc_values =%d\r\n", g_Channelinfo[0].voltage, adc_values[0]);

    /* 电流 */
    printf("current =%f ADC1_A2=%d  current_voltage=%f\r\n",
           g_Channelinfo[0].current, adc_values[4], g_Channelinfo[0].current_voltage);




    for (ch = 0; ch < 16; ch++)
    {
        printf("\r\n ch=[%d] workMode=%d ", ch, g_Channelinfo[ch].workMode);
        printf(" ch=[%d] voltage=%f--ADC=%d current=%f--ADC%d Cap_voltage=%f\r\n", ch, g_Channelinfo[ch].voltage, g_Channelinfo[ch].voltage_ADC,
               g_Channelinfo[ch].current, g_Channelinfo[ch].current_ADC, g_Channelinfo[ch].Cap_voltage);
        printf(" ch=[%d] fault=0x%X \r\n", ch, g_Channelinfo[ch].fault.all);
    }     
}

/*=================================================================================================
 * 函数名称:  Get_Send_dev_Data
 * 功能描述:  获取设备信息状态
 * 说明:      用于设备状态上报
 *================================================================================================*/
void Get_Send_dev_Data(void)
{
    // TODO: 实现设备状态数据获取
}

/*=================================================================================================
 * End of File
 *================================================================================================*/
