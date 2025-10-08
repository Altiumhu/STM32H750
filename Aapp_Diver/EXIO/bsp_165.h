#ifndef __BSP_165_H_
#define __BSP_165_H_

#include "sys.h"
#include <stdio.h>
#include "delay.h"

/*
实时读取定义：ex_165_read
1：将会读取到芯片当时的状态，并更新缓存
0：将会读取到缓存的状态，使用ex_165_cache_up函数去更新缓存状态
*/
#define EX_165_READ_RT			1	

#define EX_165_RESET 0			//清除引脚
#define EX_165_SET   1  		//设置引脚

#define EX_165_CHIP_NUM			6		//片数量
#define	EX_165_PIN_0			0x01	//EXIO pin 0 
#define	EX_165_PIN_1			0x02	//EXIO pin 1 
#define	EX_165_PIN_2			0x04	//EXIO pin 2 
#define	EX_165_PIN_3			0x08	//EXIO pin 3 
#define	EX_165_PIN_4			0x10	//EXIO pin 4 
#define	EX_165_PIN_5			0x20	//EXIO pin 5 
#define	EX_165_PIN_6			0x40	//EXIO pin 6 
#define	EX_165_PIN_7			0x80	//EXIO pin 7
#define	EX_165_PIN_ALL		    0xff	//EXIO pin all 

#define EX_165_MODE_MSB			0		//高位在前-左
#define EX_165_MODE_LSB			1		//高位在后-右
#define EX_165_DELAY			0		//时钟延时NOP
#define EX_165_DEBUG			0		//调试输出

#define EX_165_MODE_CHIP		EX_165_MODE_LSB		//后级芯片为高
#define EX_165_MODE_DATA	    EX_165_MODE_LSB		//先读取到的是低位位数据

#define EX_165_CLK(x)		GPIO_WritePins(GPIO_PORT_B,GPIO_PIN_03,(en_pin_state_t)x)   //时钟	上升沿	
#define EX_165_LD(x)		GPIO_WritePins(GPIO_PORT_B,GPIO_PIN_04,(en_pin_state_t)x)	//锁存	
#define EX_165_DATA			GPIO_ReadInputPins(GPIO_PORT_B,GPIO_PIN_05)					//数据

void ex_165_init(void);
void ex_165_cache_up(void);
int ex_165_read(uint32_t chip, uint32_t pin, uint32_t *value);

#endif







