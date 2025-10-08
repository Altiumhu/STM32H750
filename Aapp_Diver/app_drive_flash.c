

#include "head.h"
#include "app_drive_flash.h"

/* 要写入到STM32 FLASH的字符串数组 */
const uint8_t g_text_buf[] = {"1256 STM32H7 FLASH TEST"};

#define TEXT_LENTH sizeof(g_text_buf) /* 数组长度 */

/* SIZE表示字长(4字节), 大小必须是4的整数倍, 如果不是的话, 强制对齐到4的整数倍 */
#define SIZE TEXT_LENTH / 4 + ((TEXT_LENTH % 4) ? 1 : 0)

#define FLASH_SAVE_ADDR ADDR_FLASH_SECTOR_7 /* 设置FLASH 保存地址(必须为4的整数倍，且所在扇区要大于本代码所占用到的扇区,STM32H7一个扇区是128K). \
									  否则,写操作的时候,可能会导致擦除整个扇区,从而引起部分程序丢失.引起死机. */

/**
 * @brief       从指定地址读取一个字 (32位数据)
 * @param       faddr   : 读取地址 (此地址必须为4的倍数!!)
 * @retval      读取到的数据 (32位)
 */
uint32_t stmflash_read_word(uint32_t faddr)
{
	return *(volatile uint32_t *)faddr;
}

/**
 * @brief       从指定地址开始读出指定长度的数据
 * @param       raddr : 起始地址
 * @param       pbuf  : 数据指针
 * @param       length: 要读取的字(32位)数,即4个字节的整数倍
 * @retval      无
 */
void stmflash_read(uint32_t raddr, uint32_t *pbuf, uint32_t length)
{
	uint32_t i;

	for (i = 0; i < length; i++)
	{
		pbuf[i] = stmflash_read_word(raddr); /* 读取4个字节(1个字) */
		raddr += 4;							 /* 偏移4个字节 */
	}
}

/**
 * @brief       获取某个地址所在的flash扇区
 * @param       faddr   : flash地址
 * @retval      0~7 ,addr所在的扇区编号
 */
uint8_t stmflash_get_flash_sector(uint32_t faddr)
{
	if (faddr < ADDR_FLASH_SECTOR_1)
		return 0;
	else if (faddr < ADDR_FLASH_SECTOR_2)
		return 1;
	else if (faddr < ADDR_FLASH_SECTOR_3)
		return 2;
	else if (faddr < ADDR_FLASH_SECTOR_4)
		return 3;
	else if (faddr < ADDR_FLASH_SECTOR_5)
		return 4;
	else if (faddr < ADDR_FLASH_SECTOR_6)
		return 5;
	else if (faddr < ADDR_FLASH_SECTOR_7)
		return 6;

	return 7;
}

/**
 * @brief       在FLASH 指定位置, 写入指定长度的数据(自动擦除)
 * @note        因为STM32H7的扇区实在太大,没办法本地保存扇区数据,所以本函数写地址如果非0XFF
 *              ,那么会先擦除整个扇区且不保存扇区数据.所以写非0XFF的地址,将导致整个扇区数据丢失.
 *              建议写之前确保扇区里没有重要数据,最好是整个扇区先擦除了,然后慢慢往后写.
 * @param       waddr   : 起始地址 (此地址必须为32的倍数!!,否则写入出错!)
 * @param       pbuf    : 数据指针
 * @param       length  : 要写入的 字(32位)数(就是要写入的32位数据的个数,一次至少写入32字节,即8个字)
 * @retval      无
 */
