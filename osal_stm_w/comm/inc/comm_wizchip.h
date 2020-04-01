/**
 * @file    comm_wizchip.h
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
 * All rights res
 */
#ifndef COMM_WIZCHIP_H
#define COMM_WIZCHIP_H
#include "types.h"
#include "comm.h"


#if (defined(CFG_OSAL_COMM) && defined(CFG_USE_NET) && defined(CFG_WIZCHIP) && defined(CFG_OSAL_COMM_WIZCHIP))
/**
 * @brief ��ȡComm�����ṹ���
 * @param sn: COMMͨ��ö�� @ref EnWizchipNetChannel
 * @retval COMM�����ṹ���
 */
CommTypeDef* comm_wizchip_getInstance(UINT8 sn);

#endif
#endif //CFG_WIZCHIP
