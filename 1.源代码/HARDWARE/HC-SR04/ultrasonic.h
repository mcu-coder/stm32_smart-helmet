#ifndef __ULTRASONIC_H
#define	__ULTRASONIC_H
#include "stm32f10x.h"
#include "adcx.h"
#include "delay.h"
#include "math.h"

/*****************辰哥单片机设计******************
											STM32
 * 文件			:	HC-SR04超声波传感器h文件                   
 * 版本			: V1.0
 * 日期			: 2024.8.27
 * MCU			:	STM32F103C8T6
 * 接口			:	见代码							
 * BILIBILI	:	辰哥单片机设计
 * CSDN			:	辰哥单片机设计
 * 作者			:	辰哥

**********************BEGIN***********************/


/***************根据自己需求更改****************/
// ULTRASONIC GPIO宏定义

#define		ULTRASONIC_GPIO_CLK								RCC_APB2Periph_GPIOA
#define 	ULTRASONIC_GPIO_PORT							GPIOA
#define 	ULTRASONIC_TRIG_GPIO_PIN					GPIO_Pin_1
#define 	ULTRASONIC_ECHO_GPIO_PIN					GPIO_Pin_4

#define 	TRIG_Send  PAout(1)
#define 	ECHO_Reci  PAin(4)

/*********************END**********************/

void Ultrasonic_Init(void);
float UltrasonicGetLength(void);

void OpenTimerForHc(void);
void CloseTimerForHc(void); 
u32 GetEchoTimer(void);

#endif /* __ADC_H */

