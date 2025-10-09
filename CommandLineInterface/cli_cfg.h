#ifndef CLI_CFG_H_
#define CLI_CFG_H_


// 启用高性能特性
#define CLI_ENABLE_HIGH_PERFORMANCE  1

// 缓冲区配置（利用H7大RAM）
#define CLI_INPUT_BUFFER_SIZE        256
#define CLI_OUTPUT_BUFFER_SIZE       512
#define CLI_HISTORY_BUFFER_SIZE      5
#define CLI_MAX_COMMANDS             32
#define CLI_MAX_ARGS                 8

// 启用高级功能
#define CLI_ENABLE_FLOATING_POINT    1  // 利用H7硬件FPU
#define CLI_ENABLE_COLOR_OUTPUT      1
#define CLI_ENABLE_AUTO_COMPLETE     1
#define CLI_ENABLE_HISTORY           1
#define CLI_ENABLE_TIMESTAMP         1

// 特殊功能（安全相关）
#define CLI_ENABLE_COMMAND_LOCK      1
#define CLI_LOCK_TIMEOUT_MS          (5 * 60 * 1000)  // 5分钟无操作锁定

// 性能优化
#define CLI_USE_DMA_FOR_OUTPUT       1
#define CLI_USE_CACHE_OPTIMIZATION   1

// 硬件相关宏
#define CLI_GET_TICK()              HAL_GetTick()
#define CLI_ASSERT(expr)            do { if(!(expr)) while(1); } while(0)



#define CLI_ENABLE_COMMAND_LOCK  1


// 扩展支持RTT和以太网CLI

#define CLI_ENABLE_RTT_CHANNEL     0
#define CLI_ENABLE_ETHERNET        0



#endif


