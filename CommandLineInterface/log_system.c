#include "log_system.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "head.h"

// 模块名称映射
 const char *module_names[LOG_MODULE_COUNT] = {
    "MAIN", "NET", "FS", "PERIPH", "SYS"
};

// 日志级别名称映射
 const char *level_names[LOG_LEVEL_COUNT] = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "CRIT"
};

// 日志配置
static struct {
    LogLevel global_level;
    LogLevel module_levels[LOG_MODULE_COUNT];
    LogOutput output;
    UART_HandleTypeDef *huart;
    uint8_t initialized;
} log_config;

// 日志缓冲区
#define LOG_BUFFER_SIZE 256
static char log_buffer[LOG_BUFFER_SIZE];

void log_init(UART_HandleTypeDef *huart, LogOutput output) {
    // 默认配置
    log_config.global_level = LOG_INFO;
    log_config.output = output;
    log_config.huart = huart;
    log_config.initialized = 1;
    
    // 设置所有模块默认级别
    for (int i = 0; i < LOG_MODULE_COUNT; i++) {
        log_config.module_levels[i] = log_config.global_level;
    }
    
    LOG_INFO(LOG_MODULE_SYSTEM, "Log system initialized. Output: %d", output);
}

void log_set_global_level(LogLevel level) {
    if (level < LOG_LEVEL_COUNT) {
        log_config.global_level = level;
        LOG_INFO(LOG_MODULE_SYSTEM, "Global log level set to %s", level_names[level]);
    }
}

void log_set_module_level(LogModule module, LogLevel level) {
    if (module < LOG_MODULE_COUNT && level < LOG_LEVEL_COUNT) {
        log_config.module_levels[module] = level;
        LOG_INFO(LOG_MODULE_SYSTEM, "Module %s log level set to %s", 
                module_names[module], level_names[level]);
    }
}

void log_message(LogLevel level, LogModule module, const char *file, int line, const char *fmt, ...) {
    if (!log_config.initialized || level < LOG_TRACE || level >= LOG_LEVEL_COUNT || 
        module < 0 || module >= LOG_MODULE_COUNT) {
        return;
    }
    
    // 检查日志级别过滤
    if (level < log_config.global_level || level < log_config.module_levels[module]) {
        return;
    }
    
    // 获取当前时间戳
    uint32_t timestamp = HAL_GetTick();
    
    // 提取文件名（不含路径）
    const char *filename = strrchr(file, '/');
    if (!filename) filename = strrchr(file, '\\');
    filename = filename ? filename + 1 : file;
    
    // 格式化日志头
    int header_len = snprintf(log_buffer, LOG_BUFFER_SIZE, 
        "[%6d][%s][%s] ", timestamp, module_names[module], level_names[level]);
    
    // 格式化日志内容
    va_list args;
    va_start(args, fmt);
    int content_len = vsnprintf(log_buffer + header_len, LOG_BUFFER_SIZE - header_len, fmt, args);
    va_end(args);
    
    // 添加换行符
    int total_len = header_len + content_len;
    if (total_len < LOG_BUFFER_SIZE - 2) {
        log_buffer[total_len] = '\r';
        log_buffer[total_len + 1] = '\n';
        log_buffer[total_len + 2] = '\0';
        total_len += 2;
    }
    
    // 输出日志
    switch (log_config.output) {
        case LOG_OUTPUT_UART:
            if (log_config.huart) {
                HAL_UART_Transmit_DMA(log_config.huart, (uint8_t*)log_buffer, total_len);
            }
            break;
        case LOG_OUTPUT_SWO:
            for (int i = 0; i < total_len; i++) {
                ITM_SendChar(log_buffer[i]);
            }
            break;
        case LOG_OUTPUT_RTT:
            // SEGGER_RTT_Write(0, log_buffer, total_len);
            break;
        default:
            break;
    }
}



void User_CLI_Cmd(void)
{


    // 注册用户自定义命令
    const cli_command my_cmd =
    {
        "led",
        cmd_led_control,
        "Control LEDs: on|off|toggle [id]",
        "led on 1 led toggle 2"
    };
    cli_register_command(&my_cmd);

    LOG_INFO(LOG_MODULE_MAIN, "System started");
    cli_printf("Invalid password\r\n");


}

