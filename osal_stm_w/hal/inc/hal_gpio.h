/**
 * @file    hal_gpio.h
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
#ifndef _HAL_GPIO_H
#define _HAL_GPIO_H

#include "hal_cfg.h"
#include "hal_types.h"

#ifdef CFG_HAL_GPIO

/**
 * @brief GPIO���Ź���ģʽ
 * 		  ����������ͬʱ������������ĸ��ù���ʱ������Ϊ�������
 *		  ���������ڽ��������븴�ù���ʱ������Ϊ���ա���������������
 */
typedef enum
{
	HAL_GPIOMode_AIN = 0x0,			/*!< ģ������ */
	HAL_GPIOMode_IN_FLOATING = 0x04,	/*!< �������� */
	HAL_GPIOMode_IPD = 0x28,			/*!< �������� */
	HAL_GPIOMode_IPU = 0x48,			/*!< �������� */
	HAL_GPIOMode_Out_OD = 0x14,		/*!< ��©��� */
	HAL_GPIOMode_Out_PP = 0x10,		/*!< ������� */
	HAL_GPIOMode_AF_OD = 0x1C,		/*!< ���ÿ�©��� */
	HAL_GPIOMode_AF_PP = 0x18		/*!< ����������� */
}HAL_GPIOMODE_TYPE;

/**
 * @brief: GPIO��ʼ��
 * @param: gpiox GPIO����
 * @param: pin GPIO�˿����ź�
 * @param: mode GPIO����ģʽ @ref HAL_GPIOMODE_TYPE
 * @return: void
 */
void HalGpioInit(HANDLE gpiox, UINT32 pin, HAL_GPIOMODE_TYPE mode);

/**
 * @brief: дGPIO����
 * @param: gpiox GPIO����
 * @param: pin GPIO�˿����ź�
 * @param: bOn GPIO��ƽ״̬��TRUEΪ�ߣ�FALSEΪ�ͣ�
 * @return: void
 */
void HalGpioWrite(HANDLE gpiox, UINT32 pin, BOOL bOn);

/**
 * @brief: ��GPIO����˿�
 * @param: gpiox GPIO����
 * @param: num GPIO�˿����ź�
 * @param: bOut GPIO�˿����ͣ�1Ϊ�����0Ϊ����
 * @return: ���Ÿߵ�ƽΪTRUE���͵�ƽΪFALSE
 */
BOOL HalGpioReadBit(HANDLE gpiox, UINT32 pin, BOOL bOut);

/**
 * @brief: ��GPIO����˿�
 * @param: gpiox GPIO����
 * @param: num GPIO�˿����ź�
 * @param: bOut GPIO�˿����ͣ�1Ϊ�����0Ϊ����
 * @return: ���Ÿߵ�ƽΪTRUE���͵�ƽΪFALSE
 */
UINT16 HalGpioRead(HANDLE gpiox, BOOL bOut);


#endif //CFG_HAL_GPIO
#endif

