/***********************************************************************
* Copyright (C) 2022, LAI Corporation.
*
* File Name: 	  user_crc_data.h
* File Mark:
* Description:
* Others: version  2020-09-30, V2.1.0, firmware for GD32F30x 
* Version:
* Author: Hu Weiping
* Date:
* History 1:
*     Date:           2022-11-5
*     Version:        V1.0
*     Author:         Hu Weiping
**********************************************************************/
#include "head.h"
#include "user_crc_data.h"


    /* exact-width unsigned integer types */


/* ----------------------------------------------------------------------------*/
/**
 * 
多项式产生：
如x16+x12+x5+1
x16表示�?16位为1，x5表示�?5位为1
(1 << 16) | (1 << 12) | (1 << 5) | (1) = 0x11021
但是CRC16�?取低16位，写成16进制数就�? 0x1021

CRC16的算法原理：

1.根据CRC16的标准选择初值CRCIn的值�?

2.将数�?的�??一�?字节与CRCIn�?8位异或�?

3.判断最高位，若该位�? 0 左移一位，若为 1 左移一位再与�?�项式Hex码异或�?

4.重�??3直至8位全部移位�?�算结束�?

5.重�?�将所有输入数�?操作完成以上步�?�，所�?16位数�?16位CRC校验码�?

根据算法原理与标准�?�求就能简单的写出具体程序�?

 *CRC算法参数模型解释�? 
    NAME：参数模型名称�? 
    WIDTH：�?�度，即CRC比特数�? 
    POLY：生成项的简写，�?16进制表示。例如：CRC-32即是0x04C11DB7，忽略了最高位�?"1"，即完整的生    成项�?0x104C11DB7�? 
    INIT：这�?算法开始时寄存�?（crc）的初�?�化预置值，十六进制表示�? 
    REFIN：待测数�?的每�?字节�?否按位反�?，True或False�? 
    REFOUT：在计算后之后，异或输出之前，整�?数据�?否按位反�?，True或False�? 
    XOROUT：�?�算结果与�?�参数异或后得到最终的CRC值�?
    Alias：别名及其应用范围�?
 * 
    CRC16_CCITT：�?�项式x16+x12+x5+1�?0x1021），初�?��?0x0000，低位在前，高位在后，结果与0x0000异或

　　CRC16_CCITT_FALSE：�?�项式x16+x12+x5+1�?0x1021），初�?��?0xFFFF，低位在后，高位在前，结果与0x0000异或

　　CRC16_XMODEM：�?�项式x16+x12+x5+1�?0x1021），初�?��?0x0000，低位在后，高位在前，结果与0x0000异或

　　CRC16_X25：�?�项式x16+x12+x5+1�?0x1021），初�?��?0x0000，低位在前，高位在后，结果与0xFFFF异或

　　CRC16_MODBUS：�?�项式x16+x15+x5+1�?0x8005），初�?��?0xFFFF，低位在前，高位在后，结果与0x0000异或

　　CRC16_IBM：�?�项式x16+x15+x5+1�?0x8005），初�?��?0x0000，低位在前，高位在后，结果与0x0000异或

　　CRC16_MAXIM：�?�项式x16+x15+x5+1�?0x8005），初�?��?0x0000，低位在前，高位在后，结果与0xFFFF异或

　　CRC16_USB：�?�项式x16+x15+x5+1�?0x8005），初�?��?0xFFFF，低位在前，高位在后，结果与0xFFFF异或
 * 
 */
/* ----------------------------------------------------------------------------*/


/* ----------------------------------------------------------------------------*/
/**
 * @brief InvertUint8       有�?�号数据�?�?
 *
 * @param DesBuf    �?标缓�?
 * @param SrcBuf    源缓�?
 *
 * @return   void
 */
