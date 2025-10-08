/*****************************************************************************
 *
 *  All rights reserved.
 *
 *  @file     Rs485Task.h
 *  @brief    485通讯任务
 *
 *  @author
 *  @email
 *  @version  1.0.0.1
 *  @date      2022-1-13
 *  @license  GNU General Public License (
 *****************************************************************************/
/*--------------------------------- Includes -------------------------------*/

#include "head.h"

/*----------------------Constant / Macro Definitions  ----------------------*/

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define MAX_CMD 128        // 命令数量
#define FRAME_HEADER1 0xAA // 帧头1
#define FRAME_HEADER2 0x55 // 帧头2
/*---------------------------- Type Declarations  --------------------------*/
typedef struct
{
    INT8U step;                    // 解析步骤
    INT8U parseBuf[32];            // 接收解析临时缓冲区
    INT8U parseIndex;              // 接收解析临时缓冲区索引
    INT8U verifyBuf[MAX_DATA_LEN]; // 接收校验缓冲区
    Protocol protocol;             // 接收协议对象
    Queue *recvQueue;              // 接收队列
} ProtocolRecvCtrl;

typedef struct
{
    INT8U step;              // 发送步骤
    INT8U buf[MAX_DATA_LEN]; // 发送缓冲区
    INT32S len;              // 发送数据长度
    INT8U resend;            // 重发次数
    Protocol protocol;       // 接收协议对象
    Queue sendQueue;         // 发送队列
} ProtocolSendCtrl;

Queue _gRecvQueueObj = {0};

/*------------------ Global Definitions and Declarations--------------------*/
static ProtocolSendCtrl _gSendCtrl;
static ProtocolRecvCtrl _gRecvCtrl;
static Protocol _gProtocol;
 uint8_t g_pSendBuf[MAX_DATA_LEN];
uint8_t g_pRecvBuf[MAX_DATA_LEN];

// 命令列表
const uint16_t g_Validcommand[MAX_CMD] =
    {
        // 规定命令范围
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
        0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,0xA5,
        0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085,
        0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095,
//        C_PAUSE_WORKE, C_PAUSE_WORKE, C_START_WORKE,
//        C_LINK, C_CFIG, C_SAMPLE, C_WORKE_SETUP,
//        C_RESUME_WORKSTEP, C_START_SOMECHAN, C_STOP_SOME_CHANNEL,
//        C_LEDCONTROL, C_SENDLEDSTATUS, C_CALIBRATE_START, C_CALIBRATE_SAMP_Start,
//        C_CALIBRATE_SENDDA, C_CALIBRATE_SENDPAR, C_CALIBRATE_CHECKDA,
        0x4000, 0x4060, 0x4065, 0x4070,
        0x4080, 0x4082, 0x4084, 0x4086,
        0x4090, 0x4092, 0x4094, 0x4096,
        0x4091, 0x4093, 0x4095, 0x4097,
        0x50, 0x51, 0x52, 0x53};

// static p485ProMaster_Cmd_Func _g485ProtocolCb = NULL;

static p485ProMaster_ModbusCmd_Func _g485ProtocolCb_Modbus = NULL;

INT32U gFrameByteNuber; // 计算帧数据内容前面的字节长度

// 字符合成4个字节整型 小端模式
INT16U AppUser_CharTo_uint16_Samll(INT8U *data)
{
    INT16U vd = 0;
    vd = data[0] << 8 | data[1];
    return vd;
}
INT16U AppUser_CharTo_uint16_Big(INT8U *data)
{
    INT16U vd = 0;
    vd = data[1] << 8 | data[0];
    return vd;
}

INT32U AppUser_CharTo_uint32_Big(INT8U *data)
{
    INT32U vd = 0;
    vd = (INT32U)data[3] << 24 | (INT32U)data[2] << 16 | (INT32U)data[1] << 8 | (INT32U)data[0];
    return vd;
}

// 字符合成4个字节整型 小端模式
INT16U AppUser_uint16_CharTo_Big(INT16U num, INT8U *data)
{
    data[1] = num;
    data[0] = num >> 8;
    return sizeof(num);
}
INT16U AppUser_uint16_CharTo_Samll(INT16U num, INT8U *data)
{
    data[1] = num & 0x00FF;
    data[0] = (num) >> 8;
    return sizeof(num);
}

