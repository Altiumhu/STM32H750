/***************************** (C) COPYRIGHT ************************************
* File Name          : W25xx_Qspi.h
* Author             : 模组星球嵌入式
* Version            : V1.0
* Date               : 30/11/2019
* Description        : STM32h7的QUAD SPI驱动程序头文件
* Note               : 版权所有  严禁盗版
********************************************************************************
* 实验平台:模组星球STM32H743VIT6开发板   
* 淘宝店铺:https://shop71600107.taobao.com/

********************************************************************************/
#ifndef __W25_QPI_H_
#define __W25_QPI_H_

#include "stm32h7xx.h"
#include "stm32_types.h"
#include "stm32h7xx_hal.h"

#include "W25Q128.h"

#define FLASH_SPI   0    //SPI模式
#define FLASH_QPI   1    //QPI模式
#define FLASH_OP_METHOD  FLASH_QPI   //FLASH操作接口为QPI,速度较快

#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17
#define W25Q256 0XEF18

//QSPI错误码
#define QSPI_OK            ((uint8_t)0x00)
#define QSPI_ERROR         ((uint8_t)0x01)
#define QSPI_BUSY          ((uint8_t)0x02)
#define QSPI_NOT_SUPPORTED ((uint8_t)0x04)
#define QSPI_SUSPENDED     ((uint8_t)0x08)

/* Definition for QSPI clock resources */
#define QSPI_CLK_ENABLE()          __HAL_RCC_QSPI_CLK_ENABLE()
#define QSPI_CLK_DISABLE()         __HAL_RCC_QSPI_CLK_DISABLE()
#define QSPI_CS_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define QSPI_CLK_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define QSPI_D0_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOD_CLK_ENABLE()
#define QSPI_D1_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOD_CLK_ENABLE()
#define QSPI_D2_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOE_CLK_ENABLE()
#define QSPI_D3_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOD_CLK_ENABLE()

#define QSPI_FORCE_RESET()         __HAL_RCC_QSPI_FORCE_RESET()
#define QSPI_RELEASE_RESET()       __HAL_RCC_QSPI_RELEASE_RESET()

/* Definition for QSPI Pins */
#define QSPI_CS_PIN                GPIO_PIN_6
#define QSPI_CS_GPIO_PORT          GPIOB
#define QSPI_CLK_PIN               GPIO_PIN_2
#define QSPI_CLK_GPIO_PORT         GPIOB
#define QSPI_D0_PIN                GPIO_PIN_11
#define QSPI_D0_GPIO_PORT          GPIOD
#define QSPI_D1_PIN                GPIO_PIN_12
#define QSPI_D1_GPIO_PORT          GPIOD
#define QSPI_D2_PIN                GPIO_PIN_2
#define QSPI_D2_GPIO_PORT          GPIOE
#define QSPI_D3_PIN                GPIO_PIN_13
#define QSPI_D3_GPIO_PORT          GPIOD

/*******************************************************************************
* Function Name  : QSPI_Init
* Description    : QSPI初始化
* Input          : None
* Output         : None
* Return         : None
* Note			 : None
*******************************************************************************/
uint8_t QSPI_Init(void);

/*******************************************************************************
* Function Name  : QSPI_Read_SR
* Description    : 从SPI FLASH读取状态寄存器
* Input          : 
* Output         : pData: 读取的数据缓存
* Return         : 读操作结果
* Note			 : None
*******************************************************************************/
uint8_t QSPI_Read_SR(uint8_t cmd, uint8_t* pData);

/*******************************************************************************
* Function Name  : QSPI_Read
* Description    : 从SPI FLASH读取数据
* Input          : ReadAddr: 读起始地址
				   Size: 读取数据长度
				   dummy:空闲周期
* Output         : pData: 读取的数据缓存
* Return         : 读操作结果
* Note			 : None
*******************************************************************************/
uint8_t QSPI_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size, uint8_t dummy);

/*******************************************************************************
* Function Name  : QSPI_Write
* Description    : 向SPI FLASH写入数据
* Input          : pData: 待编程的数据缓存
				   WriteAddr: 编程起始地址
				   Size: 编程数据长度
* Output         : None
* Return         : 编程结果
* Note			 : None
*******************************************************************************/
uint8_t QSPI_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);

