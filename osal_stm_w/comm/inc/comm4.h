/**
 * @file    comm4.h
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
#ifndef COMM4_H
#define COMM4_H
#include "types.h"
#include "comm.h"


/**
 * @brief 获取Comm操作结构句柄
 * @param eChennal: COMM通道枚举 @ref CommChannelDef
 * @retval COMM操作结构句柄
 */
CommTypeDef* comm4_getInstance(void);

 
#endif //__COMM4_H_
