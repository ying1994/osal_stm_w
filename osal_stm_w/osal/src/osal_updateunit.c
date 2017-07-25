/**
 * @file    bd_updateunit.c
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   ������Ԫ
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


/* ϵͳ������Ԫ�ص��������� */
UpdateUnitCBack_t* m_hCallBackInstance = NULL;

/**
 * @brief ϵͳ������Ԫ��Ϣ����
 * @param pMsg ������Ϣ������
 * @return void
 */
void osal_updateunit_OnCommMsg(MsgTypeDef* pMsg)
{
	if (NULL == m_hCallBackInstance)
		return;
	if (!osal_router_checkAddress(pMsg->uAddr))//���Ǳ�������Ϣ
		return;
	if (pMsg->blockID != UPDATE_UNIT)//����ϵͳ������Ϣ
		return;
	
	switch (pMsg->functionID)
	{
	case START:/* ��ʼ���� */
		if (m_hCallBackInstance->OnStart != NULL)
			m_hCallBackInstance->OnStart(pMsg);
		break;
	case READY:/* ����׼����� */
		if (m_hCallBackInstance->OnReady != NULL)
			m_hCallBackInstance->OnReady(pMsg);
		break;
	case UPDATING:/* ������ */
		if (m_hCallBackInstance->OnUPDATING != NULL)
			m_hCallBackInstance->OnUPDATING(pMsg);
		break;
	case END:/* ������� */
		if (m_hCallBackInstance->Onend != NULL)
			m_hCallBackInstance->Onend(pMsg);
		break;
	case RESULT:/* ��������������� */
		if (m_hCallBackInstance->OnResult != NULL)
			m_hCallBackInstance->OnResult(pMsg);
		break;
	case LAST_RESULT:/* ��������������� */
		if (m_hCallBackInstance->OnLastResult != NULL)
			m_hCallBackInstance->OnLastResult(pMsg);
		break;
	default:
		break;
	}
}


/**
 * @brief ϵͳ������Ԫ��ʼ��
 * @param pMsg ������Ϣ������
 * @return void
 */
void osal_updateunit_Init(UpdateUnitCBack_t *hCallBacks)
{
	m_hCallBackInstance = hCallBacks;
}

#endif // (defined(CFG_OSAL_COMM) && defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_UPDATEUNIT))