INT16U AppUser_uint32_CharTo_Samll(INT32U num, INT8U *data)
{
    //  data[1] = num & 0x00FF;
    //  data[0] = (num) >> 8;

    data[0] = num & 0xff;
    data[1] = (num >> 8) & 0xff;
    data[2] = (num >> 16) & 0xff;
    data[3] = (num >> 24) & 0xff;

    return 4;
}
float AppUser_CharToFloat_Samll(INT8U *data)
{
    union_flaot float32;
    float32.chstr[0] = data[0];
    float32.chstr[1] = data[1];
    float32.chstr[2] = data[2];
    float32.chstr[3] = data[3];
    return (float32.uNumber);
}
uint32_t AppUser_CharToU32_Samll(INT8U *data)
{
    union_U32 UINT32data;
    UINT32data.chstr[0] = data[0];
    UINT32data.chstr[1] = data[1];
    UINT32data.chstr[2] = data[2];
    UINT32data.chstr[3] = data[3];
    return (UINT32data.uNumber);
}
INT16U AppUser_FloatToChar_Samll(float flaotnumber, INT8U *data)
{
    union_flaot float32;
    data[0] = float32.chstr[0];
    data[1] = float32.chstr[1];
    data[2] = float32.chstr[2];
    data[3] = float32.chstr[3];
    return sizeof(float32);
}
/*---------------------------Variable Declarations--------------------------*/
/*------------------------------Static Function-----------------------------*/

Protocol *Get_gProtocolOBj(void)
{
    return &_gProtocol;
}

// 接收串口数据接口
void AppUser_ReceivingDataInterface(uint8_t uRxdata)
{
    static volatile uint8_t a485RxBuffer[1] = {0};

    a485RxBuffer[0] = (uint8_t)uRxdata;
    QueuePush(&_gRecvQueueObj, (uint8_t*)a485RxBuffer); // 消息队列入口，数据通过改函数接收数据
}
/**********************************************************************
 * Function:     Init_gProtocol
 * Description:   初始化通讯帧
 * Input:         void
 * Output:
 * Return:      void
 * Others:
 * Modify Date:    Version:    Author:        Modification:
 * -----------------------------------------------
 * 2023-11-31     V1.0        Hu Weiping
 **********************************************************************/
void Init_gProtocol(void)
{
    _gProtocol.id = 0x01;
    //  _gProtocol.Frame_header[1] = FRAME_HEADER2;
    gFrameByteNuber = sizeof(_gProtocol.id) + sizeof(_gProtocol.cmd) + sizeof(_gProtocol.Reg) + sizeof(_gProtocol.Reg_Number); // 计算协议的字节长度
}

/**********************************************************************
 * Function:     _ResetParse
 * Description:   复位和清空
 * Input:         void
 * Output:
 * Return:      void
 * Others:
 * Modify Date:    Version:    Author:        Modification:
 * -----------------------------------------------
 * 2022-12-05     V1.0        Hu Weiping
 **********************************************************************/
static void _ResetParse(void)
{
    _gRecvCtrl.parseIndex = 0;
    // memset(_gRecvCtrl.parseBuf, 0, sizeof(_gRecvCtrl.parseBuf));
}
/**********************************************************************
 * Function:     IsValidcommand
 * Description:  有效命令返回1,否则反之
 * Input:         cmd---检测命令
 * Output:
 * Return:      void
 * Others:
 * Modify Date:    Version:    Author:        Modification:
 * -----------------------------------------------
 * 2022-12-05     V1.0        Hu Weiping
 **********************************************************************/
INT16U IsValidcommand(INT16U cmd)
{
    INT16U i, flag = 0;
    for (i = 0; i < MAX_CMD; i++)
    {
        if (cmd == g_Validcommand[i])
        {
            flag = 1;
            break;
        }
    }
    return flag;
}

/**********************************************************************
 * Function:     _Rs485ProtocolParse
 * Description:   解析串口数据
 * Input:         void
 * Output:
 * Return:      void
 * Others:
 * Modify Date:    Version:    Author:        Modification:
 * -----------------------------------------------
 * 2022-12-05     V1.0        Hu Weiping
 **********************************************************************/
