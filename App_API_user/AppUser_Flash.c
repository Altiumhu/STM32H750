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


/*
  1.获取SMEC88的UID号, 获取STM32的ID, 获取STM32随机数
  2.验证PIN
  3.内外部认证
  4.SHA1=>前置数据^随机数
  5.密文读
  6.读数据
  7.写数据
  8.构造算法(PA口数据->密文送加密芯片, 密文返回)

  如果直接引用,请将print的调试信息去除
*/

void SMEC_Test(void)
{
	/*各种密钥,不会在I2C线路上传输,可以使用同一组.应该将密钥分散存储,防止主控芯片被破解后,被攻击者在二进制码中找到密钥 */
	unsigned char InternalKey[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};//内部认证密钥,必须和SMEC88一致
	unsigned char ExternalKey[16] = {0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F};//外部认证密钥,必须和SMEC88一致
	unsigned char SHA1_Key[16] = {0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,0x2E,0x2F};   //哈希算法认证密钥,必须和SMEC88一致
	unsigned char MKey[16] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F};   //主控密钥,用于产生过程密钥,必须和SMEC88一致

	unsigned char Pin[8] = {0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc};			//Pin认证密钥,必须和SMEC88一致
		
	unsigned char bStm32Uid[12] = {0};			//存放STM32的UID
	unsigned char bSmec88Uid[12] = {0};		//存放SMEC88的UID
	unsigned short RandomSeek;					//随机数种子
	unsigned char bRandom[8] = {0};				//存放随机数
	unsigned char bSessionKey[8] = {0};			//存放过程密钥,过程密钥为临时产生的密钥
	unsigned char bDataBuf[64] = {0};
	unsigned char ret, bLen;
	unsigned short i, j;

	/*利用ADC悬空引脚产生随机数*/
//	Adc_Init();									//可放在主程序中
//	RandomSeek = Get_Adc_RandomSeek();			//利用ADC悬空引脚产生随机数

//	/*获取STM32的UID*/
//	GetStm32Uid(bStm32Uid);
//	printf("GetStm32Uid: ");
//  PrintHex(bStm32Uid, 12);
//	printf("\r\n");
		
	/*获取SMEC88的UID*/
	ret = SMEC_GetUid(bSmec88Uid);
	if(ret)
	{
		printf("SMEC_GetUid -> Error !\r\n");
		while(1);
	}
	printf("SMEC_GetUid: ");
  PrintHex(bSmec88Uid, 12);
	printf("\r\n");

	/*将随机数RandomSeek，再做一次随机处理(与STM32的UID, SMEC88的UID作绑定, 使得即使相同情况下,不同的STM32,SMEC88随机数种子也不同)*/
	for(i = 0; i < 6; i += 2)
	{
		/*使RandomSeek与STM32的UID相关*/
		j = (bStm32Uid[i] << 8) + bStm32Uid[i + 1];
		RandomSeek ^= j;

		/*使RandomSeek与SMEC88的UID相关*/
		j = (bSmec88Uid[i] << 8) + bSmec88Uid[i + 1];
		RandomSeek ^= j;
	}
	srand(RandomSeek);
	printf("RandomSeek: %04x \r\n", RandomSeek);

	/*PIN码验证*/
	ret = SMEC_CheckPin(Pin, (unsigned char)sizeof(Pin));
	if(ret)
	{
		printf("SMEC_CheckPin -> Error !\r\n");
		while(1);
	}
	printf("SMEC_CheckPin OK !\r\n");

	/*内部认证, 主控芯片对SMEC88加密芯片合法性判断*/
	for(i = 0; i < 8; i ++)
	{
		bRandom[i] = (unsigned char) rand();
	}
	ret = SMEC_IntrAuth(InternalKey, bRandom);
	if(ret)
	{
		printf("SMEC_IntrAuth -> Error !\r\n");
		while(1);
	}
	printf("SMEC_IntrAuth OK !\r\n");

	/*外部认证, SMEC88加密芯片对主控芯片合法性判断*/
	ret = SMEC_ExtrAuth(ExternalKey);
	if(ret)
	{
		printf("SMEC_ExtrAuth -> Error !\r\n");
		while(1);
	}
	printf("SMEC_ExtrAuth OK !\r\n");

	/*SHA1摘要算法认证, 数据长度可自己设定*/
	for(i = 0; i < 16; i ++)
	{
		bDataBuf[i] = (unsigned char) rand();
	}
	ret = SMEC_Sha1Auth(SHA1_Key, (unsigned char)sizeof(SHA1_Key), bDataBuf, 16);
	if(ret)
	{
		printf("SMEC_Sha1Auth -> Error !\r\n");
		while(1);
	}
	printf("SMEC_Sha1Auth OK !\r\n");

	/*调用加密芯片内部计算圆周长算法*/
	bDataBuf[0] = 0x02;
	ret = SMEC_CircleAlg(bDataBuf, 1, bDataBuf, &bLen);
	if(ret)
	{
		printf("SMEC_CircleAlg -> Error !\r\n");
		while(1);
	}
	printf("SMEC_CircleAlg OK, C = %02x !\r\n", bDataBuf[0]);

	/*产生过程密钥,用于后续的Flash数据加密读,及构造的"端口数据运算"*/
	for(i = 0; i < 8; i ++)
	{
		bRandom[i] = (unsigned char) rand();
	}
	ret = SMEC_GenSessionKey(MKey, bRandom, bSessionKey);
	if(ret)
	{
		printf("SMEC_GenSessionKey -> Error !\r\n");
		while(1); 
	}
	printf("SMEC_GenSessionKey OK !\r\n");

	/*密文读取Flash数据*/
	ret = SMEC_CryptReadFlash(bSessionKey, 0x0000, bDataBuf, 16);
	if(ret)
	{
		printf("SMEC_CryptReadFlash -> Error !\r\n");
		while(1);
	}
	printf("SMEC_CryptReadFlash OK:\r\n");
    PrintHex(bDataBuf, 16);

	/*读取Flash数据*/
	ret = SMEC_ReadFlash(0x0000, bDataBuf, 16);
	if(ret)
	{
		printf("SMEC_ReadFlash -> Error !\r\n");
		while(1);
	}
	printf("SMEC_ReadFlash OK:\r\n");
    PrintHex(bDataBuf, 16);

	/*写Flash数据*/
	for(i = 0; i < 16; i ++)
	{
		bDataBuf[i] = (unsigned char) i;
	}
	ret = SMEC_WriteFlash(0x0000, bDataBuf, 16);
	if(ret)
	{
		printf("SMEC_WriteFlash -> Error !\r\n");
		while(1);
	}
 	printf("SMEC_WriteFlash OK!\r\n");

	/*构造"端口数据运算", 可以用实际的PA~PC端口数据*/
	bDataBuf[0] = 0x00;
	bDataBuf[1] = 0x00;
	ret = SMEC_GpioAlg(bSessionKey, bDataBuf,2, bDataBuf);
	if(ret)
	{
		printf("SMEC_GpioAlg -> Error !\r\n");
		while(1);
	}
 	printf("SMEC_GpioAlg OK:\r\n");
    PrintHex(bDataBuf, 2);

	/*调用加密芯片内部计算圆周长算法,并密文在线路上传输*/
	bDataBuf[0] = 0x02;
	ret = SMEC_CircleAlgCrypt(bSessionKey, bDataBuf, 1, bDataBuf, &bLen);
	if(ret)
	{
		printf("SMEC_CircleAlgCrypt -> Error !\r\n");
		while(1);
	}
	printf("SMEC_CircleAlgCrypt OK, C = %02x !\r\n", bDataBuf[0]);
}
