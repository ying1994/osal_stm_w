/**
 * @file    at24cxx.h
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
#ifndef _AT24CXX_H_
#define _AT24CXX_H_

#include "hal_types.h"
#include "hal_i2c.h"


//֧�ֵ�EEPROM�ͺ�
#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767  

#define AT24XX_TYPE AT24C02//����ʹ�õ�EEPROM�ͺ�

/**
 * @brief AT24CXX ��ʼ��
 * @param hiic I2C�������
 * @param uID �豸ID
 * @retval 
 */
void at24cxx_Init(HALI2CTypeDef* hiic, UINT8 uID);

/**
 * @brief AT24CXX �ͷ���Դ
 * @retval 
 */
void at24cxx_deInit(void);

/**
 * @brief AT24CXX ���ֽ�
 * @param offset ��ȡ��ַ
 * @retval ���ض�ȡ��������
 */
UCHAR at24cxx_ReadByte(UINT16 offset);

/**
 * @brief AT24CXX д�ֽ�
 * @param offset д���ַ
 * @param data д������
 * @retval ����д��״̬��TRUE��ʾд��ɹ���FALSE��ʾд��ʧ��
 */
int at24cxx_WriteByte(UINT16 offset, UCHAR data);

/**
 * @brief ��ȡ AT24CXX оƬ
 * @param offset ���ݴ洢��ַ
 * @param buf ���ݴ洢ָ��
 * @param size �������ݴ�С
 * @retval ����ʵ�ʶ�ȡ�����ݴ�С
 */
int at24cxx_Read(UINT16 offset, UCHAR* buf, UINT32 size);

/**
 * @brief д AT24CXX оƬ
 * @param offset ���ݴ洢��ַ
 * @param buf ���ݴ洢ָ��
 * @param size д�����ݴ�С
 * @retval ����ʵ��д������ݴ�С
 */
int at24cxx_Write(UINT16 offset, UCHAR* buf, UINT32 size);

#endif
