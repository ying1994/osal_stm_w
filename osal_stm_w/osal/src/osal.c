/**
 * @file    osal.c
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

#include "stdafx.h"	
#include "osal.h"

/**
 * @brief ��ʱ����
 * @param nus:  ��ʱʱ��(��λ: us), ���ֵ: 1864000
 * @retval None
 */
void delay_us(UINT32 nus)
{
    usleep(nus);
}

/**
 * @brief ��ʱ����
 * @param nus:  ��ʱʱ��(��λ: ms), ���ֵ: 1864
 * @retval None
 */
void delay_ms(UINT32 nms)
{
    msleep(nms);
}

