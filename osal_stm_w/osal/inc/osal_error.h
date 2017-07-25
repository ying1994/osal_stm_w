/**
 * @file    osal_error.h
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
 * All rights Reserved
 *
 */

#ifndef ERROR_H
#define ERROR_H
#include "types.h"

/** 
 * @defgroup ErrorCode 
 * @brief ϵͳִ�г���Ĵ������
 * @{
 */
#define ERR_UNKNOW_ERROR        0xffff  /*!< δ֪���� */
#define ERR_NO_ERROR			0x0000  /*!< �޴��� */
#define ERR_ADDR_NOTEXIT		0x0001	/*!< �Ƿ���ַ��ָ����ַ������ */
#define ERR_CHANNEL_NOEXIT		0x0002  /*!< ָ����ͨ�������� */
#define ERR_CHECKCRC_ERROR		0x0003	/*!< CRCУ����� */
#define ERR_CHECKSIZE_ERROR		0x0004	/*!< �ļ���СУ����� */
#define ERR_SET_PARITY_FAIL     0x0005
#define ERR_SET_STOPBIT_FAIL    0x0006
#define ERR_SET_FLOWCTRL_FAIL   0x0007

/**
 * @}
 */

/**
 * @brief �������ĳ���״̬
 * @param uErr �������
 * @return void
 */
void osal_error_setLastError(UINT16 uErr);

/**
 * @brief ��ȡִ�е����ĳ���״̬
 * @param None
 * @return ������� @ref ErrorCode
 */
UINT16 osal_error_getLastError(void);


#endif
