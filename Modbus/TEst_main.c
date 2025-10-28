#include "main.h"
#include "modbus_master.h"
#include "modbus_config.h"
#include "head.h"



UART_HandleTypeDef huart1;
ModbusMaster modbus_master;

// 回调函数示例
void ReadRegistersCallback(uint8_t slave_addr, Modbus_Status status, uint16_t *data, uint16_t count)
{
    if (status == MODBUS_OK) {
        // 处理读取到的数据
        for (int i = 0; i < count; i++) {
            printf("Register %d: 0x%04X\n", i, data[i]);
        }
    } else {
        printf("Read failed with error: %d\n", status);
    }
}

void WriteRegisterCallback(uint8_t slave_addr, Modbus_Status status, uint16_t *data, uint16_t count)
{
    if (status == MODBUS_OK) {
        printf("Write successful\n");
    } else {
        printf("Write failed with error: %d\n", status);
    }
}

int modbus_main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART1_UART_Init();
    
    // 初始化Modbus主站
    ModbusMaster_Init(&modbus_master, &huart1);
    
    // 示例：读取保持寄存器
    uint16_t read_data[10];
    Modbus_ReadHoldingRegisters(&modbus_master, 0x01, 0x0000, 10, 
                               read_data, 1000, ReadRegistersCallback);
    
    // 示例：写单个寄存器
    Modbus_WriteSingleRegister(&modbus_master, 0x01, 0x1000, 0x1234, 
                             1000, WriteRegisterCallback);
    
    while (1) {
        // 主循环处理Modbus通信
        ModbusMaster_Process(&modbus_master);
        
        HAL_Delay(10);
    }
}

void SystemClock_Config(void)
{
    // 系统时钟配置
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    
    // 配置HSE、PLL等
    // ... 根据具体硬件配置
}

void MX_USART1_UART_Init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 9600;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart1);
}

void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // 使能GPIO时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    // RS485方向控制引脚
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // USART1引脚
    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}