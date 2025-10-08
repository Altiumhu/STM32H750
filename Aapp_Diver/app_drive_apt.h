#ifndef USRE_APP_DRIVE_APT_H_
#define USRE_APP_DRIVE_APT_H_

#include "head.h"


/****************************************************************************************************/
/* 定时器 定义 */

/* TIMX PWM输出定义 
 * 这里输出的PWM控制LED0的亮度
 */
#define TIMX_PWM_CHY_GPIO_PORT         GPIOB
#define TIMX_PWM_CHY_GPIO_PIN          GPIO_PIN_0
#define TIMX_PWM_CHY_GPIO_AF           GPIO_AF2_TIM3                                /* AF功能选择 */
#define TIMX_PWM_CHY_GPIO_CLK_ENABLE() do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)  /* PB口时钟使能 */

#define TIMX_PWM                       TIM3                                         /* TIM3 */
#define TIMX_PWM_CHY                   TIM_CHANNEL_3                                /* 通道Y,  1<= Y <=4 */
#define TIMX_PWM_CHY_CLK_ENABLE()      do{ __HAL_RCC_TIM3_CLK_ENABLE(); }while(0)   /* TIM3 时钟使能 */

/****************************************************************************************************/

void timx_pwm_chy_init(uint32_t arr, uint16_t psc);                                 /* 定时器TIMX 通道Y PWM输出初始化函数 */

extern  void APT_Config_Init(void);

extern void APT_UP_Data(void);



#endif