static INT32S _Rs485ProtocolParse(const INT8U data)
{

	static INT8U str_temp[4];
	static INT8U RX_Flag = 0;
#if 1
	
	//DBG_PRINTF("\r\ndata: %X ",data);
	if (_gRecvCtrl.parseIndex < MAX_DATA_LEN)
	{
		///_gRecvCtrl.parseBuf[_gRecvCtrl.parseIndex++] = data;

		switch (_gRecvCtrl.step)
		{
		case 0:
			_gRecvCtrl.protocol.id = data; /// 主机发送过来的地址1个字节  这地方需要判断设备的地址

			//_gProtocol.id = g_Handle_REC_Device.dev_id; // 设备ID

			if (_gRecvCtrl.protocol.id == _gProtocol.id) ///
			{
				_gRecvCtrl.step++;
			}
			else
			{
				_gRecvCtrl.step = 0;

			   // printf("\r\n id ERR!=%X \r\n",_gRecvCtrl.protocol.id );
			}
			RX_Flag = 0;
           // printf("\r\n id=%X \r\n",_gRecvCtrl.protocol.id );
			break;

		case 1: // Modbus-RTU 指令码
			str_temp[0] = data;

			_gRecvCtrl.protocol.cmd = data;
			if (IsValidcommand(_gRecvCtrl.protocol.cmd))
			{
				_gRecvCtrl.step++;
			}
			else
			{
				_gRecvCtrl.step = 0;
			}
			 //printf("\r\n _gRecvCtrl.protocol.cmd: %X %X %X\r\n",_gRecvCtrl.protocol.cmd,str_temp[0],str_temp[1]);
			break;
		case 2: // 寄存器地址
		{
			str_temp[0] = data;
			_gRecvCtrl.step++;
		}
		break;
		case 3: // 寄存器地址
		{
			str_temp[1] = data;
			_gRecvCtrl.protocol.Reg = AppUser_CharTo_uint16_Samll(str_temp);

			//printf("\r\n _gRecvCtrl.protocol.Reg: %X %X %X\r\n",_gRecvCtrl.protocol.Reg,str_temp[0],str_temp[1]);
			_gRecvCtrl.step++;
		}
		break;
		case 4: // 寄存器个数
		{
			str_temp[0] = data;
			_gRecvCtrl.step++;
		}
		break;
		case 5: // 寄存器个数
		{
			str_temp[1] = data;
			_gRecvCtrl.protocol.Reg_Number = AppUser_CharTo_uint16_Samll(str_temp);
			if(_gRecvCtrl.protocol.cmd == CMD_0x04) //对0x04读取个数做限制
			{
				if (_gRecvCtrl.protocol.Reg_Number < 512)
				{
					_gRecvCtrl.step++;
				}
				else
				{
					_gRecvCtrl.step = 0;
				}
			}
			else
			{
				_gRecvCtrl.step++;
			}

		 //  DBG_PRINTF("\r\n _gRecvCtrl.protocol.Reg_Number: %X %X %X\r\n",_gRecvCtrl.protocol.Reg_Number,str_temp[0],str_temp[1] );
		}
		break;
		default: //

			if (RX_Flag == 0)
			{
				RX_Flag = 1; // 第一次进入这个地方,一帧数据进入一次
				if (_gRecvCtrl.protocol.cmd != CMD_0x10)
				{
					_gRecvCtrl.protocol.len = 0;
				}
				else
				{
					_gRecvCtrl.protocol.len = _gRecvCtrl.protocol.Reg_Number * 2 + 1; // 0x10解析接收数据写入内容
				}
			}
            // DBG_PRINTF("\r\n _gRecvCtrl.step: %d \r\n",_gRecvCtrl.step);
			if (_gRecvCtrl.step >= 5)
			{
				 _gRecvCtrl.protocol.data[_gRecvCtrl.protocol.indx] = data;

				//DBG_PRINTF("\r\n  len  %X  indx %X  %d data: %X",_gRecvCtrl.protocol.len,
				// _gRecvCtrl.protocol.indx,
				// _gRecvCtrl.step,
				//_gRecvCtrl.protocol.data[_gRecvCtrl.protocol.indx]);
				_ResetParse();
				_gRecvCtrl.step++;
				_gRecvCtrl.protocol.indx = _gRecvCtrl.protocol.indx + 1;
			}

			if (((_gRecvCtrl.protocol.len + sizeof(_gRecvCtrl.protocol.CRC32)) == _gRecvCtrl.protocol.indx) && _gRecvCtrl.step >= 5) //
			{
			 volatile	INT16U index = 0, i;
			 volatile	INT32U calVerify = 0x23;
				// 对已接收的数据进行校验
				memset(_gRecvCtrl.verifyBuf, 0, MAX_DATA_LEN);

				_gRecvCtrl.verifyBuf[index++] = _gRecvCtrl.protocol.id;
				_gRecvCtrl.verifyBuf[index++] = _gRecvCtrl.protocol.cmd;

				//				index += AppUser_uint16_CharTo_Samll(_gRecvCtrl.protocol.frame_indx, _gRecvCtrl.verifyBuf + index);

				index += AppUser_uint16_CharTo_Samll(_gRecvCtrl.protocol.Reg, _gRecvCtrl.verifyBuf + index);

				index += AppUser_uint16_CharTo_Samll(_gRecvCtrl.protocol.Reg_Number, _gRecvCtrl.verifyBuf + index);

				for (i = 0; i < _gRecvCtrl.protocol.len; i++)
				{
					_gRecvCtrl.verifyBuf[index++] = _gRecvCtrl.protocol.data[i];
				}

				_gRecvCtrl.protocol.CRC32 = AppUser_CharTo_uint16_Samll(&_gRecvCtrl.protocol.data[_gRecvCtrl.protocol.len]);

				// calVerify = AppUse_CheckSum(_gRecvCtrl.verifyBuf, index); // CRC校验码小端模式
				// calVerify = (INT32U) CRC16_XMODEM(_gRecvCtrl.verifyBuf,index); // CRC校验码小端模式
				///	calVerify = CRC16_XMODEM_SmallMode(_gRecvCtrl.verifyBuf, index); // CRC校验码小端模式
				calVerify = modbus_crc(_gRecvCtrl.verifyBuf, index); // CRC校验码小端模式

				if (calVerify != _gRecvCtrl.protocol.CRC32) // 校验不通过
				{
						printf("\r\nCRC16Honst=%XMCU=%X  \r\n ", _gRecvCtrl.protocol.CRC32, calVerify);
					//DBG_PRINTF(" \r\ncrc_Code ERR Honst:%X != MCU:%X  \r\n", _gRecvCtrl.protocol.CRC32, calVerify);

					for (i = 0; i < index; i++)
						printf(" \r\nRX_pack %X %d  \r\n", _gRecvCtrl.verifyBuf[i], i);
					_ResetParse();
					_gRecvCtrl.step = 0;
					RX_Flag = 0;
					memset(&_gRecvCtrl.protocol, 0, sizeof(_gRecvCtrl.protocol));
				}
				else
				{
					if (_g485ProtocolCb_Modbus != NULL)
					{
					
						//printf("\r\n ok cmd\r\n");
						//  printf("\r\n 调用 AppUser_ModBus_RTU_CmdAnalys函数 \r\n ");
						_g485ProtocolCb_Modbus(&_gRecvCtrl.protocol); // 调用 AppUser_ModBus_RTU_CmdAnalys函数
					}
					else
					{
						//	printf("\r\n   未初始化 指针函数\r\n");
					}
					_ResetParse();
					_gRecvCtrl.step = 0;
					RX_Flag = 0;

					memset(&_gRecvCtrl.protocol, 0, sizeof(_gRecvCtrl.protocol));
				}
			}
			break;
		}
	}

#endif
	return 1;
}

