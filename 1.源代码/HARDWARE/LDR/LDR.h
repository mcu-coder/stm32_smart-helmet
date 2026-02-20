#ifndef __LDR_H
#define	__LDR_H
#include "stm32f10x.h"
#include "adcx.h"
#include "delay.h"
#include "math.h"

/*****************辰哥单片机设计******************
											STM32
 * 文件			:	光敏传感器h文件                   
 * 版本			: V1.0
 * 日期			: 2024.8.9
 * MCU			:	STM32F103C8T6
 * 接口			:	见代码							
 * BILIBILI	:	辰哥单片机设计
 * CSDN			:	辰哥单片机设计
 * 作者			:	辰哥

**********************BEGIN***********************/

#define LDR_READ_TIMES	10  //光照传感器ADC循环读取次数

/***************根据自己需求更改****************/
// LDR GPIO宏定义
#define		LDR_GPIO_CLK							RCC_APB2Periph_GPIOA
#define 	LDR_GPIO_PORT							GPIOA
#define 	LDR_GPIO_PIN							GPIO_Pin_0
/*********************END**********************/


// ADC 通道宏定义
#define   ADC_CHANNEL               ADC_Channel_0

//#define    ADC_IRQ                       ADC3_IRQn
//#define    ADC_IRQHandler                ADC3_IRQHandler

void LDR_Init(void);
uint16_t LDR_Average_Data(void);
uint16_t LDR_LuxData();

#endif /* __ADC_H */

