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
 * @brief: GPIO初始化
 * @param: gpiox GPIO类型
 * @param: pin GPIO端口引脚号
 * @param: mode GPIO模式
 * @return: void
 */
void HalGpioInit(HANDLE gpiox, UINT32 pin, HAL_GPIOMODE_TYPE mode)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = (UINT16)pin;				 	//端口配置
	GPIO_InitStructure.GPIO_Mode = (GPIOMode_TypeDef)mode; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init((GPIO_TypeDef*)gpiox, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB
}

/**
 * @brief: 写GPIO数据
 * @param: gpiox GPIO类型
 * @param: value GPIO写入的值
 * @return: void
 */
void HalGpioWrite(HANDLE gpiox, UINT16 value)
{
	GPIO_Write(gpiox, value);
}

/**
 * @brief: 写GPIO数据
 * @param: gpiox GPIO类型
 * @param: pin GPIO端口引脚号
 * @param: bOn GPIO电平状态（TRUE为高，FALSE为低）
 * @return: void
 */
 void HalGpioWriteBit(HANDLE gpiox, UINT32 pin, BOOL bOn)
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
 * @brief: 读GPIO输入端口
 * @param: gpiox GPIO类型
 * @param: num GPIO端口引脚号
 * @return: 引脚高电平为TRUE，低电平为FALSE
 */
 BOOL HalGpioReadInputBit(HANDLE gpiox, UINT32 pin)
 {
	 if ((((GPIO_TypeDef*)gpiox)->IDR & (UINT16)pin) != (uint32_t)Bit_RESET)
		 return TRUE;
	 else
		return FALSE;
 }

/**
 * @brief: 读GPIO输出端口
 * @param: gpiox GPIO类型
 * @param: pin GPIO端口引脚号
 * @return: 引脚高电平为TRUE，低电平为FALSE
 */
BOOL HalGpioReadOutputBit(HANDLE gpiox, UINT32 pin)
{
	if ((((GPIO_TypeDef*)gpiox)->ODR & (UINT16)pin) != (uint32_t)Bit_RESET)
		return TRUE;
	else
		return FALSE;
}

/**
 * @brief: 读GPIO输入端口
 * @param: gpiox GPIO类型
 * @param: num GPIO端口引脚号
 * @return: 引脚高电平为TRUE，低电平为FALSE
 */
UINT16 HalGpioReadInput(HANDLE gpiox)
{
	return ((GPIO_TypeDef*)gpiox)->IDR;
}

/**
 * @brief: 读GPIO输出端口
 * @param: gpiox GPIO类型
 * @param: pin GPIO端口引脚号
 * @return: 引脚高电平为TRUE，低电平为FALSE
 */
UINT16 HalGpioReadOutput(HANDLE gpiox)
{
	return ((GPIO_TypeDef*)gpiox)->ODR;
}

/**
 * @brief: 读GPIO输入端口
 * @param: gpiox GPIO类型
 * @param: num GPIO端口引脚号
 * @param: bOut GPIO端口类型，1为输出，0为输入
 * @return: 引脚高电平为TRUE，低电平为FALSE
 */
BOOL HalGpioReadBit(HANDLE gpiox, UINT32 pin, BOOL bOut)
{
	if (bOut)
		return HalGpioReadOutputBit(gpiox, pin);
	else
		return HalGpioReadInputBit(gpiox, pin);
}

/**
 * @brief: 读GPIO输入端口
 * @param: gpiox GPIO类型
 * @param: num GPIO端口引脚号
 * @param: bOut GPIO端口类型，1为输出，0为输入
 * @return: 引脚高电平为TRUE，低电平为FALSE
 */
UINT16 HalGpioRead(HANDLE gpiox, BOOL bOut)
{
	if (bOut)
		return HalGpioReadOutput(gpiox);
	else
		return HalGpioReadInput(gpiox);
}

#endif //CFG_HAL_GPIO
