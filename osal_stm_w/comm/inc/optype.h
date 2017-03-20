/**
 * @file    optype.h
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   ʵ�����ݲ����Ĳ������ͺͲ���״̬���壬�������ݲ����Ĺ���ID�����
 *          ������������
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 * COPYRIGHT NOTICE  
 * Copyright 2016, wsf 
 * All rights reserved. 
 *
 */
#ifndef OPTYPE_H
#define OPTYPE_H


/**
 * @brief ���ݲ������Ͷ���
 */
typedef enum _OperationTypeDef
{
	OP_STATUS	= 0x00,		/*!< ���ݲ���״̬ */
	OP_SET		= 0x01,		/*!< �������ò��� */
	OP_GET		= 0x02,		/*!< �������ݶ�ȡ���� */
	OP_SETGET	= 0x03,		/*!< �������ü���ȡ���� */
	OP_ERROR	= 0xff		/*!< ����ִ�г��� */
}OperationTypeDef;



#endif /* OPTYPE_H */
 
