#ifndef __USART2_H
#define __USART2_H

#include "stm32f10x.h"
#include <stdbool.h>
#define USART2_RX_BUF_SIZE 128
#define USART2_RX_BUFFER_SIZE 32
// 初始化函数
void USART2_Init(void); 
// 发送单字节
void USART2_SendByte(uint8_t data);
// 发送字符串
void USART2_SendString(const char* str);
void USART2_IRQHandler(void);
uint8_t* USART2_GetRxBuffer(void);
uint8_t USART2_GetRxData(void);    // 获取接收的字节
bool USART2_GetRxFlag(void);       // 获取接收标志
void USART2_ClearRxFlag(void);     // 清除接收标志
bool USART2_GetReceivedFlag(void);  
void USART2_ClearReceivedFlag(void); 
#endif /* __USART2_H */
