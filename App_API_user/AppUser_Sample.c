

#include "head.h"
#include "AppUser_Sample.h"



volatile ChannelInfo g_Channelinfo[BOARD_CHANNEL_NUM]; // 通道信息
void AppUser_ChannelInfo_Debug(void);

void ADC_Filter(void);


void  GetADC_Driver_Result(void)
{
	static  uint16_t CD4052_Chnum =0;//
	static  uint16_t Samp_ChnumTimer =0;//
    // 启动转换前刷新Cache
//    SCB_CleanDCache_by_Addr((uint32_t *)adc_values, sizeof(adc_values));

    // 主循环中读取ADC值

	  
	switch(CD4052_Chnum)
	{
		case 0:
			      Samp_ChnumTimer ++;
		        if(Samp_ChnumTimer>=10)
						{
							CD4052_Chnum=1;
							Samp_ChnumTimer =0;
						}
			      SetCD4052(CD4052_Chnum);
						g_Channelinfo[0].voltage_ADC= adc1Result[0]; // 通道 1 电压ADC
						g_Channelinfo[4].voltage_ADC= adc1Result[1]; // 通道 5 电压ADC
						
						g_Channelinfo[8].voltage_ADC= adc1Result[2]; // 通道 9 电压ADC
						g_Channelinfo[12].voltage_ADC= adc1Result[3]; // 通道 13 电压ADC
						
						g_Channelinfo[0].current_ADC= adc1Result[4]; // 通道 1 电流ADC
						g_Channelinfo[4].current_ADC= adc1Result[5]; // 通道 5 电流ADC
						g_Channelinfo[8].current_ADC= adc1Result[6]; // 通道 9 电流ADC
						g_Channelinfo[12].current_ADC= adc1Result[7]; // 通道 13 电流ADC
						
						g_Channelinfo[0].current_DC_ADC= adc1Result[8]; // 通道 1 电流ADC
						g_Channelinfo[4].current_DC_ADC= adc1Result[9]; // 通道 5 电流ADC
						g_Channelinfo[8].current_DC_ADC= adc1Result[10]; // 通道 9 电流ADC
						g_Channelinfo[12].current_DC_ADC= adc1Result[11]; // 通道 13 电流ADC	
						
						//处理电容端电压和端口电压
						if(g_Channelinfo[0].workMode==POWER_PRECHARGE ||g_Channelinfo[0].workMode == POWER_INIT )
						{
						  Set_Sample_Channel_VPortGPIO(AD_V_CAP_EN);//采集电容电压
						  g_Channelinfo[0].Cap_voltage_ADC= adc1Result[12]; // 通道 1 电容ADC
						  g_Channelinfo[4].Cap_voltage_ADC= adc1Result[13]; // 通道 5 电容ADC		
						}
						else if( g_Channelinfo[0].workMode == POWER_FAULT||POWER_RUN_CHARGE == g_Channelinfo[0].workMode )
						{
							Set_Sample_Channel_VPortGPIO(AD_V_PORT);//采集电容电压
						  g_Channelinfo[0].voltage_port_ADC = adc1Result[12]; // 通道 1 电流ADC
						  g_Channelinfo[4].voltage_port_ADC = adc1Result[13]; // 通道 5 电流ADC	
						}


						
			     break;
	
		case 1:
			      Samp_ChnumTimer ++;
		        if(Samp_ChnumTimer>=10)
						{
							CD4052_Chnum=2;
							Samp_ChnumTimer =0;
						}
			      SetCD4052(CD4052_Chnum);
						g_Channelinfo[1].voltage_ADC= adc1Result[0]; // 通道 2 电压ADC
						g_Channelinfo[5].voltage_ADC= adc1Result[1]; // 通道 6 电压ADC						
						g_Channelinfo[9].voltage_ADC= adc1Result[2]; // 通道 10 电压ADC
						g_Channelinfo[13].voltage_ADC= adc1Result[3]; // 通道 14 电压ADC
						
						g_Channelinfo[1].current_ADC= adc1Result[4]; // 通道 2 电流ADC
						g_Channelinfo[5].current_ADC= adc1Result[5]; // 通道 6 电流ADC
						g_Channelinfo[9].current_ADC= adc1Result[6]; // 通道 10 电流ADC
						g_Channelinfo[13].current_ADC= adc1Result[7]; // 通道 14 电流ADC
						
						g_Channelinfo[1].current_DC_ADC= adc1Result[8]; // 通道 2 电流ADC
						g_Channelinfo[5].current_DC_ADC= adc1Result[9]; // 通道 6 电流ADC
						g_Channelinfo[9].current_DC_ADC= adc1Result[10]; // 通道 10 电流ADC
						g_Channelinfo[13].current_DC_ADC= adc1Result[11]; // 通道 14 电流ADC	
						//处理电容端电压和端口电压
						if(g_Channelinfo[0].workMode==POWER_PRECHARGE ||g_Channelinfo[0].workMode == POWER_INIT )
						{
						  Set_Sample_Channel_VPortGPIO(AD_V_CAP_EN);//采集电容电压
						  g_Channelinfo[1].Cap_voltage_ADC= adc1Result[12]; // 通道 2 电容ADC
						  g_Channelinfo[5].Cap_voltage_ADC= adc1Result[13]; // 通道 6 电容ADC		
						}
						else if( g_Channelinfo[0].workMode == POWER_FAULT||POWER_RUN_CHARGE == g_Channelinfo[0].workMode )
						{
							Set_Sample_Channel_VPortGPIO(AD_V_PORT);//采集端口电压
						  g_Channelinfo[1].voltage_port_ADC = adc1Result[12]; // 通道 2 电流ADC
						  g_Channelinfo[5].voltage_port_ADC = adc1Result[13]; // 通道 6 电流ADC	
						}									
			     break;	
		case 2:
			      Samp_ChnumTimer ++;
		        if(Samp_ChnumTimer>=10)
						{
							CD4052_Chnum=3;
							Samp_ChnumTimer =0;
						}
			      SetCD4052(CD4052_Chnum);
						g_Channelinfo[2].voltage_ADC= adc1Result[0]; // 通道 3 电压ADC
						g_Channelinfo[6].voltage_ADC= adc1Result[1]; // 通道 7 电压ADC						
						g_Channelinfo[9].voltage_ADC= adc1Result[2]; // 通道 11 电压ADC
						g_Channelinfo[14].voltage_ADC= adc1Result[3]; // 通道 15 电压ADC
						
						g_Channelinfo[2].current_ADC= adc1Result[4]; // 通道 3 电流ADC
						g_Channelinfo[6].current_ADC= adc1Result[5]; // 通道 7 电流ADC
						g_Channelinfo[9].current_ADC= adc1Result[6]; // 通道 11 电流ADC
						g_Channelinfo[14].current_ADC= adc1Result[7]; // 通道 15 电流ADC
						
						g_Channelinfo[2].current_DC_ADC= adc1Result[8]; // 通道3 电流ADC
						g_Channelinfo[6].current_DC_ADC= adc1Result[9]; // 通道 7 电流ADC
						g_Channelinfo[9].current_DC_ADC= adc1Result[10]; // 通道 11 电流ADC
						g_Channelinfo[14].current_DC_ADC= adc1Result[11]; // 通道 15 电流ADC	
						//处理电容端电压和端口电压
						if(g_Channelinfo[0].workMode==POWER_PRECHARGE ||g_Channelinfo[0].workMode == POWER_INIT )
						{
						  Set_Sample_Channel_VPortGPIO(AD_V_CAP_EN);//采集电容电压
						  g_Channelinfo[1].Cap_voltage_ADC= adc1Result[12]; // 通道 2 电容ADC
						  g_Channelinfo[5].Cap_voltage_ADC= adc1Result[13]; // 通道 6 电容ADC		
						}
						else if( g_Channelinfo[0].workMode == POWER_FAULT||POWER_RUN_CHARGE == g_Channelinfo[0].workMode )
						{
							Set_Sample_Channel_VPortGPIO(AD_V_PORT);//采集端口电压
						  g_Channelinfo[1].voltage_port_ADC = adc1Result[12]; // 通道 2 电流ADC
						  g_Channelinfo[5].voltage_port_ADC = adc1Result[13]; // 通道 6 电流ADC	
						}									
			     break;	
		case 3:
			      Samp_ChnumTimer ++;
		        if(Samp_ChnumTimer>=10)
						{
							CD4052_Chnum=1;
							Samp_ChnumTimer =0;
						}
			      SetCD4052(CD4052_Chnum);
						g_Channelinfo[3].voltage_ADC= adc1Result[0]; // 通道 4 电压ADC
						g_Channelinfo[7].voltage_ADC= adc1Result[1]; // 通道 8 电压ADC						
						g_Channelinfo[11].voltage_ADC= adc1Result[2]; // 通道 12 电压ADC
						g_Channelinfo[15].voltage_ADC= adc1Result[3]; // 通道 16 电压ADC
						
						g_Channelinfo[3].current_ADC= adc1Result[4]; // 通道 4 电流ADC
						g_Channelinfo[7].current_ADC= adc1Result[5]; // 通道 8 电流ADC
						g_Channelinfo[11].current_ADC= adc1Result[6]; // 通道 12 电流ADC
						g_Channelinfo[15].current_ADC= adc1Result[7]; // 通道 16 电流ADC
						
						g_Channelinfo[3].current_DC_ADC= adc1Result[8]; // 通道 4 电流ADC
						g_Channelinfo[7].current_DC_ADC= adc1Result[9]; // 通道 8 电流ADC
						g_Channelinfo[11].current_DC_ADC= adc1Result[10]; // 通道 12 电流ADC
						g_Channelinfo[15].current_DC_ADC= adc1Result[11]; // 通道 16 电流ADC	
																	
			     break;							
	
	}
	
    adc1Result[0] = adc_values[0]; // PA0 
    adc1Result[1] = adc_values[1]; // PA1
    adc1Result[2] = adc_values[2]; // PA2
    adc1Result[3] = adc_values[3]; // PA3
    adc1Result[4] = adc_values[4]; // PA4
    adc1Result[5] = adc_values[5]; // PA5
    adc1Result[6] = adc_values[6]; // PA6
    adc1Result[7] = adc_values[7]; // PA7
    adc1Result[8] = adc_values[8]; // PC0

    adc1Result[9] = adc_values[9]; // PA5
    adc1Result[10] = adc_values[10]; // PA6
    adc1Result[11] = adc_values[11]; // PA7
    adc1Result[12] = adc_values[12]; // PC0
    adc1Result[13] = adc_values[13]; // PC0
    // 读取后使Cache无效
//    SCB_InvalidateDCache_by_Addr((uint32_t *)adc_values, sizeof(adc_values));
}

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
#if 0
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
	
	g_Channelinfo[0].voltage = adc_values[0]*5.03548E-05*2.2f;
	
	g_Channelinfo[0].current = adc_values[4]*0.000335088f;
	
	g_Channelinfo[0].current_voltage = adc_values[4]*5.03548E-05f;
	
	g_Channelinfo[0].Cap_voltage = adc_values[12]*5.03548E-05f*3.033333f;
	

}

