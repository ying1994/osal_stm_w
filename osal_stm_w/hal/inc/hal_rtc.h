/**
 * @file    hal_rtc.h
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
#ifndef HAL_RTC_H
#define HAL_RTC_H

#include "hal_board.h"
#include "hal_types.h"

#ifdef CFG_HAL_RTC

typedef	uint32 time_t;

/*
 * A structure for storing all kinds of useful information about the
 * current (or another) time.
 */
struct tm
{
	int	tm_sec;		/* Seconds: 0-59 (K&R says 0-61?) */
	int	tm_min;		/* Minutes: 0-59 */
	int	tm_hour;	/* Hours since midnight: 0-23 */
	int	tm_mday;	/* Day of the month: 1-31 */
	int	tm_mon;		/* Months *since* january: 0-11 */
	int	tm_year;	/* Years since 1970 */
	int	tm_wday;	/* Days since Sunday (0-6) */
	int	tm_yday;	/* Days since Jan. 1: 0-365 */
	int	tm_isdst;	/* +1 Daylight Savings Time, 0 No DST, * -1 don't know */
};

/**
 * @brief RTC时钟初始化
 * @param None
 * @retval 返回初始化状态: 0 成功, -1 失败
 */
int hal_rtc_init(void);

/**
 * @brief 设置RTC时钟
 * @param t 设置时间
 * @retval 返回当前时间
 */
time_t hal_rtc_set(struct tm* t);

/**
 * @brief 更新RTC时钟
 * @param t 读取的时间缓冲区
 * @retval 返回当前时间
 */
time_t hal_rtc_get(time_t* t);

/**
 * @brief 设置时间
 * @param t 设置时间
 * @retval 返回当前时间
 */
time_t settime(time_t t);

/**
 * @brief 获取最新时间
 * @param t 读取的时间缓冲区
 * @retval 返回当前时间
 */
time_t time(time_t *t);

#endif //CFG_HAL_RTC
#endif