/* ----------------------------------------------------------------------------*/
void InvertUint8(uint16_t *DesBuf, uint8_t *SrcBuf)
{
    int i;
    uint8_t temp = 0;
    
    for(i = 0; i < 8; i++)
    {
        if(SrcBuf[0] & (1 << i))
        {
            temp |= 1<<(7-i);
        }
    }
    DesBuf[0] = temp;
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief InvertUint16      有�?�号2字节数据�?�?
 *
 * @param DesBuf     �?标缓�?
 * @param SrcBuf     源缓�?
 *
 * @return  void 
 */
/* ----------------------------------------------------------------------------*/
void InvertUint16(uint16_t *DesBuf, uint16_t *SrcBuf)  
{  
    int i;  
    uint16_t temp = 0;    
    
    for(i = 0; i < 16; i++)  
    {  
        if(SrcBuf[0] & (1 << i))
        {          
            temp |= 1<<(15 - i);  
        }
    }  
    DesBuf[0] = temp;  
}



//这里为了效率，我�?不需要将所有Refin和refout为true的输入输出数�?移位�?�?
//�?需要将poly二项式转换后，运算时将左移变为右�?
unsigned short CRC16_CCITT(unsigned char *data, unsigned int datalen)
{
	unsigned short wCRCin = 0x0000;
	unsigned short wCPoly = 0x1021;
//	unsigned char wChar = 0;
	
	InvertUint16(&wCPoly,&wCPoly);
	while (datalen--) 	
	{
		wCRCin ^= *(data++);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x01)
				wCRCin = (wCRCin >> 1) ^ wCPoly;
			else
				wCRCin = wCRCin >> 1;
		}
	}
	return (wCRCin);
}

/****************************Info********************************************** 
 * Name:    CRC-16/CCITT-FALSE   x16+x12+x5+1 
 * Width:	16 
 * Poly:    0x1021 
 * Init:    0xFFFF 
 * Refin:   False 
 * Refout:  False 
 * Xorout:  0x0000 
 * Note: 
 *****************************************************************************/ 
unsigned short CRC16_CCITT_FALSE(unsigned char *data, unsigned int datalen)
{
	unsigned short wCRCin = 0xFFFF;
	unsigned short wCPoly = 0x1021;
	
	while (datalen--) 	
	{
		wCRCin ^= *(data++) << 8;
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	return (wCRCin);
}

/****************************Info********************************************** 
 * Name:    CRC-16/XMODEM       x16+x12+x5+1 
 * Width:	16 
 * Poly:    0x1021 
 * Init:    0x0000 
 * Refin:   False 
 * Refout:  False 
 * Xorout:  0x0000 
 * Alias:   CRC-16/ZMODEM,CRC-16/ACORN 
 *****************************************************************************/ 
unsigned short CRC16_XMODEM(unsigned char *data, unsigned int datalen)
{
	unsigned short wCRCin = 0x0000;
	unsigned short wCPoly = 0x1021;
	
	while (datalen--) 	
	{
		wCRCin ^= (*(data++) << 8);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}
	return (wCRCin);
}


/****************************Info**********************************************
 * Name:    CRC-16/XMODEM       x16+x12+x5+1
 * Width:   16
 * Poly:    0x1021
 * Init:    0x0000
 * Refin:   False
 * Refout:  False
 * Xorout:  0x0000
 * Alias:   CRC-16/ZMODEM,CRC-16/ACORN
 *****************************************************************************/
unsigned short CRC16_XMODEM_SmallMode(unsigned char *data, unsigned int datalen)
{
    unsigned short wCRCin = 0x0000;
    unsigned short wCPoly = 0x1021;
    unsigned short tmepL = 0;
    unsigned short tmepH = 0;

    while (datalen--)
    {
        wCRCin ^= (*(data++) << 8);
        for(int i = 0;i < 8;i++)
        {
            if(wCRCin & 0x8000)
                wCRCin = (wCRCin << 1) ^ wCPoly;
            else
                wCRCin = wCRCin << 1;
        }
    }

    tmepH |= (wCRCin&0x00FF)<<8U;
    tmepL |= (wCRCin&0xFF00)>>8U;
    wCRCin = tmepH|tmepL;
    return (wCRCin);
}

/* ----------------------------------------------------------------------------*/
/**
 * @brief CRC16_MODBUS      MODBUS模型crc校验
 *
 * @param puchMsg        校验缓存
 * @param usDataLen      校验长度
 *
 * @return  
 */
/* ----------------------------------------------------------------------------*/
unsigned short CRC16_MODBUS(unsigned char *data, unsigned int datalen)
{
	unsigned short wCRCin = 0xFFFF;
	unsigned short wCPoly = 0x8005;
	
	InvertUint16(&wCPoly,&wCPoly);
	while (datalen--) 	
	{
		wCRCin ^= *(data++);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x01)
				wCRCin = (wCRCin >> 1) ^ wCPoly;
			else
				wCRCin = wCRCin >> 1;
		}
	}
	return (wCRCin);
}



