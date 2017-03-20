/**
 * @file    osal_error.c
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   ������ģ��
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
#include "osal_error.h"

static UINT16 m_uLastError = 0;

/**
 * @brief �������ĳ���״̬
 * @param uErr �������
 * @return void
 */
void osal_error_setLastError(UINT16 uErr)
{
	m_uLastError = uErr;
}


/**
 * @brief ��ȡִ�е����ĳ���״̬
 * @param None
 * @return ������� @ref ErrorCode
 */
UINT16 osal_error_getLastError(void)
{
	return m_uLastError;
}
