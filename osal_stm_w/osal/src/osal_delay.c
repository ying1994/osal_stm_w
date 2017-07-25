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


#include "osal_delay.h"
#include "hal_board.h"

/**
 * @brief 延时函数
 * @param nus:  延时时间(单位: us), 最大值: 1864000
 * @retval None
 */
void delay_us(UINT32 nus)
{
    usleep(nus);
}

/**
 * @brief 延时函数
 * @param nus:  延时时间(单位: ms), 最大值: 1864
 * @retval None
 */
void delay_ms(UINT32 nms)
{
    msleep(nms);
}