/* ----------------------------------------------------------------------------*/
/**
 * @brief CRC16_MODBUS      MODBUS模型crc校验 低字节在前，高字节在�?
 *
 * @param puchMsg        校验缓存
 * @param usDataLen      校验长度
 *
 * @return  
 */
/* ----------------------------------------------------------------------------*/
unsigned short CRC16_MODBUS_SmallMode(unsigned char *data, unsigned int datalen)
{
	unsigned short wCRCin = 0xFFFF;
	unsigned short wCPoly = 0x8005;
	unsigned short dataMode = 0x00;
	
	InvertUint16(&wCPoly,&wCPoly);
	while (datalen--) 	
	{
		wCRCin ^= *(data++);
		for(int i = 0;i < 8;i++)
		{
			if(wCRCin & 0x01)
				wCRCin = (wCRCin >> 1) ^ wCPoly;
			else
				wCRCin = wCRCin >> 1;
		}
	}
	dataMode = wCRCin<<8 | (wCRCin&0xFF00)>>8;
	return (dataMode);
}


/*****************************************************************************
 * �? �? �?  : _Cal_Xor
 * 函数功能  : 计算XOR校验�?
 * 输入参数  : INT8U *data  起�?�字�?
			   INT32U len   需要�?�算的字节长�?
 * 输出参数  : �?
 * �? �? �?  : xor校验�?
 * �?    �?  :

*****************************************************************************/
 unsigned char _Cal_Xor(unsigned char *data, unsigned short len)
{
	unsigned short i = 0;
	unsigned char xor = 0;

	for (i = 0; i < len; i++)
	{
		xor ^= data[i];
	}
	
	return xor;
}

// 计算Modbus CRC16校验�?
uint16_t modbus_crc(uint8_t *data, uint16_t length) {
    uint16_t crc = 0xFFFF; // 初�?��?
    uint16_t tmepL = 0;
    uint16_t tmepH = 0;
	
    for (uint16_t i = 0; i < length; i++) {
        crc ^= data[i]; // 异或当前数据字节
        
        // 处理每个字节�?8�?
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) { // 检查最低位
                crc = (crc >> 1) ^ 0xA001; // 右移并异或�?�项�?
            } else {
                crc >>= 1; // 直接右移
            }
        }
    }
        // 发送时应先低字节后高字�?
     tmepH |= (crc&0x00FF)<<8U;
     tmepL |= (crc&0xFF00)>>8U;
     crc = tmepH|tmepL;


//     crc = crc_hi|crc_lo;
    return crc;
}


const unsigned short crc16tab[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

 unsigned short crc16_ccitt(const unsigned char *buf, unsigned int len)
{
    register unsigned int counter;
    register unsigned short crc = 0;
    for(counter = 0; counter < len; counter++) {
        crc = (crc << 8) ^ crc16tab[((crc >> 8) ^ * (unsigned char *)buf++) & 0x00FF];
    }
    return crc;
}

uint16_t swap_endian_16(uint16_t value) {
    return (value >> 8) | (value << 8);
}
uint32_t swap_endian_32(uint32_t value) {
    return ((value >> 24) & 0x000000FF) |  // 移动最高字节到最低位
           ((value >> 8)  & 0x0000FF00) |  // 移动次高字节到�?�低�?
           ((value << 8)  & 0x00FF0000) |  // 移动次低字节到�?�高�?
           ((value << 24) & 0xFF000000);   // 移动最低字节到最高位
}




// CRC16-CCITT多项�? (x^16 + x^12 + x^5 + 1)
#define CRC16_POLY 0x1021

// 计算CRC16校验�?
//uint16_t CRC16_Calculate(const uint8_t *data, uint32_t length) {
//    uint16_t crc = 0xFFFF; // 初�?��?
//    
//    for (uint32_t i = 0; i < length; i++) {
//        crc ^= (uint16_t)data[i] << 8;
//        
//        for (uint8_t j = 0; j < 8; j++) {
//            if (crc & 0x8000) {
//                crc = (crc << 1) ^ CRC16_POLY;
//            } else {
//                crc <<= 1;
//            }
//        }
//    }
//    
//    return crc;
//}
uint16_t AppUse_CheckSum(uint8_t *data, INT16U num)
{
	uint16_t sum = 0, i;

	for (i = 0; i < num; i++)
	{
		sum += data[i];
	}
	return sum;
}


/*********************************************************************************************************
											END FILE
*********************************************************************************************************/



