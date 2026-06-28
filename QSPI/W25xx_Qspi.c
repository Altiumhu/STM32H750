/*****************************
* File Name          : W25xx_Qspi.c
* Author             : 模友科技嵌入式
* Version            : V1.0
* Date               : 30/11/2019
* Description        : STM32h7的QUAD SPI FLASH驱动源文件
* Note               : 版权所有  上铁科技
********************************************************************************
* 实验平台:模友科技STM32H743VIT6开发板   
* 店铺地址:https://shop71600107.taobao.com

********************************************************************************/

#include "head.h"
#include "W25xx_Qspi.h"
#include "delay.h"
#include "MyIncludes.h"
QSPI_HandleTypeDef QSPIHandle;

//端口初始化
void QSPI_MspInit(void)
{
	GPIO_InitTypeDef   GPIO_InitStruct;

	//使能QSPI时钟
	QSPI_CLK_ENABLE();
	//复位QSPI接口
	QSPI_FORCE_RESET();
	QSPI_RELEASE_RESET();
	//使能QSPI端口相关GPIO时钟
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();

	//配置CS引脚
	GPIO_InitStruct.Pin       = QSPI_CS_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_NOPULL;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
	HAL_GPIO_Init(QSPI_CS_GPIO_PORT, &GPIO_InitStruct);

	//配置CLK引脚
	GPIO_InitStruct.Pin       = QSPI_CLK_PIN;
	GPIO_InitStruct.Pull      = GPIO_NOPULL;
	GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
	HAL_GPIO_Init(QSPI_CLK_GPIO_PORT, &GPIO_InitStruct);

	//D0
	GPIO_InitStruct.Pin       = QSPI_D0_PIN;
	GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
	HAL_GPIO_Init(QSPI_D0_GPIO_PORT, &GPIO_InitStruct);

	//D1
	GPIO_InitStruct.Pin       = QSPI_D1_PIN;
	GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
	HAL_GPIO_Init(QSPI_D1_GPIO_PORT, &GPIO_InitStruct);

	//D2
	GPIO_InitStruct.Pin       = QSPI_D2_PIN;
	GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
	HAL_GPIO_Init(QSPI_D2_GPIO_PORT, &GPIO_InitStruct);

	//D3
	GPIO_InitStruct.Pin       = QSPI_D3_PIN;
	GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
	HAL_GPIO_Init(QSPI_D3_GPIO_PORT, &GPIO_InitStruct);

	//配置NVIC中断
	HAL_NVIC_SetPriority(QUADSPI_IRQn, 0x0F, 0);
	HAL_NVIC_EnableIRQ(QUADSPI_IRQn);
}

/*******************************************************************************
* Function Name  : QSPI_Init
* Description    : QSPI初始化
* Input          : None
* Output         : None
* Return         : None
* Note			 : None
*******************************************************************************/
uint8_t QSPI_Init(void)
{ 
	QSPIHandle.Instance = QUADSPI;

	if(HAL_QSPI_DeInit(&QSPIHandle) != HAL_OK)
	{
		return QSPI_ERROR;
	}
	
	QSPI_MspInit();   //初始化端口

	QSPIHandle.Init.ClockPrescaler     = 10; //时钟预分频,480/(20+1)=22.9MHz
	QSPIHandle.Init.FifoThreshold      = 32; //FIFO阈值设置(改大避免数据丢失)
	QSPIHandle.Init.SampleShifting     = QSPI_SAMPLE_SHIFTING_HALFCYCLE; //采样移位,半周期
	QSPIHandle.Init.FlashSize          = POSITION_VAL(QSPI_FLASH_SIZE) - 1; //FLASH大小
	QSPIHandle.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_5_CYCLE; //片选高电平时间
	QSPIHandle.Init.ClockMode          = QSPI_CLOCK_MODE_0;      //选择模式0(模式0~3,选择模式0)
	QSPIHandle.Init.FlashID            = QSPI_FLASH_ID_1;        //选择FLASH1或FLASH2
	QSPIHandle.Init.DualFlash          = QSPI_DUALFLASH_DISABLE; //禁止双闪存

	if(HAL_QSPI_Init(&QSPIHandle) != HAL_OK)
	{
		//printf("QSPI Init ERR!!!\r\n");
		return QSPI_ERROR;
	}
	
	#if FLASH_OP_METHOD == FLASH_QPI
	uint8_t stareg2=0;
	
	//退出QPI模式
	if (QSPI_ModeExit(&QSPIHandle) != QSPI_OK)
	{
	//	return QSPI_ERROR;
	}
	
	//读状态寄存器(status2),判断QE位
	QSPI_Read_SR(READ_STATUS_REG2_CMD,&stareg2);
	if((stareg2&0x02) == 0)
	{
		//写状态寄存器,使QE位为1
		if(QSPI_WriteQE(&QSPIHandle) != QSPI_OK)
		{
			//printf("QSPI WR QE ERR!!!\r\n");
			return QSPI_ERROR;
		}
	}
	//实际延时,等待FLASH稳定
	delay_ms(1000);
	//使能FLASH进入QSPI模式,该函数同时也是QSPI模式
	if(QSPI_ModeEnter(&QSPIHandle) != QSPI_OK)   
	{
		//printf("QSPI Enter ERR!!!\r\n");
		return QSPI_NOT_SUPPORTED;
	}
	
	#else
	//退出QSPI模式
	if (QSPI_ModeExit(&QSPIHandle) != QSPI_OK)
	{
		return QSPI_ERROR;
	}
	#endif
  
	return QSPI_OK;
}