typedef INT32S (*p4_Rs485Protocol_Func)(const INT8U data);

p4_Rs485Protocol_Func _Rs485Protocol = NULL;
/**********************************************************************
 * Function:     AppUser_PortocolRecv
 * Description:   串口中断接收到数据，读取消息队列数据
 * Input:         void
 * Output:
 * Return:      void
 * Others:
 * Modify Date:    Version:    Author:        Modification:
 * -----------------------------------------------
 * 2022-12-05     V1.0        Hu Weiping
 **********************************************************************/
void AppUser_PortocolRecv(void)
{
    static INT16U outTimer = 0;
    if (_gRecvCtrl.recvQueue != NULL)
    {
        _Rs485Protocol = _Rs485ProtocolParse;
        // 接收队列中不为空
        if (!IsQueueEmpty(_gRecvCtrl.recvQueue))
        {
            INT8U data = 0;

            while (!IsQueueEmpty(_gRecvCtrl.recvQueue))
            {
                outTimer = 0;
                QueuePull(_gRecvCtrl.recvQueue, &data); // 消息队列拿出数据
                _Rs485Protocol(data);
            }
            // 更新接收时间
        }
        else
        {
            outTimer++;
            if (outTimer > 1000) // 数据包超时处理
            {
                outTimer = 0;
                _gRecvCtrl.step = 0;
            }
        }
    }
}
/**********************************************************************
 * Function:     AppUser_Rs485ProtocolPack_SendData
 * Description:   打包数据发送
 * Input:         void
 * Output:
 * Return:      void
 * Others:
 * Modify Date:    Version:    Author:        Modification:
 * -----------------------------------------------
 * 2022-12-05     V1.0        Hu Weiping
 **********************************************************************/
