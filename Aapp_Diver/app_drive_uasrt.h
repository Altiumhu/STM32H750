
#ifndef USRE_APP_DRIVE_USART_H_
#define USRE_APP_DRIVE_USART_H_

#include "head.h"


extern uint8_t uart_rx_flag;



extern void App_Drive_UASRT_Init(void);

extern void UART0_ReadRX_Task(void);
extern void UART2_ReadRX_Task(void);


extern void RS485_Send_Data(uint8_t *sendbuff, uint8_t len);

extern void UART0_Send_Data(uint8_t *sendbuff, uint8_t len);


#endif




