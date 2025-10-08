
#ifndef RS485_TASK_H_
#define RS485_TASK_H_

#include "head.h"

/*--------------------------------- Includes -------------------------------*/

/*----------------------Constant / Macro Definitions  ----------------------*/
#define MAX_DATA_LEN (128)

#define USART6_REC_LEN 128 // 定义最大接收字节数
#define USART6_TRA_LEN 128 // 发送缓冲,最大只能定义到65535

#define DEV_INIT_ONLIN 0 // 设备初始状态
#define DEV_ONLIN 1		 // 设备在线状态
#define DEV_ONT_ONLIN 2	 // 设备离线状态

typedef union union_flaot
{
	float uNumber;
	uint8_t chstr[4]; // e
} union_flaot;


typedef union union_U32
{
	uint32_t uNumber;
	uint8_t chstr[4]; // e
} union_U32;

/*---------------------------- Type Declarations  --------------------------*/

#if 0
typedef struct  
{
	INT8U Frame_header[2];//帧头
	INT16U  frame_indx;//第几帧
	INT16U cmd;//命令字号
	INT16U len; //数据长度
	INT8U data[68];
	INT16U CRC32; //CRC32不包括帧头
	INT16U indx;//
}Protocol;
#else
typedef struct
{
	 INT8U id;			// 帧头
	 INT8U cmd;			// 命令字号
	 INT16U Reg;		// 寄存器
	 INT16U Reg_Number; // 数据个数
	 INT16U len;		// 数据长度
						 // INT8U   *data;
	 INT8U data[256 + 3];
	 INT16U CRC32; // CRC32不包括帧头
	 INT16U indx;  //
} Protocol;

#endif

typedef INT32S (*p485ProMaster_Cmd_Func)(INT8U num, INT8U commd, INT8U *data, INT32U len);
typedef INT32S (*p485ProMaster_ModbusCmd_Func)(Protocol *pRecvCtrlmodbus);

extern INT16U IsValidcommand(INT16U cmd);

extern void AppUser_ReceivingDataInterface(uint8_t uRxdata);
extern void RS485_ModbusCmdTask(void *p_arg);
extern INT32S AppUser_Rs485Send_Data(const Protocol *protocol);

extern void Init_gProtocol(void);
extern void AppUser_PortocolRecv(void);

extern INT16U AppUser_uint16_CharTo_Samll(INT16U num, INT8U *data);
extern INT16U AppUser_uint16_CharTo_Big(INT16U num, INT8U *data);
extern INT32U AppUser_CharTo_uint32_Big(INT8U *data);


extern INT16U AppUser_uint32_CharTo_Samll(INT32U num, INT8U *data);

extern INT16U AppUser_CharTo_uint16_Samll(INT8U *data);
extern INT16U AppUser_CharTo_uint16_Big(INT8U *data);

extern float AppUser_CharToFloat_Samll(INT8U *data);
extern INT16U AppUser_FloatToChar_Samll(float flaotnumber, INT8U *data);


extern  uint32_t AppUser_CharToU32_Samll(INT8U *data);

extern void Get_Primary_Parameter(void);

extern Protocol *Get_gProtocolOBj(void);
extern void Modbus_Init(void);

/*------------------------------Function Implement -------------------------*/
#endif
/*****************************************************************************
 *                                    END FILE
 *****************************************************************************/
