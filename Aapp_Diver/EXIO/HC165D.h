#ifndef _HC165D_H_
#define  _HC165D_H_

#ifdef   HC165D_GLOBALS
#define  HC165D_EXT
#else
#define  HC165D_EXT extern
#endif

#include "sys.h"
#include <stdio.h>
#include "delay.h"
#include "head.h"

#define EX_HC165_CHIP_MAX_NUM   8 //165的片数最大
#define EX_HC165_CHIP_NUM       2 //165的片数
#define EX_165_PIN_NUM         (8*EX_HC165_CHIP_NUM) //165扩展输入IO数量


enum eHC165_IO_No
{
	iHC165_PIN_0 = 0,
	iHC165_PIN_1,
	iHC165_PIN_2,
	iHC165_PIN_3,
	iHC165_PIN_4,
	iHC165_PIN_5,
	iHC165_PIN_6,
	iHC165_PIN_7,
	iHC165_PIN_8,
	iHC165_PIN_9,
	iHC165_PIN_10,
	iHC165_PIN_11,
	iHC165_PIN_12,
	iHC165_PIN_13,
	iHC165_PIN_14,
	iHC165_PIN_15,
	iHC165_PIN_16,
	iHC165_PIN_17,
	iHC165_PIN_18,
	iHC165_PIN_19,
	iHC165_PIN_20,
	iHC165_PIN_21,
	iHC165_PIN_22,
	iHC165_PIN_23,
	iHC165_PIN_24,
	iHC165_PIN_25,
	iHC165_PIN_26,
	iHC165_PIN_27,
	iHC165_PIN_28,
	iHC165_PIN_29,
	iHC165_PIN_30,
	iHC165_PIN_31,
    iHC165_PIN_32,
	iHC165_PIN_33,
	iHC165_PIN_34,
	iHC165_PIN_35,
	iHC165_PIN_36,
	iHC165_PIN_37,
	iHC165_PIN_38,
	iHC165_PIN_39,
	iHC165_PIN_40,
	iHC165_PIN_41,
	iHC165_PIN_42,
	iHC165_PIN_43,
	iHC165_PIN_44,
	iHC165_PIN_45,
	iHC165_PIN_46,
	iHC165_PIN_47,
	iHC165_PIN_MAX,
};

 //定义端口
 enum CHIP_IO_PORT
 {
		 iPortA = 0,
		 iPortB,
		 iPortC,
		 iPortD,
		 iPortE,
		 iPortF,
		 iPortG,
		 iPortH,
		 iPortI,
		 iPortNull,
		 iPortMax = iPortNull,
 };

 
  //定义IO模式
 enum IO_MODE
 {
		 iOutput_Mode = 0,   //输出模式
		 iInput_Mode,        //输入模式
		 iIoModeMax,
 };
 
/*********************************************************/


/************************** HC165D-IO接口 *****************************/
HC165D_EXT void extIO__Init(enum IO_MODE mode, enum CHIP_IO_PORT port, int32_t pin);
 
//HC165D_EXT unsigned long long extIO_ReadBits(enum CHIP_IO_PORT port, uint8_t spin, uint8_t epin);
//HC165D_EXT unsigned long long extIO_ReadBit(enum CHIP_IO_PORT port, uint8_t pin);

HC165D_EXT uint32_t extIO_ReadBits(enum CHIP_IO_PORT port, uint8_t spin, uint8_t epin);
HC165D_EXT uint32_t extIO_ReadBit(enum CHIP_IO_PORT port, uint8_t pin);
HC165D_EXT uint32_t EXTIO_HC165_ReadBit(enum CHIP_IO_PORT port, uint8_t pin);
HC165D_EXT  uint32_t EXTIO_HC165_Chip_ReadData(enum CHIP_IO_PORT port,uint16_t chip);
 
HC165D_EXT void EXTIO_HC165_Init(void);
/**********************************************************************/

#endif
