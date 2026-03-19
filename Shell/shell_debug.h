/**
 * @file shell_debug.h
 * @brief 嵌入式Shell调试交互程序 - 头文件
 */

#ifndef __SHELL_DEBUG_H
#define __SHELL_DEBUG_H

#include "main.h"
#include <stdint.h>

/* 函数声明 */
void Shell_Init(void);
void Shell_Task(void);
void Shell_UART_RxHandler(uint8_t data);
void Shell_SendChar(char c);
void Shell_SendString(const char *str);

#endif /* __SHELL_DEBUG_H */
