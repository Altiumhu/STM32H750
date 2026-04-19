#include "head.h"

//SMEC88 IIC驱动程序，IO口模拟IIC协议


#define IIC_NOACK         TRUE
#define IIC_ACK           FALSE
#define HIGHT             TRUE
#define LOW               FALSE

#define IIC_DELAYTIME     4				 	//bit位延时
#define ACKCHECKTIME      2000			//等待ACK超时时间
#define IIC_START_TIMES   500				//发送（START信号+地址字节） 的次数


//===========================      与SMEC88通讯的管脚定义      ===========================//
#define IIC_SCL_PIN				GPIO_PIN_8			//不同IO修改这里
#define IIC_SDA_PIN				GPIO_PIN_9			//不同IO修改这里
#define IIC_PORT					GPIOB				//不同IO修改这里


//===========================      I2C通讯相关      ===========================//
//---------------------------------------------------------
//函数介绍：模拟端口基本配置初始化
//参数说明：
//			void
//返回值说明：
//			void
//---------------------------------------------------------
void SMEC_I2cInit(void)
{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN; 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//GPIO_Speed_2MHz; 
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
//	GPIO_Init(IIC_PORT, &GPIO_InitStructure); 
//	
	
	  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /*Configure GPIO pins : LED_G_Pin UN_OCP_Pin */
  GPIO_InitStruct.Pin = IIC_SDA_PIN ;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(IIC_PORT, &GPIO_InitStruct);

//	GPIO_InitStructure.GPIO_Pin = IIC_SCL_PIN; 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//GPIO_Speed_2MHz; 
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//GPIO_Mode_Out_OD;//GPIO_Mode_Out_PP; 
//	GPIO_Init(IIC_PORT, &GPIO_InitStructure); 
		
//	  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /*Configure GPIO pins : LED_G_Pin UN_OCP_Pin */
  GPIO_InitStruct.Pin = IIC_SCL_PIN ;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(IIC_PORT, &GPIO_InitStruct);
		
}

//数据信号设为输入模式
void SdaInputMode(void)
{
	  GPIO_InitTypeDef GPIO_InitStruct = {0};

//  GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN; 
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//GPIO_Speed_2MHz; 
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
//  GPIO_Init(IIC_PORT, &GPIO_InitStructure); 
	
  GPIO_InitStruct.Pin = IIC_SDA_PIN ;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(IIC_PORT, &GPIO_InitStruct);
}

//数据信号设为输出模式
void SdaOutputMode(void)
{
//  GPIO_InitTypeDef  GPIO_InitStructure;

//  GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN; 
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//GPIO_Speed_2MHz; 
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
//  GPIO_Init(IIC_PORT, &GPIO_InitStructure); 
	
	  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /*Configure GPIO pins : LED_G_Pin UN_OCP_Pin */
  GPIO_InitStruct.Pin = IIC_SDA_PIN ;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(IIC_PORT, &GPIO_InitStruct);

}

void IIC_Delay(unsigned int times)
{
	uint16_t loop;
	for(loop=0;loop<times;loop++)
	{
		;
	}
}

//时钟信号输出高电平
void IIC_SetSclPin(void)
{
	//GPIO_WriteBit(IIC_PORT,IIC_SCL_PIN,Bit_SET);
	  HAL_GPIO_WritePin(IIC_PORT, IIC_SCL_PIN, GPIO_PIN_SET); // 
}

//时钟信号输出低电平
void IIC_ClrSclPin(void)
{
	//GPIO_WriteBit(IIC_PORT,IIC_SCL_PIN,Bit_RESET);
	 HAL_GPIO_WritePin(IIC_PORT, IIC_SCL_PIN, GPIO_PIN_RESET); // 
}

//数据信号输出高电平
void IIC_SetSdaPin(void)
{
	//GPIO_WriteBit(IIC_PORT,IIC_SDA_PIN,Bit_SET);
		  HAL_GPIO_WritePin(IIC_PORT, IIC_SDA_PIN, GPIO_PIN_SET); // 
}

//数据信号输出低电平
void IIC_ClrSdaPin(void)
{
	//GPIO_WriteBit(IIC_PORT,IIC_SDA_PIN,Bit_RESET);
	 HAL_GPIO_WritePin(IIC_PORT, IIC_SDA_PIN, GPIO_PIN_RESET); // 
}

//获取数据信号上的输入
bool IIC_GetSdaStus(void)
{
	

	return (bool)(HAL_GPIO_ReadPin(IIC_PORT,IIC_SDA_PIN));
}

bool IIC_CheckACK(void)
{
	return IIC_GetSdaStus();
}

//时钟信号输出HLow
void IIC_SetScl(bool HLow)
{
	HLow?IIC_SetSclPin():IIC_ClrSclPin();
}

//数据信号输出HLow
void IIC_SetSda(bool HLow)
{
	HLow?IIC_SetSdaPin():IIC_ClrSdaPin();
}

//---------------------------------------------------------
//函数介绍：i2c起始信号产生
//参数说明：
//			void
//返回值说明：
//			void
//---------------------------------------------------------
void IIC_Start(void)
{
	SdaOutputMode();

	IIC_SetSda(HIGHT);
	IIC_SetScl(HIGHT);
	IIC_Delay(IIC_DELAYTIME);
	IIC_SetSda(LOW);
	IIC_Delay(IIC_DELAYTIME);
	IIC_SetScl(LOW);
}

