/**
 * @file    debugunit.h
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   调试单元
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 * COPYRIGHT NOTICE  
 * Copyright 2016, wsf 
 * All rights res
 *
 */
#ifndef __DEBUGUNIT_H_
#define __DEBUGUNIT_H_

#include "comm.h"

/**
 * @brief 调试单元消息处理
 * @param pMsg 接收到的消息句柄
 * @return void
 */
void DebugUnit_OnMsgEvent(MsgTypeDef* pMsg);

#endif //__DEBUGUNIT_H_
