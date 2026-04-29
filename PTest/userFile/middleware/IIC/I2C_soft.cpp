#include "I2C_soft.h"

// 假设引脚配置为：开漏输出 (Open-Drain), 无上拉/上拉可选, 高速模式
#define IIC_SCL_HIGH()  GPIOA->BSRR = GPIO_PIN_8
#define IIC_SCL_LOW()   GPIOA->BSRR = (uint32_t)GPIO_PIN_8 << 16
#define IIC_SDA_HIGH()  GPIOA->BSRR = GPIO_PIN_9
#define IIC_SDA_LOW()   GPIOA->BSRR = (uint32_t)GPIO_PIN_9 << 16
#define READ_SDA()      ((GPIOA->IDR & GPIO_PIN_9) != 0)

// 标准模式 100kHz 延迟 5us；快速模式 400kHz 延迟 2us
#define IIC_DELAY_TIME  2

// 初始化 DWT 计数器
void IIC_Delay_Init(void) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void IIC_Init(void){
	IIC_Delay_Init();
}

// 精确微秒延迟
void IIC_Delay_us(uint32_t us) {
    uint32_t startTick = DWT->CYCCNT;
    uint32_t delayTicks = us * (SystemCoreClock / 1000000);
    while ((DWT->CYCCNT - startTick) < delayTicks);
}

void IIC_Start(void) {
    IIC_SDA_HIGH();
    IIC_SCL_HIGH();
    IIC_Delay_us(IIC_DELAY_TIME);
    IIC_SDA_LOW();
    IIC_Delay_us(IIC_DELAY_TIME);
    IIC_SCL_LOW();
    IIC_Delay_us(IIC_DELAY_TIME);
}

void IIC_Stop(void) {
    IIC_SDA_LOW();
    IIC_Delay_us(IIC_DELAY_TIME);
    IIC_SCL_HIGH();
    IIC_Delay_us(IIC_DELAY_TIME);
    IIC_SDA_HIGH();
    IIC_Delay_us(IIC_DELAY_TIME);
}

uint8_t IIC_Wait_Ack(void) {
    uint8_t retry = 0;
    IIC_SDA_HIGH(); IIC_Delay_us(IIC_DELAY_TIME);
    IIC_SCL_HIGH(); IIC_Delay_us(IIC_DELAY_TIME);
    while (READ_SDA()) {
        retry++;
        if (retry > 250) { IIC_Stop(); return 1; }
    }
    IIC_SCL_LOW();
    return 0;
}

// 主机发送应答
void IIC_Ack(void) {
    IIC_SDA_LOW(); IIC_Delay_us(IIC_DELAY_TIME);
    IIC_SCL_HIGH(); IIC_Delay_us(IIC_DELAY_TIME);
    IIC_SCL_LOW();
}

// 主机发送非应答 (一般用于接收最后一个字节后)
void IIC_NAck(void) {
    IIC_SDA_HIGH(); IIC_Delay_us(IIC_DELAY_TIME);
    IIC_SCL_HIGH(); IIC_Delay_us(IIC_DELAY_TIME);
    IIC_SCL_LOW();
}

void IIC_Send_Byte(uint8_t txd) {
    for (uint8_t i = 0; i < 8; i++) {
        if (txd & 0x80) IIC_SDA_HIGH();
        else IIC_SDA_LOW();
        txd <<= 1;
        IIC_Delay_us(IIC_DELAY_TIME);
        IIC_SCL_HIGH();
        IIC_Delay_us(IIC_DELAY_TIME);
        IIC_SCL_LOW();
        IIC_Delay_us(IIC_DELAY_TIME);
    }
}

uint8_t IIC_Read_Byte(uint8_t ack) {
    uint8_t receive = 0;
    IIC_SDA_HIGH(); // 释放总线准备读取
    for (uint8_t i = 0; i < 8; i++) {
        IIC_SCL_HIGH();
        IIC_Delay_us(IIC_DELAY_TIME);
        receive <<= 1;
        if (READ_SDA()) receive++;
        IIC_SCL_LOW();
        IIC_Delay_us(IIC_DELAY_TIME);
    }
    if (ack) IIC_Ack();
    else IIC_NAck();
    return receive;
}