
 
#ifndef __CANFR_H_
#define __CANFR_H_

#include "sys.h"
#include "head.h"

#define	CANMASTER			0//设置为主机

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

int CanFr_Init(void);
int CanFr_SendData(uint8_t addrSlave, uint8_t cmd, uint8_t* data, uint16_t dataLen);
//Can 发送用户帧率
int CanFr_SendFrame(CanFrame_t* pFrame) ;
//获取接收到的Can数据包，如果没有数据包，返回Null
CanFrame_t* CanFr_GetFrame( void );
#endif

 
/********************************End of File************************************/

