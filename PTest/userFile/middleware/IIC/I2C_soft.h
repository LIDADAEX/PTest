#pragma once

#include "stm32f4xx_hal.h"

// 假设引脚配置为：开漏输出 (Open-Drain), 无上拉/上拉可选, 高速模式
#define IIC_SCL_HIGH()  GPIOA->BSRR = GPIO_PIN_8
#define IIC_SCL_LOW()   GPIOA->BSRR = (uint32_t)GPIO_PIN_8 << 16
#define IIC_SDA_HIGH()  GPIOA->BSRR = GPIO_PIN_9
#define IIC_SDA_LOW()   GPIOA->BSRR = (uint32_t)GPIO_PIN_9 << 16
#define READ_SDA()      ((GPIOA->IDR & GPIO_PIN_9) != 0)

// 标准模式 100kHz 延迟 5us；快速模式 400kHz 延迟 2us
#define IIC_DELAY_TIME  2

void IIC_Init();
void IIC_Delay_us(uint32_t us);
void IIC_Start(void);
void IIC_Stop(void);
uint8_t IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);
void IIC_Send_Byte(uint8_t txd);
uint8_t IIC_Read_Byte(uint8_t ack);