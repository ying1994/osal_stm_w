/**
 * @file    osal_error.h
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
 * All rights Reserved
 *
 */

#ifndef ERROR_H
#define ERROR_H
#include "types.h"

/** 
 * @defgroup ErrorCode 
 * @brief 系统执行出错的错误代码
 * @{
 */
#define ERR_UNKNOW_ERROR        0xffff  /*!< 未知错误 */
#define ERR_NO_ERROR			0x0000  /*!< 无错误 */
#define ERR_ADDR_NOTEXIT		0x0001	/*!< 非法地址，指定地址不存在 */
#define ERR_CHANNEL_NOEXIT		0x0002  /*!< 指定的通道不存在 */
#define ERR_CHECKCRC_ERROR		0x0003	/*!< CRC校验错误 */
#define ERR_CHECKSIZE_ERROR		0x0004	/*!< 文件大小校验错误 */
#define ERR_SET_PARITY_FAIL     0x0005
#define ERR_SET_STOPBIT_FAIL    0x0006
#define ERR_SET_FLOWCTRL_FAIL   0x0007

/**
 * @}
 */

/**
 * @brief 设置最后的出错状态
 * @param uErr 出错代码
 * @return void
 */
void osal_error_setLastError(UINT16 uErr);

/**
 * @brief 获取执行的最后的出错状态
 * @param None
 * @return 出错代码 @ref ErrorCode
 */
UINT16 osal_error_getLastError(void);


#endif
