/**
 * @file    osal_net.h
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
#ifndef OSAL_NET_H
#define OSAL_NET_H
#include "stdafx.h"	
#include "types.h"
#include "global.h"

#ifdef CFG_USE_NET
/**
 * @brief 检查字符串参数有效性（字符串不能全为0xFF）
 * @param msg 待检查字符串
 * @param len 字符串长度
 * @return 参数有效返回TRUE，否则返回FALSE
 */
BOOL osal_net_CheckParamStr(UCHAR* msg, UINT16 len);
/**
 * @brief 网络初始化
 * @param None
 * @return void
 */
void osal_net_init(void);

#endif //CFG_USE_NET

#endif //OSAL_H
