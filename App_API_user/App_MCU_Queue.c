#include "head.h"
#include "App_MCU_Queue.h"

CircularQueue uartQueue; // 全局队列实例

Modbus_RTU_Protocol g_Handle_Modbus_RTU_Protocol;

void Modbus_RTU_ProtocolParse_Queue(uint8_t data);

// 初始化循环队列
void Queue_Init(void)
{
    uartQueue.front = 0;
    uartQueue.rear = 0;
    uartQueue.count = 0;
}

// 检查队列是否满
bool Queue_IsFull(void)
{
    return uartQueue.count == QUEUE_SIZE;
}

// 检查队列是否空
bool Queue_IsEmpty(void)
{
    return uartQueue.count == 0;
}

// 入队操作（中断中使用）
bool Queue_Push(uint8_t data)
{
    if (Queue_IsFull())
    {
        return false; // 队列满时丢弃数据
    }

    uartQueue.buffer[uartQueue.rear] = data;
    uartQueue.rear = (uartQueue.rear + 1) % QUEUE_SIZE;
    uartQueue.count++;
    return true;
}

// 出队操作（主程序中使用）
bool Queue_Pop(uint8_t *data)
{
    if (Queue_IsEmpty())
    {
        return false;
    }

    *data = uartQueue.buffer[uartQueue.front];
    uartQueue.front = (uartQueue.front + 1) % QUEUE_SIZE;
    uartQueue.count--;
    return true;
}

// 解析结构体初始化
void Init_Modbus_RTU_Protocol(void)
{
    memset(&g_Handle_Modbus_RTU_Protocol, 0, sizeof(Modbus_RTU_Protocol)); // 初始化存储数据结构体
    g_Handle_Modbus_RTU_Protocol.id = 0x01;
}

//内部MCU modbus协议解析任务
void Modbus_RTU_Queue_vTask(void)
{
    uint8_t receivedData;
    while (Queue_Pop(&receivedData)) //  从队列中取出数据
    {
        //Modbus_RTU_ProtocolParse_Queue(receivedData); // 指令解析
      //  printf("\r\nreceivedData=0x%X ",receivedData);

        _Rs_TCP_ProtocolParse(receivedData);
    }
}
//
void Modbus_RTU_ProtocolParse_Queue(uint8_t data)
{

    static uint8_t RX_Flag = 0,datalen=0;

    switch (g_Handle_Modbus_RTU_Protocol.step)
    {
    case 0:
        if (g_Handle_Modbus_RTU_Protocol.id == data) ///// 设备ID
        {
            g_Handle_Modbus_RTU_Protocol.step++;
        }
        else
        {
            g_Handle_Modbus_RTU_Protocol.step = 0;

            // DBG_PRINTF("\r\n id ERR!=%X \r\n",_gRecvCtrl.protocol.id );
        }
        RX_Flag = 0;
        // DBG_PRINTF("\r\n id=%X \r\n",_gRecvCtrl.protocol.id );
        break;

    case 1: // Modbus-RTU 指令码

        g_Handle_Modbus_RTU_Protocol.cmd = data;
        if (IsValidcommand(g_Handle_Modbus_RTU_Protocol.cmd))
        {
            g_Handle_Modbus_RTU_Protocol.step++;
        }
        else
        {
            g_Handle_Modbus_RTU_Protocol.step = 0;
        }
        // printf("\r\n _gRecvCtrl.protocol.cmd: %X %X %X\r\n",_gRecvCtrl.protocol.cmd,str_temp[0],str_temp[1]);
        break;
    case 2: // 数据长度
    {
        g_Handle_Modbus_RTU_Protocol.len =data;
        if(g_Handle_Modbus_RTU_Protocol.len!=0x14)
        {
          g_Handle_Modbus_RTU_Protocol.step++;
        }
        else
        {
             g_Handle_Modbus_RTU_Protocol.step = 0;
        }
    }
    break;
    case 3: // 接收数据内容
    {
        g_Handle_Modbus_RTU_Protocol.data[g_Handle_Modbus_RTU_Protocol.indx] =data;
        g_Handle_Modbus_RTU_Protocol.indx=g_Handle_Modbus_RTU_Protocol.indx+1;
        datalen = g_Handle_Modbus_RTU_Protocol.len+sizeof(g_Handle_Modbus_RTU_Protocol.CRC16);
        if(datalen == g_Handle_Modbus_RTU_Protocol.len)
        {

            
        }
      
    }
    break;
     default: //
     
     break;
#if 0
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
					//	printf("\r\nCRC16Honst=%XMCU=%X   CRC16=%X\r\n ", _gRecvCtrl.protocol.CRC32, calVerify);
					DBG_PRINTF(" \r\ncrc_Code ERR Honst:%X != MCU:%X  \r\n", _gRecvCtrl.protocol.CRC32, calVerify);

					for (i = 0; i < index; i++)
						DBG_PRINTF(" \r\nRX_pack %X %d  \r\n", _gRecvCtrl.verifyBuf[i], i);
					_ResetParse();
					_gRecvCtrl.step = 0;
					RX_Flag = 0;
					memset(&_gRecvCtrl.protocol, 0, sizeof(_gRecvCtrl.protocol));
				}
				else
				{
					if (_g485ProtocolCb_Modbus != NULL)
					{
						DBG_PRINTF("\r\n ok cmd\r\n");
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
#endif
    }
}