/**
 * @file    application.c
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
#include "stdafx.h"
#include "osal_stm_w_cfg.h"

#include "application.h"
#include "debugunit.h"

#if !ENABLE_BOOTLOADER_CODE


/**
 * @brief ϵͳ��ʼ������Ϣ����
 * @param pMsg ���յ�����Ϣ���
 * @return void
 */
static void UpdateUint_OnStart(MsgTypeDef* pMsg)
{
	if (pMsg->functionID != START)
		return;
	if (pMsg->len != 2)
		return;
	HalIapJmp2addr(HAL_BOOTLOADER_BASE_ADDR);
}
/**
 * @brief ϵͳ����׼�������Ϣ����
 * @param pMsg ���յ�����Ϣ���
 * @return void
 */
static void UpdateUint_OnReady(MsgTypeDef* pMsg)
{
}
/**
 * @brief ϵͳ������������Ϣ����
 * @param pMsg ���յ�����Ϣ���
 * @return void
 */
static void UpdateUint_OnUPDATING(MsgTypeDef* pMsg)
{
}
/**
 * @brief ��������������� 
 * @param pMsg ���յ�����Ϣ���
 * @return void
 */
static void UpdateUint_OnResult(MsgTypeDef* pMsg)
{
}
/**
 * @brief ���������������
 * @param pMsg ���յ�����Ϣ���
 * @return void
 */
static void UpdateUint_OnLastResult(MsgTypeDef* pMsg)
{
}
/**
 * @brief ϵͳ���������Ϣ����
 * @param pMsg ���յ�����Ϣ���
 * @return void
 */
static void UpdateUint_Onend(MsgTypeDef* pMsg)
{
}

/**
 * @brief ͨѶ��������¼��ص�����
 * @param uChannel �����¼�ͨ������ַ
 * @param bIsConneted ָ��ͨ���Ƿ����ߣ�TRUE ָ��ͨ�����ߣ� FALSE ָ��ͨ������
 * @return void
 */
static void CommRouter_OnConnetEnent(UINT16 uAddr, BOOL bIsConneted)
{
}

/**
 * @brief ����·����Ϣ
 * @param pMsg �����͵���Ϣ���
 * @return void
 */
static void CommRouter_SendMsg(MsgTypeDef* pMsg)
{
}

/* ϵͳ������Ԫʵ�� */	
static UpdateUnitCBack_t m_hUpdateInstance = 
{
	UpdateUint_OnStart,
	UpdateUint_OnReady,
	UpdateUint_OnUPDATING,
	UpdateUint_OnResult,
	UpdateUint_OnLastResult,
	UpdateUint_Onend
};

/* ·�ɵ�Ԫʵ�� */
static OSALRouterCBack_t m_hRouterInstance =
{
	CommRouter_OnConnetEnent,
	CommRouter_SendMsg
};

/**
 * @brief ���Ե�Ԫ��Ϣ������
 */
static void OnCommMsgEvent(MsgTypeDef* pMsg)
{
	//TODO: Add your codes here.
	switch (pMsg->blockID)
	{
	case DEBUG_UNIT:
		DebugUnit_OnMsgEvent(pMsg);
	break;
	default:
		break;
	}
}

/**
 * @brief ���Ե�Ԫ��Ϣ������
 */
static void OnDebugMsgEvent(MsgTypeDef* pMsg)
{
	pMsg->uSerPort = OSAL_ROUTE_PORT0;//��¼��ϢԴ�˿�
	if (osal_router_OnCommMsg(pMsg))
	{
		//TODO: Add your codes here.
		OnCommMsgEvent(pMsg);
	}
}

/**
 * @brief application��ʼ��
 * @param None
 * @retval None
 */
void application_init(void)
{
	CommTypeDef* hComm = NULL;
	//UCHAR blocks[] = {UPDATE_UNIT, DEBUG_UNIT, ROUTER_UNIT};
	
	//���Ե�Ԫ��ʼ�� 
	hComm = comm_getInstance(COMM_CHANNEL0);
	hComm->init(hal_uart_getinstance(HAL_UART1));
	hComm->add_rx_obser(OnDebugMsgEvent);
	osal_router_setCommPort(hComm, OSAL_ROUTE_PORT0);
	
	osal_router_init(&m_hRouterInstance);
	bd_updateunit_Init(&m_hUpdateInstance);
	
}

#endif //!ENABLE_BOOTLOADER_CODE
