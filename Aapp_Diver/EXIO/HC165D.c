/***********************************************************************************
 * 文 件 名   : HC165D.c
 * 负 责 人   : 
 * 创建日期   : 2022年11月12日
 * 文件描述   : 驱动层-IO模块->HC165D扩展输入IO驱动及IO管理
 * 版权说明   :
 * 其    他   : 
 * 修改日志   : 
***********************************************************************************/
#define HC165D_GLOBALS
#include "HC165D.h"
//#include "MyLog.h"


 

#define EXIO_HC165_CLK(x)		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,(GPIO_PinState)x)   //时钟	上升沿	
#define EXIO_HC165_LD(x)		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,(GPIO_PinState)x)	//锁存	
#define EXIO_HC165_DATA		  HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3)					//数据


//#define EXIO_HC165_CLK(x)		GPIO_WritePins(GPIO_PORT_B,GPIO_PIN_03,(en_pin_state_t)x)   //时钟	上升沿	
//#define EXIO_HC165_LD(x)		GPIO_WritePins(GPIO_PORT_B,GPIO_PIN_04,(en_pin_state_t)x)	//锁存	
//#define EXIO_HC165_DATA		    GPIO_ReadInputPins(GPIO_PORT_B,GPIO_PIN_05)					//数据

/*********************************************************************
*                        扩展IO操作接口
**********************************************************************/

static volatile uint32_t HC165_Reg[EX_HC165_CHIP_MAX_NUM]= {0};

static volatile int32_t HC165D_Stop = 0;//停止更新标志位

//扩展IO初始化
void extIO__Init(enum IO_MODE mode, enum CHIP_IO_PORT port, int32_t pin)
{
	  	
}

void HC165D_Stop_Update(void)
{
    HC165D_Stop = 1;
}

void HC165D_Start_Update(void)
{
    HC165D_Stop = 0;
}

/**********************************************************************
* Function: 	 更新165扩展输入IO的状态
* Description:   读取165串联芯片IO状态
* Input: 	     port---MUC端口号
                 
                 pin ---HC165整个IO数量
* Output:        
* Return: 	    void
* Others:       PB4---Data;PB3--clk;PB5---LOAD
* Modify Date:    Version:    Author:	      Modification:
* -----------------------------------------------
* 2023-11-9	  V1.0	      Hu Weiping
**********************************************************************/
uint32_t HC165D_UpdateIOState(void) 
{
	uint32_t num = 0;//,temp = 0;
	uint32_t ulRecData[EX_HC165_CHIP_MAX_NUM] ={0};
    static int32_t flag = 0;//验证是否发生了重入
    
    if(HC165D_Stop) 
        return 0;

    //暂时简单防错，有风险。后续可改互斥锁，但是互斥锁会导致等待，对于时序要求严格的情形无法适用
    if(0 == flag) 
        flag = 1;
    else 
    {
        printf("不允许多线程操作.\n\r");
        return 0;
    }

	//加载一次数据到寄存器
    EXIO_HC165_LD(GPIO_PIN_RESET);
	
  __NOP();__NOP();__NOP(); __NOP();__NOP();__NOP();__NOP(); __NOP();
   EXIO_HC165_LD(GPIO_PIN_SET);
 
   __NOP();__NOP();__NOP(); __NOP();__NOP();__NOP();__NOP(); __NOP();
	for(num=0; num < EX_HC165_CHIP_NUM; num++)
	{   
        for(int32_t i=0; i < 8; i++)
        {
            if(EXIO_HC165_DATA) ulRecData[num] |= 1<<((7-i));
            EXIO_HC165_CLK(0);
           //20ns以上
           __NOP(); __NOP();
           __NOP();__NOP(); __NOP(); __NOP();
            EXIO_HC165_CLK(1);  
           //20ns以上
           __NOP(); __NOP();
           __NOP();__NOP(); __NOP(); __NOP();
       
        }
            
	    HC165_Reg[num] = ulRecData[num];   
	}
    flag = 0;
    return 0;
}

//扩展IO读多个位
uint32_t extIO_ReadBits(enum CHIP_IO_PORT port, uint8_t spin, uint8_t epin)
{
	uint32_t ulRecData = 0,num;
    
	HC165D_UpdateIOState();//每次都更新所有输入IO
    
    for(num=0; num < EX_HC165_CHIP_NUM; num++)
    {
     	printf("\r\n HC165_Reg[%d] %X\r\n ",num,HC165_Reg[num]);
    }
	return ulRecData;
}



