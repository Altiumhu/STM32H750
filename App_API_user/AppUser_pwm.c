

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
      // g_epwmHandle[ch].High_MOS_Timer_TBPRD =(uint32_t) ((EPWM_DUTY_MAX * (float32_t)(CHARGE_EPWM_TIMER_TBPRD << 8))*0.01f);
       g_epwmHandle[ch].High_MOS_PHS = 0;
       g_epwmHandle[ch].High_MOS_DUTY = 2933U;
       g_epwmHandle[ch].High_MOS_LOW_DUTY = g_epwmHandle[ch].High_MOS_DUTY; //高压侧上管占空比

       g_epwmHandle[ch].High_MOS_DTF = 40;
       g_epwmHandle[ch].High_MOS_DTB = 10;//285ns
      // g_epwmHandle[ch].High_MOS_DUTY_MAX = g_epwmHandle[ch].High_MOS_Timer_TBPRD>>1 202752;
       g_epwmHandle[ch].High_MOS_DUTY_MAX = 3200;//36.40% 97750
       g_epwmHandle[ch].High_MOS_DUTY_MIN = 10; //5200
       g_epwmHandle[ch].High_MOS_STA = 0;
      // g_epwmHandle[ch].High_MOS_OpenFlag = 0;

       g_epwmHandle[ch].Low_MOS_Timer_TBPRD=g_epwmHandle[ch].High_MOS_Timer_TBPRD;
       g_epwmHandle[ch].Low_MOS_PHS = 0;
       g_epwmHandle[ch].Low_MOS_DUTY = 5500U ;
       g_epwmHandle[ch].Low_MOS_DTF = 40;
       g_epwmHandle[ch].Low_MOS_DTB = 4500;
       g_epwmHandle[ch].Low_MOS_DUTY_MAX = g_epwmHandle[ch].Low_MOS_Timer_TBPRD>>1;
       g_epwmHandle[ch].Low_MOS_DUTY_MIN = 5000;
       g_epwmHandle[ch].Low_MOS_STA = 0;
       g_epwmHandle[ch].Low_MOS_OpenFlag = 0;




 
   }
}
// PWM更新寄存器值
void Updata_EPWM_Handle(void)
{
  uint32_t channel;
	
	

	
  for(channel=0;channel<16;channel++)
	{
			
		
		
		  if(g_epwmHandle[channel].High_MOS_DUTY>=g_epwmHandle[channel].High_MOS_DUTY_MAX)
			{
			  g_epwmHandle[channel].High_MOS_DUTY =g_epwmHandle[channel].High_MOS_DUTY_MAX;
			}
			
		  if(g_epwmHandle[channel].High_MOS_DUTY<=g_epwmHandle[channel].High_MOS_DUTY_MIN)
			{
			  g_epwmHandle[channel].High_MOS_DUTY =g_epwmHandle[channel].High_MOS_DUTY_MIN;
			}
		
			switch (channel)
			{
			case 0:
							__HAL_TIM_SET_COMPARE (&htim1, TIM_CHANNEL_1 , 	g_epwmHandle[channel].High_MOS_DUTY);
				break;
			case 1:
							__HAL_TIM_SET_COMPARE ( &htim1, TIM_CHANNEL_2, g_epwmHandle[channel].High_MOS_DUTY);
				break;
			case 2:
							__HAL_TIM_SET_COMPARE (&htim1, TIM_CHANNEL_3 , g_epwmHandle[channel].High_MOS_DUTY);
				break;
			case 3:
							__HAL_TIM_SET_COMPARE ( &htim1, TIM_CHANNEL_4, g_epwmHandle[channel].High_MOS_DUTY);
				break;
			case 4:
							__HAL_TIM_SET_COMPARE (&htim2, TIM_CHANNEL_1 , g_epwmHandle[channel].High_MOS_DUTY);
				break;
			case 5:
							__HAL_TIM_SET_COMPARE ( &htim2, TIM_CHANNEL_2, g_epwmHandle[channel].High_MOS_DUTY);
				break;
			case 6:
							__HAL_TIM_SET_COMPARE (&htim2, TIM_CHANNEL_3 , g_epwmHandle[channel].High_MOS_DUTY);
				break;
			case 7:
							__HAL_TIM_SET_COMPARE ( &htim2, TIM_CHANNEL_4, g_epwmHandle[channel].High_MOS_DUTY); 
				break;
			
			case 8:
							__HAL_TIM_SET_COMPARE (&htim3, TIM_CHANNEL_1 , g_epwmHandle[channel].High_MOS_DUTY);
				break;
			case 9:
							__HAL_TIM_SET_COMPARE ( &htim3, TIM_CHANNEL_2,g_epwmHandle[channel].High_MOS_DUTY);
				break;
			case 10:
							__HAL_TIM_SET_COMPARE (&htim4, TIM_CHANNEL_3 , g_epwmHandle[channel].High_MOS_DUTY);
				break;
			case 11:
							__HAL_TIM_SET_COMPARE ( &htim4, TIM_CHANNEL_4, g_epwmHandle[channel].High_MOS_DUTY); 
				break;
			case 12:
							__HAL_TIM_SET_COMPARE (&htim8, TIM_CHANNEL_1 ,g_epwmHandle[channel].High_MOS_DUTY);
				break;
			case 13:
							__HAL_TIM_SET_COMPARE ( &htim8, TIM_CHANNEL_2, g_epwmHandle[channel].High_MOS_DUTY);
				break;
			case 14:
							__HAL_TIM_SET_COMPARE (&htim8, TIM_CHANNEL_3 , g_epwmHandle[channel].High_MOS_DUTY);
				break;
			case 15:
							__HAL_TIM_SET_COMPARE ( &htim8, TIM_CHANNEL_4, g_epwmHandle[channel].High_MOS_DUTY);
				break;
			
			}
			}
}

 void pwm_stop(uint16_t channel)
{

	switch (channel)
	{
	case 0:
         	__HAL_TIM_SET_COMPARE (&htim1, TIM_CHANNEL_1 , 0);
		break;
	case 1:
         	__HAL_TIM_SET_COMPARE ( &htim1, TIM_CHANNEL_2, 0);  
		break;
	case 2:
         	__HAL_TIM_SET_COMPARE (&htim1, TIM_CHANNEL_3 , 0);
		break;
	case 3:
         	__HAL_TIM_SET_COMPARE ( &htim1, TIM_CHANNEL_4, 0);  
		break;
	case 4:
         	__HAL_TIM_SET_COMPARE (&htim2, TIM_CHANNEL_1 , 0);
		break;
	case 5:
         	__HAL_TIM_SET_COMPARE ( &htim2, TIM_CHANNEL_2, 0);  
		break;
	case 6:
         	__HAL_TIM_SET_COMPARE (&htim2, TIM_CHANNEL_3 , 0);
		break;
	case 7:
         	__HAL_TIM_SET_COMPARE ( &htim2, TIM_CHANNEL_4, 0);  
		break;
	
	case 8:
         	__HAL_TIM_SET_COMPARE (&htim3, TIM_CHANNEL_1 , 0);
		break;
	case 9:
         	__HAL_TIM_SET_COMPARE ( &htim3, TIM_CHANNEL_2, 0);  
		break;
	case 10:
         	__HAL_TIM_SET_COMPARE (&htim4, TIM_CHANNEL_3 , 0);
		break;
	case 11:
         	__HAL_TIM_SET_COMPARE ( &htim4, TIM_CHANNEL_4, 0);  
		break;
	case 12:
         	__HAL_TIM_SET_COMPARE (&htim8, TIM_CHANNEL_1 , 0);
		break;
	case 13:
         	__HAL_TIM_SET_COMPARE ( &htim8, TIM_CHANNEL_2, 0);  
		break;
	case 14:
         	__HAL_TIM_SET_COMPARE (&htim8, TIM_CHANNEL_3 , 0);
		break;
	case 15:
         	__HAL_TIM_SET_COMPARE ( &htim8, TIM_CHANNEL_4, 0);  
		break;
	
	}
}

inline void pwm_start(uint16_t channel, uint16_t mode)
{

	switch (channel)
	{
	case 0:
         HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
		break;
	case 1:
         HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2); 
		break;
	case 2:
         HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
		break;
	case 3:
         HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4); 
		break;
	
	case 4:
         HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
		break;
	case 5:
         HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); 
		break;
	case 6:
         HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
		break;
	case 7:
         HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4); 
		break;
	

	case 8:
         HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
		break;
	case 9:
         HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2); 
		break;
	case 10:
         HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
		break;
	case 11:
         HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4); 
		break;
	
	case 12:
         HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
		break;
	case 13:
         HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2); 
		break;
	case 14:
         HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);
		break;
	case 15:
         HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4); 
		break;
	}
	
	
}

void Debug_PWM(void)
{
	uint32_t channel;
	
	
	 HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_1);
	__HAL_TIM_SET_COMPARE (&htim12, TIM_CHANNEL_1 , 4000);
	for(channel =0;channel<16;channel++)
	{
	   pwm_start(channel,0);
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
