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

#define OSAL_TASK_MAX 50	/** 最大任务数 */

/* 任务控制结构 */
typedef struct
{
	OsalTaskCback_t hTaskHandle;	/* 任务句柄 */
	UINT32 utime;						/* 任务执行时间片计数器 */
	UINT32 uperiod;						/* 任务执行周期 */
}OSAL_TASK_TYPE;

/* 任务存储结构 */
static OSAL_TASK_TYPE m_hTasks[OSAL_TASK_MAX] = {NULL};

/* 当前有效任务数 */
static UINT16 m_u16TaskCount = 0;

extern UINT32 g_u32SysTick;

/**
 * @brief: 创建一个任务，并将该任务添加到任务执行队列中。
 * @param: 任务函数句柄
 * @retval: 任务号
 */
 INT16 osal_task_create(OsalTaskCback_t taskfunc, UINT32 utime)
 {
	 INT16 i = 0;
	 if ((NULL == taskfunc) || (m_u16TaskCount >= OSAL_TASK_MAX))
		 return -1;
		 
	 for (i = 0; i < m_u16TaskCount; ++i)
	 {
		 if (taskfunc == m_hTasks[i].hTaskHandle)//任务已经存在，返回任务号
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
 * @brief: 关闭指定任务，并将其从任务队列中移除。
 * @param: 任务函数句柄
 * @retval: void
 */ 
 VOID osal_task_kill(OsalTaskCback_t taskfunc)
 {
	 INT16 i = 0;
	 if (NULL == taskfunc)
		 return;
		 
	 for (i = 0; i < m_u16TaskCount; ++i)
	 {
		 if (taskfunc == m_hTasks[i].hTaskHandle)//找到指定任务
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
 * @brief: 关闭所有任务
 * @param: void
 * @retval: void
 */
 void osal_task_killAll(void)
 {
	 m_u16TaskCount = 0;
 }
 
/**
 * @brief: 设置任务执行周期
 * @param taskfunc: 任务函数句柄
 * @param utime:  任务执行周期(单位：tick)
 * @retval: 任务号
 */
 INT16 osal_task_setRunPeriod(OsalTaskCback_t taskfunc, UINT32 utime)
 {
	 INT16 i = 0;
	 if ((NULL == taskfunc))
		 return -1;
		 
	 for (i = 0; i < m_u16TaskCount; ++i)
	 {
		 if (taskfunc == m_hTasks[i].hTaskHandle)//任务已经存在，返回任务号
		 {
			 m_hTasks[m_u16TaskCount].utime = 0;
			 m_hTasks[m_u16TaskCount]. uperiod = utime;
			 break;
		 }
	 }
	 
	 return i;
 }
 
 static inline UINT32 subtraction(UINT32 a, UINT32 b)
 {
	 return (a >= b) ? (a - b) : (a+(0xffffffff - b));
 }
 
 #include "stdio.h"
/**
 * @brief: 启动任务，在main函数的消息循环中调用，以保证各任务正常运行
 * @param: void
 * @retval: void
 */
 void osal_task_run(void)
 {
	 UINT16 i = 0;
		 
	 for (i = 0; i < m_u16TaskCount; ++i)
	 {
		 //++m_hTasks[i].utime;
		 if (subtraction(g_u32SysTick, m_hTasks[i].utime) >= m_hTasks[i]. uperiod)
		 {
			m_hTasks[i].hTaskHandle();
			m_hTasks[i].utime = g_u32SysTick;
		 }
	 }
 }
 
