

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


#ifndef __USER_CRC_DATA_H
#define __USER_CRC_DATA_H	
#include "head.h"

extern unsigned short CRC16_CCITT(unsigned char *data, unsigned int datalen);
extern unsigned short CRC16_CCITT_FALSE(unsigned char *data, unsigned int datalen);
extern unsigned short CRC16_XMODEM(unsigned char *data, unsigned int datalen);
extern unsigned short CRC16_MODBUS(unsigned char *data, unsigned int datalen);
extern unsigned short CRC16_MODBUS_SmallMode(unsigned char *data, unsigned int datalen);

extern unsigned short CRC16_XMODEM_SmallMode(unsigned char *data, unsigned int datalen);

extern  unsigned char _Cal_Xor(unsigned char *data, unsigned short len);

extern unsigned short crc16_ccitt(const unsigned char *buf, unsigned int len);


extern uint16_t AppUse_CheckSum(uint8_t *data, INT16U num);

extern uint16_t modbus_crc(uint8_t *data, uint16_t length) ;


extern uint16_t swap_endian_16(uint16_t value);

extern uint32_t swap_endian_32(uint32_t value);

uint16_t CRC16_Calculate(const uint8_t *data, uint32_t length) ;

#endif

/*********************************************************************************************************
											END FILE
*********************************************************************************************************/
