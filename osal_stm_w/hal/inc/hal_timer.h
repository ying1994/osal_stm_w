/**
 * @file    bd_timer.h
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
#ifndef _HAL_TIMER_H
#define _HAL_TIMER_H

#include "hal_cfg.h"
#include "hal_types.h"

#ifdef CFG_HAL_TIMER


/**
 * @brief 独立定时器回调函数原型
 * @param None
 * @retval void
 */
 typedef void (*HalTimerCBack_t)(void);

/**
 * @brief 注册一个独立定时器
 * @param hFunc 共享定时器回调函数句柄
 * @param time  定时时间长度（单位：ms）
 * @retval 注册成功返回当前定时器编号，失败返回-1
 */
INT32 HalSetIndeTimer(HalTimerCBack_t hTimerFunc, UINT32 ums);

/**
 * @brief 注册一个共享定时器
 * @param hTimerFunc 共享定时器回调函数句柄
 * @param ums  定时时间长度（单位：20ms）
 * @retval 注册成功返回当前定时器编号，失败返回-1
 */
INT32 HalSetShareTimer(HalTimerCBack_t hTimerFunc, UINT32 ums);

/**
 * @brief 通过定时器句柄注销一个定时器
 * @param hFunc 定时器回调函数句柄
 */
void HalKillTimerWithFunc(HalTimerCBack_t hTimerFunc);

/**
 * @brief 通过定时器ID注销一个定时器
 * @param uID 定时器ID
 */
void HalKillTimerWithID(INT32 uID);

/**
 * @brief 通过定时器句柄清除定时器重新计时
 * @param hFunc 定时器回调函数句柄
 */
void HalClearTimerWithFunc(HalTimerCBack_t hTimerFunc);

/**
 * @brief 通过定时器ID注销清除定时器重新计时
 * @param uID 定时器ID
 */
void HalClearTimerWithID(INT32 uID);

#endif //CFG_HAL_TIMER
#endif
