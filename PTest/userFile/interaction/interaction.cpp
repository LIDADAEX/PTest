#include "interaction.h"

#include <stdio.h>

#include "driver/INA226/INA226.h"
#include "middleware/IIC/I2C_Soft.h"
#include "middleware/USART/USART_Hard.h"

INA226 _INA226;

bool flag_1ms = false;
bool flag_100ms = false;

bool readFlag(bool& flag) {
    bool flagTemp = flag;
    flag = false;
    return flagTemp;
}

void HAL_SYSTICK_Callback(void) {
	static uint16_t cnt = 0;
	if(cnt < 100)
		cnt ++;
	else{
		cnt = 0;
		flag_100ms = true;
	}
    flag_1ms = true;
	
}

void USART2_CallBack(uint8_t* Buffer, uint16_t Length) {}

extern "C" {

void ITR_Init(void) {
    IIC_Init();

    UART_Init(&huart2, USART2_CallBack, 64);

    _INA226.Init(0.001, 1);
}

void ITR_Loop(void) {
	static INA226_Data data;
    if (readFlag(flag_1ms)) {
        _INA226.TickHandler();
        data = _INA226.GetData();
    }
	if(flag_100ms){
		printf("\rV: %.2fV | I: %.1fmA | P: %.1fmW    ", data.bus_voltage_v, data.current_ma, data.power_mw);
	}
}
}
