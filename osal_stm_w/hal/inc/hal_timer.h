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
 * @brief ������ʱ���ص�����ԭ��
 * @param None
 * @retval void
 */
 typedef void (*HalTimerCBack_t)(void);

/**
 * @brief ע��һ��������ʱ��
 * @param hFunc ����ʱ���ص��������
 * @param time  ��ʱʱ�䳤�ȣ���λ��ms��
 * @retval ע��ɹ����ص�ǰ��ʱ����ţ�ʧ�ܷ���-1
 */
INT32 HalSetIndeTimer(HalTimerCBack_t hTimerFunc, UINT32 ums);

/**
 * @brief ע��һ������ʱ��
 * @param hTimerFunc ����ʱ���ص��������
 * @param ums  ��ʱʱ�䳤�ȣ���λ��20ms��
 * @retval ע��ɹ����ص�ǰ��ʱ����ţ�ʧ�ܷ���-1
 */
INT32 HalSetShareTimer(HalTimerCBack_t hTimerFunc, UINT32 ums);

/**
 * @brief ͨ����ʱ�����ע��һ����ʱ��
 * @param hFunc ��ʱ���ص��������
 */
void HalKillTimerWithFunc(HalTimerCBack_t hTimerFunc);

/**
 * @brief ͨ����ʱ��IDע��һ����ʱ��
 * @param uID ��ʱ��ID
 */
void HalKillTimerWithID(INT32 uID);

/**
 * @brief ͨ����ʱ����������ʱ�����¼�ʱ
 * @param hFunc ��ʱ���ص��������
 */
void HalClearTimerWithFunc(HalTimerCBack_t hTimerFunc);

/**
 * @brief ͨ����ʱ��IDע�������ʱ�����¼�ʱ
 * @param uID ��ʱ��ID
 */
void HalClearTimerWithID(INT32 uID);

#endif //CFG_HAL_TIMER
#endif
