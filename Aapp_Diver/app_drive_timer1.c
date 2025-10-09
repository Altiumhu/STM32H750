

#include "head.h"

#include "app_drive_timer1.h"

volatile uint32_t timer7_counter = 0;
volatile uint32_t timer7_1000ms_counter = 0;
volatile uint8_t led_state = 0;

void App_Drive_InitTimer_7(void)
{
  // 启动TIM7中断
  HAL_TIM_Base_Start_IT(&htim7);
//  HAL_TIM_Base_Start_IT(&htim6);
}

#if 1
/* TIM7中断回调函数 */


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

  short temp;
  if (htim->Instance == TIM7)
  {

    timer7_1000ms_counter++;
    if (timer7_1000ms_counter > 10) // 1秒钟
    {  
//       Sys_Run_Led();   

      timer7_1000ms_counter = 0;
    }
    Debug_Timer_1ms();

//    sample_irq_handler();
//  //
//    Read_GPIO_vTask(); // 读取IO状态

//    Smoke_vTask();



    // Read_GPIO_vTask(); // 读取IO状态
    timer7_counter++;
    if (timer7_counter > 10) // 10秒钟
    {
      timer7_counter = 0;

      temp = adc3_get_temperature(); /* 得到温度值 */
      if (temp < 0)
      {
        temp = -temp;
      }

      // printf(" the temperature data is %2d.%2d degrees Celsius\r\n", temp /100, temp % 100);
    }
  }

  static uint16_t ms = 0;

  if (htim->Instance == TIM6)
  {
  }
}

#endif
