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

//��ʱ��1 PWM ͨ������
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

//��ʱ��2 PWM ͨ������
#define TIM2_PWM_CH1_PORT                GPIOA
#define TIM2_PWM_CH1_PIN                 GPIO_Pin_0
#define TIM2_PWM_CH2_PORT                GPIOA
#define TIM2_PWM_CH2_PIN                 GPIO_Pin_1
#define TIM2_PWM_CH3_PORT                GPIOA
#define TIM2_PWM_CH3_PIN                 GPIO_Pin_2
#define TIM2_PWM_CH4_PORT                GPIOA
#define TIM2_PWM_CH4_PIN                 GPIO_Pin_3

//��ʱ��3 PWM ͨ������
#define TIM3_PWM_CH1_PORT                GPIOA
#define TIM3_PWM_CH1_PIN                 GPIO_Pin_6
#define TIM3_PWM_CH2_PORT                GPIOA
#define TIM3_PWM_CH2_PIN                 GPIO_Pin_7
#define TIM3_PWM_CH3_PORT                GPIOB
#define TIM3_PWM_CH3_PIN                 GPIO_Pin_0
#define TIM3_PWM_CH4_PORT                GPIOB
#define TIM3_PWM_CH4_PIN                 GPIO_Pin_1

//��ʱ��4 PWM ͨ������
#define TIM4_PWM_CH1_PORT                GPIOB
#define TIM4_PWM_CH1_PIN                 GPIO_Pin_6
#define TIM4_PWM_CH2_PORT                GPIOB
#define TIM4_PWM_CH2_PIN                 GPIO_Pin_7
#define TIM4_PWM_CH3_PORT                GPIOB
#define TIM4_PWM_CH3_PIN                 GPIO_Pin_8
#define TIM4_PWM_CH4_PORT                GPIOB
#define TIM4_PWM_CH4_PIN                 GPIO_Pin_9

//��ʱ��5 PWM ͨ������
#define TIM5_PWM_CH1_PORT                GPIOA
#define TIM5_PWM_CH1_PIN                 GPIO_Pin_0
#define TIM5_PWM_CH2_PORT                GPIOA
#define TIM5_PWM_CH2_PIN                 GPIO_Pin_1
#define TIM5_PWM_CH3_PORT                GPIOA
#define TIM5_PWM_CH3_PIN                 GPIO_Pin_2
#define TIM5_PWM_CH4_PORT                GPIOA
#define TIM5_PWM_CH4_PIN                 GPIO_Pin_3

//��ʱ��8 PWM ͨ������
#define TIM8_PWM_CH1_PORT                GPIOC
#define TIM8_PWM_CH1_PIN                 GPIO_Pin_6
#define TIM8_PWM_CH2_PORT                GPIOC
#define TIM8_PWM_CH2_PIN                 GPIO_Pin_7
#define TIM8_PWM_CH3_PORT                GPIOC
#define TIM8_PWM_CH3_PIN                 GPIO_Pin_8
#define TIM8_PWM_CH4_PORT                GPIOC
#define TIM8_PWM_CH4_PIN                 GPIO_Pin_9

//��ʱ��8 PWM ͨ������
#define TIM8_PWM_CH1N_PORT                GPIOA
#define TIM8_PWM_CH1N_PIN                 GPIO_Pin_7
#define TIM8_PWM_CH2N_PORT                GPIOB
#define TIM8_PWM_CH2N_PIN                 GPIO_Pin_0
#define TIM8_PWM_CH3N_PORT                GPIOB
#define TIM8_PWM_CH3N_PIN                 GPIO_Pin_1

/** 
 * @brief PWM����
 */
typedef enum
{
	HAL_PWM_Polarity_High = TIM_OCPolarity_High, /*<! �߼��� */
	HAL_PWM_Polarity_Low = TIM_OCPolarity_Low, /*<! �ͼ��� */
}HALPWMPolarityTypeDef;

/** 
 * @brief PWMͨ��
 */
typedef enum
{
	HAL_PWM_CH1 = 0, /*<! PWMͨ��1 */
	HAL_PWM_CH2, /*<! PWMͨ��2 */
	HAL_PWM_CH3, /*<! PWMͨ��3 */
	HAL_PWM_CH4, /*<! PWMͨ��4 */
	HAL_PWM_CH_SIZE, /*<! PWMͨ���� */
}HALPWMChannelTypeDef;

/** 
 * @brief PWM ͨ��ʹ�ܱ��
 */
typedef enum
{
	HAL_PWM_MASK_CH1 =				0x0001, /*<! PWMͨ��1ʹ�� */
	HAL_PWM_MASK_CH2 =				0x0002, /*<! PWMͨ��1ʹ�� */
	HAL_PWM_MASK_CH3 =				0x0004, /*<! PWMͨ��1ʹ�� */
	HAL_PWM_MASK_CH4 =				0x0008, /*<! PWMͨ��1ʹ�� */
}HALPWMMaskTypeDef;

/** 
 * @brief PWM�����ṹ����
 */
typedef struct
{
	/**
	 * @brief PWM��ʼ��
	 * @param uus PWM���� (��λ: us)
	 * @param uPolarity PWM���� @ref HALPWMPolarityTypeDef
	 * @param uMask PWM��ʼ��ͨ����־ @ref HALPWMMaskTypeDef
	 * @retval None
	 */
	void (*init)(UINT32 uus, UINT16 uPolarity, UINT16 uMask);
	
	/**
	 * @brief PWM��Դע��
	 * @param None
	 * @retval None
	 */
	void (*deInit)(void);
	
	/**
	 * @brief ���� PWM ռ�ձ�
	 * @param eCh PWMͨ�� @ref HALPWMChannelTypeDef
	 * @param dutycycle PWM ռ�ձ� (��λ: %)
	 * @retval None
	 */
	void (*setDutyCycle)(UINT8 eCh, UINT8 dutycycle);
	
}HALPwmTypeDef;


/**
 * @brief ��ȡI2C�����ṹ���
 * @param eChannel I2C ͨ�� @ref HALI2CNumer
 * @retval I2C�����ṹ���
 */
HALPwmTypeDef* HalPwmGetInstance(HalIndeTimerIDsTypeDef eId);

#endif //defined(CFG_HAL_PWM) && defined(CFG_HAL_TIMER)
#endif //HAL_PWM_H
