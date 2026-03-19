/**
 * @file shell_port.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2019-02-22
 * 
 * @copyright (c) 2019 Letter
 * 
 */



#include "head.h"

//#include "FreeRTOS.h"
//#include "task.h"
#include "shell.h"
//#include "serial.h"
//#include "stm32f4xx_hal.h"
//#include "usart.h"
//#include "cevent.h"
//#include "log.h"


Shell shell;
char shellBuffer[512];

extern void USART1_Send(uint8_t *buff, uint16_t Size);

/**
 * @brief 用户shell写
 * 
 * @param data 数据
 * @param len 数据长度
 * 
 * @return short 实际写入的数据长度
 */
short userShellWrite(char *data, unsigned short len)
{
    
	while(len--)
	{
       // AppUsr_USART_SendByte(*data++);
		
		//HAL_UART_Transmit(&huart1, data, 1, 100);
		
		USART1_Send(*data++,1);

	}
    
 //   serialTransmit(&debugSerial, (uint8_t *)data, len, 0x1FF);
    
    
    return len;
}

extern uint8_t huart1_rx_buffer[1];		   // 单字节接收缓冲区
/**
 * @brief 用户shell读
 * 
 * @param data 数据
 * @param len 数据长度
 * 
 * @return short 实际读取到
 */
short userShellRead(char *data, unsigned short len)
{
//    return serialReceive(&debugSerial, (uint8_t *)data, len, 0);
    
#if 0    
   int i = 0;
	short lenTmp = 0;
	for(i=0; i<len; i++)
	{
		while(RESET == usart_flag_get(USART2, USART_FLAG_RBNE));	
		*data = (char)(usart_data_receive(USART2)); 
		//printf("%c", *data);
		data++;
		lenTmp++;		
	}
	
#else
	  
	     HAL_UART_Receive_IT(&huart1, huart1_rx_buffer, 1);
	*data = huart1_rx_buffer[0]; 
	
#endif
	
	return 1;	

}

/**
 * @brief 用户shell上锁
 * 
 * @param shell shell
 * 
 * @return int 0
 */
int userShellLock(Shell *shell)
{
    //xSemaphoreTakeRecursive(shellMutex, portMAX_DELAY);
//    xSemaphoreTake(shellMutex, portMAX_DELAY);
    return 0;
}

/**
 * @brief 用户shell解锁
 * 
 * @param shell shell
 * 
 * @return int 0
 */
int userShellUnlock(Shell *shell)
{
   // xSemaphoreGiveRecursive(shellMutex);
  //  xSemaphoreGive(shellMutex);
    return 0;
}

/**
 * @brief 用户shell初始化
 * 
 */
void userShellInit(void)
{
//    shellMutex = xSemaphoreCreateMutex();

    shell.write = userShellWrite;
    shell.read = userShellRead;
//    shell.lock = userShellLock;
//    shell.unlock = userShellUnlock;
    shellInit(&shell, shellBuffer, 512);
//    if (xTaskCreate(shellTask, "shell", 256, &shell, 5, NULL) != pdPASS)
//    {
//       printf("shell task creat failed");
//    }
}


//CEVENT_EXPORT(EVENT_INIT_STAGE2, userShellInit);





