
#ifndef CLI_SYSTEM_H_
#define CLI_SYSTEM_H_




#include "head.h"


// 在文件开头添加编码声明
#pragma execution_character_set("utf-8") // 对于Keil MDK

// 或者确保IDE设置为UTF-8编码

#define CLI_MAX_CMD_LENGTH    128
#define CLI_MAX_ARG_COUNT     8
#define CLI_HISTORY_SIZE      5


#define CLI_ERR_PARAM -1
#define CLI_OK 0
#define CLI_ERR_AUTH 0


// 命令处理函数类型
typedef void (*cli_cmd_handler)(int argc, char **argv);

// 命令结构体
typedef struct {
    const char *name;         // 命令名称
    cli_cmd_handler handler;  // 处理函数
    const char *help;         // 帮助信息
    const char *usage;        // 用法示例
} cli_command;

// CLI初始化
void cli_init(UART_HandleTypeDef *huart);

// 处理输入字符
void cli_process_char(char c);

// 注册新命令
void cli_register_command(const cli_command *cmd);

// 打印CLI提示符
void cli_print_prompt(void);


void cli_process_command(char *cmd);

extern void cli_printf(const char *format, ...) ;
#endif


