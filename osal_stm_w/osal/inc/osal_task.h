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

/**
 * @brief: 任务函数原型。为保证程序正常运行，在任务函数中不能有死循环存在。
 * @param: void
 * @retval: void
 */
typedef void (*OsalTaskCback_t)(void);

/**
 * @brief: 创建一个任务，并将该任务添加到任务执行队列中。
 * @param taskfunc: 任务函数句柄
 * @param utime:  任务执行周期
 * @retval: 任务号
 */
 INT16 osal_task_create(OsalTaskCback_t taskfunc, UINT32 utime);
 
/**
 * @brief: 关闭指定任务
 * @param: 任务函数句柄
 * @retval: void
 */ 
 void osal_task_kill(OsalTaskCback_t taskfunc);

/**
 * @prief: 关闭所有任务
 * @param: void
 * @retval: void
 */
 void osal_task_killAll(void);
 
/**
 * @brief: 设置任务执行周期
 * @param taskfunc: 任务函数句柄
 * @param utime:  任务执行周期
 * @retval: 任务号
 */
 INT16 osal_task_setRunPeriod(OsalTaskCback_t taskfunc, UINT32 utime);
 
/**
 * @brief: 启动任务，在main函数的消息循环中调用，以保证各任务正常运行
 * @param: void
 * @retval: void
 */
 void osal_task_run(void);

 
 
#endif
