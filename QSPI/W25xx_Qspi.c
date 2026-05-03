/***************************** (C) COPYRIGHT ************************************
* File Name          : W25xx_Qspi.c
* Author             : 模组星球嵌入式
* Version            : V1.0
* Date               : 30/11/2019
* Description        : STM32h7的QUAD SPI驱动程序源文件
* Note               : 版权所有  严禁盗版
********************************************************************************
* 实验平台:模组星球STM32H743VIT6开发板  
* 淘宝店铺:https://shop71600107.taobao.com/

********************************************************************************/
#include "W25xx_Qspi.h"
#include "delay.h"

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
	//使能QSPI端口所在GPIO时钟
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();

	//配置CS管脚
	GPIO_InitStruct.Pin       = QSPI_CS_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_NOPULL;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
	HAL_GPIO_Init(QSPI_CS_GPIO_PORT, &GPIO_InitStruct);

	//配置CLK管脚
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

	QSPIHandle.Init.ClockPrescaler     = 2; //时钟预分频,来自AHB,FAHB/(Prescaler+1)
	QSPIHandle.Init.FifoThreshold      = 4; //FIFO阈值级别
	QSPIHandle.Init.SampleShifting     = QSPI_SAMPLE_SHIFTING_HALFCYCLE; //采样移位,半个周期
	QSPIHandle.Init.FlashSize          = POSITION_VAL(QSPI_FLASH_SIZE) - 1; //FLASH大小
	QSPIHandle.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_5_CYCLE; //片选高电平时间
	QSPIHandle.Init.ClockMode          = QSPI_CLOCK_MODE_0;      //选择模式0还是模式3,这里模式0
	QSPIHandle.Init.FlashID            = QSPI_FLASH_ID_1;        //选择FLASH1还是FLASH2
	QSPIHandle.Init.DualFlash          = QSPI_DUALFLASH_DISABLE; //禁止双缓存

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
		//写状态寄存器的QE位为1
		if(QSPI_WriteQE(&QSPIHandle) != QSPI_OK)
		{
			//printf("QSPI WR QE ERR!!!\r\n");
			return QSPI_ERROR;
		}
	}
	//适当延时,等待FLASH稳定
	delay_ms(1000);
	//配置FLASH进入QSPI模式,此后所有操作均为QSPI方式
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
* Description    : 写使能,SPI模式,用于初始化时写状态寄存器
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
* Description    : 从SPI FLASH读取状态寄存器
* Input          : None
* Output         : pData: 读取的数据缓存
* Return         : 读操作结果
* Note			 : None
*******************************************************************************/
uint8_t QSPI_Read_SR(uint8_t cmd, uint8_t* pData)
{
	QSPI_CommandTypeDef s_command;

	//初始化读命令
	#if FLASH_OP_METHOD == FLASH_QPI
	s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.DataMode          = QSPI_DATA_4_LINES;
	s_command.DummyCycles       = 0;  //空闲周期数
	#else
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.DummyCycles       = 0;  //空闲周期数
	#endif
	s_command.Instruction       = cmd;   //读状态寄存器命令
	s_command.AddressSize       = QSPI_ADDRESS_8_BITS;
	s_command.Address           = 0;                   //读地址
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.NbData            = 1;                        //读取数据大小
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	//发送读命令
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	//接收读取的数据
	if (HAL_QSPI_Receive(&QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

/*******************************************************************************
* Function Name  : QSPI_WriteQE
* Description    : 写状态寄存器的QE位,准备进入QPI模式
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

	//使能写操作
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

	//写入状态寄存器
	if (HAL_QSPI_Transmit(&QSPIHandle, &cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

/*******************************************************************************
* Function Name  : QSPI_ModeEnter
* Description    : 配置进入QSPI模式,配置时采用单线,W25Q128在调用此函数前需要将寄
                   存器中的QE位置1否则忽略进入QPI命令
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
* Description    : 配置退出QSPI模式,配置时采用4线?
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
* Function Name  : QSPI_Read
* Description    : 从SPI FLASH读取数据
* Input          : ReadAddr: 读起始地址
				   Size: 读取数据长度
				   dummy:空闲周期
* Output         : pData: 读取的数据缓存
* Return         : 读操作结果
* Note			 : None
*******************************************************************************/
uint8_t QSPI_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size, uint8_t dummy)
{
	QSPI_CommandTypeDef s_command;

	//初始化读命令
	#if FLASH_OP_METHOD == FLASH_QPI
	s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
	s_command.DataMode          = QSPI_DATA_4_LINES;
	s_command.DummyCycles       = dummy;//QUAD_READ_DUMMY_CYCLES;  //空闲周期数(2)
	#else
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.DummyCycles       = SPI_READ_DUMMY_CYCLES;  //空闲周期数(8)
	#endif
	s_command.Instruction       = FAST_READ_CMD;   //快速读命令
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
	s_command.Address           = ReadAddr;                   //读地址
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.NbData            = Size;                        //读取数据大小
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	//发送读命令
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	//接收读取的数据
	if (HAL_QSPI_Receive(&QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

/*******************************************************************************
* Function Name  : QSPI_Write
* Description    : 向SPI FLASH写入数据
* Input          : pData: 待编程的数据缓存
				   WriteAddr: 编程起始地址
				   Size: 编程数据长度
* Output         : None
* Return         : 编程结果
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

	//写入的数据小于一页数据
	if (current_size > Size)
	{
		current_size = Size;
	}

	current_addr = WriteAddr;
	end_addr = WriteAddr + Size;   //结束地址

	//初始化编程命令
	#if FLASH_OP_METHOD == FLASH_QPI
	s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
	s_command.DataMode          = QSPI_DATA_4_LINES;
	#else
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	#endif
	s_command.Instruction       = PAGE_PROG_CMD;  			//快速编程命令
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	//一页一页的执行编程动作,直到编程地址到达结束地址处
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

		//写入数据
		if (HAL_QSPI_Transmit(&QSPIHandle, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
		{
			return QSPI_ERROR;
		}

		//等待编程结束  
		if (QSPI_AutoPollingMemReady(&QSPIHandle, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
		{
			return QSPI_ERROR;
		}

		//更新地址,准备下一页编程
		current_addr += current_size;
		pData += current_size;
		current_size = ((current_addr + QSPI_FLASH_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : QSPI_FLASH_PAGE_SIZE;
	} while (current_addr < end_addr);

	return QSPI_OK;
}

/*******************************************************************************
* Function Name  : QSPI_Erase_Block
* Description    : FLASH的块擦除操作
* Input          : erase_cmd: SECTOR_4K_ERASE_CMD/SECTOR_32K_ERASE_CMD/
				   SECTOR_64K_ERASE_CMD
				   BlockAddress: 擦除块地址
* Output         : None
* Return         : 0 擦除成功  1 擦除失败
* Note			 : 用于4K、32K、64K擦除
*******************************************************************************/
uint8_t QSPI_Erase_Block(uint8_t erase_cmd, uint32_t BlockAddress)
{
	QSPI_CommandTypeDef s_command;

	//初始化擦除命令
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

	//初始化擦除命令
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
* Function Name  : QSPI_ResetMemory
* Description    : 复位QSPI Memory
* Input          : hqspi   句柄
* Output         : None
* Return         : QSPI_OK/QSPI_ERROR
* Note			 : None
*******************************************************************************/
uint8_t QSPI_ResetMemory(QSPI_HandleTypeDef *hqspi)
{
	QSPI_CommandTypeDef s_command;

	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = RESET_ENABLE_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_NONE;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_NONE;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	//发送复位使能命令
	if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	//发送复位命令
	s_command.Instruction = RESET_MEMORY_CMD;
	if (HAL_QSPI_Command(hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	//等待操作完成
	if (QSPI_AutoPollingMemReady(hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != QSPI_OK)
	{
		return QSPI_ERROR;
	}

	return QSPI_OK;
}

/*******************************************************************************
* Function Name  : QSPI_AutoPollingMemReady
* Description    : 配置QSPI为自动轮寻模式
* Input          : hqspi   句柄   Timeout  超时
* Output         : None
* Return         : QSPI_OK/QSPI_ERROR
* Note			 : None
*******************************************************************************/
uint8_t QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi, uint32_t Timeout)
{
	uint8_t stareg2=0;
	
	QSPI_Read_SR(READ_STATUS_REG_CMD,&stareg2);
	
	while(stareg2==0x01);

	return QSPI_OK;
}

/*******************************************************************************
* Function Name  : QSPI_Flash_ReadID
* Description    : 读取芯片ID  
* Input          : None
* Output         : None
* Return         : 2个字节，第一个为制造商ID: 0xEF, 第二个为器件ID
                   W25Q128  0XEF17		//16M字节
* Note			 : 支持QPI
*******************************************************************************/   	  
u16 QSPI_Flash_ReadID(void)
{
	u16 Temp = 0;
	u8 UID[4] = {0,0,0,0};
	
	QSPI_CommandTypeDef s_command;

	//初始化读命令	
	#if FLASH_OP_METHOD == FLASH_QPI
	s_command.InstructionMode   = QSPI_INSTRUCTION_4_LINES;
	s_command.DataMode          = QSPI_DATA_4_LINES;
	s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
	#else
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.DataMode          = QSPI_DATA_1_LINE;
	s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
	#endif
	s_command.Instruction       = QSPI_READ_ID;   					//读ID命令
	
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DummyCycles       = 0;
	s_command.NbData            = 3;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	//发送读命令
	if (HAL_QSPI_Command(&QSPIHandle, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}

	//接收读取的数据
	if (HAL_QSPI_Receive(&QSPIHandle, UID, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
	{
		return QSPI_ERROR;
	}
	
	Temp = (UID[2] << 8) + UID[1];

	return (Temp);
}

/*******************************************************************************
* Function Name  : W25_Flash_Write_NoCheck
* Description    : 无检验写SPI FLASH,在指定地址开始写入指定长度的数据,但是要确保地址不越界!
* Input          : WriteAddr:开始写入的地址(24bit)
				   pBuffer:数据存储区
				   NumByteToWrite:要写入的字节数(最大65535)
* Output         : None
* Return         : None
* Note			 : 必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
				   具有自动换页功能 
*******************************************************************************/
void SPI_Flash_Write_NoCheck(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	 
	  
	pageremain = 256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite <= pageremain)
		pageremain = NumByteToWrite;//不大于256个字节

	while(1)
	{	   
		QSPI_Write(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
		}
	};	    
} 

u8 SPI_FLASH_BUFFER[4096];
/*******************************************************************************
* Function Name  : W25_Flash_Write
* Description    : 写SPI FLASH,在指定地址开始写入指定长度的数据
* Input          : WriteAddr:开始写入的地址(24bit)
				   pBuffer:数据存储区
				   NumByteToWrite:要写入的字节数(最大65535)
* Output         : None
* Return         : None
* Note			 : 必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
				   具有自动换页功能,该函数带擦除操作!
*******************************************************************************/ 		 
void SPI_Flash_Write(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)  	
{ 
	u32 secpos;			//扇区地址
	u16 secoff;			//扇区内偏移
	u16 secremain;		//扇区剩余空间	   
 	u16 i;    
	u8 * SPI_FLASH_BUF;	
	  
   	SPI_FLASH_BUF = SPI_FLASH_BUFFER;			//可以通过申请内存的方式实现	     
 	secpos=WriteAddr/4096;//扇区地址  
	secoff=WriteAddr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小   

 	if(NumByteToWrite <= secremain) secremain = NumByteToWrite;//不大于4096个字节
	while(1) 
	{	
		QSPI_Read(SPI_FLASH_BUF, secpos*4096, 4096,QUAD_READ_DUMMY_CYCLES);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(SPI_FLASH_BUF[secoff+i]!=0XFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			QSPI_Erase_Block(SECTOR_4K_ERASE_CMD,secpos*4096); //擦除这个扇区
			for(i=0;i<secremain;i++)	   //复制
			{
				SPI_FLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			SPI_Flash_Write_NoCheck(SPI_FLASH_BUF,secpos*4096,4096);//写入整个扇区  
		}
		else 
		{
			SPI_Flash_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 
		}				   
		if(NumByteToWrite == secremain)
		{
			break;//写入结束了
		}
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	pBuffer+=secremain;  //指针偏移
			WriteAddr+=secremain;//写地址偏移	   
		   	NumByteToWrite-=secremain;				//字节数递减
			if(NumByteToWrite>4096)secremain=4096;	//下一个扇区还是写不完
			else secremain=NumByteToWrite;			//下一个扇区可以写完了
		}	 
	}	 
}