//--------------------------------------------------------- 
//函数介绍：i2c停止信号产生
//参数说明：
//			void
//返回值说明：
//			void
//---------------------------------------------------------
void IIC_Stop(void)
{
	SdaOutputMode();
	
	IIC_SetSda(LOW);
	IIC_SetScl(HIGHT);

	IIC_Delay(IIC_DELAYTIME);
	IIC_SetSda(HIGHT);
	IIC_Delay(IIC_DELAYTIME);
}

//---------------------------------------------------------
//函数名: i2c发送一个字节
//参数说明：
//			ucVal : 字节数据
//返回值说明：
//			0 - 成功
//          1 - 失败
//---------------------------------------------------------
bool IIC_Send(unsigned char ucVal)
{
	bool Ack;
	uint16_t AckLoop;
	uint16_t i;

	SdaOutputMode();

	for(i=0;i<8;i++)
	{
		IIC_SetScl(LOW);
		IIC_Delay(IIC_DELAYTIME);
		IIC_SetSda((ucVal & 0x80)?TRUE:FALSE);
		IIC_Delay(IIC_DELAYTIME);
		IIC_SetScl(HIGHT);
		IIC_Delay(IIC_DELAYTIME);
		IIC_Delay(IIC_DELAYTIME);
		ucVal<<= 1;
	}
	IIC_SetScl(LOW);
	SdaInputMode();

	IIC_Delay(IIC_DELAYTIME);
	IIC_Delay(IIC_DELAYTIME);
	IIC_SetScl(HIGHT);
	IIC_Delay(IIC_DELAYTIME);
	IIC_Delay(IIC_DELAYTIME);
	Ack = IIC_NOACK;

	for(AckLoop=0;AckLoop<ACKCHECKTIME;AckLoop++) //260us
	{
		if(!IIC_GetSdaStus())
		{
			Ack = IIC_ACK;
			break;
		}
		IIC_Delay(IIC_DELAYTIME);
	}
	IIC_SetScl(LOW);
	return Ack;			//return success=0 / failure=1
}

//---------------------------------------------------------
//函数名: i2c读取一个字节
//参数说明：
//			bACK - 读取一个字节完成后, 设置SDA状态值
//返回值说明：
//			接收到的数据
//---------------------------------------------------------
unsigned char IIC_Read(bool bACK)
{
	unsigned char Data;
	unsigned char i;

	SdaInputMode();

	Data = 0;
	for(i=0;i<8;i++)
	{
		Data <<= 1;
		IIC_SetScl(LOW);
		IIC_Delay(IIC_DELAYTIME);
		IIC_Delay(IIC_DELAYTIME);
		IIC_Delay(IIC_DELAYTIME);
		IIC_SetScl(HIGHT);
		IIC_Delay(IIC_DELAYTIME);
		IIC_Delay(IIC_DELAYTIME);
		if(IIC_GetSdaStus())
			Data |= 1;
		else
			Data &= 0xfe;
	}

	IIC_SetScl(LOW);
	SdaOutputMode();

	IIC_Delay(IIC_DELAYTIME);
    IIC_SetSda(bACK);
	IIC_Delay(IIC_DELAYTIME);
	IIC_SetScl(HIGHT);
	IIC_Delay(IIC_DELAYTIME);
	IIC_Delay(IIC_DELAYTIME);
	IIC_SetScl(LOW);
	return Data;
}

//---------------------------------------------------------
//函数名: 读出1串数据
//参数说明：
//			addr - I2C地址
//			buf	 - 要读取数据缓冲区指针
//			len	 - 待读取长度
//返回值说明：
//			接收到的数据
//---------------------------------------------------------
bool IIC_ReadWithAddr(unsigned char addr, unsigned char *buf, unsigned char len)
{
	unsigned int i;
	i = IIC_START_TIMES;
	
	while (i) 
	{
		IIC_Start();
		if (IIC_Send(addr) == IIC_ACK) break;
		if (--i == 0) return IIC_NOACK;
	}
	
	for(i = 0; i < len -1 ; i++)
	{
		buf[i] = IIC_Read(IIC_ACK);
	}

	buf[i] = IIC_Read(IIC_NOACK);

	IIC_Stop();

	return IIC_ACK;
}

//---------------------------------------------------------
//函数名: 写入1串数据
//参数说明：
//			addr - I2C地址
//			buf	 - 要写入数据缓冲区指针
//			len	 - 待写入长度
//返回值说明：
//			接收到的数据
//---------------------------------------------------------
bool IIC_WriteWithAddr(unsigned char addr, unsigned char *buf, unsigned char len)
{
	unsigned int i;
	i = IIC_START_TIMES;

	while (i) 
	{
		IIC_Start();
		if (IIC_Send(addr) == IIC_ACK) break;
		if (--i == 0) return IIC_NOACK;
	}

	for(i = 0; i < len; i++)
	{
		if(IIC_Send(buf[i]))
			return IIC_NOACK;
	}

	IIC_Delay(IIC_DELAYTIME);
	IIC_Delay(IIC_DELAYTIME);

	IIC_Stop();

	return IIC_ACK;

}
