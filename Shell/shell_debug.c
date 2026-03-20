/**
 * @file shell_debug.c
 * @brief 嵌入式Shell调试交互程序
 * @author  胡伟平
 * @date 2026-03-19
 */

#include "main.h"
#include "usart.h"
#include "gpio.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "head.h"
/* ============================================
 * Shell配置
 * ============================================ */
#define SHELL_MAX_CMD_LEN     64      // 最大命令长度
#define SHELL_MAX_ARGS        8       // 最大参数数量
#define SHELL_MAX_HISTORY     10      // 命令历史记录数

/* ============================================
 * 命令结构体
 * ============================================ */
typedef struct {
    const char *name;                // 命令名称
    const char *help;                 // 帮助信息
    void (*func)(int argc, char *argv[]);  // 命令处理函数
} ShellCommand_t;

/* ============================================
 * 全局变量
 * ============================================ */
static char rx_buffer[SHELL_MAX_CMD_LEN];
static uint8_t rx_index = 0;
static char history[SHELL_MAX_HISTORY][SHELL_MAX_CMD_LEN];
static uint8_t history_count = 0;
static uint8_t history_index = 0;

// 转义序列处理
static char esc_buffer[8];
static uint8_t esc_count = 0;
static uint8_t in_escape = 0;

/* ============================================
 * 命令处理函数声明
 * ============================================ */
static void cmd_help(int argc, char *argv[]);
static void cmd_led(int argc, char *argv[]);
static void cmd_gpio(int argc, char *argv[]);
static void cmd_mem(int argc, char *argv[]);
static void cmd_reset(int argc, char *argv[]);
static void cmd_version(int argc, char *argv[]);
static void cmd_clear(int argc, char *argv[]);
static void cmd_info(int argc, char *argv[]);
static void cmd_adc(int argc, char *argv[]);
static void cmd_task(int argc, char *argv[]);

extern void shell_Debug_PWM(int argc, char *argv[]);

static void cmd_myfunc(int argc, char *argv[]);


extern void debug_show_en(int argc, char *argv[]);
/* ============================================
 * 命令表
 * ============================================ */
static const ShellCommand_t commands[] = {
    {"help",   "显示帮助信息",          cmd_help},
    {"led",    "LED控制: led on|off",   cmd_led},
    {"gpio",   "GPIO操作: gpio read/write pin value", cmd_gpio},
    {"mem",    "内存操作: mem read addr", cmd_mem},
    {"reboot",  "系统复位",              cmd_reset},
    {"version","显示版本信息",          cmd_version},
    {"clear",  "清屏",                 cmd_clear},
    {"info",   "显示系统信息",          cmd_info},
    {"adc",    "ADC读取",               cmd_adc},
    {"task",   "Show Task Status",    cmd_task},

    {"pwm",   "Debug PWM",        shell_Debug_PWM},
    {"debug",   "debug_show_en",        debug_show_en},
		
		{"vset",   "VRef_Set",        shell_Set_VRef},
	  {"iset",   "IRef_Set",        shell_Set_IRef},
    {"mycmd",  "My Command",      cmd_myfunc},
    {NULL, NULL, NULL}  // 结束标志
};

/* ============================================
 * 串口发送函数
 * ============================================ */
void Shell_SendChar(char c)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&c, 1, 100);
}

void Shell_SendString(const char *str)
{
    while (*str) {
        Shell_SendChar(*str++);
    }
}

void Shell_SendLine(const char *str)
{
    Shell_SendString(str);
    Shell_SendString("\r\n");
}

/* ============================================
 * 打印提示符
 * ============================================ */
void Shell_PrintPrompt(void)
{
    Shell_SendString("\r\nroot>> ");
}

/* ============================================
 * 命令解析
 * ============================================ */
