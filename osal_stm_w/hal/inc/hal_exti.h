/**
 * @file    hal_exti.h
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
#ifndef _HAL_EXTI_H
#define _HAL_EXTI_H

#include "hal_cfg.h"
#include "hal_types.h"

#ifdef CFG_HAL_EXTI


/**
 * @brief: �ⲿ�ж��¼��ص�����
 * @param: void
 * @retval: void
 */
typedef void (*HalExtiCBlack_t)(void);

/**
 * @brief �ⲿ�ж�Դ
 */
typedef enum
{
	HAL_EXTI_LINE0 = 0,	/*!< �ⲿ�ж�0 */
	HAL_EXTI_LINE1,		/*!< �ⲿ�ж�1 */
	HAL_EXTI_LINE2,		/*!< �ⲿ�ж�2 */
	HAL_EXTI_LINE3,		/*!< �ⲿ�ж�3 */
	HAL_EXTI_LINE4,		/*!< �ⲿ�ж�4 */
	HAL_EXTI_LINE5,		/*!< �ⲿ�ж�5 */
	HAL_EXTI_LINE6,		/*!< �ⲿ�ж�6 */
	HAL_EXTI_LINE7,		/*!< �ⲿ�ж�7 */
	HAL_EXTI_LINE8,		/*!< �ⲿ�ж�8 */
	HAL_EXTI_LINE9,		/*!< �ⲿ�ж�9 */
	HAL_EXTI_LINE10,		/*!< �ⲿ�ж�10 */
	HAL_EXTI_LINE11,		/*!< �ⲿ�ж�11 */
	HAL_EXTI_LINE12,		/*!< �ⲿ�ж�12 */
	HAL_EXTI_LINE13,		/*!< �ⲿ�ж�13 */
	HAL_EXTI_LINE14,		/*!< �ⲿ�ж�14 */
	HAL_EXTI_LINE15,		/*!< �ⲿ�ж�15 */
	HAL_EXTI_SOURCE_MAX	/*!< �ⲿ�ж����� */
}HAL_EXTI_SOURCE;

/**
 * @brief �ⲿ�жϹ���ģʽ
 */
typedef enum
{
	HAL_EXTI_MODE_EVENT = EXTI_Mode_Event,			/*!< �¼�ģʽ */
	HAL_EXTI_MODE_INTERRUPT = EXTI_Mode_Interrupt	/*!< �ж�ģʽ */
}HAL_EXTI_MODE;

/**
 * @brief �ⲿ�жϴ�����ʽ
 */
typedef enum
{
	HAL_EXTI_TRIGGER_FALLING = EXTI_Trigger_Falling,					/*!< �����ش��� */
	HAL_EXTI_TRIGGER_RISING = EXTI_Trigger_Rising,					/*!< �½��ش��� */
	HAL_EXTI_TRIGGER_RISING_FALLING = EXTI_Trigger_Rising_Falling	/*!< �������½��ؾ����� */
}HAL_EXTI_TRIGGER;

/**
 * @brief ע��һ���ⲿ�¼��ж�
 * @param hPortSource: �ⲿ�¼��жϴ����˿�
 * @param eSource: �ж�Դ
 * @param eMode: �жϹ�����ʽ
 * @param eTrigger: �жϴ�����ʽ
 * @param hExtiEventCallBlack: �ж��¼��ص��������
 * @retval None
 */
void HalExtiRegist(UINT8 hPortSource, HAL_EXTI_SOURCE eSource, HAL_EXTI_MODE eMode, HAL_EXTI_TRIGGER eTrigger, HalExtiCBlack_t hExtiEventCB);

#endif //CFG_HAL_EXTI
#endif