void stmflash_write(uint32_t waddr, uint32_t *pbuf, uint32_t length)
{
	FLASH_EraseInitTypeDef FlashEraseInit;
	HAL_StatusTypeDef FlashStatus = HAL_OK;
	uint32_t SectorError = 0;
	uint32_t addrx = 0;
	uint32_t endaddr = 0;

	if (waddr < STM32_FLASH_BASE || waddr % 32 ||	   /* 写入地址小于 STM32_FLASH_BASE, 或不是32的整数倍, 非法. */
		waddr > (STM32_FLASH_BASE + STM32_FLASH_SIZE)) /* 写入地址大于 STM32_FLASH_BASE + STM32_FLASH_SIZE, 非法. */
	{
		printf("Error: Invalid address\r\n");
		return;
	}
	// 2. 禁用数据缓存（操作期间）
	SCB_DisableDCache();

	HAL_FLASH_Unlock();			  /* FLASH解锁 */
	addrx = waddr;				  /* 写入的起始地址 */
	endaddr = waddr + length * 4; /* 写入的结束地址 */

	if (addrx < 0X1FF00000) /* 只有主存储区,才需要执行擦除操作!! */
	{
		while (addrx < endaddr) /* 扫清一切障碍.(对非FFFFFFFF的地方,先擦除) */
		{
			if (stmflash_read_word(addrx) != 0XFFFFFFFF) /* 有非0XFFFFFFFF的地方,要擦除这个扇区 */
			{
				FlashEraseInit.Banks = FLASH_BANK_1;
				FlashEraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;		  /* 擦除类型，扇区擦除 */
				FlashEraseInit.Sector = stmflash_get_flash_sector(addrx); /* 要擦除的扇区 */
				FlashEraseInit.NbSectors = 1;							  /* 一次只擦除一个扇区 */
				FlashEraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;	  /* 电压范围，设置Flash写/擦除操作使用32位并行位数 */

				if (HAL_FLASHEx_Erase(&FlashEraseInit, &SectorError) != HAL_OK)
				{
					printf(" 7 Flash erase error\r\n");
					break; /* 发生错误了 */
				}

				SCB_CleanInvalidateDCache(); /* 清除无效的D-Cache */
			}
			else
			{
				addrx += 4;
			}

			FLASH_WaitForLastOperation(FLASH_WAITETIME, FLASH_BANK_1); /* 等待上次操作完成 */
		}
	}

	FlashStatus = FLASH_WaitForLastOperation(FLASH_WAITETIME, FLASH_BANK_1); /* 等待上次操作完成 */

	if (FlashStatus == HAL_OK)
	{
		while (waddr < endaddr) /* 写入地址小于结束地址，还未写完 */
		{
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, waddr, (uint64_t)pbuf) != HAL_OK) /* 写入数据 */
			{
				printf(" 3  Flash program error\r\n");
				break; /* 写入异常 */
			}

			waddr += 32;
			pbuf += 8;
		}
	}
	SCB_EnableDCache();
	HAL_FLASH_Lock(); /* FLASH上锁 */
}
void stm32H7_flash_write(uint32_t waddr, uint32_t *pbuf, uint32_t length)
{
	FLASH_EraseInitTypeDef FlashEraseInit;
	HAL_StatusTypeDef FlashStatus = HAL_OK;
	uint32_t SectorError = 0;
	uint32_t addrx = 0;
	uint32_t endaddr = 0;

	if (waddr < STM32_FLASH_BASE || waddr % 32 ||	   /* 写入地址小于 STM32_FLASH_BASE, 或不是32的整数倍, 非法. */
		waddr > (STM32_FLASH_BASE + STM32_FLASH_SIZE)) /* 写入地址大于 STM32_FLASH_BASE + STM32_FLASH_SIZE, 非法. */
	{
		printf("Error: Invalid address\r\n");
		return;
	}
	// 2. 禁用数据缓存（操作期间）
	SCB_DisableDCache();

	HAL_FLASH_Unlock();			  /* FLASH解锁 */
	addrx = waddr;				  /* 写入的起始地址 */
	endaddr = waddr + length * 4; /* 写入的结束地址 */

	// if (addrx < 0X1FF00000) /* 只有主存储区,才需要执行擦除操作!! */
	// {
	// 	while (addrx < endaddr) /* 扫清一切障碍.(对非FFFFFFFF的地方,先擦除) */
	// 	{
	// 		if (stmflash_read_word(addrx) != 0XFFFFFFFF) /* 有非0XFFFFFFFF的地方,要擦除这个扇区 */
	// 		{
	// 			FlashEraseInit.Banks = FLASH_BANK_1;
	// 			FlashEraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;		  /* 擦除类型，扇区擦除 */
	// 			FlashEraseInit.Sector = stmflash_get_flash_sector(addrx); /* 要擦除的扇区 */
	// 			FlashEraseInit.NbSectors = 1;							  /* 一次只擦除一个扇区 */
	// 			FlashEraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;	  /* 电压范围，设置Flash写/擦除操作使用32位并行位数 */

	// 			if (HAL_FLASHEx_Erase(&FlashEraseInit, &SectorError) != HAL_OK)
	// 			{
	// 				printf("Flash erase error\r\n");
	// 				break; /* 发生错误了 */
	// 			}

	// 			SCB_CleanInvalidateDCache(); /* 清除无效的D-Cache */
	// 		}
	// 		else
	// 		{
	// 			addrx += 4;
	// 		}

	// 		FLASH_WaitForLastOperation(FLASH_WAITETIME, FLASH_BANK_1); /* 等待上次操作完成 */
	// 	}
	// }

	FlashStatus = FLASH_WaitForLastOperation(FLASH_WAITETIME, FLASH_BANK_1); /* 等待上次操作完成 */

	if (FlashStatus == HAL_OK)
	{
		while (waddr < endaddr) /* 写入地址小于结束地址，还未写完 */
		{
			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, waddr, (uint64_t)pbuf) != HAL_OK) /* 写入数据 */
			{
				printf(" 5 Flash program error\r\n");
				break; /* 写入异常 */
			}

			waddr += 32;
			pbuf += 8;
		}
	}
	SCB_EnableDCache();
	HAL_FLASH_Lock(); /* FLASH上锁 */
}
/******************************************************************************************/
/* 测试用代码 */