/*******************************************************************************
* Function Name  : QSPI_Erase_Block
* Description    : FLASH的块擦除操作
* Input          : erase_cmd: SECTOR_4K_ERASE_CMD/SECTOR_32K_ERASE_CMD/
				   SECTOR_64K_ERASE_CMD
				   BlockAddress: 擦除块地址
* Output         : None
* Return         : 0 擦除成功  1 擦除失败
* Note			 : 用于4K、32K、64K擦除
*******************************************************************************/
uint8_t QSPI_Erase_Block(uint8_t erase_cmd, uint32_t BlockAddress);

/*******************************************************************************
* Function Name  : QSPI_Erase_Chip
* Description    : 片擦除
* Input          : None
* Output         : None
* Return         : 0 擦除成功  1 擦除失败
* Note			 : None
*******************************************************************************/
uint8_t QSPI_Erase_Chip(void);

/*******************************************************************************
* Function Name  : W25_Flash_Write
* Description    : 写SPI FLASH,在指定地址开始写入指定长度的数据
* Input          : WriteAddr:开始写入的地址(24bit)
				   pBuffer:数据存储区
				   NumByteToWrite:要写入的字节数(最大65535)
* Output         : None
* Return         : None
* Note			 : 必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
				   具有自动换页功能,该函数带擦除操作!
*******************************************************************************/ 		 
void SPI_Flash_Write(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);

/*******************************************************************************
* Function Name  : QSPI_Flash_ReadID
* Description    : 读取芯片ID  
* Input          : None
* Output         : None
* Return         : 2个字节，第一个为制造商ID: 0xEF, 第二个为器件ID
                   W25Q128  0XEF17		//16M字节
* Note			 : 支持QPI
*******************************************************************************/  
u16 QSPI_Flash_ReadID(void);

/*******************************************************************************
* Function Name  : QSPI_ResetMemory
* Description    : 复位QSPI Memory
* Input          : hqspi   句柄
* Output         : None
* Return         : QSPI_OK/QSPI_ERROR
* Note			 : None
*******************************************************************************/
uint8_t QSPI_ResetMemory(QSPI_HandleTypeDef *hqspi);

/*******************************************************************************
* Function Name  : QSPI_WriteEnableSPI
* Description    : 写使能,SPI模式,用于初始化时写状态寄存器
* Input          : hqspi   句柄
* Output         : None
* Return         : QSPI_OK/QSPI_ERROR
* Note			 : None
*******************************************************************************/
uint8_t QSPI_WriteEnableSPI(QSPI_HandleTypeDef *hqspi);

/*******************************************************************************
* Function Name  : QSPI_WriteQE
* Description    : 写状态寄存器的QE位,准备进入QPI模式
* Input          : hqspi   句柄
* Output         : None
* Return         : QSPI_OK/QSPI_ERROR
* Note			 : None
*******************************************************************************/
uint8_t QSPI_WriteQE(QSPI_HandleTypeDef *hqspi);

/*******************************************************************************
* Function Name  : QSPI_ModeEnter
* Description    : 配置进入QSPI模式,配置时采用单线,W25Q128在调用此函数前需要将寄
                   存器中的QE位置1否则忽略进入QPI命令
* Input          : hqspi   句柄
* Output         : None
* Return         : QSPI_OK/QSPI_ERROR
* Note			 : None
*******************************************************************************/
uint8_t QSPI_ModeEnter(QSPI_HandleTypeDef *hqspi);

/*******************************************************************************
* Function Name  : QSPI_ModeExit
* Description    : 配置退出QSPI模式,配置时采用4线?
* Input          : hqspi   句柄
* Output         : None
* Return         : QSPI_OK/QSPI_ERROR
* Note			 : None
*******************************************************************************/
uint8_t QSPI_ModeExit(QSPI_HandleTypeDef *hqspi);

/*******************************************************************************
* Function Name  : QSPI_WriteEnable
* Description    : 写使能,QPI
* Input          : hqspi   句柄
* Output         : None
* Return         : QSPI_OK/QSPI_ERROR
* Note			 : None
*******************************************************************************/
uint8_t QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi);

/*******************************************************************************
* Function Name  : QSPI_AutoPollingMemReady
* Description    : 配置QSPI为自动轮寻模式
* Input          : hqspi   句柄   Timeout  超时
* Output         : None
* Return         : QSPI_OK/QSPI_ERROR
* Note			 : None
*******************************************************************************/
uint8_t QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi, uint32_t Timeout);

#endif
