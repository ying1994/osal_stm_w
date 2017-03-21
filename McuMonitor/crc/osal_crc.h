#ifndef BD_CRC_H
#define BD_CRC_H


/**
 * @file    bd_crc.h
 * @author  WSF
 * @version V1.0.0
 * @date    2014.09.11
 * @brief   CRC校验模块
 ******************************************************************************
 * @attention
 *
 */
#include <qglobal.h>

/**
 * @brief 计算CRC16
 * @param uCRC16 CRC16的初始值
 * @param pData 输入数据指针
 * @param uLen 输入数据长度
 * @return UINT16 CRC校验结果
 */
quint16 bd_crc_CRC16(quint16 uCRC16, quint8* pData, quint32 uLen);

/**
 * @brief 计算CRC32
 * @param uCRC16 CRC32的初始值
 * @param pData 输入数据指针
 * @param uLen 输入数据长度
 * @return UINT32 CRC校验结果
 */
quint32 bd_crc_CRC32(quint32 uCRC32, quint8* pData, quint32 uLen);


#endif // BD_CRC_H