/**
 * @brief       测试写数据(写1个字)
 * @param       waddr : 起始地址
 * @param       wdata : 要写入的数据
 * @retval      无
 */
void test_write(uint32_t waddr, uint32_t wdata)
{
	stmflash_write(waddr, &wdata, 1); /* 写入一个字 */
}

FLASH_EraseInitTypeDef EraseInitStruct;

/*******************************************************************************
 * Function Name  : GetSector
 * Description    : 获取地址所在扇区
 * Input          : Address   FLASH地址
 * Output         : None
 * Return         : 扇区号
 * Note			 : None
 *******************************************************************************/
static uint32_t GetSector(uint32_t Address)
{
	uint32_t sector = 0;

	if (((Address < ADDR_FLASH_SECTOR_1_BANK1) && (Address >= ADDR_FLASH_SECTOR_0_BANK1)) ||
		((Address < ADDR_FLASH_SECTOR_1_BANK2) && (Address >= ADDR_FLASH_SECTOR_0_BANK2)))
	{
		sector = FLASH_SECTOR_0;
	}
	else if (((Address < ADDR_FLASH_SECTOR_2_BANK1) && (Address >= ADDR_FLASH_SECTOR_1_BANK1)) ||
			 ((Address < ADDR_FLASH_SECTOR_2_BANK2) && (Address >= ADDR_FLASH_SECTOR_1_BANK2)))
	{
		sector = FLASH_SECTOR_1;
	}
	else if (((Address < ADDR_FLASH_SECTOR_3_BANK1) && (Address >= ADDR_FLASH_SECTOR_2_BANK1)) ||
			 ((Address < ADDR_FLASH_SECTOR_3_BANK2) && (Address >= ADDR_FLASH_SECTOR_2_BANK2)))
	{
		sector = FLASH_SECTOR_2;
	}
	else if (((Address < ADDR_FLASH_SECTOR_4_BANK1) && (Address >= ADDR_FLASH_SECTOR_3_BANK1)) ||
			 ((Address < ADDR_FLASH_SECTOR_4_BANK2) && (Address >= ADDR_FLASH_SECTOR_3_BANK2)))
	{
		sector = FLASH_SECTOR_3;
	}
	else if (((Address < ADDR_FLASH_SECTOR_5_BANK1) && (Address >= ADDR_FLASH_SECTOR_4_BANK1)) ||
			 ((Address < ADDR_FLASH_SECTOR_5_BANK2) && (Address >= ADDR_FLASH_SECTOR_4_BANK2)))
	{
		sector = FLASH_SECTOR_4;
	}
	else if (((Address < ADDR_FLASH_SECTOR_6_BANK1) && (Address >= ADDR_FLASH_SECTOR_5_BANK1)) ||
			 ((Address < ADDR_FLASH_SECTOR_6_BANK2) && (Address >= ADDR_FLASH_SECTOR_5_BANK2)))
	{
		sector = FLASH_SECTOR_5;
	}
	else if (((Address < ADDR_FLASH_SECTOR_7_BANK1) && (Address >= ADDR_FLASH_SECTOR_6_BANK1)) ||
			 ((Address < ADDR_FLASH_SECTOR_7_BANK2) && (Address >= ADDR_FLASH_SECTOR_6_BANK2)))
	{
		sector = FLASH_SECTOR_6;
	}
	else if (((Address < ADDR_FLASH_SECTOR_0_BANK2) && (Address >= ADDR_FLASH_SECTOR_7_BANK1)) ||
			 ((Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_7_BANK2)))
	{
		sector = FLASH_SECTOR_7;
	}
	else
	{
		sector = FLASH_SECTOR_7;
	}

	return sector;
}

/*******************************************************************************
* Function Name  : StmFalsh_Read
* Description    : STM32H7的FLASH读取
* Input          : ReadAddr: 读地址
				   Len:读取长度
* Output         : rd_buff: 读取数据存储空间
* Return         : None
* Note			 : None
*******************************************************************************/
void StmFalsh_Read(uint32_t ReadAddr, uint8_t *rd_buff, uint32_t Len)
{
	uint32_t i;

	for (i = 0; i < Len; i++)
	{
		*(rd_buff++) = *((uint8_t *)(ReadAddr + i));
	}
}