static void parse_command(char *line)
{
    char *argv[SHELL_MAX_ARGS];
    int argc = 0;
    char *token;
    
    // 去除换行符
    char *p = strchr(line, '\r');
    if (p) *p = '\0';
    p = strchr(line, '\n');
    if (p) *p = '\0';
    
    // 空命令
    if (strlen(line) == 0) {
        return;
    }
    
    // 保存到历史记录
    if (history_count < SHELL_MAX_HISTORY) {
        strcpy(history[history_count++], line);
    }
    history_index = 0;
    
    // 解析命令和参数
    token = strtok(line, " ");
    while (token != NULL && argc < SHELL_MAX_ARGS) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }
    
    // 查找并执行命令
    for (int i = 0; commands[i].name != NULL; i++) {
        if (strcmp(argv[0], commands[i].name) == 0) {
            commands[i].func(argc, argv);
            return;
        }
    }
    
    Shell_SendString("Error: Unknown command '");
    Shell_SendString(argv[0]);
    Shell_SendString("'\r\n");
    Shell_SendString("Type 'help' for available commands.\r\n");
}

/* ============================================
 * 命令处理函数实现
 * ============================================ */
static void cmd_help(int argc, char *argv[])
{
    Shell_SendString("\r\n=== Available Commands ===\r\n");
    
    for (int i = 0; commands[i].name != NULL; i++) {
        Shell_SendString(commands[i].name);
        Shell_SendString(" - ");
        Shell_SendString(commands[i].help);
        Shell_SendString("\r\n");
    }
    
    Shell_SendString("\r\nUsage:\r\n");
    Shell_SendString("  led on      - Turn LED on\r\n");
    Shell_SendString("  led off     - Turn LED off\r\n");
    Shell_SendString("  gpio read 5 - Read GPIO Pin 5\r\n");
    Shell_SendString("  mem read 0x20000000 - Read memory\r\n");
}

static void cmd_led(int argc, char *argv[])
{
    if (argc < 2) {
        Shell_SendString("Usage: led on|off\r\n");
        return;
    }
    
    if (strcmp(argv[1], "on") == 0) {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
        Shell_SendString("LED is ON\r\n");
    } else if (strcmp(argv[1], "off") == 0) {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);
        Shell_SendString("LED is OFF\r\n");
    } else {
        Shell_SendString("Usage: led on|off\r\n");
    }
}

static void cmd_gpio(int argc, char *argv[])
{
    if (argc < 4) {
        Shell_SendString("Usage: gpio read/write pin value\r\n");
        Shell_SendString("Example: gpio read 5\r\n");
        Shell_SendString("         gpio write 5 1\r\n");
        return;
    }
    
    if (strcmp(argv[1], "read") == 0) {
        uint16_t pin = atoi(argv[2]);
        GPIO_PinState state = HAL_GPIO_ReadPin(GPIOE, pin);
        Shell_SendString("GPIO Pin ");
        Shell_SendString(argv[2]);
        Shell_SendString(" = ");
        Shell_SendString(state == GPIO_PIN_SET ? "HIGH" : "LOW");
        Shell_SendString("\r\n");
    } else if (strcmp(argv[1], "write") == 0) {
        uint16_t pin = atoi(argv[2]);
        uint8_t value = atoi(argv[3]);
        HAL_GPIO_WritePin(GPIOE, pin, value ? GPIO_PIN_SET : GPIO_PIN_RESET);
        Shell_SendString("GPIO Pin ");
        Shell_SendString(argv[2]);
        Shell_SendString(" set to ");
        Shell_SendString(value ? "HIGH" : "LOW");
        Shell_SendString("\r\n");
    }
}

static void cmd_mem(int argc, char *argv[])
{
    if (argc < 3) {
        Shell_SendString("Usage: mem read address\r\n");
        Shell_SendString("Example: mem read 0x20000000\r\n");
        return;
    }
    
    if (strcmp(argv[1], "read") == 0) {
        uint32_t addr = strtol(argv[2], NULL, 16);
        uint32_t value = *((volatile uint32_t *)addr);
        
        Shell_SendString("Address: ");
        Shell_SendString(argv[2]);
        Shell_SendString("\r\nValue: 0x");
        
        char buf[16];
        sprintf(buf, "%08X", value);
        Shell_SendString(buf);
        Shell_SendString("\r\n");
    }
}

