

#include "head.h"
#include "appUser_Lowpass.h"


/*
s=tf('s');
den=s*500e-3/5+1
GS=1/den  

step(GS)
ts=1/50e3

D_GS=c2d(GS,ts)
*/

uint32_t A_SR_5ms_lowpass(uint32_t input,uint32_t reinitialize)//5ms
{
	static struct LOWPASS
	{
		uint32_t u;
		uint32_t u1;
		uint32_t y1;
		uint32_t y;
		uint32_t A1;
		uint32_t B1;
	}SRLOWPASS   = {0, 0, 0, 0, 64880, 655};//Q16 0.99 64881,655 1152一开始会打200ns吧
	if(reinitialize)
	{
		SRLOWPASS.u  = 0;
		SRLOWPASS.u1 = 0;
		SRLOWPASS.y1 = 0;
		SRLOWPASS.y  = 0;
	}
	else
	{
		SRLOWPASS.y1 = SRLOWPASS.y;
		SRLOWPASS.u1 = SRLOWPASS.u;
		SRLOWPASS.u  = input;
		SRLOWPASS.y  =   (SRLOWPASS.B1 * SRLOWPASS.u1 + SRLOWPASS.A1 * SRLOWPASS.y1)>>16;	
	}
	return SRLOWPASS.y;
}


 // 设置输出参考电压为24.0V，通过一阶低通滤波实现12~23.9V的软启动
float Set_Ref_5ms_lowpass(float input,uint32_t reinitialize)//5ms
{
	static struct LOWPASS
	{
		float u;
		float u1;
		float y1;
		float y;
		float A1;
		float B1;
	}VREFLOWPASS = {1.0f, 1.0f, 1.0f,1.0f,  0.9802f, 0.0198f};
	if(reinitialize)
	{
		VREFLOWPASS.u = 0.0f;
		VREFLOWPASS.u1 = 0.0f;
		VREFLOWPASS.y1 = 9.0f;
		VREFLOWPASS.y = 9.0f;
	}
	else
	{
		VREFLOWPASS.y1 = VREFLOWPASS.y;
		VREFLOWPASS.u1 = VREFLOWPASS.u;
		VREFLOWPASS.u  = input;
		//VREFLOWPASS.y= VREFLOWPASS.B0 * VREFLOWPASS.u + VREFLOWPASS.B1 * VREFLOWPASS.u1 + VREFLOWPASS.A1 * VREFLOWPASS.y1;由于B0为0
		VREFLOWPASS.y=VREFLOWPASS.B1 * VREFLOWPASS.u1 + VREFLOWPASS.A1 * VREFLOWPASS.y1;
	}
	return VREFLOWPASS.y;
}
 // 通过一阶低通滤波10sm
float Set_Ref_10ms_lowpass(float input,uint32_t reinitialize)//5ms
{
	static struct LOWPASS
	{
		float u;
		float u1;
		float y1;
		float y;
		float A1;
		float B1;
	}VREFLOWPASS = {12.0f, 12.0f, 12.0f, 12.0f,  0.999f, 0.001f};
	if(reinitialize)
	{
		VREFLOWPASS.u = 0.0f;
		VREFLOWPASS.u1 = 0.0f;
		VREFLOWPASS.y1 = 12.0f;
		VREFLOWPASS.y = 12.0f;
	}
	else
	{
		VREFLOWPASS.y1 = VREFLOWPASS.y;
		VREFLOWPASS.u1 = VREFLOWPASS.u;
		VREFLOWPASS.u  = input;
		//VREFLOWPASS.y= VREFLOWPASS.B0 * VREFLOWPASS.u + VREFLOWPASS.B1 * VREFLOWPASS.u1 + VREFLOWPASS.A1 * VREFLOWPASS.y1;由于B0为0
		VREFLOWPASS.y=VREFLOWPASS.B1 * VREFLOWPASS.u1 + VREFLOWPASS.A1 * VREFLOWPASS.y1;
	}
	return VREFLOWPASS.y;
}

 // 通过一阶低通滤波100ms
