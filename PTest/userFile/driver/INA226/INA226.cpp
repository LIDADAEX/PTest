#include "INA226.h"

void INA226::Write_Reg(uint8_t reg, uint16_t data) {
    IIC_Start();
    IIC_Send_Byte(_device_addr << 1);  
    IIC_Wait_Ack();
    IIC_Send_Byte(reg);
    IIC_Wait_Ack();
    IIC_Send_Byte((uint8_t)(data >> 8));
    IIC_Wait_Ack();
    IIC_Send_Byte((uint8_t)(data & 0xFF));
    IIC_Wait_Ack();
    IIC_Stop();
}

uint16_t INA226::Read_Reg(uint8_t reg) {
    uint16_t data = 0;
    IIC_Start();
    IIC_Send_Byte(_device_addr << 1);
    IIC_Wait_Ack();
    IIC_Send_Byte(reg);
    IIC_Wait_Ack();

    IIC_Start(); 
    IIC_Send_Byte((_device_addr << 1) | 0x01);
    IIC_Wait_Ack();

    data = IIC_Read_Byte(1); 
    data <<= 8;
    data |= IIC_Read_Byte(0); 
    IIC_Stop();

    return data;
}

void INA226::Init(float r_shunt, float target_lsb_ma) {
    _current_lsb = target_lsb_ma;
    _cal_value = (uint16_t)(0.00512f / ((_current_lsb / 1000.0f) * r_shunt));

    Write_Reg(REG_CONFIG, INA226_CFG_VAL);  
    Write_Reg(REG_CALIB, _cal_value);
}

void INA226::TickHandler() {

    uint16_t bv_raw = Read_Reg(REG_BUSV);
    _data.bus_voltage_v = bv_raw * 0.00125f;

    int16_t cur_raw = (int16_t)Read_Reg(REG_CURRENT);
    _data.current_ma = cur_raw * _current_lsb;

    uint16_t pwr_raw = Read_Reg(REG_POWER);
    _data.power_mw = pwr_raw * (_current_lsb * 25.0f);

    int16_t sv_raw = (int16_t)Read_Reg(REG_SHUNTV);
    _data.shunt_voltage_mv = sv_raw * 0.0025f;

    _data.is_data_ready = true;
}

INA226_Data INA226::GetData() {
    _data.is_data_ready = false; 
    return _data;
}