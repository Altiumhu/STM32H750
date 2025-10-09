#ifndef LOG_SYSTEM_H_
#define LOG_SYSTEM_H_

#include "head.h"

// 日志级别定义
typedef enum {
    LOG_TRACE = 0,  // 详细跟踪信息
    LOG_DEBUG,      // 调试信息
    LOG_INFO,       // 常规信息
    LOG_WARNING,    // 警告信息
    LOG_ERROR,      // 错误信息
    LOG_CRITICAL,   // 严重错误
    LOG_LEVEL_COUNT // 级别总数
} LogLevel;

// 日志模块定义
typedef enum {
    LOG_MODULE_MAIN = 0,
    LOG_MODULE_NETWORK,
    LOG_MODULE_FS,
    LOG_MODULE_PERIPH,
    LOG_MODULE_SYSTEM,
    LOG_MODULE_COUNT
} LogModule;

// 日志输出通道
typedef enum {
    LOG_OUTPUT_DISABLED = 0,
    LOG_OUTPUT_UART,
    LOG_OUTPUT_SWO,
    LOG_OUTPUT_RTT
} LogOutput;



extern const char *module_names[LOG_MODULE_COUNT] ;
extern const char *level_names[LOG_LEVEL_COUNT] ;
// 初始化日志系统
void log_init(UART_HandleTypeDef *huart, LogOutput output);

// 设置全局日志级别
void log_set_global_level(LogLevel level);

// 设置模块日志级别
void log_set_module_level(LogModule module, LogLevel level);

// 日志输出函数
void log_message(LogLevel level, LogModule module, const char *file, int line, const char *fmt, ...);

// 日志宏定义
#define LOG_TRACE(module, ...)  log_message(LOG_TRACE, module, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(module, ...)  log_message(LOG_DEBUG, module, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(module, ...)   log_message(LOG_INFO, module, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(module, ...)   log_message(LOG_WARNING, module, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(module, ...)  log_message(LOG_ERROR, module, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_CRIT(module, ...)   log_message(LOG_CRITICAL, module, __FILE__, __LINE__, __VA_ARGS__)



void User_CLI_Cmd(void);

#endif


