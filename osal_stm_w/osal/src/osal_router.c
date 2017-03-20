/**
 * @file    osal_router.c
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   ϵͳ·��ת������ģ��
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
#include "osal_stm_w_cfg.h"

#include "comm.h"
#include "osal_router.h"
#include "osal_error.h"


/* ·�ɵĶ˿��շ�ʵ����� */
static CommTypeDef* m_hCommHandle[COMM_CHANNEL_SIZE] = {NULL};
/* ������Ϣ�������� */
static OSALRouterCBack_t* m_hRouterInstance = NULL;
/* ����ַ */
static UINT16 m_uAddress = 0;

//�豸��ַ
static void OnDeviceAddress(MsgTypeDef* pMsg)
{
	UINT16 uAddr = 0;
	if (OP_SETGET == pMsg->opType)
	{
		uAddr = (pMsg->data[0] << 8) | pMsg->data[1];
		uAddr = osal_sock_ntohs(uAddr);
		uAddr = osal_router_setAddress(uAddr);
	}
	if (OP_GET & pMsg->opType)
	{
		uAddr = osal_router_getAddress();
		uAddr = osal_sock_htons(uAddr);
		pMsg->data[0] = (UCHAR)(uAddr >> 8);
		pMsg->data[1] = (UCHAR)(uAddr);
		pMsg->len = 2;
		pMsg->opType = OP_STATUS;
		
		osal_router_sendMsg(pMsg->uSerPort, pMsg);
	}
}

static void OnCommMsg(MsgTypeDef* pMsg)
{
	switch(pMsg->functionID)
	{
	case COMM_HANDLE:
		//if (pMsg->opType & OP_GET)
		{
			pMsg->opType = OP_STATUS;
			osal_router_sendMsg(pMsg->uSerPort, pMsg);
		}
		break;
	case DEV_ADDR:
		OnDeviceAddress(pMsg);
		break;
	default:
		break;
	}
}
/**
 * @brief ������Ϣ·��ת��
 * @param pMsg ���յ�����Ϣ���
 * @return �Ƿ��Ǳ�����Ϣ
 * @retval TRUE ������Ϣ�� FALSE �Ǳ�����Ϣ
 */
BOOL osal_router_OnCommMsg(MsgTypeDef* pMsg)
{
	if (osal_router_checkAddress(pMsg->uAddr))
	{
		if (m_hRouterInstance != NULL)
		{
			if (m_hRouterInstance->OnConnetEnent != NULL)
			{
				m_hRouterInstance->OnConnetEnent(pMsg->uSerPort, TRUE);
			}
		}
		switch(pMsg->blockID)
		{
		case UPDATE_UNIT:
			bd_updateunit_OnCommMsg(pMsg);
			break;
		case ROUTER_UNIT:
			OnCommMsg(pMsg);
			break;
		default:
			break;
		}
		return TRUE;
	}
	
	return FALSE;
}

/**
 * @brief ���һ��·��Э��ת���˿�
 * @param hCommHandle ·��Э��ת�������
 * @param uPortAddr Э��ת���Ķ˿ڵ�ַ
 * @return void
 */
BOOL osal_router_setCommPort(CommTypeDef* hCommHandle, UINT16 uPortAddr)
{
	if (uPortAddr >= COMM_CHANNEL_SIZE)
	{
		osal_error_setLastError(ERR_CHANNEL_NOEXIT);
		return FALSE;
	}
	if (m_hCommHandle[uPortAddr] != NULL)
		m_hCommHandle[uPortAddr]->deInit();
	
	m_hCommHandle[uPortAddr] = hCommHandle;
	
	return TRUE;
}

/**
 * @brief ���һ�����õ���Ϣ�洢�ṹ���
 * @param None
 * @return ����һ�����õ���Ϣ���
 */
MsgTypeDef* osal_router_getFreeMsgHandle(void)
{
	static MsgTypeDef msg;
	
	return &msg;
}

/**
 * @brief ·����Ϣ����
 * @param uAddr ������Ϣ��ַ
 * @param pMsg Ԥ���͵���Ϣ�洢���
 * @return BOOL ��Ϣ�����Ƿ�ɹ�
 * @retval TRUE ��Ϣ���ͳɹ���FALSE ��Ϣ����ʧ��
 */
BOOL osal_router_sendMsg(UINT8 uPortAddr, MsgTypeDef* pMsg)
{
	if (uPortAddr >= COMM_CHANNEL_SIZE)//�Ƿ���ַ
	{
		osal_error_setLastError(ERR_ADDR_NOTEXIT);
		return FALSE;
	}
	
	if (m_hCommHandle[uPortAddr] != NULL)
	{
		pMsg->uAddr = osal_router_getAddress();
		m_hCommHandle[uPortAddr]->transmit(pMsg);
	}
	
	return TRUE;
}

/**
 * @brief ͨѶ������������ͼ�ʱ��
 */
static void OnRouterCommHandleTimer(void)
{
	//TODO: �ڴ��������������ͨѶ����
	//MsgTypeDef* pMsg = osal_router_getFreeMsgHandle();
	//pMsg->uAddr = 0;
	//pMsg->blockID = ROUTER_UNIT;
	//pMsg->functionID = COMM_HANDLE;
	//pMsg->opType = OP_STATUS;
	//pMsg->len = 0;
	
	//osal_router_sendMsg(0, pMsg);
}

/**
 * @brief ͨѶ�������������
 */
static void OnRouterTimer(void)
{
	//TODO: �ڴ��������������ͨѶ����
}

/**
 * @brief ���ý���ַ
 * @param uAddr ���ؽ���ַ
 * @return ���ر��ؽ���ַ
 */
UINT16 osal_router_setAddress(UINT16 uAddr)
{
	m_uAddress = uAddr;
	HalFlashReadWriteE2PROW(APP_DEVICE_ADDRESS_ADDR, (UCHAR*)&uAddr, sizeof(uAddr));
	return m_uAddress;
}
/**
 * @brief ��ȡ����ַ
 * @param None
 * @return ���ر��ؽ���ַ
 */
UINT16 osal_router_getAddress(void)
{
	return m_uAddress;
}

/**
 * @brief ������ַ�Ƿ��Ǳ�����ַ
 * @param uAddr ����ַ
 * @return ��ַ�Ϸ�����TRUE, ���򷵻�FALSE
 */
BOOL osal_router_checkAddress(UINT16 uAddr)
{
	if ((uAddr == 0xffff) || (uAddr == m_uAddress))
		return TRUE;
	
	return FALSE;
}

/**
 * @brief ·�ɹ��ܵ�Ԫ��ʼ����ע��·�ɻ�������ص�����
 * @param hRouterBase ·����Ϣ����ص�������
 * @return void
 */
void osal_router_init(OSALRouterCBack_t* hRouterBase)
{
	UINT16 uAddress = 0;
	HalFlashRead(APP_DEVICE_ADDRESS_ADDR, (UCHAR*)&uAddress, sizeof(uAddress));
	if ((uAddress != 0xffff))
		m_uAddress = uAddress;
	TRACE("Device Address: %d\r\n", m_uAddress);
		
	HalSetShareTimer(OnRouterCommHandleTimer, 1000);//����������ʱ�����ڷ���������
	HalSetShareTimer(OnRouterTimer, 1000);//����������ʱ����ؽ���������¼�
	m_hRouterInstance = hRouterBase;
}

