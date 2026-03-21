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
  GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET); // PA11

  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET); // PA11

  /*Configure GPIO pins : LED_G_Pin UN_OCP_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET); // PA11

  /*Configure GPIO pins : LED_G_Pin UN_OCP_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12 | GPIO_PIN_15, GPIO_PIN_RESET); // PB12 和PB15

  /*Configure GPIO pins : LED_G_Pin UN_OCP_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1 | GPIO_PIN_0, GPIO_PIN_RESET); // PB12 和PB15
}

void Set_ULock_GPIO(void)
{
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1 | GPIO_PIN_0, GPIO_PIN_RESET); // PB12 和PB15
  HAL_Delay(5);
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1 | GPIO_PIN_0, GPIO_PIN_SET); // PB12 和PB15
}
