/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    fdcan.h
  * @brief   This file contains all the function prototypes for
  *          the fdcan.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FDCAN_H__
#define __FDCAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern FDCAN_HandleTypeDef hfdcan1;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_FDCAN1_Init(void);

/* USER CODE BEGIN Prototypes */

#define NumMaxCanData		8
#define NumMaxTpCanData		1024
#define SizeHeaderCanFrame	4	//can数据包的头

//Can帧类型
typedef enum 
{
	CanFrtameType_Short=4001,		//短帧
	CanFrtameType_TpStart,			//长帧首帧
	CanFrtameType_Tp,				//长帧中间帧
	CanFrtameType_TpEnd,			//长帧尾帧
}CanFrtameType_t;


typedef struct
{
	uint16_t dataLen;
	uint8_t addrSlave;
	uint8_t cmd;
	uint8_t* data;	
}CanFrame_t;

#define BUFFERSIZE_REC		64		//接收缓存大小
#define BUFFERSIZE_SEND		64		//发送缓存大小

#define TIMEOUT_SENDONEPACK	500			//发送单包超时时间，ms

#define QueueHeadForward(Queue,Size) do{(Queue).head++;\
										if((Queue).head>=Size)(Queue).head=0;\
										(Queue).size++;\
										if((Queue).size>=Size){(Queue).size=Size;(Queue).tail=(Queue).head;}}while(0)

#define QueueTailForward(Queue,Size) do{(Queue).tail++;\
										if((Queue).tail>=Size)(Queue).tail=0;\
										(Queue).size--;}while(0)
#pragma pack (4)
typedef union 
{
	//按照32位访问
	uint32_t dWordData;			                    //字操作定义
	//按照16bit访问
	uint16_t wordData[2];
	//按照8bit访问
	uint8_t  byteData[4];
	//按照具体的项目访问
	struct
	{

		uint32_t	EXID	:29;			            //11位STID（扩展帧的高11位）18bit EXID（扩展帧的低18位）			
	}canItem;
	//按照用户定义访问
	struct
	{

		//一下为EXID部分，发送顺序是MSB，仲裁时，0（显性电平）优先
		uint32_t addrSlave		:8;					//分机地址，主机的地址固定为0，分机地址如果是0xff,表示广播
		uint32_t cmd				:8;					//命令
		uint32_t userPar			:12;				//用户参数，可以作为数据索引使用				
		uint32_t direct			:1;					//方向，主机向分机发送为0，分机向主机发送为1
	}userItemp;
}PackHeader_t;


typedef struct
{	
	uint16_t userPar;			//用户的参数
	uint8_t addrSlave;			//分机地址, 对于分机来说，不需要该变量，因为就是自己的地址
	uint8_t dataLen;				//数据长度
	uint8_t cmd;					//命令
	uint8_t data[NumMaxCanData];	//数据指针
}CanShortFrame_t;

typedef struct
{
	CanFrame_t frame;	//长帧数据
	uint16_t checkValueRec;	//接收到的校验值
	uint16_t checkValueCal;	//计算的校验值
	uint16_t lenIndex;		//当前已经接收或者发送的长度
	uint8_t status;			//当前状态，0：空闲，1，正在接收，2，接收完成
}TpProcessData;
typedef struct 
{
	struct
	{
		CanShortFrame_t		frameBuffer[BUFFERSIZE_REC];//接收帧缓存地址空间	
		uint32_t 				timeLastRec;				//单包接收时间
		PackHeader_t		packHeader;
		//CAN_RxHeaderTypeDef	rxHeader;		
		//TpProcessData 		tpProcess;					//长包处理数据
		uint8_t					tpData[NumMaxTpCanData];	//长包的数据部分
		CanFrame_t			shortFrame;					//返回给用户的短包
		uint8_t					spData[NumMaxCanData];		//短包的数据部分
		uint16_t 				head;
		uint16_t 				tail;
		uint16_t 				size;
	}recStruct;	
	struct
	{
		CanShortFrame_t		frameBuffer[BUFFERSIZE_SEND];//发送包缓存地址空间		
		PackHeader_t		packHeader;
	//	CAN_TxHeaderTypeDef	txHeader;
		uint32_t 				timeLastSend;				//单包发送时间	
		uint16_t 				head;
		uint16_t 				tail;
		uint16_t 				size;
		uint16_t 				isSending;					//正在发送状态，这个主要用于上次发送失败的标记，下次再次尝试发送
	}sendStruct;	
	uint8_t addrSelf;										//本机地址，主机的地址是0
}Can_Data;
#pragma pack ()
/* FDCAN 引脚 定义 */

