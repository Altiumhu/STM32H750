/*
 * AppUuser_Temper.c
 *
 *  Created on: 2023年5月20日
 *      Author: admin
 */
#include "head.h"     // head.h 头文件
#include "AppUuser_Temper.h"


#if 1

const uint16_t Res5kValue[TEMP_SCALE] =
{
    2335, 2289, 2243, 2197, 2152, 2105, 2060, 2013, 1967, 1921,
    1875, 1831, 1786, 1741, 1697, 1653, 1610, 1567, 1525, 1484,
    1443, 1403, 1363, 1325, 1287, 1250, 1213, 1177, 1142, 1108,
    1075, 1043, 1011, 980, 950, 921, 892, 864, 838, 811,
    786, 761, 737, 714, 692, 670, 648, 628, 608, 589,
    570, 552, 534, 518, 501, 485, 470, 455, 441, 427,
    413, 401, 388, 376, 364, 353, 342, 332, 321, 311,
    302, 293, 284, 275, 267, 259, 251, 243, 236, 229,
    222, 216, 209, 203, 197, 192, 186, 181, 175, 170,
    166, 161, 156, 152, 148, 144, 140, 136, 132, 128,
    125, 121, 118, 115, 112, 109, 106, 103, 100, 98,
    95, 92, 90, 88, 85, 83, 81, 79, 77, 75,
    73, 71, 70, 68, 66, 65, 63, 61, 60, 58,
    57, 56, 54, 53, 52, 50, 49, 48, 47, 46,
    45, 44, 43, 42, 41, 40, 39, 38, 37, 36
} ;

// //10K热敏电阻，10K分压，2.5V供电
// const INT16U Res10kValue[TEMP_SCALE] =
// {
// 2374,2345,2316,2286,2256,2225,2193,2162,2129,2097,
// 2064,2031,1997,1963,1929,1895,1861,1826,1792,1757,
// 1722,1688,1653,1619,1585,1551,1517,1483,1450,1417,
// 1384,1351,1319,1287,1256,1225,1195,1165,1135,1106,
// 1078,1050,1022,995,969,943,917,892,868,844,
// 821,798,776,754,733,713,692,673,654,635,
// 617,599,582,566,549,534,518,503,489,475,
// 461,448,435,423,410,399,387,376,365,355,
// 345,335,325,316,307,298,290,282,274,266,
// 259,251,244,238,231,225,218,212,207,201,
// 195,190,185,180,175,170,166,161,157,153,
// 149,145,141,137,134,130,127,124,120,117,
// 114,111,109,106,103,101,98,96,93,91,
// 89,86,84,82,80,78,76,75,73,71,
// 69,68,66,65,63,62,60,59,57,56
// };

// //10K热敏电阻，10K分压，3.285V供电
// const INT16U Res10kValue[TEMP_SCALE] =
// {
// 3119,3082,3043,3004,2964,2923,2882,2840,2798,2755,

// 2712,2668,2624,2580,2535,2490,2445,2400,2354,2309,
// 2263,2218,2173,2127,2082,2038,1993,1949,1905,1862,
// 1818,1776,1734,1692,1651,1610,1570,1531,1492,1454,
// 1416,1379,1343,1308,1273,1239,1205,1173,1141,1109,
// 1079,1049,1020,991 ,964 ,936 ,910 ,884 ,859 ,835 ,
// 811 ,788 ,765 ,743 ,722 ,701 ,681 ,662 ,643 ,624 ,
// 606 ,589 ,572 ,555 ,539 ,524 ,509 ,494 ,480 ,466 ,
// 453 ,440 ,428 ,416 ,404 ,392 ,381 ,370 ,360 ,350 ,
// 340 ,331 ,321 ,312 ,304 ,295 ,287 ,279 ,272 ,264 ,
// 257 ,250 ,243 ,237 ,230 ,224 ,218 ,212 ,206 ,201 ,
// 196 ,190 ,185 ,181 ,176 ,171 ,167 ,163 ,158 ,154 ,
// 150 ,146 ,143 ,139 ,136 ,132 ,129 ,126 ,123 ,120 ,
// 117 ,114 ,111 ,108 ,106 ,103 ,101 ,98  ,96  ,94  ,
// 91  ,89  ,87  ,85  ,83  ,81  ,79  ,77  ,76  ,74  ,
// //
// };


