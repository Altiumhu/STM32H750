/***************************** (C) COPYRIGHT ************************************
* File Name          : W25Q128.h
* Author             : 模组星球嵌入式
* Version            : V1.0
* Date               : 08/18/2016
* Description        : W25Q128 FLASH芯片操作命令
* Note               : 版权所有  严禁盗版
********************************************************************************
* 实验平台:星光STM32F756开发板 
* 淘宝店铺:https://shop71600107.taobao.com/

********************************************************************************/
#ifndef __W25Q128_H_
#define __W25Q128_H_

#define QSPI_FLASH_SIZE                  0x1000000 //FLASH大小16MB
#define QSPI_FLASH_PAGE_SIZE             0x100     //页大小256字节

//读空闲周期
#define QUAD_READ_DUMMY_CYCLES      2    //QPI
#define QUAD_LOWREAD_DUMMY_CYCLES   8    //QPI
#define SPI_READ_DUMMY_CYCLES       8    //SPI

#define FLASH_BULK_ERASE_MAX_TIME         250000    //片擦除超时
#define FLASH_SECTOR_ERASE_MAX_TIME       3000      //64KB擦除超时
#define FLASH_SUBSECTOR_ERASE_MAX_TIME    800       //4KB擦除超时

//寄存器指令表 
#define RESET_ENABLE_CMD                     0x66   //使能复位
#define RESET_MEMORY_CMD                     0x99   //复位器件

//读ID指令
#define READ_DEVICE_ID_CMD                   0xAB
#define READ_JedecDeviceID_CMD               0x9F
#define READ_ManufactDeviceID_CMD			 0x90 

//电源控制指令
#define PowerDown_CMD						 0xB9 
#define ReleasePowerDown_CMD				 0xAB 

//读操作指令
#define READ_CMD                             0x03    //读操作
#define FAST_READ_CMD                        0x0B    //快速读操作
#define DUAL_OUT_FAST_READ_CMD               0x3B    //快速双倍读
#define DUAL_INOUT_FAST_READ_CMD             0xBB
#define QUAD_OUT_FAST_READ_CMD               0x6B
#define QUAD_INOUT_FAST_READ_CMD             0xEB

#define WRITE_ENABLE_CMD                     0x06    //写使能
#define WRITE_DISABLE_CMD                    0x04    //写禁止
#define READ_STATUS_REG_CMD                  0x05    //读状态寄存器
#define WRITE_STATUS_REG_CMD                 0x01    //写状态寄存器
#define READ_STATUS_REG2_CMD                 0x35    //读状态寄存器
#define WRITE_STATUS_REG2_CMD                0x31    //写状态寄存器
#define READ_STATUS_REG3_CMD                 0x15    //读状态寄存器
#define WRITE_STATUS_REG3_CMD                0x11    //写状态寄存器

#define W25X_ReadStatusReg1		0x05 
#define W25X_ReadStatusReg2		0x35 
#define W25X_ReadStatusReg3		0x15 
#define W25X_WriteStatusReg1    0x01 
#define W25X_WriteStatusReg2    0x31 
#define W25X_WriteStatusReg3    0x11 

//编程指令
#define PAGE_PROG_CMD                        0x02   //页编程指令
#define QUAD_IN_FAST_PROG_CMD                0x32   //QUAD页编程指令

//擦除操作指令
#define SECTOR_4K_ERASE_CMD                  0x20        //4KB扇区擦除命令
#define SECTOR_32K_ERASE_CMD				 0x52        //32KB扇区擦除命令
#define SECTOR_64K_ERASE_CMD                 0xD8        //64KB扇区擦除命令
#define BULK_ERASE_CMD                       0xC7        //整片擦除命令

//读ID
#define QSPI_READ_ID						 0x90        //读厂家识别码和ID

#define QSPI_SetReadParam		             0xC0 

#define QSPI_MODE_ENTER_CMD        			 0x38     //进入QSPI模式
#define QSPI_MODE_EXIT_CMD         			 0xFF     //退出QSPI模式

//状态寄存器位定义
#define QSPI_SR_QE                 			 0x02     //bit1,位于状态寄存器2

//寄存器内容定义
#define FLASH_SR_WIP                      ((uint8_t)0x01)    /*!< Write in progress */
#define FLASH_SR_WREN                     ((uint8_t)0x02)    /*!< Write enable latch */

#endif
