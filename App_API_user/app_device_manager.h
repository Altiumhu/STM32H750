#ifndef APPAPI_DEVICE_MANAGER_H_
#define APPAPI_DEVICE_MANAGER_H_

#include "head.h"
#define  FW_REV   18    //固件版本号表示1.8


#define  BOARD_OUT_VOLT   4.2f    //输出电压

typedef struct
{
    //boost前级设备信息
    float     Boost_VBUS;
    float     Boost_temp;
    float     Boost_Curr;
    uint32_t  Boost_State;

//LLC后级设备信息
    float      temp;
    uint16_t  RunState;
    uint16_t  workMode;
    uint16_t  Err_State;

}DEVICE_State_Information;


extern volatile DEVICE_State_Information g_DEVICE_Information;

//4G外发参数结构体
struct Modbus_data_Regs
{
    uint16_t OutPut_Volt;//输出电压
    uint16_t OutPut_Curr;//输出电流
    uint16_t OutPut_Pow;//输出功率
    uint16_t Power_Generation;//发电量
    uint16_t Equipment_Temperature;//设备温度
    uint16_t AC_Status;//市电状态
    uint16_t Error_Code;//故障信息

    uint16_t RES1;//
    uint16_t RES2;//
};
//4G外发参数共用体
union MODBUS_DATA_UNION
{
    uint16_t ModbusBase[9];
    struct Modbus_data_Regs Byte;
};
extern union MODBUS_DATA_UNION Modbus_data;


//LLC外发参数结构体
struct LLC_Modbus_data_Regs
{
    uint16_t OutPut_Volt;//输出电压
    uint16_t OutPut_Curr;//输出电流
    uint16_t Input_Vbus;//输入母线电压
    uint16_t Equipment_Temperature;//设备温度
    uint16_t V_Ref;//目标电压
    uint16_t I_Ref;//目标电流
    uint16_t Error_Code;//故障信息
    uint16_t LLC_Status;
    uint16_t RES1;//
};
//LLC发参数共用体
union LLC_MODBUS_DATA_UNION
{
    uint16_t ModbusBase[9];
    struct LLC_Modbus_data_Regs Byte;
};
extern union LLC_MODBUS_DATA_UNION LLC_Modbus_data;


// 校准类型
typedef enum Calibration
{
    Caltype_VIN_BUS = 0U,     //继电器输入电压
    Caltype_V_OUT,         //输出电压
    Caltype_Current,        // 输出电流
    Caltype_Current_12VBUS,        // 输出电流

    Caltype_SENSE_VO,        // 输出电流

} Calibration_type;

#define Ref_MAX_NUM        5U  // 最大校准数量
#define Actual_MAX_NUM     5U  // 实际需要校准数量
#define TYPE_MAX_NUM       5U  // 实际需要校准数量

typedef struct
{
    float Ref_Vaule[Ref_MAX_NUM]; //校准目标数据
    float Actual_Sample[Ref_MAX_NUM]; //实际采集数据
    float ka;        //斜率
    float kb;        //b值
    float R;         //方差
    uint16_t Actual_Num; //实际需要校准个数
    uint16_t CRC_NUM;

}Calibration_Device;

extern volatile Calibration_Device g_Calibration_Device[TYPE_MAX_NUM]; //继电器高压侧输入电压



typedef struct
{
    uint16_t dev_id; //设备ID
    float V_Ref_REC_Vaule; //保存校准目标电压
    float I_Ref_REC_Vaule; //保存校准目标电压

    float V_OUT_REC_ka; //电压校准系数ka
    float V_OUT_REC_kb; //电压校准系数ka
    float I_OUT_REC_ka; //电流校准系数ka
    float I_OUT_REC_kb; //电流校准系数kb
    uint16_t CRC_Code;//校准数据校验码
    uint16_t FW_Code;//固件升级校验码  AA--表示有新的固件需要升级 00或者0xFF表示不需要
    uint16_t FW_Rev;// 版本号
    uint8_t FW_CRC32_Code[4];//固件CRC32 存放4个字节

}RED_Device_Data;




extern volatile RED_Device_Data g_Handle_REC_Device; //存储需要保存的数据

extern void AppUser_Init_REC_Device(void);


extern void AppUser_Write_Init_REC_Device(void);

#endif





