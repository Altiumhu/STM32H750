#include "head.h"
#include "app_api_gpio.h"
#include "gpio.h"

Structural_Smoke g_Structural_Smoke;

typedef void (*GPIO_Callback)(GPIO_TypeDef *, uint16_t);

void GPIO_Array_Init(GPIO_Config *config);

// MCU_IO_OUT 引脚配置数组
GPIO_Config MCU_IO_OUT[MCU_IO_OUT_MAX] = {

//    {GPIOG, MCU_IO_OUT1_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}, // MCU_IO_OUT1
//    {GPIOG, MCU_IO_OUT2_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}, // MCU_IO_OUT2
//    {GPIOG, MCU_IO_OUT3_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}, // MCU_IO_OUT3

//    {GPIOG, MCU_IO_OUT4_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}, // MCU_IO_OUT4
//    {GPIOG, MCU_IO_OUT5_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}, // MCU_IO_OUT5
//    {GPIOG, MCU_IO_OUT6_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}, // MCU_IO_OUT6

//    {GPIOD, MCU_IO_OUT7_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}, // MCU_IO_OUT7
//    {GPIOD, MCU_IO_OUT8_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}, // MCU_IO_OUT8
//    {GPIOD, MCU_IO_OUT9_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}, // MCU_IO_OUT9

//    {GPIOD, MCU_IO_OUT10_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}, // MCU_IO_OUT10
//    {GPIOD, MCU_IO_OUT11_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}, // MCU_IO_OUT11
//    {GPIOD, MCU_IO_OUT12_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}, // MCU_IO_OUT12

//    // 三色灯
//    {GPIOE, LEDA_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}, // MCU_IO_OUT5
//    {GPIOE, LEDB_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}, // MCU_IO_OUT6
//    {GPIOE, LEDG_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}, // MCU_IO_OUT6
//    {GPIOE, LEDR_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}, // MCU_IO_OUT6

//    // 蜂鸣器
//    {GPIOE, MCU_BEEP_OUT_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}, // MCU_IO_OUT4

//    // 呼吸灯
//    {GPIOB, LED1_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}, // MCU_IO_OUT4
//    {GPIOB, LED2_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}, // MCU_IO_OUT4
//    // RS485控制
//    {GPIOB, RS485_Dir_Pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0}, // MCU_IO_OUT4
//                                                                                      // 报警器

    {NULL, 0, 0, 0, 0, 0} // 结束标记
};

// MCU_IO_OUT 引脚配置数组初始化
void MCU_IO_OUT_Init(void)
{
    for (int i = 0; i < MCU_IO_OUT_MAX; i++)
    {
        GPIO_Array_Init(&MCU_IO_OUT[i]);
    }
}

void GPIO_Array_Operation(GPIO_Config *config, GPIO_Callback callback)
{
    for (int i = 0; config[i].port != NULL; i++)
    {
        callback(config[i].port, config[i].pin);
    }
}

void GPIO_Array_Init(GPIO_Config *config)
{
    for (int i = 0; config[i].port != NULL; i++)
    {
        GPIO_InitTypeDef GPIO_InitStruct = {0};

        // 启用端口时钟
        if (config[i].port == GPIOA)
            __HAL_RCC_GPIOA_CLK_ENABLE();
        else if (config[i].port == GPIOB)
            __HAL_RCC_GPIOB_CLK_ENABLE();
        else if (config[i].port == GPIOC)
            __HAL_RCC_GPIOC_CLK_ENABLE();

        else if (config[i].port == GPIOD)
            __HAL_RCC_GPIOD_CLK_ENABLE();
        else if (config[i].port == GPIOE)
            __HAL_RCC_GPIOE_CLK_ENABLE();
        else if (config[i].port == GPIOF)
            __HAL_RCC_GPIOF_CLK_ENABLE();
        else if (config[i].port == GPIOG)
            __HAL_RCC_GPIOG_CLK_ENABLE();
        // 添加其他端口...

        // 配置GPIO参数
        GPIO_InitStruct.Pin = config[i].pin;
        GPIO_InitStruct.Mode = config[i].mode;
        GPIO_InitStruct.Pull = config[i].pull;
        GPIO_InitStruct.Speed = config[i].speed;
        GPIO_InitStruct.Alternate = config[i].alternate;

        HAL_GPIO_Init(config[i].port, &GPIO_InitStruct);
    }
}


// Boost检查
uint16_t LLC_BOOST_OK_cnt = 0;
void Get_Boost_GPIO_OK(void)
{
}

