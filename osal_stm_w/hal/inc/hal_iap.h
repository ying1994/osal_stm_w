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
 * All rights Reserved
 *
 */
#ifndef _HAL_IAP_H
#define _HAL_IAP_H

#include "hal_cfg.h"
#include "hal_types.h"

#ifdef CFG_HAL_IAP

/**
 * @brief App������
 * @param uAddress: ��̵�ַ�����Ե�ַ(��0x800D000)
 * @param pBuff: ���ݴ洢ָ��
 * @param size: ������ݴ�С
 * @retval ��̳ɹ�����TRUE��ʧ�ܷ���FALSE
 */
BOOL HalIapProgramApp(UINT32 uAddress, const UCHAR *pBuff, UINT32 size);

/**
 * @brief IAP������
 * @param uAddress: ��̵�ַ�����Ե�ַ(��0x800D000)
 * @param pBuff: ���ݴ洢ָ��
 * @param size: ������ݴ�С
 * @retval ��ȡ�ɹ�����TRUE��ʧ�ܷ���FALSE
 */
BOOL HalIapRead(UINT32 uAddress, UCHAR *pBuff, UINT32 size);

/**
 * @brief IAPдFlash EEPROM
 * @param uAddress: д�����ݵ�ַ�����Ե�ַ(��0x8000000)
 * @param pBuff: ���ݴ洢ָ��
 * @param size: д�����ݴ�С
 * @retval д��ɹ�����TRUE��ʧ�ܷ���FALSE
 */
BOOL HalIapRW_EEPROM(UINT32 uAddress, UCHAR* pBuff, UINT32 size);
/**
 * @brief ����APP����Ŀ�ִ��״̬
 * @param bIsAppAvailable APP�����Ƿ�ִ��
 * @return void
 */
void HalIapSetIsAppActive(BOOL bIsAppAvailable);

/**
 * @brief ��ȡAPP����Ŀ�ִ��״̬
 * @param None 
 * @return APP�����Ƿ�ִ��
 * @retval TRUEΪ�����ִ�У�FALSEΪ���򲻿�ִ��
 */
BOOL HalIapIsAppActive(void);

/**
 * @brief ��ת��ָ���ĵ�ִַ�г���
 * @param uAddr: ָ����ת��Ŀ�ĵ�ַ
 * @retval None
 */
void HalIapJmp2addr(UINT32 uAddr);

/**
 * @brief ����ӳ���ж�������
 * @param uAddr: ӳ���ж��������ַ
 * @retval None
 */
void HalIapRemap(UINT32 uAddr);

#endif //CFG_HAL_IAP
#endif
