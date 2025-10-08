#include "bsp_165.h"

uint8_t ex_165_cache[EX_165_CHIP_NUM];

//==================================================================
//函数：ex_165_init
//作者：MZH
//日期：2022-03-16
//功能：初始化165
//参数：
//返回值：
//修改记录：
//==================================================================
//示例：
//==================================================================
void ex_165_init(void)
{
    stc_gpio_init_t stcGpioInit;

    (void)GPIO_StructInit(&stcGpioInit);
    stcGpioInit.u16PinState = PIN_STAT_RST;
    stcGpioInit.u16PinDir = PIN_DIR_OUT;
	
    (void)GPIO_Init(GPIO_PORT_B, GPIO_PIN_03, &stcGpioInit);
    (void)GPIO_Init(GPIO_PORT_B, GPIO_PIN_04, &stcGpioInit);

	 stcGpioInit.u16PinDir = PIN_DIR_IN;
	(void)GPIO_Init(GPIO_PORT_B, GPIO_PIN_05, &stcGpioInit);
	
	ex_165_cache_up();			
}
//==================================================================
//函数：ex_165_delay
//作者：MZH
//日期：2022-03-23
//功能：延时等待
//参数：
//返回值：
//修改记录：
//==================================================================
//示例：
//==================================================================
void ex_165_delay(uint32_t nop)
{
	while(nop--)__NOP();
}
//==================================================================
//函数：ex_165_send
//作者：MZH
//日期：2022-03-16
//功能：读取165寄存值
//参数：data：8位数据
//返回值：
//修改记录：
//==================================================================
//示例：
//==================================================================
void ex_165_send(uint8_t *data)
{
	uint8_t num = 0;
	uint8_t bit = 0;
	
	for(num=0; num<8; num++)
	{
		bit = 0;
#if EX_165_MODE_DATA==EX_165_MODE_LSB
		if(EX_165_DATA)bit = 0x01;
		*data |= bit<<num;
#else
		if(EX_165_DATA)bit = 0x80;
		*data |= bit>>num;
#endif		
		EX_165_CLK(0);ex_165_delay(EX_165_DELAY);
		EX_165_CLK(1);ex_165_delay(EX_165_DELAY);
	}
}
//==================================================================
//函数：ex_165_cache_up
//作者：MZH
//日期：2022-03-21
//功能：读取一次数据到缓存区
//参数：
//返回值：
//修改记录：
//==================================================================
//示例：
//==================================================================
void ex_165_cache_up(void)
{
	uint32_t num = 0;
	
	//加载一次数据到寄存器
	EX_165_LD(0);delay_us(1);
	EX_165_LD(1);
	
	for(num=0; num<EX_165_CHIP_NUM; num++)
	{
#if EX_165_MODE_CHIP==EX_165_MODE_MSB
		ex_165_send(&ex_165_cache[EX_165_CHIP_NUM-(num+1)]);
#else
		ex_165_send(&ex_165_cache[num]);
#endif
	}	
}
//==================================================================
//函数：ex_165_read
//作者：MZH
//日期：2022-03-16
//功能：读取165输入电平
//参数：chip：所选芯片
//		pin:  输出IO,支持或运算
//		value: 存放结果的指针
//返回值：
//修改记录：
//==================================================================
//示例：ex_165_read(0, EX_165_PIN_0|EX_165_PIN_3, &value);
//==================================================================
int ex_165_read(uint32_t chip, uint32_t pin, uint32_t *value)
{
	uint32_t num = 0;
	
	if(chip>= EX_165_CHIP_NUM)return -1;
	
#if EX_165_READ_RT	
	ex_165_cache_up();//读取一次数据到缓存
#endif	
	for(num=0; num<8; num++){
		if((pin&(1U<<num))==0)continue;
	
		*value |= (ex_165_cache[chip]&(1U<<num));
	}
	
	return 0;		
}




