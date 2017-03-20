/**
 * @file    function.h
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   ʵ�����ݹ��ܲ������Ͷ���
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 * COPYRIGHT NOTICE
 * Copyright 2016, wsf
 * All rights reserved.
 *
 */
#ifndef FUNCTION_H
#define FUNCTION_H

/**
 * @defgroup FunctionID
 * @brief ���ݹ��ܲ������Ͷ���
 * @{
 */

/** 
 * @defgroup UPDATE_UNIT 
 * @brief ϵͳ������Ԫ
 * @{
 */
#define STOP		0x0000	/*!< ��ʼ���� */
#define START		0x0001	/*!< ��ʼ���� */
#define READY		0x0002	/*!< ����׼����� */
#define UPDATING	0x0003	/*!< ������ */
#define RESULT		0x0004	/*!< ��������������� */
#define LAST_RESULT	0x0005	/*!< ��������������� */
#define END			0x0006	/*!< ������� */

/**
 * @}
 */

/**
 * @defgroup DEBUG_UNIT
 * @brief ϵͳ���Ե�Ԫ
 * @{
 */
#define GET_APP_VERSION		0x0011 /*!< ��ȡӦ�ó���汾�� */
#define GET_BIOS_VERSION	0x0012 /*!< ��ȡBootLoader�汾�� */
#define GET_CHIP_ID			0x0013 /*!< ��ȡоƬID */
#define NET_MSG_TEST		0x0014 /*!< ��Ϣ���� */

/**
 * @}
 */

/**
 * @defgroup ROUTER_UNIT
 * @brief ·�ɵ�Ԫ
 * @{
 */
#define COMM_HANDLE			0x0021 /*!< ͨѶ���������� */
#define DEV_ADDR			0x0022 /*!< �豸��ַ */

/**
 * @}
 */



/**
 * @}
 */
#endif /* FUNCTION_H */
