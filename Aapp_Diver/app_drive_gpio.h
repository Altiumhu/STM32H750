

#ifndef USRE_APP_DRIVE_GPIO_H_
#define USRE_APP_DRIVE_GPIO_H_

void USART1_RX_EN(void);

void USART1_TX_EN(void);


void App_GPIO_Init(void);


//#define System_LED_HIGH_LEVEL    gpio_bit_set(GPIOC, GPIO_PIN_13)    /* Set GPIO pin level */
//#define System_LED_LOW_LEVEL     gpio_bit_reset(GPIOC, GPIO_PIN_13) /* Set GPIO pin level */
// 触发翻转
#define System_LED_TRG_LEVEL HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_12) // 测试中断频率目前100Khz

#define System_DBUGGPIO_HIGH_LEVEL   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET); //PD10
#define System_DBUGGPIO_LOW_LEVEL     HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET); //PD10


#define System_LED1_TRG_LEVEL HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_14) // 测试中断频率目前100Khz

#define System_LED1_HIGH_LEVEL   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET); //PD10
#define System_LED1_LOW_LEVEL     HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET); //PD10

void Set_ULock_GPIO(void);

#endif






