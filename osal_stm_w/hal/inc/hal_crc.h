/**
 * @file    hal_crc.h
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
#ifndef _HAL_CRC_H
#define _HAL_CRC_H

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */

#include "hal_board.h"
#include "hal_types.h"


#ifdef CFG_HAL_CRC

/**
 * @brief 计算CRC16
 * @param uCRC16 CRC16的初始值
 * @param pData 输入数据指针
 * @param uLen 输入数据长度
 * @return UINT16 CRC校验结果
 */
UINT16 HalCRC16_8005(UINT16 uCRC16, UCHAR * pData, UINT16 Len);
/**
 * @brief 计算CRC16
 * @param uCRC16 CRC16的初始值
 * @param pData 输入数据指针
 * @param uLen 输入数据长度
 * @return UINT16 CRC校验结果
 */
UINT16 HalCRC16_CCITT(UINT16 uCRC16, UCHAR* pData, UINT16 uLen);

/**
 * @brief 计算CRC16
 * @param uCRC16 CRC16的初始值
 * @param pData 输入数据指针
 * @param uLen 输入数据长度
 * @return UINT16 CRC校验结果
 */
UINT16 HalCRC16(UINT16 uCRC16, UCHAR* pData, UINT16 uLen);

/**
 * @brief 计算CRC32
 * @param uCRC16 CRC32的初始值
 * @param pData 输入数据指针
 * @param uLen 输入数据长度
 * @return UINT32 CRC校验结果
 */
UINT32 HalCRC32(UINT32 uCRC32, UCHAR* pData, UINT32 uLen);



#endif //CFG_HAL_CRC
#endif
/**************************************************************************************************
 */
