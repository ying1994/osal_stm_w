/**
 * @file    comm3.h
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   ͨѶЭ�鴦��ģ�飬ʵ����������֡�Ľ�������Ϣ���͹���
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 * COPYRIGHT NOTICE  
 * Copyright 2016, wsf 
 * All rights Reserved
 */
#ifndef COMM3_H
#define COMM3_H
#include "types.h"
#include "comm.h"

#if (defined(CFG_OSAL_COMM) && defined(CFG_OSAL_COMM3))

/**
 * @brief ��ȡComm�����ṹ���
 * @param eChennal: COMMͨ��ö�� @ref CommChannelDef
 * @retval COMM�����ṹ���
 */
CommTypeDef* comm3_getInstance(void);

 
#endif // (defined(CFG_OSAL_COMM) && defined(CFG_OSAL_COMM3))

#endif //__COMM3_H_
