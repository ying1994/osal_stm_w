/**
 * @file    osal_error.c
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   出错处理模块
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 * COPYRIGHT NOTICE  
 * Copyright 2016, wsf 
 * All rights res
 *
 */


#include "osal_error.h"

static UINT16 m_uLastError = 0;

/**
 * @brief 设置最后的出错状态
 * @param uErr 出错代码
 * @return void
 */
void osal_error_setLastError(UINT16 uErr)
{
	m_uLastError = uErr;
}


/**
 * @brief 获取执行的最后的出错状态
 * @param None
 * @return 出错代码 @ref ErrorCode
 */
UINT16 osal_error_getLastError(void)
{
	return m_uLastError;
}
