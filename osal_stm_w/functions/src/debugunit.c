/**
 * @file    debugunit.c
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   调试单元
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 * COPYRIGHT NOTICE  
 * Copyright 2016, wsf 
 * All rights res
 *
 */
#include "stdafx.h"
#include "debugunit.h"

//查询BootLoader版本
static void OnGetBiosVersion(MsgTypeDef* pMsg)
{
	if (OP_GET == pMsg->opType)
	{
		CHAR* pVersion = NULL;
		UINT len = 0;
		UINT i;
		pVersion = BOOTLOADER_VERSION;
		len = sizeof(BOOTLOADER_VERSION);
		pMsg->len = len;
		for (i=0; i < len; ++i)
		{
			pMsg->data[i] =  pVersion[i];
		}
		pMsg->data[i] = '\0';
		pMsg->opType = OP_STATUS;
		
		osal_router_sendMsg(pMsg->uSerPort, pMsg);
	}
}

//查询App版本
static void OnGetAppVersion(MsgTypeDef* pMsg)
{
	if (OP_GET == pMsg->opType)
	{
		CHAR* pVersion = NULL;
		UINT len = 0;
		UINT i;
		pVersion = FRIMWARE_VERSION;
		len = sizeof(FRIMWARE_VERSION);
		pMsg->len = len;
		for (i=0; i < len; ++i)
		{
			pMsg->data[i] =  pVersion[i];
		}
		pMsg->data[i] = '\0';
		pMsg->opType = OP_STATUS;
		
		osal_router_sendMsg(pMsg->uSerPort, pMsg);
	}
}

//查询芯片ID
static void OnGetChipID(MsgTypeDef* pMsg)
{
	if (OP_GET == pMsg->opType)
	{
		pMsg->len = HalCpuGetID(pMsg->data);
		pMsg->opType = OP_STATUS;
		
		osal_router_sendMsg(pMsg->uSerPort, pMsg);
	}
}

static void OnDebugMsg(MsgTypeDef* pMsg)
{
	switch (pMsg->functionID)
	{
	case GET_BIOS_VERSION:
		OnGetBiosVersion(pMsg);
		break;
	case GET_APP_VERSION:
		OnGetAppVersion(pMsg);
		break;
	case GET_CHIP_ID:
		OnGetChipID(pMsg);
		break;
	default:
		break;
	}
}

/**
 * @brief 调试单元消息处理
 * @param pMsg 接收到的消息句柄
 * @return void
 */
void DebugUnit_OnMsgEvent(MsgTypeDef* pMsg)
{
	//DBG(TRACE("DebugUnit_OnMsgEvent\r\n"));
	switch (pMsg->blockID)
	{
	case DEBUG_UNIT:
		OnDebugMsg(pMsg);
		break;
	default:
		break;
	}
}
