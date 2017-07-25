/**
 * @file    osal_net.h
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
#ifndef OSAL_NET_H
#define OSAL_NET_H
#include "stdafx.h"	
#include "types.h"
#include "global.h"

#ifdef CFG_USE_NET
/**
 * @brief ����ַ���������Ч�ԣ��ַ�������ȫΪ0xFF��
 * @param msg ������ַ���
 * @param len �ַ�������
 * @return ������Ч����TRUE�����򷵻�FALSE
 */
BOOL osal_net_CheckParamStr(UCHAR* msg, UINT16 len);
/**
 * @brief �����ʼ��
 * @param None
 * @return void
 */
void osal_net_init(void);

#endif //CFG_USE_NET

#endif //OSAL_H
