



#include "cli_system.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "head.h"
#include "cli_cfg.h"

// 在文件开头添加编码声明
#pragma execution_character_set("utf-8") // 对于Keil MDK

// 或者确保IDE设置为UTF-8编码
// 命令列表
static const cli_command *commands[20];
static uint8_t command_count = 0;

// 输入缓冲区
static char input_buffer[CLI_MAX_CMD_LENGTH];
static uint8_t input_index = 0;

// 历史记录
static char history[CLI_HISTORY_SIZE][CLI_MAX_CMD_LENGTH];
static int8_t history_index = -1;
static int8_t current_history = -1;

// UART句柄
static UART_HandleTypeDef *cli_huart = NULL;




static char cli_output_buffer[CLI_OUTPUT_BUFFER_SIZE];
static uint16_t cli_output_index = 0;

// 内置命令
static void cmd_help(int argc, char **argv);
static void cmd_log_level(int argc, char **argv);
static void cmd_reboot(int argc, char **argv);

void cli_printf(const char *format, ...) ;

// 内置命令列表
//static const cli_command builtin_commands[] = {
////    {"help", cmd_help, "显示所有可用命令", "help [command]"},
////    {"log", cmd_log_level, "设置日志级别: global|module level", "log global DEBUG\nlog module SYS INFO"},
////    {"reboot", cmd_reboot, "重启系统", "reboot"},
////    {NULL, NULL, NULL, NULL} // 结束标记
//};
static const cli_command builtin_commands[12] = {
    // 使用指定初始化器
    {
        .name = "help",
        .handler = cmd_help,
        .help = "\r\n显示所有可用命令\r\n",
        .usage = "help [command]"
    },
    {
        .name = "log",
        .handler = cmd_log_level,
        .help = "设置日志级别: global|module level",
        .usage = "log global DEBUG\nlog module SYS INFO"
    },
    {
        .name = "reboot",
        .handler = cmd_reboot,
        .help = "重启系统",
        .usage = "reboot"
    },
    
    // 结束标记
    {0} // 使用{0}初始化所有字段为NULL/0
};
// CLI初始化
void cli_init(UART_HandleTypeDef *huart) {
    cli_huart = huart;
    input_index = 0;
    history_index = -1;
    current_history = -1;
    memset(input_buffer, 0, sizeof(input_buffer));
    memset(history, 0, sizeof(history));
    
    // 注册内置命令
    for (const cli_command *cmd = builtin_commands; cmd->name; cmd++) {
        cli_register_command(cmd);
    }
    
    // 打印欢迎信息
    const char *welcome = "\r\n*** STM32 CLI System v1.0 ***\r\nType 'help' for available commands\r\n";
//    HAL_UART_Transmit(huart, (uint8_t*)welcome, strlen(welcome), 100);
		
		    char msg[] = "Hello UART1!\r\n";
    HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
		  printf("%s",welcome);
    cli_print_prompt();
}

// 注册新命令
void cli_register_command(const cli_command *cmd) {
    if (command_count < (sizeof(commands)/sizeof(commands[0]))) {
        commands[command_count++] = cmd;
    }
}

// 打印提示符
void cli_print_prompt(void) {
    const char prompt[] = "\r\n> ";
    HAL_UART_Transmit(cli_huart, (uint8_t*)prompt, sizeof(prompt)-1, 100);
}

// 处理输入字符
void cli_process_char(char c) {
    // 回车键处理
    if (c == '\r') {
        HAL_UART_Transmit(cli_huart, (uint8_t*)"\r\n", 2, 10);
        
        if (input_index > 0) {
            // 添加到历史记录
            history_index = (history_index + 1) % CLI_HISTORY_SIZE;
            strncpy(history[history_index], input_buffer, CLI_MAX_CMD_LENGTH);
            
            // 处理命令
            cli_process_command(input_buffer);
            
            // 重置输入
            input_index = 0;
            memset(input_buffer, 0, sizeof(input_buffer));
        }
        
        cli_print_prompt();
        return;
    }
    
    // 退格键处理
    if (c == '\b' || c == 0x7F) {
        if (input_index > 0) {
            input_index--;
            input_buffer[input_index] = '\0';
            // 回显退格
            HAL_UART_Transmit(cli_huart, (uint8_t*)"\b \b", 3, 10);
        }
        return;
    }
    
    // Tab键自动补全
    if (c == '\t') {
        // 实现自动补全逻辑...
        return;
    }
    
    // 历史记录导航
    if (c == 0x1B) { // ESC序列开始
        // 处理方向键...
        return;
    }
    
    // 普通字符处理
    if (input_index < (CLI_MAX_CMD_LENGTH - 1) && c >= 32 && c <= 126) {
        input_buffer[input_index++] = c;
        input_buffer[input_index] = '\0';
        HAL_UART_Transmit(cli_huart, (uint8_t*)&c, 1, 10);
    }
}

// 处理完整命令
void cli_process_command(char *cmd) {
    char *argv[CLI_MAX_ARG_COUNT];
    int argc = 0;
    
    // 分割命令参数
    char *token = strtok(cmd, " ");
    while (token != NULL && argc < CLI_MAX_ARG_COUNT) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }
    
    if (argc == 0) return;
    
    // 查找匹配命令
    for (int i = 0; i < command_count; i++) {
        if (strcmp(argv[0], commands[i]->name) == 0) {
            commands[i]->handler(argc, argv);
            return;
        }
    }
    
    // 未找到命令
    char msg[64];
    snprintf(msg, sizeof(msg), "Command not found: %s\r\n", argv[0]);
    HAL_UART_Transmit(cli_huart, (uint8_t*)msg, strlen(msg), 100);
}

