#include "head.h"
#include "AppUser_Flash.h"

	unsigned char bStm32Uid[12] = {0};			//存放STM32的UID
	
	
int put_char(int ch)
{
 USART1_Send((uint8_t*)&ch, 1);
  return ch;
}

void PrintHex(unsigned char *str,unsigned int len)
{
	unsigned int i;
	unsigned char a1,a2;
	for(i=0;i<len;i++)
	{ 
		a1=str[i]>>4;
		if(a1<10)a1+='0';
		else a1=a1-10+'A';		
		a2=str[i]&0x0f;		
		if(a2<10)a2+='0';
		else a2=a2-10+'A';		
		put_char(a1);
		put_char(a2);       
	}
	put_char('\r');
	put_char('\n');
	
}
	
//---------------------------------------------------------
// 函数名: 获取STM32H750的UID
// 参数说明：
//   pSTM32_UID - 存放STM32的UID,12字节
// 返回值说明：
//   void
// 说明: STM32H750的UID位于0x08FFF800地址，共96位(12字节)
//---------------------------------------------------------
#define STM32_UID_ADDR_BASE   0x08FFF800  // UID起始地址
#define VIRTUAL_OFFSET_1     0x5F43454D  // 虚拟偏移量1，请修改成其他值！
#define VIRTUAL_OFFSET_2    (VIRTUAL_OFFSET_1 + 1) // 虚拟偏移量2



void STM32H750_GetUID(uint8_t *pUID)
{
//    const uint32_t STM32H750_UID_BASE = 0x08FFF800U; // STM32H750 UID基地址
//	
//    uint8_t *pUID_Reg = (uint8_t *)STM32H750_UID_BASE;
//	
//    for (uint8_t i = 0; i < 12; i++) 
//	    {
//        pUID[i] = pUID_Reg[i];
//    }
	
   // 用 volatile const 强制从特定内存区域读取
    volatile const uint32_t *pUID_Reg = (volatile const uint32_t *)(0x08FFF800);
    
    // 逐字（32位）对齐读取
    pUID[0] = (uint8_t)(pUID_Reg[0] >> 0);
    pUID[1] = (uint8_t)(pUID_Reg[0] >> 8);
    pUID[2] = (uint8_t)(pUID_Reg[0] >> 16);
    pUID[3] = (uint8_t)(pUID_Reg[0] >> 24);
    
    pUID[4] = (uint8_t)(pUID_Reg[1] >> 0);
    pUID[5] = (uint8_t)(pUID_Reg[1] >> 8);
    pUID[6] = (uint8_t)(pUID_Reg[1] >> 16);
    pUID[7] = (uint8_t)(pUID_Reg[1] >> 24);
    
    pUID[8]  = (uint8_t)(pUID_Reg[2] >> 0);
    pUID[9]  = (uint8_t)(pUID_Reg[2] >> 8);
    pUID[10] = (uint8_t)(pUID_Reg[2] >> 16);
    pUID[11] = (uint8_t)(pUID_Reg[2] >> 24);
}



void STM32H750_UID(void)
{


	/*获取STM32的UID*/
	STM32H750_GetUID(bStm32Uid);
	printf("GetStm32Uid: ");
  PrintHex(bStm32Uid, 12);
	printf("\r\n");

}
