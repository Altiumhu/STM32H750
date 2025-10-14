#include "bsp_595.h"


#define EX_595_SRCLK(x)		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,(GPIO_PinState)x)   //时钟	上升沿	
#define EX_595_RCLK(x)		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,(GPIO_PinState)x) 	//锁存	上升沿	
#define EX_595_SER(x)		  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,(GPIO_PinState)x) 	//数据


uint8_t ex_595_cache[EX_595_CHIP_NUM];


//==================================================================
//函数：ex_595_init
//作者：MZH
//日期：2022-03-16
//功能：初始化595
//参数：
//返回值：
//修改记录：
//==================================================================
//示例：
//==================================================================
void ex_595_init(void)
{
//    stc_gpio_init_t stcGpioInit;

//    (void)GPIO_StructInit(&stcGpioInit);
//    stcGpioInit.u16PinState = PIN_STAT_RST; //配置默认:0电平
//    stcGpioInit.u16PinDir = PIN_DIR_OUT;  //配置IO方向:输出模式
//	
//    (void)GPIO_Init(GPIO_PORT_B, GPIO_PIN_00, &stcGpioInit);
//    (void)GPIO_Init(GPIO_PORT_B, GPIO_PIN_01, &stcGpioInit);
//    (void)GPIO_Init(GPIO_PORT_B, GPIO_PIN_02, &stcGpioInit);
	
  /*Configure GPIO pin Output Level */
//  HAL_GPIO_WritePin(GPIOA, EXIO_595_LD_Pin|EXIO_595_CLK_Pin|EXIO_595_DATA_Pin, GPIO_PIN_RESET);
//	
	memset(ex_595_cache, 0, sizeof(ex_595_cache));
	ex_595_cache_up();

#if EX_595_DEBUG	
	printf("ex_595_init\r\n");
#endif
}
//==================================================================
//函数：ex_595_delay
//作者：MZH
//日期：2022-03-23
//功能：延时等待
//参数：
//返回值：
//修改记录：
//==================================================================
//示例：
//==================================================================
void ex_595_delay(uint32_t nop)
{
	while(nop--)__NOP();
}
//==================================================================
//函数：ex_595_send
//作者：MZH
//日期：2022-03-16
//功能：发送数据到595，但不锁存
//参数：data：8位数据
//返回值：
//修改记录：
//==================================================================
//示例：
//==================================================================
void ex_595_send(uint8_t data)
{
	uint8_t num = 0;
	
	for(num=0; num<8; num++)
	{
#if EX_595_MODE_DATA==EX_595_MODE_MSB		
		if(data&(0x80>>num))EX_595_SER(1);
		else EX_595_SER(0);
#else
		if(data&(0x01<<num))EX_595_SER = 1;
		else EX_595_SER = 0;
#endif	
		ex_595_delay(EX_595_DELAY);
		EX_595_SRCLK(0);ex_595_delay(EX_595_DELAY);
		EX_595_SRCLK (1);ex_595_delay(EX_595_DELAY);
	}
}
//==================================================================
//函数：ex_595_cache_up
//作者：MZH
//日期：2022-03-21
//功能：将缓存数据更新到芯片
//参数：
//返回值：
//修改记录：
//==================================================================
//示例：
//==================================================================
void ex_595_cache_up(void)
{
	uint32_t num = 0;
	
	for(num=0; num<EX_595_CHIP_NUM; num++)
	{
#if EX_595_MODE_CHIP==EX_595_MODE_MSB
		ex_595_send(ex_595_out[num]);
#else
		ex_595_send(ex_595_cache[EX_595_CHIP_NUM-(num+1)]);
#endif
	}
		
	EX_595_RCLK (0);ex_595_delay(EX_595_DELAY);
	EX_595_RCLK (1);	
}
//==================================================================
//函数：ex_595_write
//作者：MZH
//日期：2022-03-16
//功能：发送数据到595，并锁存
//参数：chip：所选芯片
//		pin:  输出IO,支持或运算
//		value:	输出电平
//返回值：
//修改记录：
//==================================================================
//示例：ex_595_write(0, EX_595_PIN0 | EX_595_PIN2, EX_595_SET);
//==================================================================
int ex_595_write(uint32_t chip, uint32_t pin, uint32_t value)
{
	uint32_t num = 0;
	
	if(chip>= EX_595_CHIP_NUM)return -1;
	
	for(num=0; num<8; num++){
		if((pin&(1U<<num))==0)continue;
		
		ex_595_cache[chip] &= ~(1U<<num);
		ex_595_cache[chip] |= (value&0x01)<<num;
	}
#if EX_595_WRITE_RT
	ex_595_cache_up();			//更新寄存器
#endif
	
#if EX_595_DEBUG
	printf("ex_595_write: chip = %d pin = 0x%x val = %d\r\n", chip, pin, value);
#endif
	return 0;
}
//==================================================================
//函数：ex_595_read
//作者：MZH
//日期：2022-03-16
//功能：读取595输出电平
//参数：chip：所选芯片
//		pin:  输出IO,支持或运算
//		value: 存放结果的指针
//返回值：
//修改记录：
//==================================================================
//示例：ex_595_read(0, EX_595_PIN0, &value);
//==================================================================
int ex_595_read(uint32_t chip, uint32_t pin, uint32_t *value)
{
	int res = 0;
	uint32_t num = 0;
	
	if(chip>= EX_595_CHIP_NUM)return -1;
	
	for(num=0; num<8; num++){
		if((pin&(1U<<num))==0)continue;
	
		*value |= (ex_595_cache[chip]&(1U<<num));
	}
#if EX_595_DEBUG	
	printf("ex_595_read: chip = %d pin = 0x%x val = 0x%x\r\n", chip, pin, *value);
#endif
	return res;
}