// 内置命令实现
static void cmd_help(int argc, char **argv) {
    char help_msg[128];
    
    if (argc == 1) {
        // 显示所有命令
        snprintf(help_msg, sizeof(help_msg), "Available commands (%d):\r\n", command_count);
        HAL_UART_Transmit(cli_huart, (uint8_t*)help_msg, strlen(help_msg), 100);
        
        for (int i = 0; i < command_count; i++) {
            snprintf(help_msg, sizeof(help_msg), "%-10s - %s\r\n", 
                     commands[i]->name, commands[i]->help);
            HAL_UART_Transmit(cli_huart, (uint8_t*)help_msg, strlen(help_msg), 100);
        }
    } else {
        // 显示特定命令帮助
        for (int i = 0; i < command_count; i++) {
            if (strcmp(argv[1], commands[i]->name) == 0) {
                snprintf(help_msg, sizeof(help_msg), "%s\r\nUsage: %s\r\n%s\r\n",
                         commands[i]->name, commands[i]->usage, commands[i]->help);
                HAL_UART_Transmit(cli_huart, (uint8_t*)help_msg, strlen(help_msg), 100);
                return;
            }
        }
        snprintf(help_msg, sizeof(help_msg), "No help for: %s\r\n", argv[1]);
        HAL_UART_Transmit(cli_huart, (uint8_t*)help_msg, strlen(help_msg), 100);
    }
}

static void cmd_log_level(int argc, char **argv) {
    if (argc < 3) {
        HAL_UART_Transmit(cli_huart, (uint8_t*)"Usage: log global|module level\r\n", 31, 100);
        return;
    }
    
    LogLevel level = LOG_LEVEL_COUNT;
    for (int i = 0; i < LOG_LEVEL_COUNT; i++) {
        if (strcasecmp(argv[2], level_names[i]) == 0) {
            level = (LogLevel)i;
            break;
        }
    }
    
    if (level == LOG_LEVEL_COUNT) {
        HAL_UART_Transmit(cli_huart, (uint8_t*)"Invalid log level\r\n", 19, 100);
        return;
    }
    
    if (strcasecmp(argv[1], "global") == 0) {
        log_set_global_level(level);
    } else {
        LogModule module = LOG_MODULE_COUNT;
        for (int i = 0; i < LOG_MODULE_COUNT; i++) {
            if (strcasecmp(argv[1], module_names[i]) == 0) {
                module = (LogModule)i;
                break;
            }
        }
        
        if (module == LOG_MODULE_COUNT) {
            HAL_UART_Transmit(cli_huart, (uint8_t*)"Invalid module name\r\n", 21, 100);
            return;
        }
        
        log_set_module_level(module, level);
    }
}

static void cmd_reboot(int argc, char **argv) {
    HAL_UART_Transmit(cli_huart, (uint8_t*)"Rebooting system...\r\n", 21, 100);
    HAL_NVIC_SystemReset();
}



// 获取系统时间戳（用于超时检测）
uint32_t cli_get_tick(void)
{
    return HAL_GetTick();
}



// 命令锁定功能
#if CLI_ENABLE_COMMAND_LOCK

static uint32_t last_activity_time = 0;
static uint8_t cli_locked = 0;

void cli_check_lock(void)
{
    if(CLI_ENABLE_COMMAND_LOCK && 
       (cli_get_tick() - last_activity_time > CLI_LOCK_TIMEOUT_MS))
    {
        cli_locked = 1;
        cli_printf("\r\nCLI locked due to inactivity. Enter password to unlock.\r\n");
    }
}

int unlock_cmd(int argc, char **argv)
{
    if(argc < 2) return CLI_ERR_PARAM;
    
    if(strcmp(argv[1], "secure123") == 0) {
        cli_locked = 0;
        last_activity_time = cli_get_tick();
        cli_printf("CLI unlocked\r\n");
        return CLI_OK;
    }
    
    cli_printf("Invalid password\r\n");
    return CLI_ERR_AUTH;
}
#endif






void cli_flush(void) {
    if (cli_output_index > 0) {
        HAL_UART_Transmit_DMA(cli_huart, (uint8_t*)cli_output_buffer, cli_output_index);
        cli_output_index = 0;
    }
}

void cli_printf(const char *format, ...) {
    if (!cli_huart) return;
    
    va_list args;
    va_start(args, format);
    
    // 计算可用空间
    int available = CLI_OUTPUT_BUFFER_SIZE - cli_output_index - 3; // 保留空间给\r\n\0
    
    if (available > 10) {  // 确保有足够空间
        // 格式化到缓冲区
        int len = vsnprintf(&cli_output_buffer[cli_output_index], available, format, args);
        
        if (len > 0) {
            cli_output_index += len;
            
            // 检查是否需要添加换行
            if (len >= 2 && 
                cli_output_buffer[cli_output_index-2] != '\r' && 
                cli_output_buffer[cli_output_index-1] != '\n') 
            {
                if (cli_output_index < CLI_OUTPUT_BUFFER_SIZE - 2) {
                    cli_output_buffer[cli_output_index++] = '\r';
                    cli_output_buffer[cli_output_index++] = '\n';
                }
            }
            
            // 检查缓冲区是否接近满
            if (cli_output_index > CLI_OUTPUT_BUFFER_SIZE - 50) {
                cli_flush();
            }
        }
    }
    
    va_end(args);
}