////10K热敏电阻，10K分压，3.300V供电
const uint16_t Res10kValue[TEMP_SCALE] =
{
//3134,3096,3057,3018,2977,2937,2895,2853,2811,
//2768,2724,2680,2636,2592,2547,2501,2456,2411,2365,
//2319,2274,2228,2183,2137,2092,2047,2002,1958,1914,
//1870,1827,1784,1741,1700,1658,1618,1577,1538,1499,
//1460,1423,1386,1349,1314,1279,1244,1211,1178,1146,
//1114,1084,1054,1024,996 ,968 ,941 ,914 ,888 ,863 ,
//839 ,815 ,791 ,769 ,747 ,725 ,705 ,684 ,665 ,646 ,
//627 ,609 ,592 ,574 ,558 ,542 ,526 ,511 ,497 ,482 ,
//469 ,455 ,442 ,430 ,417 ,406 ,394 ,383 ,372 ,362 ,
//351 ,342 ,332 ,323 ,314 ,305 ,297 ,288 ,280 ,273 ,
//265 ,258 ,251 ,244 ,238 ,231 ,225 ,219 ,213 ,207 ,
//202 ,197 ,191 ,186 ,181 ,177 ,172 ,168 ,163 ,159 ,
//155 ,151 ,147 ,143 ,140 ,136 ,133 ,130 ,126 ,123 ,
//120 ,117 ,114 ,111 ,109 ,106 ,104 ,101 ,99  ,96  ,
//94  ,92  ,90  ,87  ,85  ,83  ,81  ,80  ,78  ,76  ,
//
//74  ,
////


//NTC10K 3300mV  10K
    3134, 3096, 3058, 3018, 2978, 2937, 2896, 2854, 2812, 2769,
    2725, 2681, 2637, 2592, 2547, 2502, 2457, 2411, 2366, 2320,
    2274, 2229, 2183, 2138, 2093, 2048, 2003, 1958, 1914, 1870,
    1827, 1784, 1742, 1700, 1659, 1618, 1578, 1538, 1499, 1461,
    1423, 1386, 1350, 1314, 1279, 1245, 1211, 1178, 1146, 1115,
    1084, 1054, 1025, 996, 968, 941, 914, 889, 863, 839,
    815, 792, 769, 747, 726, 705, 685, 665, 646, 627,
    609, 592, 575, 558, 542, 527, 511, 497, 483, 469,
    455, 442, 430, 418, 406, 394, 383, 372, 362, 352,
    342, 332, 323, 314, 305, 297, 289, 281, 273, 265,
    258, 251, 244, 238, 231, 225, 219, 213, 207, 202,
    197, 191, 186, 181, 177, 172, 168, 163, 159, 155,
    151, 147, 143, 140, 136, 133, 130, 126, 123, 120,
    117, 114, 111, 109, 106, 104, 101, 99, 96, 94,
    92, 90, 87, 85, 83, 82, 80, 78, 76, 74,
};


////10K热敏电阻，10K分压，3.000V供电
//const uint16_t Res10kValue[TEMP_SCALE] =
//{
// 2977,2941,2904,2867,2829,2790,2751,2711,2670,2629,
// 2588,2546,2504,2462,2419,2376,2333,2290,2247,2203,
// 2160,2117,2073,2030,1987,1945,1902,1860,1818,1777,
//
// 1735,1695,1654,1615,1575,1537,1498,1461,1424,1387,
// 1352,1316,1282,1248,1215,1182,1150,1119,1089,1059,
// 1030,1001,973 ,946 ,920 ,894 ,868 ,844 ,820 ,797 ,
// 774 ,752 ,730 ,709 ,689 ,669 ,650 ,631 ,613 ,596 ,
// 579 ,562 ,546 ,530 ,515 ,500 ,486 ,472 ,458 ,445 ,
// 432 ,420 ,408 ,397 ,385 ,374 ,364 ,354 ,344 ,334 ,
// 325 ,315 ,307 ,298 ,290 ,282 ,274 ,266 ,259 ,252 ,
// 245 ,238 ,232 ,226 ,220 ,214 ,208 ,202 ,197 ,192 ,
// 187 ,182 ,177 ,172 ,168 ,163 ,159 ,155 ,151 ,147 ,
// 143 ,140 ,136 ,133 ,129 ,126 ,123 ,120 ,117 ,114 ,
// 111 ,109 ,106 ,103 ,101 ,98  ,96  ,94  ,91  ,89  ,
// 87  ,85  ,83  ,81  ,79  ,77  ,76  ,74  ,72  ,70  ,
//};
//



