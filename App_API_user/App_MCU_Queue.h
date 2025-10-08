#ifndef USRE_APP_MCU_QUEUE_H_
#define USRE_APP_MCU_QUEUE_H_
#include "head.h"
#define QUEUE_SIZE 1024  // 循环队列大小（根据需求调整）

// 循环队列结构体
typedef struct {
    uint8_t buffer[QUEUE_SIZE];
    uint16_t front;  // 队列头指针
    uint16_t rear;   // 队列尾指针
    uint16_t count;  // 当前数据量
} CircularQueue;

typedef struct
{
	 uint8_t id;			// 帧头
	 uint8_t cmd;			// 命令字号
	 uint16_t Reg;		// 寄存器
	 uint16_t Reg_Number; // 数据个数
	 uint16_t len;		// 数据长度
						 // INT8U   *data;
	 uint8_t data[QUEUE_SIZE + 3];
	 uint16_t CRC16; // CRC32不包括帧头
	 uint16_t indx;  //

    uint16_t  step;		// 发送步骤
} Modbus_RTU_Protocol;

void Queue_Init(void);

bool Queue_IsFull(void);

bool Queue_IsEmpty(void);

bool Queue_Push(uint8_t data);// 入队操作（中断中使用）


bool Queue_Pop(uint8_t *data); // 出队操作（主程序中使用）

void Init_Modbus_RTU_Protocol(void);


void Modbus_RTU_Queue_vTask(void);

#endif