/**
 * @file    comm2.h
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
#ifndef COMM2_H
#define COMM2_H
#include "types.h"
#include "comm.h"


/**
 * @brief ��ȡComm�����ṹ���
 * @param eChennal: COMMͨ��ö�� @ref CommChannelDef
 * @retval COMM�����ṹ���
 */
CommTypeDef* comm2_getInstance(void);

 
#endif //__COMM2_H_