void sample_irq_handler(void)
{
   
    ADC_Filter();        // ADC滤波
   ADC_Convert_Vaule(); // ADC转换化实际数据
}

void AppUser_Sample_Debug(void)
{

     printf("ADC1_A0=%d\r\n", adc_values[0]);
     printf("ADC1_A1=%d\r\n", adc_values[1]);
     printf("ADC1_A2=%d\r\n", adc_values[2]);
     printf("ADC1_A3=%d\r\n", adc_values[3]);

     printf("ADC1_A4=%d\r\n", adc_values[4]);
     printf("ADC1_A5=%d\r\n", adc_values[5]);

     printf("ADC1_A6=%d\r\n", adc_values[6]);
     printf("ADC1_A7=%d\r\n", adc_values[7]);
     printf("PC4 =%d\r\n", adc_values[8]);

     printf("PC5 =%d\r\n", adc_values[9]);

     printf("PB0 %d\r\n", adc_values[10]);
     printf("PB1=%d\r\n", adc_values[11]);
	
	
     printf("PC0 %d\r\n", adc_values[12]);
     printf("PC1=%d\r\n", adc_values[13]);
		 
     printf("PC1=%d\r\n", adc_values[13]);
		 
     printf("PC2 %d\r\n", adc3_values[0]);
	
     printf("PC3=%d\r\n", adc3_values[1]);
    AppUser_ChannelInfo_Debug();
}

void AppUser_ChannelInfo_Debug(void)
{

    // 峰值电流
//  DBG_PRINTF("LLC_IPeak =%f ADC1_A1=%d\r\n", g_Channelinfo[0].LLC_IPeak, adc1Result[ADC1_A8]);

//  // 采集输出电容电压
   printf("Cap_voltage =%f ADC1_A10=%d\r\n", g_Channelinfo[0].Cap_voltage, adc_values[12]);
   printf("voltage =%f adc_values =%d\r\n", g_Channelinfo[0].voltage, adc_values[0]);
//  // 电流
  printf("current =%f ADC1_A2=%d  current_voltage=%f\r\n", g_Channelinfo[0].current,adc_values[4],g_Channelinfo[0].current_voltage);
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