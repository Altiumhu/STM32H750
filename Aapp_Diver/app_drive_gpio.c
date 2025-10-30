#include "head.h"
#include "app_drive_gpio.h"


void USART1_TX_EN(void)
{
//    HAL_GPIO_WritePin(GPIOA, RS485_EN_Pin, GPIO_PIN_SET); //PA11

}

void USART1_RX_EN(void)
{

//    HAL_GPIO_WritePin(GPIOA, RS485_EN_Pin, GPIO_PIN_RESET); //PA11
}



void App_GPIO_Init(void)
{
 GPIO_InitTypeDef GPIO_InitStruct = {0};
  /*Configure GPIO pins : LED_G_Pin UN_OCP_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}



