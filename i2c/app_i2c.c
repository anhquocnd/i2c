#include "stm32f10x.h"

// Các hàm delay t? chuong trình g?c
void Delay1Ms(void);
void Delay_Ms(uint32_t u32DelayInMs);
void delay_us(uint32_t delay);

void Delay1Ms(void)
{
    TIM_SetCounter(TIM2, 0);
    while (TIM_GetCounter(TIM2) < 1000) {
    }
}

void delay_us(uint32_t delay)
{
    TIM_SetCounter(TIM2, 0);
    while (TIM_GetCounter(TIM2) < delay) {
    }
}

void Delay_Ms(uint32_t u32DelayInMs)
{
    while (u32DelayInMs) {
        Delay1Ms();
        --u32DelayInMs;
    }
}

// Các macro và hàm I2C t? chuong trình g?c
#define SDA_0 GPIO_ResetBits(GPIOB, GPIO_Pin_0)
#define SDA_1 GPIO_SetBits(GPIOB, GPIO_Pin_0)
#define SCL_0 GPIO_ResetBits(GPIOB, GPIO_Pin_1)
#define SCL_1 GPIO_SetBits(GPIOB, GPIO_Pin_1)
#define SDA_VAL (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0))

void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
uint8_t i2c_write(uint8_t u8Data);
uint8_t i2c_read(uint8_t u8Ack);

void i2c_init(void)
{
    GPIO_InitTypeDef gpioInit;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    gpioInit.GPIO_Mode = GPIO_Mode_Out_OD;
    gpioInit.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    
    GPIO_Init(GPIOB, &gpioInit);
    
    SDA_1;
    SCL_1;
}

void i2c_start(void)
{
    SCL_1;
    delay_us(3);
    SDA_1;
    delay_us(3);
    SDA_0;
    delay_us(3);
    SCL_0;
    delay_us(3);
}

void i2c_stop(void)
{
    SDA_0;
    delay_us(3);
    SCL_1;
    delay_us(3);
    SDA_1;
    delay_us(3);
}

uint8_t i2c_write(uint8_t u8Data)
{
    uint8_t i;
    uint8_t u8Ret;
    
    for (i = 0; i < 8; ++i) {
        if (u8Data & 0x80) {
            SDA_1;
        } else {
            SDA_0;
        }
        delay_us(3);
        SCL_1;
        delay_us(5);
        SCL_0;
        delay_us(2);
        u8Data <<= 1;
    }
    
    SDA_1;
    delay_us(3);
    SCL_1;
    delay_us(3);
    if (SDA_VAL) {
        u8Ret = 0;
    } else {
        u8Ret = 1;
    }
    delay_us(2);
    SCL_0;
    delay_us(5);
    
    return u8Ret;
}

uint8_t i2c_read(uint8_t u8Ack)
{
    uint8_t i;
    uint8_t u8Ret = 0;
    
    SDA_1;
    delay_us(3);
    
    for (i = 0; i < 8; ++i) {
        u8Ret <<= 1;
        SCL_1;
        delay_us(3);
        if (SDA_VAL) {
            u8Ret |= 0x01;
        }
        delay_us(2);
        SCL_0;
        delay_us(5);
    }
    
    if (u8Ack) {
        SDA_0;
    } else {
        SDA_1;
    }
    delay_us(3);
    
    SCL_1;
    delay_us(5);
    SCL_0;
    delay_us(5);
    
    return u8Ret;
}

// Hàm UART s? d?ng SPL
void uart_init(void);
void uart_send_char(char c);
void uart_send_string(char* str);

void uart_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    
    // Enable clock cho USART1 và GPIOA
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    
    // C?u hình PA9 (TX)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // C?u hình PA10 (RX) - n?u c?n nh?n d? li?u
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // C?u hình USART1
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx; // Ch? dùng TX d? g?i d? li?u
    USART_Init(USART1, &USART_InitStructure);
    
    // Kích ho?t USART1
    USART_Cmd(USART1, ENABLE);
}

void uart_send_char(char c)
{
    // Ch? cho d?n khi thanh ghi truy?n d? li?u tr?ng (TXE)
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, c);
}

void uart_send_string(char* str)
{
    while (*str) {
        uart_send_char(*str++);
    }
}

// Hàm BH1750
#define BH1750_ADDR 0x46 // Ð?a ch? I2C c?a BH1750 (0x23 << 1)
void bh1750_init(void);
uint16_t bh1750_read(void);

void bh1750_init(void)
{
    i2c_start();
    i2c_write(BH1750_ADDR);
    i2c_write(0x10); // Ch? d? do High Resolution
    i2c_stop();
    Delay_Ms(180); // Ch? kh?i t?o
}

uint16_t bh1750_read(void)
{
    uint8_t msb, lsb;
    uint16_t lux;
    
    i2c_start();
    i2c_write(BH1750_ADDR | 0x01); // Ð?c d? li?u
    
    msb = i2c_read(1);  // Byte cao
    lsb = i2c_read(0);  // Byte th?p
    
    i2c_stop();
    
    lux = ((msb << 8) | lsb) / 1.2; // Chuy?n d?i sang lux
    return lux;
}

int main(void)
{
    GPIO_InitTypeDef gpioInit;
    TIM_TimeBaseInitTypeDef timerInit;
    char buffer[20];
    
    // C?u hình clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    // C?u hình LED PC13
    gpioInit.GPIO_Mode = GPIO_Mode_Out_PP;
    gpioInit.GPIO_Pin = GPIO_Pin_13;
    gpioInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &gpioInit);
    
    // C?u hình Timer2
    timerInit.TIM_CounterMode = TIM_CounterMode_Up;
    timerInit.TIM_Period = 0xFFFF;
    timerInit.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseInit(TIM2, &timerInit);
    TIM_Cmd(TIM2, ENABLE);
    
    // Kh?i t?o I2C, UART và BH1750
    i2c_init();
    uart_init();
    bh1750_init();
    
    // G?i thông báo kh?i d?u qua UART
    uart_send_string("BH1750 Light Sensor Test\r\n");
    
    while (1)
    {
        uint16_t lux = bh1750_read();
        
        // Chuy?n d?i s? thành chu?i
        sprintf(buffer, "Light: %u lux\r\n", lux);
        uart_send_string(buffer);
        
        // Nháy LED d? báo ho?t d?ng
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
        Delay_Ms(500);
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        Delay_Ms(500);
    }
}