void Smoke_Init_Data(void)
{
    memset(&g_Structural_Smoke, 0, sizeof(Structural_Smoke));
}

void Smoke_vTask(void)
{

    if (GetBit(g_Master_Hand.IO_INPUT_Data[3], 9) == 0) // 烟雾检测 ---IO  JP_IN9
    {
        g_Structural_Smoke.smoke_en = 1;
    }
    else
    {
        //  g_Structural_Smoke.smoke_en = 0;
        g_Structural_Smoke.smoke_timer = 0;
        g_Structural_Smoke.smoke_flag = 0;
    }

    if (g_Structural_Smoke.smoke_en == 1 && g_Structural_Smoke.smoke_flag == 0)
    {

        HAL_GPIO_WritePin(MCU_IO_OUT[5].port, MCU_IO_OUT[5].pin, GPIO_PIN_SET);   // 设置高电平
        HAL_GPIO_WritePin(MCU_IO_OUT[8].port, MCU_IO_OUT[8].pin, GPIO_PIN_RESET); // 设置低电平

        g_Master_Hand.IO_OUT_Data = ClearBit(g_Master_Hand.IO_OUT_Data, 5);
        g_Master_Hand.IO_OUT_Data = ClearBit(g_Master_Hand.IO_OUT_Data, 9);
       // UserSlave_Stop(-1, 0xffff); // 通知下位机关机
        g_Structural_Smoke.smoke_flag = 1;
        // 设置托盘下降

         SetParGPIO(0);

        g_Master_Hand.IO_OUT_Data = ClearBit(g_Master_Hand.IO_OUT_Data, 0);
        g_Master_Hand.IO_OUT_Data = SetBit(g_Master_Hand.IO_OUT_Data, 1);
    }
}
// 风机检查
void Get_FAN_GPIO(void)
{
}

void Debug_GPIO(void)
{
}

void Sys_Run_Led(void)
{
    //HAL_GPIO_TogglePin(GPIOB, LED1_Pin);
}
#if 0
// 设置GPIO控制和气缸控制
void UserSet_OUT_IO_Master_pAck(tcpProtocol *frameRec)
{

    uint8_t data[16];
    uint8_t dataIndex = 0;
    uint16 muc_OutIO;

    frameRec->streamNum[0] = frameRec->data[frameRec->indx - 4]; // 流水号
    frameRec->streamNum[1] = frameRec->data[frameRec->indx - 3]; // 流水号
    frameRec->encrypt = frameRec->data[frameRec->indx - 2];      // 加密

    muc_OutIO = U8TOU16(frameRec->data);

    // 组合连接应答的数据
    data[dataIndex++] = frameRec->boxNum;
    data[dataIndex++] = frameRec->cmd;
    data[dataIndex++] = 'O';

    data[dataIndex++] = frameRec->streamNum[0]; // 流水号
    data[dataIndex++] = frameRec->streamNum[1]; // 流水号
    data[dataIndex++] = 0;                      // CRC
    memcpy(frameRec->data, data, dataIndex);
    frameRec->len = dataIndex;
    //    UserEthernet_SendFrame(data, dataIndex);

    Protocol *pProtocol;
    pProtocol = Get_gProtocolOBj();
    pProtocol->cmd = CMD_0x06;
    pProtocol->Reg = 0x4091;
    pProtocol->Reg_Number = (muc_OutIO); // 大小端交换符合Modbus

    AppUser_Msg_CmdAnalys(pProtocol);

    printf("\r\nboxSn=%X  cmd=%X  muc_OutIO=%X ", frameRec->boxNum, frameRec->cmd, (muc_OutIO));
}
#endif
// 用户自定义命令处理函数
void cmd_led_control(int argc, char **argv)
{
    if (argc < 2)
    {
        HAL_UART_Transmit(&huart1, (uint8_t *)"Usage: led on|off|toggle [id]\r\n", 31, 100);
        return;
    }

    // 实现LED控制逻辑...
   // LOG_INFO(LOG_MODULE_PERIPH, "LED command: %s %s", argv[1], argc > 2 ? argv[2] : "all");
}

// 设置指定位为1
uint16_t SetBit(uint16_t value, int bitPosition)
{
    if (bitPosition < 0 || bitPosition > 15)
        return 0;

    return (uint16_t)(value | (1 << bitPosition));
}

// 设置指定位为0
uint16_t ClearBit(uint16_t value, int bitPosition)
{
    if (bitPosition < 0 || bitPosition > 15)
        return 0;

    return (uint16_t)(value & ~(1 << bitPosition));
}

