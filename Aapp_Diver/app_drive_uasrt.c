

#include "head.h"
#include "app_drive_uasrt.h"

uint8_t rx_buffer[1];		   // 单字节接收缓冲区
uint8_t huart3_rx_buffer[1];   // 单字节接收缓冲区
uint8_t huart7_rx_buffer[1];   // 单字节接收缓冲区
uint8_t hlpuart1_rx_buffer[1]; // 单字节接收缓冲区
uint8_t rx_data, uart_rx_flag;
float votlag = 3.1415962;
uint8_t dma_msg[] = "huart1 DMA Transfer!\r\n";

__attribute__((aligned(32))) uint8_t txData[] = "huart1 DMA TEST\r\n";
/**********************************************************************
 * Function:      App_Drive_UASRT_Init
 * Description:   串口驱动
 * Input:          void
 * Output:
 * Return:       void
 * Others:
 * Modify Date:    Version:    Author:         Modification:
 * -----------------------------------------------
 * 2022-05-31      V1.0        Hu Weiping
 **********************************************************************/
void App_Drive_UASRT_Init(void)
{

	HAL_UART_Receive_IT(&huart1, rx_buffer, 1); // 启动串口接收中断

	HAL_UART_Receive_IT(&huart3, huart3_rx_buffer, 1); // 启动串口接收中断
	HAL_UART_Receive_IT(&huart7, huart7_rx_buffer, 1); // 启动串口接收中断

	//	  HAL_UART_Receive_IT(&huart5, &rx_data, 1);
	char msg[] = "Hello UART1!\r\n";
	HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);

	char huart3msg[] = "Hello UART3!\r\n";
	HAL_UART_Transmit(&huart3, (uint8_t *)huart3msg, strlen(huart3msg), HAL_MAX_DELAY);

	char hlpuart1msg[] = "Hello huart7\r\n";
	HAL_UART_Transmit(&huart7, (uint8_t *)hlpuart1msg, strlen(hlpuart1msg), HAL_MAX_DELAY);

	printf("\r\n STM32F750=%f", votlag);
	SCB_CleanDCache_by_Addr((uint32_t *)txData, sizeof(txData));

	HAL_UART_Transmit_DMA(&huart1, txData, sizeof(txData) - 1);

	HAL_UART_Transmit_DMA(&huart3, txData, sizeof(txData) - 1);
}
/**********************************************************************
 * Function:      HAL_UART_RxCpltCallback
 * Description:   串口接收回调函数
 * Input:          void
 * Output:
 * Return:       void
 * Others:
 * Modify Date:    Version:    Author:         Modification:
 * -----------------------------------------------
 * 2022-05-31      V1.0        Hu Weiping
 **********************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		// 处理接收到的数据（示例：回传数据）
		// HAL_UART_Transmit(&huart1, rx_buffer, 1, 100);

		cli_process_char(rx_buffer[0]);
		///AppUser_ReceivingDataInterface(rx_buffer[0]);

		// 重新使能中断以接收下一字节
		HAL_UART_Receive_IT(&huart1, rx_buffer, 1);
	

		//        HAL_UART_Transmit(&huart1, &rx_data, 1, HAL_MAX_DELAY);
		//        HAL_UART_Receive_IT(&huart1, &rx_data, 1);
	}
	else if (huart->Instance == USART3)
	{
		// HAL_UART_Transmit(&huart3, huart3_rx_buffer, 1, 0);
		AppUser_ReceivingDataInterface(huart3_rx_buffer[0]);
		HAL_UART_Receive_IT(&huart3, huart3_rx_buffer, 1);
	}
	else if (huart->Instance == LPUART1)
	{
		//HAL_UART_Transmit(&hlpuart1, hlpuart1_rx_buffer, 1, 0);
		//AppUser_ReceivingDataInterface(hlpuart1_rx_buffer[0]);
		HAL_UART_Receive_IT(&hlpuart1, hlpuart1_rx_buffer, 1);
	}

	else if (huart->Instance == UART7)
	{
		// HAL_UART_Transmit(&huart7, huart7_rx_buffer, 1, 0);
		AppUser_ReceivingDataInterface(huart7_rx_buffer[0]);
		HAL_UART_Receive_IT(&huart7, huart7_rx_buffer, 1);
	}
}

void UART0_ReadRX_Task(void)
{
}

void UART2_ReadRX_Task(void)
{
}

void RS485_Send_Data(uint8_t *sendbuff, uint8_t len)
{
}

void UART0_Send_Data(uint8_t *sendbuff, uint8_t len)
{
}