static Structural_Body_TEMP temp;

_Tempdataformat TempFormat;
float AppUser_Get_Channel_Temp(int ch, uint16_t adc_Sample, Sensor_typedef_enum u_PTSensor_Mode);

int32_t MCU_Get_ADC_Tempture(int ch)
{
    int32_t Vaul;

//    switch (ch)
//    {
//    case 0:
//        Vaul = g_SampleADC.ADC_1_CH_Num_Filter [ADC1_A0];
//        break;
//    case 1:
//        Vaul = g_SampleADC.ADC_1_CH_Num_Filter [ADC1_A1];
//        break;
//    case 2:
//        Vaul = g_SampleADC.ADC_1_CH_Num_Filter [ADC1_A2];
//        break;
//    case 3:
//        Vaul = g_SampleADC.ADC_1_CH_Num_Filter [ADC1_A3];
//        break;
//    case 4:
//        Vaul = g_SampleADC.ADC_1_CH_Num_Filter [ADC1_A4];
//        break;
//    case 5:
//        Vaul = g_SampleADC.ADC_1_CH_Num_Filter [ADC1_A5];
//        break;

//    case 6:
//        Vaul = g_SampleADC.ADC_1_CH_Num_Filter [ADC1_A6];
//        break;
//    case 7:
//        Vaul = g_SampleADC.ADC_1_CH_Num_Filter [ADC1_A7];
//        break;
//    }

    return Vaul;
}

/**********************************************************************
 * Function:     AppUser_Get_Channel_Temp
 * Description:   获取温度通道温度值
 * Input:         ch---通道;adc_Sample---ADC
 * Output:
 * Return:      void
 * Others:
 * Modify Date:    Version:    Author:        Modification:
 * -----------------------------------------------
 * 2022-05-31     V1.0        Hu Weiping
 **********************************************************************/
void AppUser_temp_sample(void)
{
    uint16_t i;
    for (i = 0; i < TEMP_CHANNEL_NUM; i++)
    {
        temp.adc[i] = MCU_Get_ADC_Tempture(i);
     // temp.adc[i] = 2048;
    }
    for (i = 0; i < TEMP_CHANNEL_NUM; i++)
    {
        temp.value[i] = AppUser_Get_Channel_Temp(i, temp.adc[i], NTC_10K); //MCU
        g_Master_Hand.temp_channe[i] = (uint16_t)(temp.value[i] * 10.0f);
    }
}