static INT32S AppUser_Rs485ProtocolPack(const Protocol *protocol, INT8U *buf)
{
	INT32U index = 0;

#if 1
	static INT32U uCunt = 1, dispCunt = 1;

	INT16U crc_code = 0, i;

	if (protocol == NULL || buf == NULL)
	{
		return -1;
	}
	buf[index++] = protocol->id;  //
	buf[index++] = protocol->cmd; //

	switch (protocol->cmd)
	{
	case 0x01:										  // 0x01
		buf[index++] = (protocol->len & 0xFF00) >> 8; // 打包数据长度
		buf[index++] = protocol->len;				  // 打包数据长度
		if (protocol->len > 0)						  // 打包数据
		{
			memcpy(&buf[index], protocol->data, protocol->len);
			index += protocol->len;
		}
		break;
	case 0x02:										  // 0x02
		buf[index++] = (protocol->len & 0xFF00) >> 8; // 打包数据长度
		buf[index++] = protocol->len;				  // 打包数据长度
		if (protocol->len > 0)						  // 打包数据
		{
			memcpy(&buf[index], protocol->data, protocol->len);
			index += protocol->len;
		}
		break;
	case 0x03: // 0x03
			   //		buf[index++] = (protocol->Reg&0xFF00)>>8;	 // 打包寄存器
			   //		buf[index++] = protocol->Reg;	 // 打包寄存器

		buf[index++] = protocol->len; // 长度

		for (i = 0; i < protocol->len; i++)
		{
			buf[index++] = protocol->data[i]; // 长度
		}
		break;
	case 0x04: // 0x04
			   //		buf[index++] = (protocol->Reg&0xFF00)>>8;	 // 打包寄存器
			   //		buf[index++] = protocol->Reg;	 // 打包寄存器

		buf[index++] = protocol->len; // 长度

		for (i = 0; i < protocol->len; i++)
		{
			buf[index++] = protocol->data[i]; // 长度
		}
		break;

	case 05: // 写入单寄存器 0x05
	{
		//		buf[index++] = protocol->regH; // 被写入的地址
		//		buf[index++] = protocol->regL;

		//		buf[index++] = protocol->reg_dataH; // 被写入的数据
		//		buf[index++] = protocol->reg_dataL;
	}
	break;
	case 06: // 写入单寄存器 0x06
	{
		buf[index++] = (protocol->Reg & 0xFF00) >> 8; // 打包寄存器
		buf[index++] = protocol->Reg;				  // 打包寄存器

		buf[index++] = (protocol->Reg_Number & 0xFF00) >> 8; // 长度
		buf[index++] = protocol->Reg_Number & 0x00FF;		 // 长度
	}
	break;

	case 0x10: ////写入多寄存器0x10
	{
		buf[index++] = (protocol->Reg & 0xFF00) >> 8; // 打包寄存器
		buf[index++] = protocol->Reg;				  // 打包寄存器

		buf[index++] = (protocol->Reg_Number & 0xFF00) >> 8; // 长度
		buf[index++] = protocol->Reg_Number & 0x00FF;		 // 长度
	}
	break;

	default: // 发送错误码

		buf[1] = protocol->cmd - 0x80;
		buf[2] = protocol->cmd;
		buf[3] = 0x01;
		index = 4;
		// buf[index++] = protocol->regH; // 寄存器中被写入的地址
		// buf[index++] = protocol->regL;
		// buf[index++] = '<';
		// buf[index++] = '!'; // 错误码 字符
		// buf[index++] = '<';
		// buf[index++] = '!'; // 错误码 字符
		// buf[index++] = '<';
		// buf[index++] = '!'; // 错误码 字符
		// buf[index++] = '<';
		// buf[index++] = '!'; // 错误码 字符
		//	printf("\r\n 0X=%X指令异常 !", protocol->cmd);
		break;
	}
	uCunt++;
	// 生成校验码
	crc_code = modbus_crc(&buf[0], index);

	buf[index++] = (crc_code & 0xFF00) >> 8;
	buf[index++] = crc_code & 0x00FF;
	dispCunt++;
	//  for(i =0 ;i<index;i++)
	//	{
	//			printf("\r\n  buf=%X  i=%d \r\n", buf[i],i);
	//	}

	// printf("\r\n cmd =0x%X crc_code %X index=%d  %d \r\n", protocol->cmd, crc_code, index, uCunt);
	if (uCunt >= 60000)
		uCunt = 0;
#endif
	return index;
}
/**********************************************************************
 * Function:     RS485_ModbusCmdTask
 * Description:   RS4485_modbus指令解析
 * Input:         void *p_arg
 * Output:
 * Return:      void
 * Others:      Modbus-RTU
 * Modify Date:    Version:    Author:        Modification:
 * -----------------------------------------------
 * 2022-012-05    V1.0        Hu Weiping
 **********************************************************************/