/*******************************************************************************
* Function Name  : StmFlash_Program
* Description    : STM32H7片上FLASH编程
* Input          : WriteAddr: 编程起始地址
				   wr_buff: 待编程的数据
				   Len:编程长度(按字节数)
* Output         : None
* Return         : FLASH_SUCESS   编程成功    FLASH_ERROR  编程失败
* Note			 : wr_buff  长度要是32的整数倍   Len   是32的整数倍
*******************************************************************************/
uint32_t StmFlash_Program(uint32_t WriteAddr, uint8_t *wr_buff, uint32_t Len)
{
	uint8_t err = FLASH_SUCESS;
	uint32_t Address, EndAddress;
	HAL_StatusTypeDef FlashStatus = HAL_OK;
	// 长度限制,如果不是32的整数倍,最后不足部分取消掉
	Len = Len / 32 * 32;

	Address = WriteAddr;
	EndAddress = WriteAddr + Len;

	// 解锁
	HAL_FLASH_Unlock();

	while (Address < EndAddress)
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, Address, (uint64_t)(wr_buff)) == HAL_OK) // 一次编程32字节
		{
			Address += 32;
			wr_buff += 32;
		}
		else
		{
			printf("  2 Flash program error\r\n");
			err = FLASH_ERROR;
			break;
		}
	}

	HAL_FLASH_Lock(); // FALSH上锁

	return err;
}

/*******************************************************************************
* Function Name  : StmFlash_Erase
* Description    : STM32H7片上FLASH扇区擦除操作
* Input          : Banks: 擦除bank(FLASH_BANK_1/FLASH_BANK_2)
				   Start_Addr: 擦除起始地址       End_Addr: 擦除结束地址
* Output         : None
* Return         : HAL_OK  擦除成功    其它   擦除失败
* Note			 : None
*******************************************************************************/
uint32_t StmFlash_Erase(uint32_t Banks, uint32_t Start_Addr, uint32_t End_Addr)
{
	uint8_t err;
	uint32_t FirstSector = 0, NbOfSectors = 0;
	uint32_t SECTORError = 0;

	// 解锁
	HAL_FLASH_Unlock();

	FirstSector = GetSector(Start_Addr);				 // 起始扇区
	NbOfSectors = GetSector(End_Addr) - FirstSector + 1; // 擦除扇区数
	// 设置擦除参数
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS; // 扇区擦除操作
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Banks = Banks;
	EraseInitStruct.Sector = FirstSector;	 // 起始扇区
	EraseInitStruct.NbSectors = NbOfSectors; // 擦除扇区数

	// 执行擦除操作
	err = HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError);

	HAL_FLASH_Lock(); // FALSH上锁

	return err; // 返回擦除结果
}

void TEST_Flash(void)
{

	__disable_irq();
	HAL_StatusTypeDef FlashStatus = HAL_OK;
	uint8_t datatemp[TEXT_LENTH];

	// if (StmFlash_Erase(FLASH_BANK_1, FLASH_SAVE_ADDR, 0x080FFFFF) != HAL_OK)
	// {

	// 	printf("\r\n Erase fail");
	// }
	// FlashStatus = FLASH_WaitForLastOperation(FLASH_WAITETIME, FLASH_BANK_1); /* 等待上次操作完成 */

	// if (FlashStatus == HAL_OK)
	// {
	// 	printf("\r\n Erase OK");
	// }

	//stm32H7_flash_write(FLASH_SAVE_ADDR, (uint32_t *)g_text_buf, SIZE);

	// stmflash_write(FLASH_SAVE_ADDR, (uint32_t *)g_text_buf, SIZE);
	stmflash_read(ADDR_FLASH_SECTOR_4+8*1024, (uint32_t *)datatemp, SIZE); /* 读取数据 */
	__enable_irq();

	printf("\r\n Flash read %s", datatemp);
}


void APP_Flash_Erase(void)
{
	__disable_irq();
	HAL_StatusTypeDef FlashStatus = HAL_OK;
	if (StmFlash_Erase(FLASH_BANK_1, ADDR_FLASH_SECTOR_3, ADDR_FLASH_SECTOR_4) != HAL_OK)
	{
		printf("\r\n APP_Flash_Erase Erase fail");
	}
	FlashStatus = FLASH_WaitForLastOperation(FLASH_WAITETIME, FLASH_BANK_1); /* 等待上次操作完成 */

	if (FlashStatus == HAL_OK)
	{
		printf("\r\n  APP_Flash_Erase sErase OK");
	}
	__enable_irq();

	

}