/**
 * @file    hal_cpu.h
 * @author  WSF
 * @version V1.0.0
 * @date    2015.01.01
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
#ifndef _HAL_CPU_H
#define _HAL_CPU_H

#include "hal_cfg.h"
#include "hal_types.h"


/**
 * @brief: CPU��Դ��ʼ��
 * @param: void
 * @retval: void
 */
void HalCpuInit(void);

/**
 * @brief: ��ȡCPUΨһID
 * @param: idBuf CPUΨһID
 * @retval: ID�ĳ���
 */
UCHAR HalCpuGetID(UCHAR* idBuf);

/**
 * @brief �������Ź���ʼ��
 * @param utime ���Ź���ʱʱ��(s)
 * @retval void
 */
void HalIwdgInit(UINT32 utime);

/**
 * @brief: �������Ź�ι��
 * @param: void
 * @retval: void
 */
void HalIwdgFred(void);

/**
 * @brief: ���Ź�ι������
 * @param: void
 * @retval: void
 */
void FreeIWDG_task(void);

/**
 * @brief ΢�뼶��ʱ
 * @param nus: ��ʱ΢����
 * @retval None
 */
void usleep(UINT32 nus);

/**
 * @brief ���뼶��ʱ
 * @param nus: ��ʱ������
 * @retval None
*/
void msleep(UINT16 nms);

#endif
