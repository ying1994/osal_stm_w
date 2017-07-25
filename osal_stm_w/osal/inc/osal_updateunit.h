/**
 * @file    bd_updateunit.h
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
 * All rights Reserved
 *
 */

#ifndef UPDATEUNIT_H
#define UPDATEUNIT_H

#include "osal.h"
#include "comm.h"

#if (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM) && defined(CFG_OSAL_UPDATEUNIT))

/**
 * @brief ϵͳ������Ԫ��ػص�������
 */
typedef struct _HAL_UPDATE_BASE_CLASS
{
	/**
	 * @brief ϵͳ��ʼ������Ϣ����
	 * @param pMsg ���յ�����Ϣ���
	 * @return void
	 */
	void (*OnStart)(MsgTypeDef* pMsg);
	/**
	 * @brief ϵͳ����׼�������Ϣ����
	 * @param pMsg ���յ�����Ϣ���
	 * @return void
	 */
	void (*OnReady)(MsgTypeDef* pMsg);
	/**
	 * @brief ϵͳ������������Ϣ����
	 * @param pMsg ���յ�����Ϣ���
	 * @return void
	 */
	void (*OnUPDATING)(MsgTypeDef* pMsg);
	/**
	 * @brief ��������������� 
	 * @param pMsg ���յ�����Ϣ���
	 * @return void
	 */
	void (*OnResult)(MsgTypeDef* pMsg);
	/**
	 * @brief ���������������
	 * @param pMsg ���յ�����Ϣ���
	 * @return void
	 */
	void (*OnLastResult)(MsgTypeDef* pMsg);
	/**
	 * @brief ϵͳ���������Ϣ����
	 * @param pMsg ���յ�����Ϣ���
	 * @return void
	 */
	void (*Onend)(MsgTypeDef* pMsg);
}UpdateUnitCBack_t;

/**
 * @brief ϵͳ������Ԫ��Ϣ����
 * @param pMsg ������Ϣ������
 * @return void
 */
void osal_updateunit_OnCommMsg(MsgTypeDef* pMsg);

/**
 * @brief ϵͳ������Ԫ��ʼ��
 * @param pMsg ������Ϣ������
 * @return void
 */
void osal_updateunit_Init(UpdateUnitCBack_t *hCallBacks);

#endif // (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM) && defined(CFG_OSAL_UPDATEUNIT))
#endif
