/**
 * @file    osal.h
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
#ifndef OSAL_H
#define OSAL_H
#include "types.h"

/**
 * @brief ��ʱ����
 * @param nus:  ��ʱʱ��(��λ: us)
 * @retval None
 */
void delay_us(UINT32 nus);

/**
 * @brief ��ʱ����
 * @param nus:  ��ʱʱ��(��λ: ms), ���ֵ: 1864
 * @retval None
 */
void delay_ms(UINT32 nms);


#endif //OSAL_H
