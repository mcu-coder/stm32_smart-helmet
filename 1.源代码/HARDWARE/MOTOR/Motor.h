#ifndef __MOTOR_H
#define	__MOTOR_H
#include "stm32f10x.h"
#include "delay.h"
#include "sys.h"



/***************根据自己需求更改****************/
// TB6612FNG电机驱动模块 GPIO宏定义

#define	MOTOR_CLK										RCC_APB2Periph_GPIOA

#define MOTOR_AIN1_GPIO_PIN 				GPIO_Pin_0
#define MOTOR_AIN2_GPIO_PIN 				GPIO_Pin_1
#define MOTOR_AIN1_GPIO_PORT 				GPIOA
#define MOTOR_AIN2_GPIO_PORT 				GPIOA

#define MOTOR_PWMA_GPIO_PIN 				GPIO_Pin_2
#define MOTOR_PWMA_GPIO_PORT 				GPIOA


/*********************END**********************/

void MOTOR_Init(void);
void PWM_Init(void);
void Motor_SetSpeed(int8_t Speed);

#endif



