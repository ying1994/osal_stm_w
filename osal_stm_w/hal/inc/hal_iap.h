/**
 * @file    hal_iap.h
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
#ifndef _HAL_IAP_H
#define _HAL_IAP_H

#include "hal_board.h"
#include "hal_types.h"

#ifdef CFG_HAL_IAP

/**
 * @brief App程序编程
 * @param uAddress: 编程地址，绝对地址(如0x800D000)
 * @param pBuff: 数据存储指针
 * @param size: 编程数据大小
 * @retval 编程成功返回TRUE，失败返回FALSE
 */
BOOL HalIapProgramApp(UINT32 uAddress, const UCHAR *pBuff, UINT32 size);

/**
 * @brief IAP读数据
 * @param uAddress: 编程地址，绝对地址(如0x800D000)
 * @param pBuff: 数据存储指针
 * @param size: 编程数据大小
 * @retval 读取成功返回TRUE，失败返回FALSE
 */
BOOL HalIapRead(UINT32 uAddress, UCHAR *pBuff, UINT32 size);

/**
 * @brief IAP写Flash EEPROM
 * @param uAddress: 写入数据地址，绝对地址(如0x8000000)
 * @param pBuff: 数据存储指针
 * @param size: 写入数据大小
 * @retval 写入成功返回TRUE，失败返回FALSE
 */
BOOL HalIapRW_EEPROM(UINT32 uAddress, UCHAR* pBuff, UINT32 size);
/**
 * @brief 设置APP程序的可执行状态
 * @param bIsAppAvailable APP程序是否执行
 * @return void
 */
void HalIapSetIsAppActive(BOOL bIsAppAvailable);

/**
 * @brief 获取APP程序的可执行状态
 * @param None 
 * @return APP程序是否执行
 * @retval TRUE为程序可执行，FALSE为程序不可执行
 */
BOOL HalIapIsAppActive(void);

/**
 * @brief 跳转到指定的地址执行程序
 * @param uAddr: 指定跳转的目的地址
 * @retval None
 */
void HalIapJmp2addr(UINT32 uAddr);

/**
 * @brief 重新映射中断向量表
 * @param uAddr: 映射中断向量表地址
 * @retval None
 */
void HalIapRemap(UINT32 uAddr);

#endif //CFG_HAL_IAP
#endif