/**********************************************************************
* Function: 	 EXTIO_HC165_Chip_ReadBit
* Description:   读取165串联芯片IO状态
* Input: 	     port---MUC端口号
                 
                 pin ---HC165整个IO数量
* Output:        
* Return: 	    void
* Others:       PB4---Data;PB3--clk;PB5---LOAD
* Modify Date:    Version:    Author:	      Modification:
* -----------------------------------------------
* 2023-11-9	  V1.0	      Hu Weiping
**********************************************************************/
uint32_t EXTIO_HC165_ReadBit(enum CHIP_IO_PORT port, uint8_t pin)
{
    uint8_t bitH =0,BitL=0;
    
	if (pin >= EX_165_PIN_NUM)
    {
        printf("pin(%d) err.\r\n",pin);
        return 0;
	}
    bitH= pin>>3;//取8的整数--获取165第几块芯片
    BitL = pin%8;//取8的余数--获取165第几pin
	HC165D_UpdateIOState();//每次都更新所有输入IO

	return ((HC165_Reg[bitH]>>BitL)&0x01);
}

/**********************************************************************
* Function: 	 EXTIO_HC165_Chip_ReadBit
* Description:   读取165串联芯片IO状态
* Input: 	     port---MUC端口号
                 chip---HC165第几块IO
                 pin ---HC165第几块IO的pin
* Output:        
* Return: 	    void
* Others:        PB4---Data;PB3--clk;PB5---LOAD
* Modify Date:    Version:    Author:	      Modification:
* -----------------------------------------------
* 2023-11-9	  V1.0	      Hu Weiping
**********************************************************************/
uint32_t EXTIO_HC165_Chip_ReadBit(enum CHIP_IO_PORT port,uint16_t chip, uint8_t pin)
{
	if (chip >= EX_HC165_CHIP_NUM || pin>=8)
    {
        //return 0xffffffff;
        printf(" chip= %d pin(%d) err.\r\n",chip,pin);
        return 0;
	}
    
	HC165D_UpdateIOState();//每次都更新所有输入IO

	return ((HC165_Reg[chip]>>pin)&0x01);
}
/**********************************************************************
* Function: 	 EXTIO_HC165_Chip_ReadData
* Description:   读取165串联芯片单个IO状态
* Input: 	     port---MUC端口号
                 chip---HC165第几块IO
                 pin ---HC165第几块IO的pin
* Output:        
* Return: 	    HC165_Reg[0]  第几块IC bit0~bit7数据状态
* Others:        PB4---Data;PB3--clk;PB5---LOAD
* Modify Date:    Version:    Author:	      Modification:
* -----------------------------------------------
* 2023-11-9	  V1.0	      Hu Weiping
**********************************************************************/
uint32_t EXTIO_HC165_Chip_ReadData(enum CHIP_IO_PORT port,uint16_t chip)
{
	if (chip >= EX_HC165_CHIP_NUM )
    {
        //return 0xffffffff;
        printf("chip= %d  err.\r\n",chip);
        return 0;
	}
	HC165D_UpdateIOState();//每次都更新所有输入IO
//    IOData=(uint32_t *)&HC165_Reg[chip];
	return (HC165_Reg[chip]);
}
/**********************************************************************
* Function: 	 EXTIO_HC165_Init
* Description:   扩展IO输入芯片初始化
* Input: 	      void
* Output:
* Return: 	    void
* Others:       PB4---Data;PB3--clk;PB5---LOAD
* Modify Date:    Version:    Author:	      Modification:
* -----------------------------------------------
* 2023-11-9	  V1.0	      Hu Weiping
**********************************************************************/
void EXTIO_HC165_Init(void)
{
	

  GPIO_InitTypeDef GPIO_InitStruct = {0};
	  
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	
	GPIO_InitTypeDef gpio_init_struct;                      /* GPIO配置参数存储变量 */

	gpio_init_struct.Pin = GPIO_PIN_3;                   /* KEY0引脚 */
	gpio_init_struct.Mode = GPIO_MODE_INPUT;                /* 输入 */
	gpio_init_struct.Pull = GPIO_NOPULL;                  /* 上拉 */
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;          /* 高速 */
	HAL_GPIO_Init(GPIOB, &gpio_init_struct);       /* KEY0引脚模式设置,下拉输入 */
	
  
}