/*******************************************************************************
* Function Name  : QSPI_WriteEnableSPI
* Description    : 写使能,SPI模式,用于退出时写状态寄存器
* Input          : hqspi   句柄
* Output         : None
* Return         : QSPI_OK/QSPI_ERROR
* Note			 : None
*******************************************************************************/
uint8_t QSPI_WriteEnableSPI(QSPI_HandleTypeDef *hqspi)
{
	QSPI_CommandTypeDef     s_command;

	//使能写操作
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = WRITE_ENABLE_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_NONE;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

/*******************************************************************************
* Function Name  : QSPI_WriteEnable
* Description    : 写使能,QPI
* Input          : hqspi   句柄
* Output         : None
* Return         : QSPI_OK/QSPI_ERROR
* Note			 : None
*******************************************************************************/
uint8_t QSPI_WriteEnable(QSPI_HandleTypeDef *hqspi)
{
	QSPI_CommandTypeDef     s_command;

	//使能写操作
	#if FLASH_OP_METHOD == FLASH_QPI
	s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	#else
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	#endif
	s_command.Instruction       = WRITE_ENABLE_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_NONE;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

/*******************************************************************************
* Function Name  : QSPI_Read_SR
* Description    : 读SPI FLASH读取状态寄存器
* Input          : 
* Output         : pData: 读取数据缓冲区
* Return         : 读取结果
* Note			 : None
*******************************************************************************/
uint8_t QSPI_Read_SR(uint8_t cmd, uint8_t* pData)
{
	QSPI_CommandTypeDef s_command;

	//初始化命令参数
	#if FLASH_OP_METHOD == FLASH_QPI
	s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.DataMode          = QSPI_DATA_4_LINES;
	s_command.DummyCycles       = 0;  //不需要空周期
	#else
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.DummyCycles       = 0;  //不需要空周期
	#endif
	s_command.Instruction       = cmd;   //读状态寄存器命令
	s_command.AddressSize       = QSPI_ADDRESS_8_BITS;
	s_command.Address           = 0;                   //无地址
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.NbData            = 1;                        //读取数据大小
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	//发送命令
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	//接收读取数据
	if (HAL_QSPI_Receive(&QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

/*******************************************************************************
* Function Name  : QSPI_WriteQE
* Description    : 写状态寄存器使能QE位,准备进入QPI模式
* Input          : hqspi   句柄
* Output         : None
* Return         : QSPI_OK/QSPI_ERROR
* Note			 : None
*******************************************************************************/
uint8_t QSPI_WriteQE(QSPI_HandleTypeDef *hqspi)
{
	QSPI_CommandTypeDef     s_command;
	uint8_t cmd = 0x02;
	
	//使能写
	QSPI_WriteEnableSPI(hqspi);
	//QSPI_WriteEnable(hqspi);

	//使能写状态寄存器
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = WRITE_STATUS_REG2_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.NbData  			= 1;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	//发送写状态寄存器命令
	if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	//写状态寄存器
	if (HAL_QSPI_Transmit(&QSPIHandle, &cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

/*******************************************************************************
* Function Name  : QSPI_ModeEnter
* Description    : 设置进入QSPI模式,进入时执行一次即可,W25Q128在此命令前需要先设置
                   使能QE位,否则无法进入QPI操作
* Input          : hqspi   句柄
* Output         : None
* Return         : QSPI_OK/QSPI_ERROR
* Note			 : None
*******************************************************************************/
uint8_t QSPI_ModeEnter(QSPI_HandleTypeDef *hqspi)
{
	QSPI_CommandTypeDef s_command;

	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = QSPI_MODE_ENTER_CMD;     //进入QSPI模式
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_NONE;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	/* Configure the command */
	if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

/*******************************************************************************
* Function Name  : QSPI_ModeExit
* Description    : 设置退出QSPI模式,进入时执行4线?
* Input          : hqspi   句柄
* Output         : None
* Return         : QSPI_OK/QSPI_ERROR
* Note			 : None
*******************************************************************************/
uint8_t QSPI_ModeExit(QSPI_HandleTypeDef *hqspi)
{
	QSPI_CommandTypeDef s_command;

	s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	s_command.Instruction       = QSPI_MODE_EXIT_CMD;     //退出QSPI模式
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_NONE;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	/* Configure the command */
	if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

/*******************************************************************************
* Function Name  : QSPI_AutoPollingMemReady
* Description    : 轮询等待Flash空闲
* Input          : hqspi   句柄
				   Timeout: 超时时间
* Output         : None
* Return         : QSPI_OK/QSPI_ERROR
* Note			 : None
*******************************************************************************/
uint8_t QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi, uint32_t Timeout)
{
	QSPI_CommandTypeDef s_command;
	uint8_t status;

	#if FLASH_OP_METHOD == FLASH_QPI
	s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.DataMode          = QSPI_DATA_4_LINES;
	s_command.DummyCycles       = 0;
	#else
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.DummyCycles       = 0;
	#endif
	s_command.Instruction       = READ_STATUS_REG_CMD;
	s_command.AddressSize       = QSPI_ADDRESS_8_BITS;
	s_command.Address           = 0;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.NbData            = 1;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	/* Configure the command */
	if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	/* Start polling: wait for memory ready */
	do
	{
		/* Configure the command */
		if (HAL_QSPI_Receive(hqspi, (uint8_t*)&status, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		{
			return QSPI_ERROR;
		}

		/* Check the status: WIP=0 means idle */
		if((status & 0x01) == 0)
			break;
		/* Check for the Timeout */
		if(Timeout-- == 0)
		return QSPI_ERROR;
	} while (1);

	return QSPI_OK;
}

/*******************************************************************************
* Function Name  : QSPI_Read
* Description    : 读SPI FLASH读取数据
* Input          : ReadAddr: 读取起始地址
				   Size: 读取数据长度
				   dummy:空周期数
* Output         : pData: 读取数据缓冲区
* Return         : 读取结果
* Note			 : None
*******************************************************************************/
uint8_t QSPI_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size, uint8_t dummy)
{
	QSPI_CommandTypeDef s_command;

	//初始化命令参数
	#if FLASH_OP_METHOD == FLASH_QPI
	s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
	s_command.DataMode          = QSPI_DATA_4_LINES;
	s_command.DummyCycles       = dummy;//QUAD_READ_DUMMY_CYCLES;  //空周期数(2)
	#else
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.DummyCycles       = SPI_READ_DUMMY_CYCLES;  //空周期数(8)
	#endif
	s_command.Instruction       = FAST_READ_CMD;   //快速读取命令
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
	s_command.Address           = ReadAddr;                   //读取地址
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.NbData            = Size;                        //读取数据大小
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	//发送命令
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	//接收读取数据
	if (HAL_QSPI_Receive(&QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

/*******************************************************************************
* Function Name  : QSPI_Write
* Description    : 写SPI FLASH写入数据
* Input          : pData: 待写入数据缓冲区
				   WriteAddr: 写入起始地址
				   Size: 写入数据长度
* Output         : None
* Return         : 写入结果
* Note			 : None
*******************************************************************************/
uint8_t QSPI_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size)
{
	QSPI_CommandTypeDef s_command;
	uint32_t end_addr, current_size, current_addr;

	current_addr = 0;

	while (current_addr <= WriteAddr)
	{
		current_addr += QSPI_FLASH_PAGE_SIZE;
	}
	current_size = current_addr - WriteAddr;

	//写入数据大小不能超过第一页剩余
	if (current_size > Size)
	{
		current_size = Size;
	}

	current_addr = WriteAddr;
	end_addr = WriteAddr + Size;   //结束地址

	//初始化命令参数
	#if FLASH_OP_METHOD == FLASH_QPI
	s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
	s_command.DataMode          = QSPI_DATA_4_LINES;
	#else
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	#endif
	s_command.Instruction       = PAGE_PROG_CMD;  			//页编程命令
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	//逐页逐行执行编程,直到数据地址达到结束地址
	do
	{
		s_command.Address = current_addr;
		s_command.NbData  = current_size;

		//使能写
		if (QSPI_WriteEnable(&QSPIHandle) != QSPI_OK)
		{
			return QSPI_ERROR;
		}

		//发送编程命令
		if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		{
			return QSPI_ERROR;
		}

		//写数据
		if (HAL_QSPI_Transmit(&QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		{
			return QSPI_ERROR;
		}

		//分页写入延时,避免第二页数据丢失 FLASH_LATENCY
		delay_us(10);

		//等待芯片空闲  
		if (QSPI_AutoPollingMemReady(&QSPIHandle, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
		{
			return QSPI_ERROR;
		}

		//更新地址,准备下一页
		current_addr += current_size;
		pData += current_size;
		current_size = ((current_addr + QSPI_FLASH_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : QSPI_FLASH_PAGE_SIZE;
	} while (current_addr < end_addr);

	return QSPI_OK;
}

/*******************************************************************************
* Function Name  : QSPI_Erase_Block
* Description    : FLASH扇区擦除
* Input          : erase_cmd: SECTOR_4K_ERASE_CMD/SECTOR_32K_ERASE_CMD/
				   SECTOR_64K_ERASE_CMD
				   BlockAddress: 扇区首地址
* Output         : None
* Return         : 0 擦除成功  1 擦除失败
* Note			 : 支持4K、32K、64K擦除
*******************************************************************************/
uint8_t QSPI_Erase_Block(uint8_t erase_cmd, uint32_t BlockAddress)
{
	QSPI_CommandTypeDef s_command;

	//初始化命令参数
	#if FLASH_OP_METHOD == FLASH_QPI
	s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
	#else
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
	#endif
	s_command.Instruction       = erase_cmd;           //擦除命令
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
	s_command.Address           = BlockAddress;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_NONE;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	//使能写操作
	if(QSPI_WriteEnable(&QSPIHandle) != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	//发送擦除命令
	if(HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	//等待擦除完成  
	if(QSPI_AutoPollingMemReady(&QSPIHandle, FLASH_SECTOR_ERASE_MAX_TIME) != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

/*******************************************************************************
* Function Name  : QSPI_Erase_Chip
* Description    : 片擦除
* Input          : None
* Output         : None
* Return         : 0 擦除成功  1 擦除失败
* Note			 : None
*******************************************************************************/
uint8_t QSPI_Erase_Chip(void)
{
	QSPI_CommandTypeDef s_command;

	//初始化命令参数
	#if FLASH_OP_METHOD == FLASH_QPI
	s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	#else
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	#endif
	s_command.Instruction       = BULK_ERASE_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_NONE;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	//使能写操作
	if (QSPI_WriteEnable(&QSPIHandle) != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	//发送擦除命令
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	//等待擦除完成  
	if (QSPI_AutoPollingMemReady(&QSPIHandle, FLASH_BULK_ERASE_MAX_TIME) != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

/*******************************************************************************
* Function Name  : QSPI_Flash_ReadID
* Description    : 读取Flash ID
* Input          : None
* Output         : None
* Return         : Flash ID
* Note			 : None
*******************************************************************************/
uint16_t QSPI_Flash_ReadID(void)
{
	QSPI_CommandTypeDef s_command;
	uint8_t recv_data[2];

	//初始化命令参数 - 使用JEDEC ID命令(0x9F)，QPI模式(4线)
	s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	s_command.Instruction       = QSPI_READ_ID;
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_4_LINES;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;
	s_command.AddressSize = QSPI_ADDRESS_24_BITS;
	s_command.Address = 0;
	s_command.NbData = 2;

	//发送命令 5000U HAL_QPSI_TIMEOUT_DEFAULT_VALUE
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, 50U) != HAL_OK)
	{
		return 0;
	}

	//接收数据
	if (HAL_QSPI_Receive(&QSPIHandle, recv_data, 50U) != HAL_OK)
	{
		return 0;
	}
	// recv_data[0]=厂商ID(0xEF), recv_data[1]=器件ID, recv_data[2]=容量ID
	return (recv_data[0] << 8) | recv_data[1];
}

u8 writeBuffer1[1024];
u8 readBuffer1[1024];
u16 flash_id,j;

void TEST_W25Q128(void)
{

	for(j=0; j<sizeof(writeBuffer1); j++)
	{
		writeBuffer1[j]=66;
	}
	
    uint8_t qspi_status = QSPI_Init();  // 获取返回值
    printf("QSPI_Init status: %d\r\n", qspi_status);
    
    if (qspi_status != QSPI_OK) {
        printf("QSPI Init FAILED!\r\n");
        // 可以添加错误处理
    }
    
    // 回读ID
    flash_id = QSPI_Flash_ReadID();
    printf("flash_id: 0x%x\r\n", flash_id);
	
		
		
	QSPI_Erase_Block(SECTOR_4K_ERASE_CMD,0);       //4K扇区擦除
		HAL_Delay(1000);
	QSPI_Write(writeBuffer1,0,sizeof(writeBuffer1));
	HAL_Delay(1000);
	QSPI_Read(readBuffer1,0,sizeof(readBuffer1),QUAD_READ_DUMMY_CYCLES);
	if(strncmp((char *)writeBuffer1, (char *)readBuffer1, sizeof(writeBuffer1)) == 0)    //页编程正确
	{
		printf("QSPI First ReadWrite OK!!!   ==%d\r\n",readBuffer1[1]);
	}
	
//		for(j=0; j<sizeof(writeBuffer1); j++)
//	{
//		printf("readBuffer1[%d] =%d\r\n",j,readBuffer1[j]);
//	}
//	


}
