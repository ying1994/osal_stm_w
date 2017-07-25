/**
 * @file    osal_task.c
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


#include "osal_task.h"

#define OSAL_TASK_MAX 50	/** ��������� */

/* ������ƽṹ */
typedef struct
{
	OsalTaskCback_t hTaskHandle;	/* ������ */
	UINT32 utime;						/* ����ִ��ʱ��Ƭ������ */
	UINT32 uperiod;						/* ����ִ������ */
}OSAL_TASK_TYPE;

/* ����洢�ṹ */
static OSAL_TASK_TYPE m_hTasks[OSAL_TASK_MAX] = {NULL};

/* ��ǰ��Ч������ */
static UINT16 m_u16TaskCount = 0;

/**
 * @brief: ����һ�����񣬲�����������ӵ�����ִ�ж����С�
 * @param: ���������
 * @retval: �����
 */
 INT16 osal_task_create(OsalTaskCback_t taskfunc, UINT32 utime)
 {
	 INT16 i = 0;
	 if ((NULL == taskfunc) || (m_u16TaskCount >= OSAL_TASK_MAX))
		 return -1;
		 
	 for (i = 0; i < m_u16TaskCount; ++i)
	 {
		 if (taskfunc == m_hTasks[i].hTaskHandle)//�����Ѿ����ڣ����������
		 {
			 return i;
		 }
	 }
	 m_hTasks[m_u16TaskCount].hTaskHandle = taskfunc;
	 m_hTasks[m_u16TaskCount].utime = 0;
	 m_hTasks[m_u16TaskCount]. uperiod = utime;
	 
	 ++m_u16TaskCount;
	 
	 return m_u16TaskCount;
 }
 
/**
 * @brief: �ر�ָ�����񣬲����������������Ƴ���
 * @param: ���������
 * @retval: void
 */ 
 VOID osal_task_kill(OsalTaskCback_t taskfunc)
 {
	 INT16 i = 0;
	 if (NULL == taskfunc)
		 return;
		 
	 for (i = 0; i < m_u16TaskCount; ++i)
	 {
		 if (taskfunc == m_hTasks[i].hTaskHandle)//�ҵ�ָ������
		 {
			 break;
		 }
	 }
	 if (i < m_u16TaskCount)
	 {
		 for (; i < m_u16TaskCount; ++i)
		 {
			m_hTasks[i].hTaskHandle = m_hTasks[i+1].hTaskHandle;
			m_hTasks[i].utime = m_hTasks[i+1].utime;
			m_hTasks[i]. uperiod = m_hTasks[i+1]. uperiod;
		 }
		m_u16TaskCount -= 1;
	 }
	 
 }

/**
 * @brief: �ر���������
 * @param: void
 * @retval: void
 */
 void osal_task_killAll(void)
 {
	 m_u16TaskCount = 0;
 }
 
/**
 * @brief: ��������ִ������
 * @param taskfunc: ���������
 * @param utime:  ����ִ������
 * @retval: �����
 */
 INT16 osal_task_setRunPeriod(OsalTaskCback_t taskfunc, UINT32 utime)
 {
	 INT16 i = 0;
	 if ((NULL == taskfunc))
		 return -1;
		 
	 for (i = 0; i < m_u16TaskCount; ++i)
	 {
		 if (taskfunc == m_hTasks[i].hTaskHandle)//�����Ѿ����ڣ����������
		 {
			 m_hTasks[m_u16TaskCount].utime = 0;
			 m_hTasks[m_u16TaskCount]. uperiod = utime;
			 break;
		 }
	 }
	 
	 return i;
 }
 
/**
 * @brief: ����������main��������Ϣѭ���е��ã��Ա�֤��������������
 * @param: void
 * @retval: void
 */
 void osal_task_run(void)
 {
	 INT16 i = 0;
		 
	 for (i = 0; i < m_u16TaskCount; ++i)
	 {
		 ++m_hTasks[i].utime;
		 if (m_hTasks[i].utime >= m_hTasks[i]. uperiod)
		 {
			m_hTasks[i].hTaskHandle();
			m_hTasks[i].utime = 0;
		 }
	 }
 }
 
