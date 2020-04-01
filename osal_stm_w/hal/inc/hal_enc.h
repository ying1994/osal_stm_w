/**
 * @file    hal_enc.h
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   �������ӿ�
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

//��ʱ��1 ENC ͨ������
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

//��ʱ��2 ENC ͨ������
#define TIM2_ENC_CH1_PORT                GPIOA
#define TIM2_ENC_CH1_PIN                 GPIO_Pin_0
#define TIM2_ENC_CH2_PORT                GPIOA
#define TIM2_ENC_CH2_PIN                 GPIO_Pin_1
#define TIM2_ENC_CH3_PORT                GPIOA
#define TIM2_ENC_CH3_PIN                 GPIO_Pin_2
#define TIM2_ENC_CH4_PORT                GPIOA
#define TIM2_ENC_CH4_PIN                 GPIO_Pin_3

//��ʱ��3 ENC ͨ������
#define TIM3_ENC_CH1_PORT                GPIOA
#define TIM3_ENC_CH1_PIN                 GPIO_Pin_6
#define TIM3_ENC_CH2_PORT                GPIOA
#define TIM3_ENC_CH2_PIN                 GPIO_Pin_7
#define TIM3_ENC_CH3_PORT                GPIOB
#define TIM3_ENC_CH3_PIN                 GPIO_Pin_0
#define TIM3_ENC_CH4_PORT                GPIOB
#define TIM3_ENC_CH4_PIN                 GPIO_Pin_1

//��ʱ��4 ENC ͨ������
#define TIM4_ENC_CH1_PORT                GPIOB
#define TIM4_ENC_CH1_PIN                 GPIO_Pin_6
#define TIM4_ENC_CH2_PORT                GPIOB
#define TIM4_ENC_CH2_PIN                 GPIO_Pin_7
#define TIM4_ENC_CH3_PORT                GPIOB
#define TIM4_ENC_CH3_PIN                 GPIO_Pin_8
#define TIM4_ENC_CH4_PORT                GPIOB
#define TIM4_ENC_CH4_PIN                 GPIO_Pin_9

//��ʱ��5 ENC ͨ������
#define TIM5_ENC_CH1_PORT                GPIOA
#define TIM5_ENC_CH1_PIN                 GPIO_Pin_0
#define TIM5_ENC_CH2_PORT                GPIOA
#define TIM5_ENC_CH2_PIN                 GPIO_Pin_1
#define TIM5_ENC_CH3_PORT                GPIOA
#define TIM5_ENC_CH3_PIN                 GPIO_Pin_2
#define TIM5_ENC_CH4_PORT                GPIOA
#define TIM5_ENC_CH4_PIN                 GPIO_Pin_3

//��ʱ��8 ENC ͨ������
#define TIM8_ENC_CH1_PORT                GPIOC
#define TIM8_ENC_CH1_PIN                 GPIO_Pin_6
#define TIM8_ENC_CH2_PORT                GPIOC
#define TIM8_ENC_CH2_PIN                 GPIO_Pin_7
#define TIM8_ENC_CH3_PORT                GPIOC
#define TIM8_ENC_CH3_PIN                 GPIO_Pin_8
#define TIM8_ENC_CH4_PORT                GPIOC
#define TIM8_ENC_CH4_PIN                 GPIO_Pin_9

//��ʱ��8 ENC ͨ������
#define TIM8_ENC_CH1N_PORT                GPIOA
#define TIM8_ENC_CH1N_PIN                 GPIO_Pin_7
#define TIM8_ENC_CH2N_PORT                GPIOB
#define TIM8_ENC_CH2N_PIN                 GPIO_Pin_0
#define TIM8_ENC_CH3N_PORT                GPIOB
#define TIM8_ENC_CH3N_PIN                 GPIO_Pin_1

/** 
 * @brief ENC����
 */
typedef enum
{
	HAL_ENC_Polarity_Rising = TIM_ICPolarity_Rising, /*<! �����ؼ��� */
	HAL_ENC_Polarity_Falling = TIM_ICPolarity_Falling, /*<! �½��ؼ��� */
	HAL_ENC_Polarity_BothEdge = TIM_ICPolarity_BothEdge, /*<! �������½���ͬʱ���� */
}HALENCPolarityTypeDef;

/** 
 * @brief ENC ͨ��ʹ�ܱ��
 */
typedef enum
{
	HAL_ENC_CH1 = 0x0001, /*<! ENCͨ��1ʹ�� */
	HAL_ENC_CH2 = 0x0002, /*<! ENCͨ��1ʹ�� */
	HAL_ENC_CH3 = 0x0004, /*<! ENCͨ��1ʹ�� */
	HAL_ENC_CH4 = 0x0008, /*<! ENCͨ��1ʹ�� */
}HALENCChannelTypeDef;

/** 
 * @brief ENC�����ṹ����
 */
typedef struct
{
	/**
	 * @brief ��������ʼ��
	 * @param uPeriod ENC���� (��λ: us)
	 * @param uOCPolarity ENC1���� @ref HALENCPolarityTypeDef
	 * @param uChannel ������ͨ��
	 * @retval None
	 */
	void (*init)(UINT32 uPeriod, UINT16 OCMode, UINT16 uOCPolarity, UINT16 uChannel);
	
	/**
	 * @brief ��������Դע��
	 * @param None
	 * @retval None
	 */
	void (*deInit)(void);
	
	/**
	 * @brief ��ȡ�������������
	 * @retval None
	 */
	INT32 (*readEncoderCnt)(void);
	
}HALEncTypeDef;


/**
 * @brief ��ȡENC�����ṹ���
 * @param eChannel ENC ͨ�� @ref HALENCNumer
 * @retval ENC�����ṹ���
 */
HALEncTypeDef* HalEncGetInstance(HalIndeTimerIDsTypeDef eId);

#endif //defined(CFG_HAL_ENC) && defined(CFG_HAL_TIMER)
#endif //HAL_ENC_H
