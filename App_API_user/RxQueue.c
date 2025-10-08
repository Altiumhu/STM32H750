/*****************************************************************************
 *  Copyright (C)
 *  All rights reserved.
 *
 *  @file     queue.c
 *  @brief    通用环形队列
 *
 *  @author
 *  @email
 *  @version  1.0.0.1
 *  @date      2022-1-13
 *  @license  GNU General Public License (GPL)
 *
 *****************************************************************************/
/*--------------------------------- Includes -------------------------------*/
#include <string.h>
#include "Rxqueue.h"

#include "head.h"

/*----------------------Constant / Macro Definitions  ----------------------*/
// #define USE_MALLOC
#ifdef USE_MALLOC
#define Queue_malloc(size) mymalloc(SRAMIN, size)
#define Queue_free(ptr) myfree(SRAMIN, ptr)
#else
// #error "no malloc"
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif
/*---------------------------- Type Declarations  --------------------------*/
/*------------------ Global Definitions and Declarations--------------------*/
/*---------------------------Variable Declarations--------------------------*/
/*------------------------------Static Function-----------------------------*/
/*------------------------------Function Prototype--------------------------*/
/*------------------------------Function Implement -------------------------*/
int32_t QueueInit(const uint32_t maxItem, const uint8_t itemSize, Queue *queueObj, void *data)
{
	// 参数检查
	if (queueObj == NULL || data == NULL || itemSize == 0)
	{
		return 1;
	}

	// 分配内存空间
	queueObj->data = data;

	// 队列初始化
	memset(queueObj->data, 0, maxItem * itemSize);
	queueObj->itemSize = itemSize;
	queueObj->maxItem = maxItem;
	queueObj->readIndex = 0;
	queueObj->writeIndex = 0;

	return 0;
}
//取出数据
int32_t QueuePull(Queue *queueObj, void *outValue)
{
	uint8_t *dataPtr = NULL;

	// 参数判断
	if (queueObj == NULL || outValue == NULL)
	{
		return 1;
	}

	// 队列是否空
	if (IsQueueEmpty(queueObj))
	{
		return 1;
	}

	// 队列不为空，从队列中读取数据
	dataPtr = (uint8_t *)queueObj->data;
	memcpy(outValue, &dataPtr[queueObj->readIndex * queueObj->itemSize], queueObj->itemSize);
	queueObj->readIndex = (queueObj->readIndex + 1) % queueObj->maxItem;

	return 0;
}

uint32_t QueuePullBytes(Queue *queueObj, void *outValue, const uint32_t maxLen)
{
	uint8_t *dataPtr = NULL;
	uint32_t count = 0;

	// 参数判断
	if (queueObj == NULL || outValue == NULL)
	{
		return 0;
	}

	// 队列是否空
	if (IsQueueEmpty(queueObj))
	{
		return 0;
	}

	count = QueueItemCount(queueObj);
	if (count > maxLen)
	{
		count = maxLen;
	}

	dataPtr = (uint8_t *)queueObj->data;
	if (queueObj->maxItem - queueObj->readIndex >= count)
	{ // 不存在回环
		memcpy(outValue, &dataPtr[queueObj->readIndex * queueObj->itemSize], count * queueObj->itemSize);
	}
	else
	{ // 存在回环
		uint32_t temp = 0;
		uint8_t *dstData = (uint8_t *)outValue;

		temp = queueObj->maxItem - queueObj->readIndex;
		memcpy(dstData, &dataPtr[queueObj->readIndex * queueObj->itemSize], temp * queueObj->itemSize);
		memcpy(dstData + temp * queueObj->itemSize, &dataPtr[0], (count - temp) * queueObj->itemSize);
	}
	queueObj->readIndex = (queueObj->readIndex + count) % queueObj->maxItem;

	return count;
}

int32_t QueuePush(Queue *queueObj, void *inValue)
{
	uint8_t *dataPtr = NULL;

	// 参数判断
	if (queueObj == NULL || inValue == NULL)
	{
		return 1;
	}

	// 队列是否满
	if (IsQueueFull(queueObj))
	{
		return 1;
	}

	// 队列不满，将数据压入队列
	dataPtr = (uint8_t *)queueObj->data;
	memcpy(&dataPtr[queueObj->writeIndex * queueObj->itemSize], (uint8_t *)inValue, queueObj->itemSize);
	queueObj->writeIndex = (queueObj->writeIndex + 1) % queueObj->maxItem;

	return 0;
}

int32_t QueuePushBytes(Queue *queueObj, void *inValue, const uint32_t len)
{
	uint8_t *dataPtr = NULL;
	int32_t count = 0;

	// 参数判断
	if (queueObj == NULL || inValue == NULL || len == 0)
	{
		return -1;
	}

	// 队列是否满
	if (IsQueueFull(queueObj))
	{
		return -1;
	}

	// 检查剩余空间是否能容纳本次压入的数据
	count = QueueItemCount(queueObj);
	if (queueObj->maxItem - count < len)
	{
		return -1;
	}

	// 剩余空间足够压入数据
	dataPtr = (uint8_t *)queueObj->data;
	if (queueObj->maxItem - len > queueObj->writeIndex)
	{ // 不存在回环
		memcpy(&dataPtr[queueObj->writeIndex * queueObj->itemSize], (uint8_t *)inValue, queueObj->itemSize * len);
	}
	else
	{ // 存在回环
		uint32_t temp = 0;
		uint8_t *srcData = (uint8_t *)inValue;

		temp = queueObj->maxItem - queueObj->writeIndex;
		memcpy(&dataPtr[queueObj->writeIndex * queueObj->itemSize], srcData, temp * queueObj->itemSize);
		memcpy(&dataPtr[0], srcData + temp * queueObj->itemSize, (len - temp) * queueObj->itemSize);
	}

	queueObj->writeIndex = (queueObj->writeIndex + len) % queueObj->maxItem;

	return len;
}

uint8_t IsQueueEmpty(const Queue *queueObj)
{
	// 参数判断
	if (queueObj == NULL)
	{
		return FALSE;
	}

	// 判断队列是否为空
	if (queueObj->readIndex == queueObj->writeIndex)
	{
      // printf("\r\n readIndex%d  writeIndex=%d",queueObj->readIndex ,queueObj->writeIndex);
		return TRUE;
	}

	return FALSE;
}

uint8_t IsQueueFull(const Queue *queueObj)
{
	uint8_t tmpIndex = 0;

	// 参数判断
	if (queueObj == NULL)
	{
		return FALSE;
	}

	// 判断队列是否为满
	tmpIndex = (queueObj->writeIndex + 1) % queueObj->maxItem;
	if (tmpIndex == queueObj->readIndex)
	{
		return TRUE;
	}

	return FALSE;
}

int32_t QueueItemCount(const Queue *queueObj)
{
	int32_t itemCount = 0;

	if (queueObj == NULL)
	{
		return -1;
	}

	itemCount = (queueObj->writeIndex - queueObj->readIndex + queueObj->maxItem) % queueObj->maxItem;

	return itemCount;
}
/*****************************************************************************
 *                                    END FILE
 *****************************************************************************/
