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
 * All rights res
 *
 */
#ifndef _HAL_EXTI_H
#define _HAL_EXTI_H

#include "hal_board.h"
#include "hal_types.h"
#include "stm32f10x.h"

#ifdef CFG_HAL_EXTI


/**
 * @brief: 外部中断事件回调函数
 * @param: void
 * @retval: void
 */
typedef void (*HalExtiCBlack_t)(void);

/**
 * @brief 外部中断源
 */
typedef enum
{
	HAL_EXTI_LINE0 = 0,	/*!< 外部中断0 */
	HAL_EXTI_LINE1,		/*!< 外部中断1 */
	HAL_EXTI_LINE2,		/*!< 外部中断2 */
	HAL_EXTI_LINE3,		/*!< 外部中断3 */
	HAL_EXTI_LINE4,		/*!< 外部中断4 */
	HAL_EXTI_LINE5,		/*!< 外部中断5 */
	HAL_EXTI_LINE6,		/*!< 外部中断6 */
	HAL_EXTI_LINE7,		/*!< 外部中断7 */
	HAL_EXTI_LINE8,		/*!< 外部中断8 */
	HAL_EXTI_LINE9,		/*!< 外部中断9 */
	HAL_EXTI_LINE10,		/*!< 外部中断10 */
	HAL_EXTI_LINE11,		/*!< 外部中断11 */
	HAL_EXTI_LINE12,		/*!< 外部中断12 */
	HAL_EXTI_LINE13,		/*!< 外部中断13 */
	HAL_EXTI_LINE14,		/*!< 外部中断14 */
	HAL_EXTI_LINE15,		/*!< 外部中断15 */
	HAL_EXTI_SOURCE_MAX	/*!< 外部中断总数 */
}HAL_EXTI_SOURCE;

/**
 * @brief 外部中断工作模式
 */
typedef enum
{
	HAL_EXTI_MODE_EVENT = EXTI_Mode_Event,			/*!< 事件模式 */
	HAL_EXTI_MODE_INTERRUPT = EXTI_Mode_Interrupt	/*!< 中断模式 */
}HAL_EXTI_MODE;

/**
 * @brief 外部中断触发方式
 */
typedef enum
{
	HAL_EXTI_TRIGGER_FALLING = EXTI_Trigger_Falling,					/*!< 上升沿触发 */
	HAL_EXTI_TRIGGER_RISING = EXTI_Trigger_Rising,					/*!< 下降沿触发 */
	HAL_EXTI_TRIGGER_RISING_FALLING = EXTI_Trigger_Rising_Falling	/*!< 上升沿下降沿均触发 */
}HAL_EXTI_TRIGGER;

/**
 * @brief 注册一个外部事件中断
 * @param hPortSource: 外部事件中断触发端口
 * @param eSource: 中断源
 * @param eMode: 中断工作方式
 * @param eTrigger: 中断触发方式
 * @param hExtiEventCallBlack: 中断事件回调函数句柄
 * @retval None
 */
void HalExtiRegist(UINT8 hPortSource, HAL_EXTI_SOURCE eSource, HAL_EXTI_MODE eMode, HAL_EXTI_TRIGGER eTrigger, HalExtiCBlack_t hExtiEventCB);

#endif //CFG_HAL_EXTI
#endif
