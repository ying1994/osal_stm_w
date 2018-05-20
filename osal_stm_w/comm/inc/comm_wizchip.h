/**
 * @file    comm_wizchip.h
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   通讯协议处理模块，实现数据物理帧的解析及消息发送功能
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


#if (defined(CFG_OSAL_COMM) && defined(CFG_USE_NET) && defined(CFG_WIZCHIP))
/**
 * @brief 获取Comm操作结构句柄
 * @param sn: COMM通道枚举 @ref EnWizchipNetChannel
 * @retval COMM操作结构句柄
 */
CommTypeDef* comm_wizchip_getInstance(UINT8 sn);

#endif
#endif //CFG_WIZCHIP