static void cmd_reset(int argc, char *argv[])
{
    Shell_SendString("System will reset...\r\n");
    for (int i = 0; i < 10000000; i++);  // 延时
    NVIC_SystemReset();
}

static void cmd_version(int argc, char *argv[])
{
    Shell_SendString("\r\n");
    Shell_SendString("=================================\r\n");
    Shell_SendString("  Embedded Shell Debugger\r\n");
    Shell_SendString("  Version: 1.0.0\r\n");
    Shell_SendString("  Build Date: 2026-03-19\r\n");
    Shell_SendString("  Platform: STM32H750\r\n");
    Shell_SendString("=================================\r\n");
}

static void cmd_clear(int argc, char *argv[])
{
    // 发送清屏序列
    Shell_SendString("\033[2J\033[H");  // ANSI清屏
    Shell_SendString("\r\n");
}

static void cmd_info(int argc, char *argv[])
{
    Shell_SendString("\r\n=== System Information ===\r\n");
    
    char buf[64];
    
    // 系统时钟
    uint32_t hclk = HAL_RCC_GetSysClockFreq();
    sprintf(buf, "HCLK: %lu Hz\r\n", hclk);
    Shell_SendString(buf);
    
    // Core频率
    uint32_t sysclk = HAL_RCC_GetSysClockFreq();
    sprintf(buf, "SYSCLK: %lu Hz\r\n", sysclk);
    Shell_SendString(buf);
    
    // Flash大小 (从IDCODE获取)
    Shell_SendString("Flash: 128KB (Simulated)\r\n");
    Shell_SendString("RAM: 64KB\r\n");
    
    // 芯片信息
    Shell_SendString("Chip: STM32H750VBT6\r\n");
    
    // 编译时间
    Shell_SendString("Build: "__DATE__" "__TIME__"\r\n");
}

static void cmd_myfunc(int argc, char *argv[])
{
   // 参数个数（不含命令本身）
    int num_params = argc - 1;
    
    printf("\r\n 参数个数=%d", num_params);  // 会打印 1
    printf("\r\n 第一个参数=%s", argv[1]);   // 会打印 0
    
    Shell_SendString("这是我的自定义命令！\r\n");
}

/* ============================================
 * ADC读取命令
 * ============================================ */
extern uint16_t adc1Result[16];  // ADC结果数组

static void cmd_adc(int argc, char *argv[])
{
    char buf[64];
    
    Shell_SendString("\r\n=== ADC Values ===\r\n");
    Shell_SendString("CH\tRaw\t\tVoltage(mV)\r\n");
    Shell_SendString("-----------------------------\r\n");
    
    for (int i = 0; i < 16; i++) {
        if (adc1Result[i] > 0) {
            // 假设参考电压3.3V, 12位ADC
            uint32_t voltage = (adc1Result[i] * 3300) / 65535;
            
            sprintf(buf, "%d\t%d\t\t%lu mV\r\n", i, adc1Result[i], voltage);
            Shell_SendString(buf);
        }
    }
    
    Shell_SendString("=============================\r\n");
}

/* ============================================
 * 任务状态命令 (模拟)
 * ============================================ */
static void cmd_task(int argc, char *argv[])
{
    char buf[64];
    
    Shell_SendString("\r\n=== Task Status ===\r\n");
    Shell_SendString("Name\t\tState\tPriority\r\n");
    Shell_SendString("-----------------------------\r\n");
    
    // 模拟任务信息 - 实际项目可能使用FreeRTOS
    Shell_SendString("MainTask\tRunning\t3\r\n");
    Shell_SendString("UART_Rx\tReady\t2\r\n");
    Shell_SendString("ADC_Poll\tBlocked\t4\r\n");
    Shell_SendString("CAN_Tx\tReady\t2\r\n");
    Shell_SendString("Shell\tReady\t1\r\n");
    
    Shell_SendString("=============================\r\n");
    
    // 显示系统运行时间
    uint32_t tick = HAL_GetTick();
    uint32_t seconds = tick / 1000;
    uint32_t minutes = seconds / 60;
    uint32_t hours = minutes / 60;
    
    Shell_SendString("\r\nSystem Uptime: ");
    sprintf(buf, "%luh %lum %lus\r\n", hours, minutes % 60, seconds % 60);
    Shell_SendString(buf);
}

