#ifndef __TIM3_H
#define __TIM3_H

#include "stm32f10x.h"

// PWM初始化函数
void PWM_TIM3_CH4_Init(uint16_t arr, uint16_t psc);
// 设置占空比函数（范围：0~arr）
void PWM_TIM3_SetDuty(uint16_t duty);

#endif /* __PWM_TIM3_H */
