
#ifndef RX_QUEUE_H
#define RX_QUEUE_H
/*--------------------------------- Includes -------------------------------*/
#include <stdint.h>
#include"head.h"
/*----------------------Constant / Macro Definitions  ----------------------*/
#define MAX_DATA_BUF			(128)
/*---------------------------- Type Declarations  --------------------------*/
typedef struct 
{
	uint16_t itemSize;
	uint32_t readIndex;
	uint32_t writeIndex;
	uint32_t maxItem;
	void  *data;
}Queue;
/*------------------ Global Definitions and Declarations--------------------*/
/*---------------------------Variable Declarations--------------------------*/
/*------------------------------Static Function-----------------------------*/
/*------------------------------Function Prototype--------------------------*/
extern int32_t QueueInit(const uint32_t maxItem, const uint8_t itemSize, Queue *queueObj, void *data);
extern int32_t QueuePull(Queue *queueObj, void *outValue);
extern uint32_t QueuePullBytes(Queue *queueObj, void *outValue, const uint32_t maxLen);
extern int32_t QueuePush(Queue *queueObj, void *inValue);
extern int32_t QueuePushBytes(Queue *queueObj, void *inValue, const uint32_t len);
extern uint8_t IsQueueEmpty(const Queue *queueObj);
extern uint8_t IsQueueFull(const Queue *queueObj);
extern int32_t QueueItemCount(const Queue *queueObj);

/*------------------------------Function Implement -------------------------*/

#endif
/*****************************************************************************
*                                    END FILE                                 
*****************************************************************************/
