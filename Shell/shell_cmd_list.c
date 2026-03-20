

#include "main.h"
#include "usart.h"
#include "gpio.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "head.h"

#include "shell_cmd_list.h"

/// 设置输入目标电压
void shell_Set_VRef(int argc, char *argv[])
{
	int channel[16];
	float VRef;
	int i;

	if (argc < 2)
	{
		printf("Usage: 输入非法指令\r\n");
		return;
	}

	// 参数个数（不含命令本身）
	int num_params = argc - 1;
	// 字符串转整型
	channel[0] = atoi(argv[1]) - 1;
	VRef = atof(argv[2]);

	g_Channelinfo[channel[0]].Set_PreCV = VRef * 1000.0f;

	printf("\r\n设置通道[%d]目标电压=%fmV\r\n", channel[0], g_Channelinfo[channel[0]].Set_PreCV);
}

/// 设置输出电流
void shell_Set_IRef(int argc, char *argv[])
{
	int channel[16];
	float IRef;
	int i;

	if (argc < 2)
	{
		printf("Usage: 输入非法指令\r\n");
		return;
	}

	// 参数个数（不含命令本身）
	int num_params = argc - 1;
	// 字符串转整型
	channel[0] = atoi(argv[1]) - 1;
	IRef = atof(argv[2]);

	g_Channelinfo[channel[0]].Set_PreCC = IRef * 1000.0f;
	;

	printf("\r\n设置通道[%d]目标电流=%fmA\r\n", channel[0], g_Channelinfo[channel[0]].Set_PreCC);
}