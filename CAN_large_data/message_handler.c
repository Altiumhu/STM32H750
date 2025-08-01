#include "message_handler.h"
#include "fdcan.h"
#include "system_can_config.h"

void ProcessCANMessage(uint8_t fc, uint16_t did, uint16_t sub, uint8_t* data, uint8_t size) {
  switch(fc) {
    case FC_SYSTEM_CONTROL:
      //HandleSystemControl(did, sub, data, size);
      break;
      
    case FC_DATA_TRANSFER:
      //HandleDataTransfer(did, sub, data, size);
      break;
      
    case FC_CONFIGURATION:
     // HandleConfiguration(did, sub, data, size);
      break;
      
    case FC_DIAGNOSTICS:
     // HandleDiagnostics(did, sub, data, size);
      break;
      
    case FC_BROADCAST:
     // HandleBroadcast(did, sub, data, size);
      break;
      
    default:
      // 未知功能码处理
      break;
  }
}

void HandleSystemControl(uint16_t did, uint16_t sub, uint8_t* data, uint8_t size) {
  // 示例：处理系统控制消息
  if (sub == 0x001) { // 系统重启命令
    if (data[0] == 0xAA) {
      NVIC_SystemReset();
    }
  }
}

void HandleDataTransfer(uint16_t did, uint16_t sub, uint8_t* data, uint8_t size) {
  // 示例：处理数据传输
  // 这里可以保存数据到缓冲区或处理数据包
}

void HandleConfiguration(uint16_t did, uint16_t sub, uint8_t* data, uint8_t size) {
  // 示例：处理配置消息
  if (sub == 0x100) { // 设置设备ID
    // 在实际应用中需要谨慎处理设备ID更改
  }
}

void HandleDiagnostics(uint16_t did, uint16_t sub, uint8_t* data, uint8_t size) {
  // 示例：处理诊断请求
  if (sub == 0x000) { // 状态请求
    uint8_t response[8] = {0};
    // 填充状态信息...
    FDCAN_SendMessage(FC_DIAGNOSTICS, did, 0x001, response, sizeof(response));
  }
}

void HandleBroadcast(uint16_t did, uint16_t sub, uint8_t* data, uint8_t size) {
  // 示例：处理广播消息
  if (sub == 0xFFF) { // 系统时间同步
    // 更新时间...
  }
}