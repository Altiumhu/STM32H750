#ifndef __BSP_595_H_
#define __BSP_595_H_

#include "sys.h"
#include <stdio.h>
#include <string.h>
#include "delay.h"

/*
实时定义：ex_595_write
1：将会实时写入芯片并更新缓存
0：将状态写入缓存，使用ex_595_cache_up函数去写入状态
*/
#define EX_595_WRITE_RT			1	//实时定义
#define EX_595_RESET 			0	//清除引脚
#define EX_595_SET   			1  	//设置引脚

#define EX_595_CHIP_NUM			4		//片数量
#define	EX_595_PIN_0			0x01	//EXIO pin 0 
#define	EX_595_PIN_1			0x02	//EXIO pin 1 
#define	EX_595_PIN_2			0x04	//EXIO pin 2 
#define	EX_595_PIN_3			0x08	//EXIO pin 3 
#define	EX_595_PIN_4			0x10	//EXIO pin 4 
#define	EX_595_PIN_5			0x20	//EXIO pin 5 
#define	EX_595_PIN_6			0x40	//EXIO pin 6 
#define	EX_595_PIN_7			0x80	//EXIO pin 7
#define	EX_595_PIN_ALL		    0xff	//EXIO pin all 

#define EX_595_MODE_MSB			0		//高位在前-左
#define EX_595_MODE_LSB			1		//高位在后-右
#define EX_595_DELAY			100		//时钟延时NOP
#define EX_595_DEBUG			0		//调试输出

#define EX_595_MODE_CHIP		EX_595_MODE_LSB		//后级芯片为高
#define EX_595_MODE_DATA	    EX_595_MODE_MSB		//先发送高位数据



void ex_595_init(void);
void ex_595_cache_up(void);
int ex_595_write(uint32_t chip, uint32_t pin, uint32_t value);
int ex_595_read(uint32_t chip, uint32_t pin, uint32_t *value);

#endif









