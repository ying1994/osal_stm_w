/**
 * @file    hal_enc.h
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   编码器接口
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 * COPYRIGHT NOTICE  
 * Copyright 2016, wsf 
 * All rights Reserved
 *
 */
#ifndef HAL_ENC_H
#define HAL_ENC_H
#include "hal_cfg.h"
#include "hal_types.h"

#if defined(CFG_HAL_ENC) && defined(CFG_HAL_TIMER)

#include "hal_timer.h"

//定时器1 ENC 通道定义
#define TIM1_ENC_CH1_PORT                GPIOA
#define TIM1_ENC_CH1_PIN                 GPIO_Pin_8
#define TIM1_ENC_CH2_PORT                GPIOA
#define TIM1_ENC_CH2_PIN                 GPIO_Pin_9
#define TIM1_ENC_CH3_PORT                GPIOA
#define TIM1_ENC_CH3_PIN                 GPIO_Pin_10
#define TIM1_ENC_CH4_PORT                GPIOA
#define TIM1_ENC_CH4_PIN                 GPIO_Pin_11
                                         
#define TIM1_ENC_CH1N_PORT               GPIOB
#define TIM1_ENC_CH1N_PIN                GPIO_Pin_13
#define TIM1_ENC_CH2N_PORT               GPIOB
#define TIM1_ENC_CH2N_PIN                GPIO_Pin_14
#define TIM1_ENC_CH3N_PORT               GPIOB
#define TIM1_ENC_CH3N_PIN                GPIO_Pin_15

//定时器2 ENC 通道定义
#define TIM2_ENC_CH1_PORT                GPIOA
#define TIM2_ENC_CH1_PIN                 GPIO_Pin_0
#define TIM2_ENC_CH2_PORT                GPIOA
#define TIM2_ENC_CH2_PIN                 GPIO_Pin_1
#define TIM2_ENC_CH3_PORT                GPIOA
#define TIM2_ENC_CH3_PIN                 GPIO_Pin_2
#define TIM2_ENC_CH4_PORT                GPIOA
#define TIM2_ENC_CH4_PIN                 GPIO_Pin_3

//定时器3 ENC 通道定义
#define TIM3_ENC_CH1_PORT                GPIOA
#define TIM3_ENC_CH1_PIN                 GPIO_Pin_6
#define TIM3_ENC_CH2_PORT                GPIOA
#define TIM3_ENC_CH2_PIN                 GPIO_Pin_7
#define TIM3_ENC_CH3_PORT                GPIOB
#define TIM3_ENC_CH3_PIN                 GPIO_Pin_0
#define TIM3_ENC_CH4_PORT                GPIOB
#define TIM3_ENC_CH4_PIN                 GPIO_Pin_1

//定时器4 ENC 通道定义
#define TIM4_ENC_CH1_PORT                GPIOB
#define TIM4_ENC_CH1_PIN                 GPIO_Pin_6
#define TIM4_ENC_CH2_PORT                GPIOB
#define TIM4_ENC_CH2_PIN                 GPIO_Pin_7
#define TIM4_ENC_CH3_PORT                GPIOB
#define TIM4_ENC_CH3_PIN                 GPIO_Pin_8
#define TIM4_ENC_CH4_PORT                GPIOB
#define TIM4_ENC_CH4_PIN                 GPIO_Pin_9

//定时器5 ENC 通道定义
#define TIM5_ENC_CH1_PORT                GPIOA
#define TIM5_ENC_CH1_PIN                 GPIO_Pin_0
#define TIM5_ENC_CH2_PORT                GPIOA
#define TIM5_ENC_CH2_PIN                 GPIO_Pin_1
#define TIM5_ENC_CH3_PORT                GPIOA
#define TIM5_ENC_CH3_PIN                 GPIO_Pin_2
#define TIM5_ENC_CH4_PORT                GPIOA
#define TIM5_ENC_CH4_PIN                 GPIO_Pin_3

//定时器8 ENC 通道定义
#define TIM8_ENC_CH1_PORT                GPIOC
#define TIM8_ENC_CH1_PIN                 GPIO_Pin_6
#define TIM8_ENC_CH2_PORT                GPIOC
#define TIM8_ENC_CH2_PIN                 GPIO_Pin_7
#define TIM8_ENC_CH3_PORT                GPIOC
#define TIM8_ENC_CH3_PIN                 GPIO_Pin_8
#define TIM8_ENC_CH4_PORT                GPIOC
#define TIM8_ENC_CH4_PIN                 GPIO_Pin_9

//定时器8 ENC 通道定义
#define TIM8_ENC_CH1N_PORT                GPIOA
#define TIM8_ENC_CH1N_PIN                 GPIO_Pin_7
#define TIM8_ENC_CH2N_PORT                GPIOB
#define TIM8_ENC_CH2N_PIN                 GPIO_Pin_0
#define TIM8_ENC_CH3N_PORT                GPIOB
#define TIM8_ENC_CH3N_PIN                 GPIO_Pin_1

/** 
 * @brief ENC极性
 */
typedef enum
{
	HAL_ENC_Polarity_Rising = TIM_ICPolarity_Rising, /*<! 上升沿计数 */
	HAL_ENC_Polarity_Falling = TIM_ICPolarity_Falling, /*<! 下降沿计数 */
	HAL_ENC_Polarity_BothEdge = TIM_ICPolarity_BothEdge, /*<! 上升沿下降沿同时计数 */
}HALENCPolarityTypeDef;

/** 
 * @brief ENC 通道使能标记
 */
typedef enum
{
	HAL_ENC_CH1 = 0x0001, /*<! ENC通道1使能 */
	HAL_ENC_CH2 = 0x0002, /*<! ENC通道1使能 */
	HAL_ENC_CH3 = 0x0004, /*<! ENC通道1使能 */
	HAL_ENC_CH4 = 0x0008, /*<! ENC通道1使能 */
}HALENCChannelTypeDef;

/** 
 * @brief ENC操作结构定义
 */
typedef struct
{
	/**
	 * @brief 编码器初始化
	 * @param uPeriod ENC周期 (单位: us)
	 * @param uOCPolarity ENC1极性 @ref HALENCPolarityTypeDef
	 * @param uChannel 编码器通道
	 * @retval None
	 */
	void (*init)(UINT32 uPeriod, UINT16 OCMode, UINT16 uOCPolarity, UINT16 uChannel);
	
	/**
	 * @brief 编码器资源注销
	 * @param None
	 * @retval None
	 */
	void (*deInit)(void);
	
	/**
	 * @brief 读取编码器编码计数
	 * @retval None
	 */
	INT32 (*readEncoderCnt)(void);
	
}HALEncTypeDef;


/**
 * @brief 获取ENC操作结构句柄
 * @param eChannel ENC 通道 @ref HALENCNumer
 * @retval ENC操作结构句柄
 */
HALEncTypeDef* HalEncGetInstance(HalIndeTimerIDsTypeDef eId);

#endif //defined(CFG_HAL_ENC) && defined(CFG_HAL_TIMER)
#endif //HAL_ENC_H