/**********************************************************************
* Function:     温度数据初始化
* Description:
* Input:        void
* Output:       void
* Return:       void
* Others:
* Modify Date:
  Version:          Author:       Modification:
  Notes:
* -----------------------------------------------
* 2018-07-23      V1.0        Hu Weiping
**********************************************************************/
void AppUuser_InitTempData(void)
{
    TempFormat.Resistance = (uint16_t*)&Res5kValue[0];
    TempFormat.EnvResistance = (uint16_t*)&Res10kValue[0];//将热敏电阻的ADC数据赋值
    TempFormat.Scan_enable = 1; //开启扫描模式
}
/**********************************************************************
* Function:     AmbientBinary
* Description:  二分查表
* Input:        void
* Output:       void
* Return:       void
* Others:
* Modify Date:
  Version:          Author:       Modification:
  Notes:
* -----------------------------------------------
* 2018-07-23      V1.0        Hu Weiping
**********************************************************************/
uint16_t AmbientBinary(uint16_t Value, Sensor_typedef_enum u_PTSensor_Mode)
{

    signed int i = 0, j = 0, k = 0;
    //   if(Value>InitNumber)return OverNumber;
    //   printf("\r\n u_PTSensor_Mode %d ",u_PTSensor_Mode);
    if (u_PTSensor_Mode == PT100_Sensor)
    {
        j = 100;
        // while (i <= j)
        // {

        //     k = (i + j) / 2;
        //     if (g_PT100_ADC_Value[k] < Value) /*后半部分*/
        //         i = k;
        //     else if (g_PT100_ADC_Value[k] > Value) /*前半部分*/
        //         j = k;
        //     else if (g_PT100_ADC_Value[k] == Value)
        //     {
        //         return k;
        //     }
        //     if (j == i + 1)
        //         return i;
        // }
    }
    else if (u_PTSensor_Mode == NTC_10K)
    {
        for (i = 0; i < TEMP_SCALE;)
        {
            if (Value >= TempFormat.EnvResistance[i])
            {
                return i;
            }
            else
            {
                i++;
            }
        }
    }
    else if (u_PTSensor_Mode == NTC_5K)
    {
        for (i = 0; i < TEMP_SCALE;)
        {
            if (Value >= TempFormat.Resistance[i])
            {
                return i;
            }
            else
            {
                i++;
            }
        }
    }

    return 0;
}


/**********************************************************************
 * Function:     AppUser_Get_Channel_Temp
 * Description:   获取温度通道温度值
 * Input:         ch---通道;adc_Sample---ADC
 *               u_PTSensor_Mode --传感器类型，NTC, PT100
 * Output:
 * Return:      void
 * Others:
 * Modify Date:    Version:    Author:        Modification:
 * -----------------------------------------------
 * 2022-05-31     V1.0        Hu Weiping
 **********************************************************************/
float AppUser_Get_Channel_Temp(int ch, uint16_t adc_Sample, Sensor_typedef_enum u_PTSensor_Mode)
{
    int urow;
    float g_TckTemp;
    float u_ky, u_kx;
    float u_pt100_tempture;

    urow = AmbientBinary(adc_Sample, u_PTSensor_Mode);
    // 点斜式方程 y-y0=k(x-x0)dg 1.0 是表示温度跨度，曾加float提高温度测量精度
    u_ky = (float)((float)adc_Sample - (float)TempFormat.EnvResistance[urow]);                             // 分子 TempFormat.EnvResistance[i]
    u_kx = (float)((float)TempFormat.EnvResistance[urow - 1] - (float)TempFormat.EnvResistance[urow]); // 分母不应该等于0 否则没有意义

    if ((u_kx <= 0) && urow == 0)
    {
        g_TckTemp = urow; //
    }
    else
    {
        g_TckTemp = (float)((float)urow - (1.0f * ((float)u_ky) / (float)(u_kx))); // 计算温度值

    }

    if (g_TckTemp <= 0.0f)
        g_TckTemp = 0; // 0~149℃


    // printf("\r\nurow=%d u_ky =%f  u_kx =%f  Res %d",urow,u_ky,u_kx,TempFormat.EnvResistance[urow] );

    u_pt100_tempture = g_TckTemp ; // 温度的小数点放大10倍将以整数形式存放
//   u_pt100_tempture = g_TckTemp ; // 温度的小数点放大10倍将以整数形式存放

    /// printf("\r\nu_pt100_tempture =%d " ,u_pt100_tempture);
    return u_pt100_tempture;

}

//读取温度 值
uint16_t Get_Master_TempVaule(uint16_t ch)
{
	return  g_Master_Hand.temp_channe[ch];
}
//读取温度 值
void AppUserRead_TempVaule(void)
{


}
//调试输出显示温度值
void AppUserDebug_TempVaule(void)
{
	uint16_t i;
	for(i =0;i<TEMP_CHANNEL_NUM;i++)
	printf("\r\n temp.adc[%d] =%d Vaule=%d",i,temp.adc[i],g_Master_Hand.temp_channe[i]);

//    printf("\r\n temp.adc[0] =%d",temp.adc[0]);
//    printf("\r\n temp.adc[1] =%d",temp.adc[1]);
//    // DBG_PRINTF("\r\n temp.adc[2] =%lid",temp.adc[2]);




}


#endif
