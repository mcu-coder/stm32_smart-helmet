#include "TIM3.h"

/**
  * @brief  初始化TIM3通道4（PB1）输出PWM
  * @param  arr: 自动重装载值（决定PWM频率）
  * @param  psc: 预分频系数
  * @note   PWM频率 = 72MHz / (arr * psc)
  */
void TIM3_Init(uint16_t arr, uint16_t psc) {
    GPIO_InitTypeDef GPIO_InitStruct;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;
    TIM_OCInitTypeDef TIM_OCStruct;

    // 1. 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    // 2. 配置PB1为复用推挽输出（TIM3_CH4）
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 3. 配置TIM3时基单元
    TIM_TimeBaseStruct.TIM_Period = arr - 1;
    TIM_TimeBaseStruct.TIM_Prescaler = psc - 1;
    TIM_TimeBaseStruct.TIM_ClockDivision = 0;
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStruct);

    // 4. 配置PWM模式（通道4）
    TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCStruct.TIM_Pulse = 0;  // 初始占空比0%
    TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC4Init(TIM3, &TIM_OCStruct);

    // 5. 使能预装载寄存器
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM3, ENABLE);

    // 6. 启动TIM3
    TIM_Cmd(TIM3, ENABLE);
}

/**
  * @brief  设置TIM3通道4的PWM占空比
  * @param  duty: 占空比值（0~自动重装载值arr）
  */
void PWM_TIM3_SetDuty(uint16_t duty) {
    TIM_SetCompare4(TIM3, duty);
}
