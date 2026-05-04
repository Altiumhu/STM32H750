#ifndef USRE_APP_DEBUG_H_
#define USRE_APP_DEBUG_H_
#include "head.h"

typedef struct
{
    uint32_t time;      //间隔时间
    uint32_t out;       //输出打印信息
    uint32_t echo;      //回显输出模式
    uint32_t poll;      //轮询时间
    uint16_t msg;       //任务处理消息
}Debug;


#define DEBUG_GET_TICK()   Timer_GetClock()  //系统运行时钟



#define DEBUG_STATR   0 //1--表示不打印  0表示打印

#define DEBUG_TIME_GAP  360    //间隔时间ms

extern void debug_init(void);
void debug_show_en(int argc, char *argv[]);
extern void debug_show_mode(uint16_t echo);
extern void debug_show(void);
extern void Debug_Timer_1ms(void);
extern void AppDebug_vTask(void);

#endif

