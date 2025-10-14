
#ifndef USRE_APP_DRIVE_FLASH_H_
#define USRE_APP_DRIVE_FLASH_H_


#include "head.h"

/* FLASH起始地址 */
#define STM32_FLASH_SIZE        0X100000                    /* STM32 FLASH 总大小 */
#define STM32_FLASH_BASE        0x08000000                  /* STM32 FLASH 起始地址 */

#define FLASH_WAITETIME         50000                       /* FLASH等待超时时间 */

/* FLASH 扇区的起始地址,分8个扇区,每个扇区128KB,总共1MB */
#define ADDR_FLASH_SECTOR_0    ((uint32_t)0x08000000)      /* 扇区0起始地址, 128 Kbytes */
#define ADDR_FLASH_SECTOR_1    ((uint32_t)0x08020000)      /* 扇区1起始地址, 128 Kbytes */
#define ADDR_FLASH_SECTOR_2    ((uint32_t)0x08040000)      /* 扇区2起始地址, 128 Kbytes */
#define ADDR_FLASH_SECTOR_3    ((uint32_t)0x08060000)      /* 扇区3起始地址, 128 Kbytes */
#define ADDR_FLASH_SECTOR_4    ((uint32_t)0x08080000)      /* 扇区4起始地址, 128 Kbytes */
#define ADDR_FLASH_SECTOR_5    ((uint32_t)0x080A0000)      /* 扇区5起始地址, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6    ((uint32_t)0x080C0000)      /* 扇区6起始地址, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7    ((uint32_t)0x080E0000)      /* 扇区7起始地址, 128 Kbytes */

/******************************************************************************************/

uint8_t stmflash_get_flash_sector(uint32_t faddr);                        /* 获取某个地址所在的flash扇区 */
uint32_t stmflash_read_word(uint32_t faddr);                              /* 从指定地址读取一个字 */
void stmflash_write(uint32_t waddr, uint32_t *pbuf, uint32_t length);     /* 从指定地址开始写入指定长度的数据 */
void stmflash_read(uint32_t raddr, uint32_t *pbuf, uint32_t length);      /* 从指定地址开始读出指定长度的数据 */

void test_write(uint32_t waddr, uint32_t wdata);                          /* 测试写数据 */

void StmFalsh_Read(uint32_t ReadAddr, uint8_t *rd_buff, uint32_t Len);

uint32_t StmFlash_Erase(uint32_t Banks, uint32_t Start_Addr, uint32_t End_Addr);

uint32_t StmFlash_Program(uint32_t WriteAddr, uint8_t *wr_buff, uint32_t Len);
void TEST_Flash(void);


void APP_Flash_Erase(void);


#define FLASH_BASE_ADDR      (uint32_t)(FLASH_BASE)
#define FLASH_END_ADDR       (uint32_t)(0x081FFFFF)

//扇区定义
/* Bank 1 */
#define ADDR_FLASH_SECTOR_0_BANK1     ((uint32_t)0x08000000) /* Base @ of Sector 0, 128 Kbytes */
#define ADDR_FLASH_SECTOR_1_BANK1     ((uint32_t)0x08020000) /* Base @ of Sector 1, 128 Kbytes */
#define ADDR_FLASH_SECTOR_2_BANK1     ((uint32_t)0x08040000) /* Base @ of Sector 2, 128 Kbytes */
#define ADDR_FLASH_SECTOR_3_BANK1     ((uint32_t)0x08060000) /* Base @ of Sector 3, 128 Kbytes */
#define ADDR_FLASH_SECTOR_4_BANK1     ((uint32_t)0x08080000) /* Base @ of Sector 4, 128 Kbytes */
#define ADDR_FLASH_SECTOR_5_BANK1     ((uint32_t)0x080A0000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6_BANK1     ((uint32_t)0x080C0000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7_BANK1     ((uint32_t)0x080E0000) /* Base @ of Sector 7, 128 Kbytes */

/* Bank 2 */
#define ADDR_FLASH_SECTOR_0_BANK2     ((uint32_t)0x08100000) /* Base @ of Sector 0, 128 Kbytes */
#define ADDR_FLASH_SECTOR_1_BANK2     ((uint32_t)0x08120000) /* Base @ of Sector 1, 128 Kbytes */
#define ADDR_FLASH_SECTOR_2_BANK2     ((uint32_t)0x08140000) /* Base @ of Sector 2, 128 Kbytes */
#define ADDR_FLASH_SECTOR_3_BANK2     ((uint32_t)0x08160000) /* Base @ of Sector 3, 128 Kbytes */
#define ADDR_FLASH_SECTOR_4_BANK2     ((uint32_t)0x08180000) /* Base @ of Sector 4, 128 Kbytes */
#define ADDR_FLASH_SECTOR_5_BANK2     ((uint32_t)0x081A0000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6_BANK2     ((uint32_t)0x081C0000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7_BANK2     ((uint32_t)0x081E0000) /* Base @ of Sector 7, 128 Kbytes */

enum{FLASH_SUCESS = 0, FLASH_ERROR};

#endif