#define FDCAN_RX_GPIO_PORT                GPIOA
#define FDCAN_RX_GPIO_PIN                 GPIO_PIN_11
#define FDCAN_RX_GPIO_AF                  GPIO_AF9_FDCAN1
#define FDCAN_RX_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)              /* PA口时钟使能 */

#define FDCAN_TX_GPIO_PORT                GPIOA
#define FDCAN_TX_GPIO_PIN                 GPIO_PIN_12
#define FDCAN_TX_GPIO_AF                  GPIO_AF9_FDCAN1
#define FDCAN_TX_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOA_CLK_ENABLE(); }while(0)              /* PA口时钟使能 */

/* FDCAN1接收RX0中断使能 */
#define FDCAN1_RX0_INT_ENABLE   1         /* 0,不使能;1,使能. */

/******************************************************************************************/

uint8_t fdcan_init(uint16_t presc, uint8_t tsjw, uint16_t ntseg1, uint8_t ntseg2, uint32_t mode);  /* FDCAN初始化 */
uint8_t fdcan_send_msg(uint8_t *msg, uint32_t len);                                                /* FDCAN发送数据 */
uint8_t fdcan_receive_msg(uint8_t *buf);                                                           /* FDCAN接收数据 */


/*****************************FDCAN3*************************************************************/

#define FDCAN3_RX_GPIO_PORT                GPIOG
#define FDCAN3_RX_GPIO_PIN                 GPIO_PIN_10
#define FDCAN3_RX_GPIO_AF                  GPIO_AF2_FDCAN3
#define FDCAN3_RX_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)              /* PA口时钟使能 */

#define FDCAN3_TX_GPIO_PORT                GPIOG
#define FDCAN3_TX_GPIO_PIN                 GPIO_PIN_9
#define FDCAN3_TX_GPIO_AF                  GPIO_AF2_FDCAN3
#define FDCAN3_TX_GPIO_CLK_ENABLE()        do{ __HAL_RCC_GPIOG_CLK_ENABLE(); }while(0)              /* PA口时钟使能 */

/* FDCAN1接收RX0中断使能 */
#define FDCAN3_RX0_INT_ENABLE   1         /* 0,不使能;1,使能. */

/******************************************************************************************/
extern FDCAN_HandleTypeDef       g_fdcan3_handle;             /* FDCANx句柄 */
extern FDCAN_TxHeaderTypeDef     g_fdcan3_txheade;            /* 发送消息 */
extern FDCAN_RxHeaderTypeDef     g_fdcan3_rxheade;            /* 接收消息 */
uint8_t fdcan3_init(uint16_t presc, uint8_t tsjw, uint16_t ntseg1, uint8_t ntseg2, uint32_t mode);  /* FDCAN初始化 */
uint8_t fdcan3_send_msg(uint8_t *msg, uint32_t len);                                                /* FDCAN发送数据 */
uint8_t fdcan3_receive_msg(uint8_t *buf);                                                           /* FDCAN接收数据 */


void CAN1_Send_TEST(void);//初始化测试CAN 不知道为啥需要发送，才能发送 后续解决


int CanFr_Init(void);


extern HAL_StatusTypeDef FDCAN_SendMessage(uint8_t fc, uint16_t dest_did, uint16_t sub, uint8_t* data, uint8_t size);




/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __FDCAN_H__ */

