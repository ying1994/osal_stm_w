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
 * @brief RTCʱ�ӳ�ʼ��
 * @param None
 * @retval ���س�ʼ��״̬: 0 �ɹ�, -1 ʧ��
 */
int hal_rtc_init(void);

/**
 * @brief ����RTCʱ��
 * @param t ����ʱ��
 * @retval ���ص�ǰʱ��
 */
time_t hal_rtc_set(struct tm* t);

/**
 * @brief ����RTCʱ��
 * @param t ��ȡ��ʱ�仺����
 * @retval ���ص�ǰʱ��
 */
time_t hal_rtc_get(time_t* t);

/**
 * @brief ����ʱ��
 * @param t ����ʱ��
 * @retval ���ص�ǰʱ��
 */
time_t settime(time_t t);

/**
 * @brief ��ȡ����ʱ��
 * @param t ��ȡ��ʱ�仺����
 * @retval ���ص�ǰʱ��
 */
time_t time(time_t *t);

#endif //CFG_HAL_RTC
#endif