/* ============================================
 * 显示历史命令
 * ============================================ */
static void show_history(uint8_t direction)
{
    if (history_count == 0) {
        return;
    }
    
    if (direction == 1)  // 上键 - 上一条
    {
        if (history_index < history_count) {
            history_index++;
        }
    }
    else  // 下键 - 下一条
    {
        if (history_index > 0) {
            history_index--;
        }
    }
    
    // 清除当前行
    while (rx_index > 0) {
        Shell_SendString("\b \b");
        rx_index--;
    }
    
    // 显示历史命令
    uint8_t idx = history_count - history_index;
    if (idx > 0 && idx <= history_count) {
        strcpy(rx_buffer, history[idx - 1]);
        rx_index = strlen(rx_buffer);
        Shell_SendString(rx_buffer);
    }
}

/* ============================================
 * 字符处理
 * ============================================ */
void Shell_ProcessChar(char c)
{
    // 处理转义序列 (上下键)
    if (in_escape) {
        esc_buffer[esc_count++] = c;
        
        // 检查是否完成转义序列
        if (esc_count >= 2) {
            if (esc_buffer[0] == '[') {
                if (esc_count == 2) {
                    // 可能是方向键: [A (上), [B (下), [C (右), [D (左)
                    if (c >= 'A' && c <= 'D') {
                        // 完成转义序列
                        in_escape = 0;
                        esc_count = 0;
                        
                        if (c == 'A') {  // 上键
                            show_history(1);
                            return;
                        }
                        else if (c == 'B') {  // 下键
                            show_history(0);
                            return;
                        }
                    }
                }
            }
            // 无效转义序列，重置
            in_escape = 0;
            esc_count = 0;
        }
        return;
    }
    
    // 检测转义序列开始 (ESC = 0x1B)
    if (c == 0x1B) {
        in_escape = 1;
        esc_count = 0;
        return;
    }
    
    // 处理换行
    if (c == '\r' || c == '\n') {
        Shell_SendString("\r\n");
        rx_buffer[rx_index] = '\0';
        
        if (rx_index > 0) {
            // 回显命令
            Shell_SendString(rx_buffer);
            parse_command(rx_buffer);
        }
        
        rx_index = 0;
        history_index = 0;  // 重置历史索引
        Shell_PrintPrompt();
        return;
    }
    
    // 处理退格键
    if (c == '\b' || c == 0x7F) {
        if (rx_index > 0) {
            rx_index--;
            Shell_SendString("\b \b");  // 退格+空格+退格
        }
        return;
    }
    
    // 处理普通字符
    if (rx_index < SHELL_MAX_CMD_LEN - 1) {
        rx_buffer[rx_index++] = c;
        Shell_SendChar(c);  // 回显
    }
}

/* ============================================
 * 初始化
 * ============================================ */
void Shell_Init(void)
{
    rx_index = 0;
    history_count = 0;
    history_index = 0;
    in_escape = 0;
    esc_count = 0;
    
    Shell_SendString("\r\n");
    Shell_SendString("=================================\r\n");
    Shell_SendString("  Embedded Shell Debugger\r\n");
    Shell_SendString("  Version: 1.0.0\r\n");
    Shell_SendString("  Type 'help' for commands\r\n");
    Shell_SendString("=================================\r\n");
    Shell_PrintPrompt();
}

/* ============================================
 * 定时任务（需要在主循环调用）
 * ============================================ */
void Shell_Task(void)
{
    // 检查是否有数据需要处理
    // 此函数应在串口接收中断中调用
}

/**
 * @brief 串口接收回调函数中调用此函数
 */
void Shell_UART_RxHandler(uint8_t data)
{
    Shell_ProcessChar((char)data);
}
