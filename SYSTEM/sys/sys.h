/**
 ****************************************************************************************************					 
 * @file        sys.h
 * @version     V1.0
 * @brief       系统初始化代码(包括时钟配置/中断管理/GPIO设置等)            
 ****************************************************************************************************'
 *
 * V1.0
 * 将头文件包含路径改成相对路径,避免重复设置包含路径的麻烦
 *
 ****************************************************************************************************
 */

#ifndef __SYS_H
#define __SYS_H

#include "stm32h7xx.h"
#include "core_cm7.h"
#include "stm32h7xx_hal.h"




#define U8TOU32(X) ((uint32_t)(X)[0]|(((uint32_t)(X)[1])<<8)|(((uint32_t)(X)[2])<<16)|(((uint32_t)(X)[3])<<24))
#define U8TOU24(X) ((uint32_t)(X)[0]|(((uint32_t)(X)[1])<<8)|(((uint32_t)(X)[2])<<16))
#define U8TOU16(X) ((uint32_t)(X)[0]|(((uint32_t)(X)[1])<<8)) 
#define U32TOU8(U32DATA,U8POIN) do{U8POIN[0]=	(U32DATA)&0xff; \
												U8POIN[1]=((U32DATA)>>8)&0xff;\
												U8POIN[2]=((U32DATA)>>16)&0xff;\
												U8POIN[3]=((U32DATA)>>24)&0xff;\
								}while(0)
#define U24TOU8(U32DATA,U8POIN) do{U8POIN[0]=	(U32DATA)&0xff; \
												U8POIN[1]=((U32DATA)>>8)&0xff;\
												U8POIN[2]=((U32DATA)>>16)&0xff;\
								}while(0)
#define U16TOU8(U16DATA,U8POIN) do{U8POIN[0]=	(U16DATA)&0xff; \
												U8POIN[1]=((U16DATA)>>8)&0xff;\
								}while(0)

/**
 * SYS_SUPPORT_OS用于定义系统文件夹是否支持OS
 * 0,不支持OS
 * 1,支持OS
 */
#define SYS_SUPPORT_OS         0

#define      ON      1
#define      OFF     0
#define      Write_Through()    do{ *(__IO uint32_t*)0XE000EF9C = 1UL << 2; }while(0)     /* Cache透写模式 */

/******************************************************************************************/

uint8_t get_icahce_sta(void);
uint8_t get_dcahce_sta(void);
void sys_nvic_set_vector_table(uint32_t baseaddr, uint32_t offset);                       /* 设置中断偏移量 */
void sys_cache_enable(void);                                                              /* 使能STM32H7的L1-Cahce */
uint8_t sys_stm32_clock_init(uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq); /* 配置系统时钟 */

/* 以下为汇编函数 */
void sys_wfi_set(void);              /* 执行WFI指令 */
void sys_intx_disable(void);         /* 关闭所有中断 */
void sys_intx_enable(void);          /* 开启所有中断 */
void sys_msr_msp(uint32_t addr);     /* 设置栈顶地址 */

#endif





