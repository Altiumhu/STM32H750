/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins
     PH0-OSC_IN (PH0)   ------> RCC_OSC_IN
     PH1-OSC_OUT (PH1)   ------> RCC_OSC_OUT
     PA13 (JTMS/SWDIO)   ------> DEBUG_JTMS-SWDIO
     PA14 (JTCK/SWCLK)   ------> DEBUG_JTCK-SWCLK
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, LED_G_Pin|FAN_GPIO_Pin|LED_595_LD_Pin|LED_595_CLK_Pin
                          |LED_595_DATA_Pin|EXIO_165_LD_Pin|EXIO_165_CLK_Pin|UN_OVP_Pin
                          |UN_OCP_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, SYS_LED1_Pin|SYS_LED2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, CD4052_A_Pin|GPIO_PIN_13|CD4052_B_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, AD_VD_EN_Pin|AD_V_PORT_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, EXIO_595_LD_Pin|EXIO_595_CLK_Pin|EXIO_595_DATA_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED_G_Pin UN_OCP_Pin */
  GPIO_InitStruct.Pin = LED_G_Pin|UN_OCP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : FAN_GPIO_Pin LED_595_LD_Pin LED_595_DATA_Pin EXIO_165_LD_Pin
                           EXIO_165_CLK_Pin UN_OVP_Pin */
  GPIO_InitStruct.Pin = FAN_GPIO_Pin|LED_595_LD_Pin|LED_595_DATA_Pin|EXIO_165_LD_Pin
                          |EXIO_165_CLK_Pin|UN_OVP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_595_CLK_Pin */
  GPIO_InitStruct.Pin = LED_595_CLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_595_CLK_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SYS_LED1_Pin SYS_LED2_Pin */
  GPIO_InitStruct.Pin = SYS_LED1_Pin|SYS_LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : EXIO_165_DATA_Pin */
  GPIO_InitStruct.Pin = EXIO_165_DATA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(EXIO_165_DATA_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CD4052_A_Pin PB13 CD4052_B_Pin */
  GPIO_InitStruct.Pin = CD4052_A_Pin|GPIO_PIN_13|CD4052_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : AD_VD_EN_Pin AD_V_PORT_EN_Pin */
  GPIO_InitStruct.Pin = AD_VD_EN_Pin|AD_V_PORT_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : EXIO_595_LD_Pin */
  GPIO_InitStruct.Pin = EXIO_595_LD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(EXIO_595_LD_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : EXIO_595_CLK_Pin EXIO_595_DATA_Pin */
  GPIO_InitStruct.Pin = EXIO_595_CLK_Pin|EXIO_595_DATA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : ADD1_Pin ADD2_Pin ADD3_Pin ADD4_Pin
                           ADD5_Pin ADD6_Pin */
  GPIO_InitStruct.Pin = ADD1_Pin|ADD2_Pin|ADD3_Pin|ADD4_Pin
                          |ADD5_Pin|ADD6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : BUS_OVP_Pin */
  GPIO_InitStruct.Pin = BUS_OVP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BUS_OVP_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
