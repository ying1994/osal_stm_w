/**
 * @file    comm5.h
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
 * All rights reserved. 
 *
 */
#ifndef COMM5_H
#define COMM5_H
#include "types.h"
#include "comm.h"

#if (defined(CFG_OSAL_COMM) && defined(CFG_OSAL_COMM5))

/**
 * @brief 获取Comm操作结构句柄
 * @param eChennal: COMM通道枚举 @ref CommChannelDef
 * @retval COMM操作结构句柄
 */
CommTypeDef* comm5_getInstance(void);

 
#endif // (defined(CFG_OSAL_COMM) && defined(CFG_OSAL_COMM5))

#endif //__COMM5_H_
