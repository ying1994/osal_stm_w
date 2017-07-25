/**
 * @file    bd_updateunit.c
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
#include "osal_updateunit.h"

#include "osal.h"
#include "osal_error.h"
#include "comm.h"


#if (defined(CFG_OSAL_COMM) && defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_UPDATEUNIT))


/* 系统升级单元回调函数集合 */
UpdateUnitCBack_t* m_hCallBackInstance = NULL;

/**
 * @brief 系统升级单元消息处理
 * @param pMsg 接收消息处理句柄
 * @return void
 */
void osal_updateunit_OnCommMsg(MsgTypeDef* pMsg)
{
	if (NULL == m_hCallBackInstance)
		return;
	if (!osal_router_checkAddress(pMsg->uAddr))//不是本结点的消息
		return;
	if (pMsg->blockID != UPDATE_UNIT)//不是系统升级消息
		return;
	
	switch (pMsg->functionID)
	{
	case START:/* 开始升级 */
		if (m_hCallBackInstance->OnStart != NULL)
			m_hCallBackInstance->OnStart(pMsg);
		break;
	case READY:/* 升级准备完成 */
		if (m_hCallBackInstance->OnReady != NULL)
			m_hCallBackInstance->OnReady(pMsg);
		break;
	case UPDATING:/* 升级中 */
		if (m_hCallBackInstance->OnUPDATING != NULL)
			m_hCallBackInstance->OnUPDATING(pMsg);
		break;
	case END:/* 升级完成 */
		if (m_hCallBackInstance->Onend != NULL)
			m_hCallBackInstance->Onend(pMsg);
		break;
	case RESULT:/* 单个升级结果反馈 */
		if (m_hCallBackInstance->OnResult != NULL)
			m_hCallBackInstance->OnResult(pMsg);
		break;
	case LAST_RESULT:/* 反馈最终升级结果 */
		if (m_hCallBackInstance->OnLastResult != NULL)
			m_hCallBackInstance->OnLastResult(pMsg);
		break;
	default:
		break;
	}
}


/**
 * @brief 系统升级单元初始化
 * @param pMsg 接收消息处理句柄
 * @return void
 */
void osal_updateunit_Init(UpdateUnitCBack_t *hCallBacks)
{
	m_hCallBackInstance = hCallBacks;
}

#endif // (defined(CFG_OSAL_COMM) && defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_UPDATEUNIT))