uint32_t GetBit(uint32_t number, uint32_t bitPosition)
{
    if (bitPosition >= 32)
        return 0; // 超出范围的位视为0

    return (number >> bitPosition) & 1;
}

// 函数：将指定位取反
uint32_t flip_bit(uint32_t value, int position)
{
    // 创建掩码：将1左移到指定位置
    uint32_t mask = 1 << position;
    // 使用异或操作取反特定位
    return value ^ mask;
}

void Read_GPIO_vTask(void)
{

    static uint16_t IO_INPUbit0_7, IO_INPUbit8_15;
    // __disable_irq();
    IO_INPUbit0_7 = EXTIO_HC165_Chip_ReadData(iPortNull, 0);
    IO_INPUbit8_15 = EXTIO_HC165_Chip_ReadData(iPortNull, 1);
    //   __enable_irq();

    g_Master_Hand.IO_INPUT_Data[0] = 0x0;
    g_Master_Hand.IO_INPUT_Data[1] = 0x0;
    g_Master_Hand.IO_INPUT_Data[2] = 0x0;
    g_Master_Hand.IO_INPUT_Data[3] = IO_INPUbit8_15 << 8 | IO_INPUbit0_7; // 读取传感器输入IO数据
                                                                          // printf("\r\n g_Master_Hand.IO_INPUT_Data[3]=  %X", g_Master_Hand.IO_INPUT_Data[3]);

    if (GetBit(g_Master_Hand.IO_INPUT_Data[3], 12) == 0) // 手动模式按键检测 ---IO  JP_IN13
    {
        if (GetBit(g_Master_Hand.IO_INPUT_Data[3], 10) == 0) // 上升按键检测  JP_IN11 MCU_IO_OUT[indx].port, MCU_IO_OUT[indx].pin
        {

            SetParGPIO(1);
        }
        else if (GetBit(g_Master_Hand.IO_INPUT_Data[3], 11) == 0) // 下降按键检测  JP_IN12
        {
            SetParGPIO(0);
        }
    }
}

uint8_t Get_INPUT_GPIO_Bit(uint8_t IOtype)
{
    uint8_t IOData;
#if 0
    switch (IOtype)
    {
    case ETEMPCAPSTATUSBITINDEX_STOP: // 急停
        IOData = GetBit(g_Master_Hand.IO_INPUT_Data[3], 0);
        IOData = flip_bit(IOData, 0);
        break;

    case ETEMPCAPSTATUSBITINDEX_SMOKE: // 烟雾
        IOData = GetBit(g_Master_Hand.IO_INPUT_Data[3], 8);
        IOData = flip_bit(IOData, 0);
        break;

    case ETEMPCAPSTATUSBITINDEX_POS:
        IOData = GetBit(g_Master_Hand.IO_INPUT_Data[3], 2);
        break;

    case ETEMPCAPSTATUSBITINDEX_FITURE:

        break;

    case ETEMPCAPSTATUSBITINDEX_FAN:
        break;

    case ETEMPCAPSTATUSBITINDEX_TEMPENVWARN:

        break;

    case ETEMPCAPSTATUSBITINDEX_PRESSWARN:

        break;
    }
#endif
    return IOData;
}

// 设置气缸动作
void SetParGPIO(uint8_t IO)
{

    if (IO == 1) // JP_IN11 气缸压合
    {

        HAL_GPIO_WritePin(MCU_IO_OUT[0].port, MCU_IO_OUT[0].pin, GPIO_PIN_RESET); // 设置高电平
        HAL_GPIO_WritePin(MCU_IO_OUT[1].port, MCU_IO_OUT[1].pin, GPIO_PIN_SET);   // 设置低电平
        g_Master_Hand.IO_OUT_Data = ClearBit(g_Master_Hand.IO_OUT_Data, 0);
        g_Master_Hand.IO_OUT_Data = SetBit(g_Master_Hand.IO_OUT_Data, 1);
    }
    else if (IO == 0) // JP_IN12 气缸松开
    {
        HAL_GPIO_WritePin(MCU_IO_OUT[1].port, MCU_IO_OUT[1].pin, GPIO_PIN_RESET); // 设置高电平
        HAL_GPIO_WritePin(MCU_IO_OUT[0].port, MCU_IO_OUT[0].pin, GPIO_PIN_SET);   // 设置低电平

        g_Master_Hand.IO_OUT_Data = ClearBit(g_Master_Hand.IO_OUT_Data, 1);
        g_Master_Hand.IO_OUT_Data = SetBit(g_Master_Hand.IO_OUT_Data, 0);
    }
}