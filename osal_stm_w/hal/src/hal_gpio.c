/**
 * @file    hal_gpio.c
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

#include "hal_gpio.h"

#ifdef CFG_HAL_GPIO

/**
 * @brief: GPIO��ʼ��
 * @param: gpiox GPIO����
 * @param: pin GPIO�˿����ź�
 * @param: mode GPIOģʽ
 * @return: void
 */
void HalGpioInit(HANDLE gpiox, UINT32 pin, HAL_GPIOMODE_TYPE mode)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = (UINT16)pin;				 	//�˿�����
	GPIO_InitStructure.GPIO_Mode = (GPIOMode_TypeDef)mode; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init((GPIO_TypeDef*)gpiox, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB
}

/**
 * @brief: дGPIO����
 * @param: gpiox GPIO����
 * @param: pin GPIO�˿����ź�
 * @param: bOn GPIO��ƽ״̬��TRUEΪ�ߣ�FALSEΪ�ͣ�
 * @return: void
 */
 void HalGpioWrite(HANDLE gpiox, UINT32 pin, BOOL bOn)
 {
	 if (bOn)
	 {
		 GPIO_SetBits((GPIO_TypeDef*)gpiox, (UINT16)pin);
	 }
	 else
	 {
		 GPIO_ResetBits((GPIO_TypeDef*)gpiox, (UINT16)pin);
	 }
 }

/**
 * @brief: ��GPIO����˿�
 * @param: gpiox GPIO����
 * @param: num GPIO�˿����ź�
 * @return: ���Ÿߵ�ƽΪTRUE���͵�ƽΪFALSE
 */
 BOOL HalGpioReadInputBit(HANDLE gpiox, UINT32 pin)
 {
	 if ((((GPIO_TypeDef*)gpiox)->IDR & (UINT16)pin) != (uint32_t)Bit_RESET)
		 return TRUE;
	 else
		return FALSE;
 }

/**
 * @brief: ��GPIO����˿�
 * @param: gpiox GPIO����
 * @param: pin GPIO�˿����ź�
 * @return: ���Ÿߵ�ƽΪTRUE���͵�ƽΪFALSE
 */
BOOL HalGpioReadOutputBit(HANDLE gpiox, UINT32 pin)
{
	if ((((GPIO_TypeDef*)gpiox)->ODR & (UINT16)pin) != (uint32_t)Bit_RESET)
		return TRUE;
	else
		return FALSE;
}

/**
 * @brief: ��GPIO����˿�
 * @param: gpiox GPIO����
 * @param: num GPIO�˿����ź�
 * @return: ���Ÿߵ�ƽΪTRUE���͵�ƽΪFALSE
 */
UINT16 HalGpioReadInput(HANDLE gpiox)
{
	return ((GPIO_TypeDef*)gpiox)->IDR;
}

/**
 * @brief: ��GPIO����˿�
 * @param: gpiox GPIO����
 * @param: pin GPIO�˿����ź�
 * @return: ���Ÿߵ�ƽΪTRUE���͵�ƽΪFALSE
 */
UINT16 HalGpioReadOutput(HANDLE gpiox)
{
	return ((GPIO_TypeDef*)gpiox)->ODR;
}

/**
 * @brief: ��GPIO����˿�
 * @param: gpiox GPIO����
 * @param: num GPIO�˿����ź�
 * @param: bOut GPIO�˿����ͣ�1Ϊ�����0Ϊ����
 * @return: ���Ÿߵ�ƽΪTRUE���͵�ƽΪFALSE
 */
BOOL HalGpioReadBit(HANDLE gpiox, UINT32 pin, BOOL bOut)
{
	if (bOut)
		return HalGpioReadOutputBit(gpiox, pin);
	else
		return HalGpioReadInputBit(gpiox, pin);
}

/**
 * @brief: ��GPIO����˿�
 * @param: gpiox GPIO����
 * @param: num GPIO�˿����ź�
 * @param: bOut GPIO�˿����ͣ�1Ϊ�����0Ϊ����
 * @return: ���Ÿߵ�ƽΪTRUE���͵�ƽΪFALSE
 */
UINT16 HalGpioRead(HANDLE gpiox, BOOL bOut)
{
	if (bOut)
		return HalGpioReadOutput(gpiox);
	else
		return HalGpioReadInput(gpiox);
}

#endif //CFG_HAL_GPIO