void RS485_ModbusCmdTask(void *p_arg)
{

    Init_gProtocol(); // 初始化
    memset(&_gSendCtrl, 0, sizeof(_gSendCtrl));
    memset(&_gRecvCtrl, 0, sizeof(_gRecvCtrl));
    _g485ProtocolCb_Modbus = AppUser_Msg_CmdAnalys; // RS485和CAN共用一个函数执行 modbus协议解析回调

    // 接收队列初始化
    memset(&_gRecvQueueObj, 0, sizeof(_gRecvQueueObj));
    QueueInit(128, sizeof(INT8U), &_gRecvQueueObj, g_pRecvBuf);
    _gRecvCtrl.recvQueue = &_gRecvQueueObj; // 接收消息队列指针

    // 发送队列初始化
    QueueInit(1, (uint8_t)sizeof(Protocol), &_gSendCtrl.sendQueue, g_pSendBuf);
}

void Modbus_Init(void)
{
}

INT32S Rs485SendMsg(const Protocol *protocol)
{
    uint32_t ret = 0;
    memset(_gSendCtrl.buf, 0, sizeof(_gSendCtrl.buf));
    _gSendCtrl.len = AppUser_Rs485ProtocolPack(protocol, _gSendCtrl.buf);

    return ret;
}

INT32S AppUser_Rs485Send_Data(const Protocol *protocol)
{
    volatile uint32_t ret = 0;
    memset(_gSendCtrl.buf, 0, sizeof(_gSendCtrl.buf));

    _gSendCtrl.len = AppUser_Rs485ProtocolPack(protocol, _gSendCtrl.buf);

    if (_gSendCtrl.len > 0)
    {

        // RS485_Send_Data(_gSendCtrl.buf, _gSendCtrl.len);
        //  HAL_UART_WriteIT(&g_uart2, _gSendCtrl.buf,  _gSendCtrl.len);


            // HAL_UART_Transmit(&huart1, (uint8_t *)_gSendCtrl.buf, _gSendCtrl.len, 10);
             HAL_UART_Transmit(&huart7, (uint8_t *)_gSendCtrl.buf, _gSendCtrl.len, 10);
       //  HAL_UART_Transmit_DMA(&huart1, _gSendCtrl.buf, _gSendCtrl.len);
    }

    return ret;
}

INT32S AppUser_CANSendModbus_Data(const Protocol *protocol)
{
    uint32_t ret = 0;

#if 0
    uint32_t ret = 0, option = 0, local_id = 0;

    memset(_gSendCtrl.buf, 0, sizeof(_gSendCtrl.buf));

    _gSendCtrl.len = AppUser_Rs485ProtocolPack(protocol, _gSendCtrl.buf);
    if (_gSendCtrl.len > 0)
    {
//      AppUser_Get_ExtId_And_Cmd(&local_id, &option);
        //can_send_data(local_id, option, (char *)&_gSendCtrl.buf, _gSendCtrl.len);
        // printf("\r\n  local_id = 0x %X  option = 0x %X  ",local_id,option);
    }
#endif
    return ret;
}

void AppUser_Rs485ProtocolParse(const INT8U data)
{
    _Rs485ProtocolParse(data);
}

void Get_Primary_Parameter(void)
{
}

/*****************************************************************************
 *                                    END FILE
 *****************************************************************************/
