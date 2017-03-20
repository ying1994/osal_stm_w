/**
 * @file    bd_updateunit.h
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   升级单元
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 * COPYRIGHT NOTICE  
 * Copyright 2016, wsf 
 * All rights res
 *
 */

#ifndef UPDATEUNIT_H
#define UPDATEUNIT_H

#include "types.h"
#include "comm.h"

/**
 * @brief 系统升级单元相关回调函数集
 */
typedef struct _HAL_UPDATE_BASE_CLASS
{
	/**
	 * @brief 系统开始升级消息处理
	 * @param pMsg 接收到的消息句柄
	 * @return void
	 */
	void (*OnStart)(MsgTypeDef* pMsg);
	/**
	 * @brief 系统升级准备完成消息处理
	 * @param pMsg 接收到的消息句柄
	 * @return void
	 */
	void (*OnReady)(MsgTypeDef* pMsg);
	/**
	 * @brief 系统正在升级中消息处理
	 * @param pMsg 接收到的消息句柄
	 * @return void
	 */
	void (*OnUPDATING)(MsgTypeDef* pMsg);
	/**
	 * @brief 单个升级结果反馈 
	 * @param pMsg 接收到的消息句柄
	 * @return void
	 */
	void (*OnResult)(MsgTypeDef* pMsg);
	/**
	 * @brief 反馈最终升级结果
	 * @param pMsg 接收到的消息句柄
	 * @return void
	 */
	void (*OnLastResult)(MsgTypeDef* pMsg);
	/**
	 * @brief 系统升级完成消息处理
	 * @param pMsg 接收到的消息句柄
	 * @return void
	 */
	void (*Onend)(MsgTypeDef* pMsg);
}UpdateUnitCBack_t;

/**
 * @brief 系统升级单元消息处理
 * @param pMsg 接收消息处理句柄
 * @return void
 */
void bd_updateunit_OnCommMsg(MsgTypeDef* pMsg);

/**
 * @brief 系统升级单元初始化
 * @param pMsg 接收消息处理句柄
 * @return void
 */
void bd_updateunit_Init(UpdateUnitCBack_t *hCallBacks);

#endif