float Set_Ref_100ms_lowpass(float input,uint32_t reinitialize)//5ms
{
	static struct LOWPASS
	{
		float u;
		float u1;
		float y1;
		float y;
		float A1;
		float B1;
	}VREFLOWPASS = {3.0f, 3.0f, 3.0f, 3.0f,  0.99999f, 0.00001f};

	if(reinitialize)
	{
		VREFLOWPASS.u = 0.0f;
		VREFLOWPASS.u1 = 0.0f;
		VREFLOWPASS.y1 = 3.0f;
		VREFLOWPASS.y = 3.0f;
	}
	else
	{
		VREFLOWPASS.y1 = VREFLOWPASS.y;
		VREFLOWPASS.u1 = VREFLOWPASS.u;
		VREFLOWPASS.u  = input;
		//VREFLOWPASS.y= VREFLOWPASS.B0 * VREFLOWPASS.u + VREFLOWPASS.B1 * VREFLOWPASS.u1 + VREFLOWPASS.A1 * VREFLOWPASS.y1;由于B0为0
		VREFLOWPASS.y=VREFLOWPASS.B1 * VREFLOWPASS.u1 + VREFLOWPASS.A1 * VREFLOWPASS.y1;
	}
	return VREFLOWPASS.y;
}
 // 通过一阶低通滤波500ms
float Set_Ref_500ms_lowpass(float input,uint32_t reinitialize)//500ms
{
	static struct LOWPASS
	{
		float u;
		float u1;
		float y1;
		float y;
		float A1;
		float B1;
	}VREFLOWPASS = {12.0f, 12.0f, 12.0f, 12.0f,  0.999998, 0.000002f};
	if(reinitialize)
	{
		VREFLOWPASS.u = 0.0f;
		VREFLOWPASS.u1 = 0.0f;
		VREFLOWPASS.y1 = 12.0f;
		VREFLOWPASS.y = 12.0f;
	}
	else
	{
		VREFLOWPASS.y1 = VREFLOWPASS.y;
		VREFLOWPASS.u1 = VREFLOWPASS.u;
		VREFLOWPASS.u  = input;
		//VREFLOWPASS.y= VREFLOWPASS.B0 * VREFLOWPASS.u + VREFLOWPASS.B1 * VREFLOWPASS.u1 + VREFLOWPASS.A1 * VREFLOWPASS.y1;由于B0为0
		VREFLOWPASS.y=VREFLOWPASS.B1 * VREFLOWPASS.u1 + VREFLOWPASS.A1 * VREFLOWPASS.y1;
	}
	return VREFLOWPASS.y;
}


 // 通过一阶低通滤波500ms
  float Set_Ref_10us_lowpass(float input,uint32_t reinitialize)//500ms
{
	static struct LOWPASS
	{
		float u;
		float u1;
		float y1;
		float y;
		float A1;
		float B1;
	}VREFLOWPASS = {12.0f, 12.0f, 12.0f, 12.0f,  0.6065f,  0.3935f}; 
	if(reinitialize)
	{
		VREFLOWPASS.u = 0.0f;
		VREFLOWPASS.u1 = 0.0f;
		VREFLOWPASS.y1 = 12.0f;
		VREFLOWPASS.y = 12.0f;
	}
	else
	{
		VREFLOWPASS.y1 = VREFLOWPASS.y;
		VREFLOWPASS.u1 = VREFLOWPASS.u;
		VREFLOWPASS.u  = input;
		//VREFLOWPASS.y= VREFLOWPASS.B0 * VREFLOWPASS.u + VREFLOWPASS.B1 * VREFLOWPASS.u1 + VREFLOWPASS.A1 * VREFLOWPASS.y1;由于B0为0
		VREFLOWPASS.y=VREFLOWPASS.B1 * VREFLOWPASS.u1 + VREFLOWPASS.A1 * VREFLOWPASS.y1;
	}
	return VREFLOWPASS.y;
}
