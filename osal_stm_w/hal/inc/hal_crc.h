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
 * @brief ����CRC16
 * @param uCRC16 CRC16�ĳ�ʼֵ
 * @param pData ��������ָ��
 * @param uLen �������ݳ���
 * @return UINT16 CRCУ����
 */
UINT16 HalCRC16_8005(UINT16 uCRC16, UCHAR * pData, UINT16 Len);
/**
 * @brief ����CRC16
 * @param uCRC16 CRC16�ĳ�ʼֵ
 * @param pData ��������ָ��
 * @param uLen �������ݳ���
 * @return UINT16 CRCУ����
 */
UINT16 HalCRC16_CCITT(UINT16 uCRC16, UCHAR* pData, UINT16 uLen);

/**
 * @brief ����CRC32
 * @param uCRC16 CRC32�ĳ�ʼֵ
 * @param pData ��������ָ��
 * @param uLen �������ݳ���
 * @return UINT32 CRCУ����
 */
UINT32 HalCRC32(UINT32 uCRC32, UCHAR* pData, UINT32 uLen);



#endif //CFG_HAL_CRC
#endif
/**************************************************************************************************
 */
