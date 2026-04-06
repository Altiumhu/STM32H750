#ifndef USRE_SLAVE_H_
#define USRE_SLAVE_H_
#include "head.h"

#define MaxNumChannel 16
#define SLAVENUM_MAX 16
#define DEFAULTVOLTMAX 5000
#define DEFAULTCURRENTMAX 16
#define MaxNumSamplesFifo 4 // 3 600  600---10分钟

#define MAX_SETUP_WORKE 32

//
enum WORKE_SETUP
{
	WORKE_SETUP_IDLE = 0x44, // 跳转搁置阶段 静置(D) 工步名称
	WORKE_SETUP_CC_CV = 0x41,	 /// 0x41: // 恒流恒压充电（A）
	WORKE_SETUP_DC = 43,	 // 0x43: // 恒流放电(C) 工步名称
	WORKE_SETUP_LOOP =0x52,    // 0x52: // 循环(R) 工步名称
	
	WORKE_SETUP_NUM, // 还没有收到新的信息
};

typedef enum
{
	EChannelError_OK = 0,
	EChannelError_VoltArrive,
	EChannelError_CurrArrive,
	EChannelError_TimeArrive,
	EChannelError_UserStop,
	EChannelError_UpMachineProtect,
	EChannelError_UserPause,
	EChannelError_NetError,
	EChannelError_DCZero,
	EChannelError_WorkStep = 0x0c,		  // 工步异常
	EChannelError_NoBattery = 0x12,		  // 无电池
	EChannelError_OverReserveVolt = 0x20, // 超过反向电压设定值
	EChannelError_ShutDown = 0x23,		  // 短路
	EChannelError_CCCVVERROR = 0x30,	  // CC-CV 恒压电压异常
	EChannelError_CVVolt = 0x34,
	EChannelError_MOSTEMPOVER = 0x68,
	EChannelError_VOLTNOCALIBRATE = 0x69,
	EChannelError_CCNOCALIBRATE = 0x6A,
	EChannelError_DCNOCALIBRATE = 0x6B,
	EChannelError_Can = 0x37,
	EChannelError_OVProtect = 0x73, // 下位机板过压保护
	EChannelError_UVProTect,		// 下位机板欠压保护
	EChannelError_SLAVESTARTERROR = 0x88,
} EChannelError_t;

// 升级状态
enum ESTATUSUPGRADE_T
{
	ESTATUSUPGRADE_IDLE = 0,
	ESTATUSUPGRADE_OK,
	ESTATUSUPGRADE_CHECKFAILE,
	ESTATUSUPGRADE_FAILE,
	ESTATUSUPGRADE_Down,	  // 下发完成
	ESTATUSUPGRADE_NotUpdate, // 还没有收到新的信息
};

// 下位机状态枚举
enum ESLAVESTATUS_t
{
	ESLAVESTATUS_IDLE = 0,
	ESLAVESTATUS_RECWORKSTEP,
	ESLAVESTATUS_JUMP,
	ESLAVESTATUS_PAUSE,
	ESLAVESTATUS_NUM
};
// 转发板和下位机之间通信方式如下
// 转发板主动向下位机发送数据，下位机必须返回ack
// 下位机返回的ack命令与序列号与下发的一直
// 只有一种情况下位机会主动发送数据给转发板，就是下位机断开连接的时候
// 会定时发送连接命令给转发板
// 转发板和下位机之间的通信命令
enum EMSCMD_t
{
	EMTOSCMD_Link = 0,				   // 连接下位机
	EMTOSCMD_Setpar,				   // 设置参数
	EMTOSCMD_SampleStart,			   // 开始采样
	EMTOSCMD_SampleQuest,			   // 请求采样数据
	EMTOSCMD_SendWorkStepInfo,		   // 下发工步信息
	EMTOSCMD_StartWorkStep,			   // 启动工步
	EMTOSCMD_StartSomeChannelWorkStep, // 部分通道启动工步
	EMTOSCMD_StopWorkStep,			   // 停止工步
	EMTOSCMD_PauseWorkStep,			   // 暂定工步
	EMTOSCMD_ContinueWorkStep,		   // 继续工步
	EMTOSCMD_JumpWorkStep,			   // 工步跳转
	EMTOSCMD_StopWorkStepw,			   // 部分通道停止停止工步

	EMTOSCMDIAP_UpGradeInfo, // 发送升级信息
	EMTOSCMDIAP_UpGradeData, // 发送升级数据

	EMTOSCMDCALIBRATE_ENTER,   // 进入修调程序，关闭当前的工步
	EMTOSCMDCALIBRATE_SETDA,   // 修调程序发送DA
	EMTOSCMDCALIBRATE_STOP,	   // 修调程序停止
	EMTOSCMDCALIBRATE_START,   // 修调程序启动
	EMTOSCMDCALIBRATE_SAMPLE,  // 修调程序采样
	EMTOSCMDCALIBRATE_SENDPAR, // 修调程序发送校准参数
	EMTOSCMDCALIBRATE_CHECK,   // 修调程序复检

	EMTOSCMD_LEDControl, // 设置指示灯控制模式命令
	EMTOSCMD_LEDStatus,	 // 指示灯状态命令

	EMTOSCMD_SampleQuestAck, // 发送采样数据确认

	EMTOSCMD_Num
};

#pragma pack(1)
// 工步参数数据流定义
typedef struct
{
	uint8_t index;			 // 工步索引号
	uint8_t type;			 // 工步类型
	uint8_t currentStart[4]; // 启动电流
	uint8_t voltLimit[2];	 // 限制电压
	uint8_t currentLimit[4]; // 终止电流
	uint8_t timeLimit[4];	 // 终止时间
} WorkStepInfoStream_t;

typedef struct
{

	uint8_t chnum[32];		// 通道数量
	uint8_t runWorke_setup; // 运行工步号
	uint8_t Run_Cyc_indx;	// 01 ----运行循环号
} StartWorkeChanne;

typedef struct
{

	uint8_t adc_c[4];
	uint8_t adc_v[2];
	uint8_t adc_temp[2];

	uint8_t step;
	uint8_t status;
	uint8_t error;
	uint8_t loopSn;

} SampleData_t;

#pragma pack()

extern WorkStepInfoStream_t g_WorkStepInfoStream[BOARD_CHANNEL_NUM][MAX_SETUP_WORKE]; // 工步信息最大32个
extern StartWorkeChanne g_SetChanneWorke;											  // 设置启动通道
int UserSlave_Init(void);

void UserSlave_Update(void);

void UserSlave_SendLink(void);

void UserSlave_Update(void);

extern uint8_t GetTotal_steps(void); // 获取总工步数

#endif
