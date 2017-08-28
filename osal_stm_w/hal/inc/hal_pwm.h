/**
 * @file    hal_pwm.h
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 * COPYRIGHT NOTICE  
 * Copyright 2016, wsf 
 * All rights Reserved
 *
 */
#ifndef HAL_PWM_H
#define HAL_PWM_H
#include "hal_cfg.h"
#include "hal_types.h"

#if defined(CFG_HAL_PWM) && defined(CFG_HAL_TIMER)

#include "hal_timer.h"

//定时器1 PWM 通道定义
#define TIM1_PWM_CH1_PORT                GPIOA
#define TIM1_PWM_CH1_PIN                 GPIO_Pin_8
#define TIM1_PWM_CH2_PORT                GPIOA
#define TIM1_PWM_CH2_PIN                 GPIO_Pin_9
#define TIM1_PWM_CH3_PORT                GPIOA
#define TIM1_PWM_CH3_PIN                 GPIO_Pin_10
#define TIM1_PWM_CH4_PORT                GPIOA
#define TIM1_PWM_CH4_PIN                 GPIO_Pin_11
                                         
#define TIM1_PWM_CH1N_PORT               GPIOB
#define TIM1_PWM_CH1N_PIN                GPIO_Pin_13
#define TIM1_PWM_CH2N_PORT               GPIOB
#define TIM1_PWM_CH2N_PIN                GPIO_Pin_14
#define TIM1_PWM_CH3N_PORT               GPIOB
#define TIM1_PWM_CH3N_PIN                GPIO_Pin_15

//定时器2 PWM 通道定义
#define TIM2_PWM_CH1_PORT                GPIOA
#define TIM2_PWM_CH1_PIN                 GPIO_Pin_0
#define TIM2_PWM_CH2_PORT                GPIOA
#define TIM2_PWM_CH2_PIN                 GPIO_Pin_1
#define TIM2_PWM_CH3_PORT                GPIOA
#define TIM2_PWM_CH3_PIN                 GPIO_Pin_2
#define TIM2_PWM_CH4_PORT                GPIOA
#define TIM2_PWM_CH4_PIN                 GPIO_Pin_3

//定时器3 PWM 通道定义
#define TIM3_PWM_CH1_PORT                GPIOA
#define TIM3_PWM_CH1_PIN                 GPIO_Pin_6
#define TIM3_PWM_CH2_PORT                GPIOA
#define TIM3_PWM_CH2_PIN                 GPIO_Pin_7
#define TIM3_PWM_CH3_PORT                GPIOB
#define TIM3_PWM_CH3_PIN                 GPIO_Pin_0
#define TIM3_PWM_CH4_PORT                GPIOB
#define TIM3_PWM_CH4_PIN                 GPIO_Pin_1

//定时器4 PWM 通道定义
#define TIM4_PWM_CH1_PORT                GPIOB
#define TIM4_PWM_CH1_PIN                 GPIO_Pin_6
#define TIM4_PWM_CH2_PORT                GPIOB
#define TIM4_PWM_CH2_PIN                 GPIO_Pin_7
#define TIM4_PWM_CH3_PORT                GPIOB
#define TIM4_PWM_CH3_PIN                 GPIO_Pin_8
#define TIM4_PWM_CH4_PORT                GPIOB
#define TIM4_PWM_CH4_PIN                 GPIO_Pin_9

//定时器5 PWM 通道定义
#define TIM5_PWM_CH1_PORT                GPIOA
#define TIM5_PWM_CH1_PIN                 GPIO_Pin_0
#define TIM5_PWM_CH2_PORT                GPIOA
#define TIM5_PWM_CH2_PIN                 GPIO_Pin_1
#define TIM5_PWM_CH3_PORT                GPIOA
#define TIM5_PWM_CH3_PIN                 GPIO_Pin_2
#define TIM5_PWM_CH4_PORT                GPIOA
#define TIM5_PWM_CH4_PIN                 GPIO_Pin_3

//定时器8 PWM 通道定义
#define TIM8_PWM_CH1_PORT                GPIOC
#define TIM8_PWM_CH1_PIN                 GPIO_Pin_6
#define TIM8_PWM_CH2_PORT                GPIOC
#define TIM8_PWM_CH2_PIN                 GPIO_Pin_7
#define TIM8_PWM_CH3_PORT                GPIOC
#define TIM8_PWM_CH3_PIN                 GPIO_Pin_8
#define TIM8_PWM_CH4_PORT                GPIOC
#define TIM8_PWM_CH4_PIN                 GPIO_Pin_9

//定时器8 PWM 通道定义
#define TIM8_PWM_CH1N_PORT                GPIOA
#define TIM8_PWM_CH1N_PIN                 GPIO_Pin_7
#define TIM8_PWM_CH2N_PORT                GPIOB
#define TIM8_PWM_CH2N_PIN                 GPIO_Pin_0
#define TIM8_PWM_CH3N_PORT                GPIOB
#define TIM8_PWM_CH3N_PIN                 GPIO_Pin_1

/** 
 * @brief PWM极性
 */
typedef enum
{
	HAL_PWM_Polarity_High = TIM_OCPolarity_High, /*<! 高极性 */
	HAL_PWM_Polarity_Low = TIM_OCPolarity_Low, /*<! 低极性 */
}HALPWMPolarityTypeDef;

/** 
 * @brief PWM通道
 */
typedef enum
{
	HAL_PWM_CH1 = 0, /*<! PWM通道1 */
	HAL_PWM_CH2, /*<! PWM通道2 */
	HAL_PWM_CH3, /*<! PWM通道3 */
	HAL_PWM_CH4, /*<! PWM通道4 */
	HAL_PWM_CH_SIZE, /*<! PWM通道数 */
}HALPWMChannelTypeDef;

/** 
 * @brief PWM操作结构定义
 */
typedef struct
{
	/**
	 * @brief PWM初始化
	 * @param uus PWM周期 (单位: us)
	 * @param uPolarity PWM极性 @ref HALPWMPolarityTypeDef
	 * @retval None
	 */
	void (*init)(UINT32 uus, UINT16 uPolarity);
	
	/**
	 * @brief PWM资源注销
	 * @param None
	 * @retval None
	 */
	void (*deInit)(void);
	
	/**
	 * @brief 设置 PWM 占空比
	 * @param eCh PWM通道 @ref HALPWMChannelTypeDef
	 * @param dutycycle PWM 占空比 (单位: %)
	 * @retval None
	 */
	void (*setDutyCycle)(UINT8 eCh, UINT8 dutycycle);
	
}HALPwmTypeDef;


/**
 * @brief 获取I2C操作结构句柄
 * @param eChannel I2C 通道 @ref HALI2CNumer
 * @retval I2C操作结构句柄
 */
HALPwmTypeDef* HalPwmGetInstance(HalIndeTimerIDsTypeDef eId);

#endif //defined(CFG_HAL_PWM) && defined(CFG_HAL_TIMER)
#endif //HAL_PWM_H
