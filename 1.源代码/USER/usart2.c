#include "usart2.h"
#include "string.h"
#include "stdio.h"


#define USART2_RX_BUF_SIZE 128 

static uint8_t USART2_RxBuffer[USART2_RX_BUF_SIZE];  // 接收缓冲区
static volatile uint16_t USART2_RxIndex = 0;          // 接收索引
static volatile bool USART2_RxFlag = false;           // 接收完成标志

/**
  * @brief  初始化USART2
  * @param  baudrate: 波特率（如9600、115200）
  */
void USART2_Init() {
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    // 1. 使能时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 2. 配置GPIO（PA2-TX, PA3-RX）
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;  // TX-复用推挽输出
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;  // RX-浮空输入
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 3. 配置USART2参数
    USART_InitStruct.USART_BaudRate = 9600;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &USART_InitStruct);

    // 4. 配置中断（接收中断）
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    // 5. 使能USART2
    USART_Cmd(USART2, ENABLE);
}

void USART2_SendByte(uint8_t data) {
    // 等待发送数据寄存器（TXE）为空（硬件准备好接收下一个字节）
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    // 将数据写入发送寄存器，硬件自动发送
    USART_SendData(USART2, data);
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
}



void USART2_SendString(const char* str) {
    while (*str) {
        USART2_SendByte(*str++);
    }
}

void USART2_IRQHandler(void) {
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        uint8_t rx_byte = USART_ReceiveData(USART2);  // 读取接收字节
        
        // 基于现有缓冲区存储：未满则存数据，避免溢出
        if (USART2_RxIndex < USART2_RX_BUF_SIZE - 1) {  // 留1字节存'\0'（可选）
            USART2_RxBuffer[USART2_RxIndex++] = rx_byte;
            USART2_RxBuffer[USART2_RxIndex] = '\0';  // 字符串结尾（方便后续判断）
        } else {
            USART2_RxIndex = 0;  // 缓冲区溢出，重置
        }
        
        USART2_RxFlag = true;  // 置位接收完成标志
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);  // 清除中断标志
    }
}


// 其他辅助函数
bool USART2_GetReceivedFlag(void) { return USART2_RxFlag; }
uint16_t USART2_GetReceivedLength(void) { return USART2_RxIndex; }
uint8_t* USART2_GetRxBuffer(void) { return (uint8_t*)USART2_RxBuffer; }
void USART2_ClearReceivedFlag(void) { USART2_RxFlag = false; USART2_RxIndex = 0; }


