/**
 * @file    osal_task.h
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

#ifndef _OSAL_TASK_H
#define _OSAL_TASK_H
#include "types.h"

#define OSAL_TASK_MAX 50	/** ��������� */

/**
 * @brief: ������ԭ�͡�Ϊ��֤�����������У����������в�������ѭ�����ڡ�
 * @param: void
 * @retval: void
 */
typedef void (*OsalTaskCback_t)(void);

/**
 * @brief: ����һ�����񣬲�����������ӵ�����ִ�ж����С�
 * @param taskfunc: ���������
 * @param utime:  ����ִ������(��λ��tick)
 * @retval: �����
 */
 INT16 osal_task_create(OsalTaskCback_t taskfunc, UINT32 utime);
 
/**
 * @brief: �ر�ָ������
 * @param: ���������
 * @retval: void
 */ 
 void osal_task_kill(OsalTaskCback_t taskfunc);

/**
 * @prief: �ر���������
 * @param: void
 * @retval: void
 */
 void osal_task_killAll(void);
 
/**
 * @brief: ��������ִ������
 * @param taskfunc: ���������
 * @param utime:  ����ִ������(��λ��tick)
 * @retval: �����
 */
 INT16 osal_task_setRunPeriod(OsalTaskCback_t taskfunc, UINT32 utime);
 
/**
 * @brief: ����������main��������Ϣѭ���е��ã��Ա�֤��������������
 * @param: void
 * @retval: void
 */
 void osal_task_run(void);

 
 
#endif
