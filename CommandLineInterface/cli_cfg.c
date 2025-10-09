// 扩展支持RTT和以太网CLI

#include "head.h"

// 
#if CLI_ENABLE_RTT_CHANNEL
#include "SEGGER_RTT.h"

void cli_rtt_send(const char *data, uint16_t len)
{
    SEGGER_RTT_Write(0, data, len);
}
#endif

#if CLI_ENABLE_ETHERNET
#include "lwip/api.h"

void cli_ethernet_send(struct netconn *conn, const char *data, uint16_t len)
{
    netconn_write(conn, data, len, NETCONN_COPY);
}
#endif

// 统一发送接口
void cli_send_data(const char *data, uint16_t len)
{
    // 串口通道
    #if CLI_USE_DMA_FOR_OUTPUT
    HAL_UART_Transmit_DMA(&huart1, (uint8_t*)data, len);
    #else
    HAL_UART_Transmit(&huart3, (uint8_t*)data, len, 100);
    #endif
    
    // RTT通道
    #if CLI_ENABLE_RTT_CHANNEL
    cli_rtt_send(data, len);
    #endif
    
    // 以太网通道
    #if CLI_ENABLE_ETHERNET
    if(ethernet_cli_conn)
        cli_ethernet_send(ethernet_cli_conn, data, len);
    #endif
}