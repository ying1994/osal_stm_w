/**
 * @file    debugunit.c
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   ���Ե�Ԫ
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

//��ѯBootLoader�汾
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

//��ѯApp�汾
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

//��ѯоƬID
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
 * @brief ���Ե�Ԫ��Ϣ����
 * @param pMsg ���յ�����Ϣ���
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
