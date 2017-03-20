/**
 * @file    hal_cpu.h
 * @author  WSF
 * @version V1.0.0
 * @date    2015.01.01
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
#ifndef _HAL_CPU_H
#define _HAL_CPU_H


#include "hal_board.h"
#include "hal_types.h"

#ifdef CFG_HAL_CPU

/**
 * @brief: CPU资源初始化
 * @param: void
 * @retval: void
 */
void HalCpuInit(void);

/**
 * @brief: 获取CPU唯一ID
 * @param: idBuf CPU唯一ID
 * @retval: ID的长度
 */
UCHAR HalCpuGetID(UCHAR* idBuf);

/**
 * @brief 独立看门狗初始化
 * @param utime 看门狗定时时间(s)
 * @retval void
 */
void HalIwdgInit(UINT32 utime);

/**
 * @brief: 独立看门狗喂狗
 * @param: void
 * @retval: void
 */
void HalIwdgFred(void);

/**
 * @brief 微秒级延时
 * @param nus: 延时微秒数
 * @retval None
 */
void usleep(UINT32 nus);

/**
 * @brief 毫秒级延时
 * @param nus: 延时毫秒数
 * @retval None
*/
void msleep(UINT16 nms);
	
#endif //CFG_HAL_CPU
#endif
