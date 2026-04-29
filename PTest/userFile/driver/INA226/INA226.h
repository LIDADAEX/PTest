#pragma once

#include "middleware/IIC/I2C_Soft.h"

/* 寄存器地址 */
#define REG_CONFIG       0x00  // 配置寄存器
#define REG_SHUNTV       0x01  // 分流电压 (Shunt Voltage)
#define REG_BUSV         0x02  // 总线电压 (Bus Voltage)
#define REG_POWER        0x03  // 功率 (Power)
#define REG_CURRENT      0x04  // 电流 (Current)
#define REG_CALIB        0x05  // 校准寄存器 (Calibration)
#define REG_MASK         0x06  // 屏蔽/使能
#define REG_ALERT        0x07  // 报警限制

/* 配置参数：16次平均, 1.1ms转换时间, 连续模式 */
#define INA226_CFG_VAL   0x4527

struct INA226_Data {
    float bus_voltage_v;    // 总线电压 (V)
    float current_ma;       // 电流 (mA)
    float power_mw;         // 功率 (mW)
    float shunt_voltage_mv; // 分流压降 (mV)
    bool  is_data_ready;    // 数据是否更新
};

class INA226 {
public:
    // 构造函数：传入地址和采样电阻信息
    INA226(uint8_t addr = 0x40) : _device_addr(addr) {}

    void Init(float r_shunt, float target_lsb_ma);

    void TickHandler();

    INA226_Data GetData();
		
private:
    uint8_t  _device_addr;
    uint16_t _cal_value;
    float    _current_lsb; // mA per bit
    INA226_Data _data;

    void Write_Reg(uint8_t reg, uint16_t data);
    uint16_t Read_Reg(uint8_t reg);

};