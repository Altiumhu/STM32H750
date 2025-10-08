

#include "head.h"
#include "AppUser_pwm.h"

#define EPWM_DUTY_MAX 99.99f

volatile EPWM_Handle g_epwmHandle[EPWM_CH_NUM];

volatile FAN_Handle gFAN_Handle[EPWM_CH_NUM];

void Set_TIM8_PWM_Duty(uint32_t channel, float duty);

// 配置PWM初始化数据
void HAL_EPWM_Config(uint16_t ch)
{
	// for(ch =0;ch<BOARD_CHANNEL_NUM;ch++)
	{
	}
}
// PWM更新寄存器值
inline void Updata_EPWM_Handle(void)
{
}

inline void pwm_stop(uint16_t channel)
{

	switch (channel)
	{
	case 0:

		break;
	case 1:

		break;
	}
}

inline void pwm_start(uint16_t channel, uint16_t mode)
{

	switch (channel)
	{
	case 0:

		break;
	case 1:

		break;
	}
}

void Debug_PWM(void)
{
}
void Set_FAN_Duty(uint32_t channel, uint8_t FAN_Num)
{

	switch (channel)
	{
	case TIM_CHANNEL_1:
		switch (FAN_Num)
		{
		case 0:
			Set_TIM8_PWM_Duty(channel, 0);
			break;
		case 1:
			Set_TIM8_PWM_Duty(channel, 0.2f);
			break;
		case 2:
			Set_TIM8_PWM_Duty(channel, 0.5f);
			break;
		case 3:
			Set_TIM8_PWM_Duty(channel, 0.75f);
			break;
		case 4:
			Set_TIM8_PWM_Duty(channel, 0.99f);
			break;
		}
		break;
	case TIM_CHANNEL_2:
		switch (FAN_Num)
		{
		case 0:
			Set_TIM8_PWM_Duty(channel, 0);
			break;
		case 1:
			Set_TIM8_PWM_Duty(channel, 0.2f);
			break;
		case 2:
			Set_TIM8_PWM_Duty(channel, 0.5f);
			break;
		case 3:
			Set_TIM8_PWM_Duty(channel, 0.75f);
			break;
		case 4:
			Set_TIM8_PWM_Duty(channel, 0.99f);
			break;
		}
		break;
		break;

	case TIM_CHANNEL_3:
		switch (FAN_Num)
		{
		case 0:
			Set_TIM8_PWM_Duty(channel, 0);
			break;
		case 1:
			Set_TIM8_PWM_Duty(channel, 0.2f);
			break;
		case 2:
			Set_TIM8_PWM_Duty(channel, 0.5f);
			break;
		case 3:
			Set_TIM8_PWM_Duty(channel, 0.75f);
			break;
		case 4:
			Set_TIM8_PWM_Duty(channel, 0.99f);
			break;
		}
		break;
	case TIM_CHANNEL_4:
		switch (FAN_Num)
		{
		case 0:
			Set_TIM8_PWM_Duty(channel, 0);
			break;
		case 1:
			Set_TIM8_PWM_Duty(channel, 0.2f);
			break;
		case 2:
			Set_TIM8_PWM_Duty(channel, 0.5f);
			break;
		case 3:
			Set_TIM8_PWM_Duty(channel, 0.75f);
			break;
		case 4:
			Set_TIM8_PWM_Duty(channel, 0.99f);
			break;
		}
		break;
		break;
	}
}

/**
 * @brief  设置TIM8 PWM占空比
 * @param  channel: PWM通道 (TIM_CHANNEL_1 到 TIM_CHANNEL_4)
 * @param  duty: 占空比 (0-1000, 1000对应100%)
 * @retval None
 */
void Set_TIM8_PWM_Duty(uint32_t channel, float duty)
{
	// 确保duty在有效范围内
	//  if(duty > 1000) duty = 1000;

	float fPeriod = (float)htim8.Init.Period;

	// 计算比较寄存器值
	uint32_t pulse = (uint32_t)(fPeriod * duty);

	// 更新PWM占空比
	__HAL_TIM_SET_COMPARE(&htim8, channel, pulse);
}

//void UserMachine_Set_FAN_PWM_Ack(tcpProtocol *frameRec)
//{
//	uint8_t data[16];
//	uint8_t dataIndex = 0,Fan1_num,Fan2_num;

//	frameRec->streamNum[0] = frameRec->data[frameRec->indx-4]; // 流水号
//    frameRec->streamNum[1] = frameRec->data[frameRec->indx-3]; // 流水号
//    frameRec->encrypt = frameRec->data[frameRec->indx-2];      // 加密

//	Fan1_num=frameRec->data[0];
//	Fan2_num=frameRec->data[1];
//	// 组合连接应答的数据
//	data[dataIndex++] = frameRec->boxNum;
//	data[dataIndex++] = frameRec->cmd;
//	data[dataIndex++] = 'O';

// 	data[dataIndex++] =  frameRec->streamNum[0];//流水号
//	data[dataIndex++] =  frameRec->streamNum[1];//流水号
//	data[dataIndex++] = 0; // CRC
//	memcpy(frameRec->data, data, dataIndex);

//	frameRec->len=dataIndex;

//	uint16_t fan_num = 0;
//	Protocol *pProtocol;
//	pProtocol = Get_gProtocolOBj();
//	pProtocol->cmd = CMD_0x06;
//	pProtocol->Reg = 0x4092;
//	fan_num = Fan1_num;
//	fan_num = fan_num << 8 | Fan2_num;
//	pProtocol->Reg_Number = fan_num;

//	AppUser_Msg_CmdAnalys(pProtocol);

//	printf("\r\nboxSn=%X  cmd=%X   FAN=%X ", frameRec->boxNum, frameRec->cmd, pProtocol->Reg_Number);
//}
