#ifndef USRE_APP_DRIVE_ADC_H_
#define USRE_APP_DRIVE_ADC_H_
#include "head.h"     // head.h 头文件


/******************************************************************************************/
/* ADC及引脚 定义 */


#define ADC_ADCX_CHY_GPIO_PORT              GPIOA
#define ADC_ADC0_CH0_GPIO_PIN               GPIO_PIN_0
#define ADC_ADC1_CH1_GPIO_PIN               GPIO_PIN_1
#define ADC_ADCX_CHY_GPIO_CLK_ENABLE()      do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)   /* PA口时钟使能 */

#define ADC_ADCX                            ADC1
#define ADC_ADCX_CHY                        ADC_CHANNEL_15                                /* 通道Y, 0 <= Y <= 19 */ 
#define ADC_ADCX_CHY_CLK_ENABLE()           do{ __HAL_RCC_ADC12_CLK_ENABLE(); }while(0)   /* ADC1/2 时钟使能 */

/******************************************************************************************/


#define ADC_1_CH_NUM_MAX 9 // ADC_A通道数量最大数量
#define ADC_2_CH_NUM_MAX 16 // ADC_A通道数量最大数量


#define ADC_BUFFER_SIZE 9  //
extern uint16_t adcBuffer[ADC_BUFFER_SIZE];  // 

#define FILTER_NUM 3   //滤波数据 2^4=16次

extern uint16_t adc1Result[ADC_1_CH_NUM_MAX];
extern uint16_t adc2Result[ADC_2_CH_NUM_MAX];

typedef enum ADC1_CH_A
{
	  ADC1_A0 = 0U, //
    ADC1_A1, //
    ADC1_A2,      //
    ADC1_A3,      //
    ADC1_A4,      //
    ADC1_A5,      //
    ADC1_A6,      //
    ADC1_A7,     //
    ADC1_A8,     //
    ADC1_A9,     //
    ADC1_A10,     //
    ADC1_A11,     //
    ADC1_A12,     //
    ADC1_A13,     //
    ADC1_A14,     //
    ADC1_A15,     //

} power_ADC1_CH_A;

typedef enum ADC2_CH_A
{
    ADC2_A1 = 0U, //
    ADC2_A2,      //
    ADC2_A5, //
    ADC2_A7,      //
    ADC2_A8,      //
    ADC2_A10,      //
    
    ADC2_B0 , //
    ADC2_B1,      
    ADC2_B2,     //
} power_ADC2_CH_A;

extern void TIMER1_GetADC_Result(void);
// 采样变量结构体
typedef struct
{
    long ADC_1_CH_Sum[ADC_1_CH_NUM_MAX];        // ADC_A 通道数量原始数据
    long ADC_1_CH_Num_Filter[ADC_1_CH_NUM_MAX]; // ADC_A 通道数量滤波数据

    long ADC_2_CH_Sum[ADC_2_CH_NUM_MAX];        // ADC_C 通道数量原始数据
    long ADC_2_CH_Num_Filter[ADC_2_CH_NUM_MAX]; // ADC_C 通道数量滤波数据

} SampleADC;

extern SampleADC g_SampleADC; //



extern  void GetADC_Driver_Result(void);



///****************************************************************************************************/
///* ADC3内部温度传感器通道 定义 */ 

//#define ADC3_TEMPSENSOR_CHX             ADC_CHANNEL_TEMPSENSOR

///****************************************************************************************************/

//extern void adc3_init(void);                                                                       /* ADC3初始化 */
//uint32_t adc3_get_result(ADC_HandleTypeDef adc_handle, uint32_t ch);                        /* 获得某个通道的ADC转换结果 */
//uint32_t adc3_get_result_average(ADC_HandleTypeDef adc_handle, uint32_t ch, uint8_t times); /* 得到某个通道给定次数采样的平均值 */

short adc3_get_temperature(void);                                                           /* ADC3 获取内部温度传感器的温度值 */



#endif




