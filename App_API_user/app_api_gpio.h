#ifndef USRE_APP_API_GPIO_H_
#define USRE_APP_API_GPIO_H_


#include "head.h"
#include "gpio.h"

#define MCU_IO_OUT_MAX 40
typedef struct {
    GPIO_TypeDef* port;  // 
    uint16_t pin;        // 
    uint32_t mode;       // 
    uint32_t pull;       // 
    uint32_t speed;      // 
    uint32_t alternate;  // 
} GPIO_Config;




typedef struct
{ 
	  uint16_t smoke_flag;
    uint16_t smoke_en;
    uint16_t smoke_timer;

} Structural_Smoke;



extern Structural_Smoke g_Structural_Smoke;

extern  GPIO_Config MCU_IO_OUT[MCU_IO_OUT_MAX] ;

extern void Get_Boost_GPIO_OK(void);

extern void Get_FAN_GPIO(void);

extern void Smoke_Init_Data(void);

extern void Smoke_vTask(void);

void Debug_GPIO(void);

extern void Sys_Run_Led(void);

//extern void UserSet_OUT_IO_Master_pAck(tcpProtocol *frameRec);

extern void UserSet_RGB_Led(uint16_t mode);

extern void cmd_led_control(int argc, char **argv);

extern void Read_GPIO_vTask(void);

extern void MCU_IO_OUT_Init(void);

extern uint16_t ClearBit(uint16_t value, int bitPosition);


extern uint16_t SetBit(uint16_t value, int bitPosition);


extern uint32_t GetBit(uint32_t number, uint32_t bitPosition);

extern uint32_t flip_bit(uint32_t value, int position);

extern uint8_t Get_INPUT_GPIO_Bit(uint8_t IOtype);

extern void SetCD4052(uint8_t IO);




#endif






