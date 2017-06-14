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
 * All rights res
 *
 */
#ifndef _HAL_GPIO_H
#define _HAL_GPIO_H

#include "hal_board.h"
#include "hal_types.h"

#ifdef CFG_HAL_GPIO

/**
 * @brief GPIO引脚工作模式
 * 		  当引脚用于同时具有输入输出的复用功能时需设置为复用输出
 *		  当引脚用于仅具有输入复用功能时需设置为浮空、下拉、上拉输入
 */
typedef enum
{
	HAL_GPIOMode_IN_FLOATING = 0x04,	/*!< 浮空输入 */
	HAL_GPIOMode_IPD = 0x28,			/*!< 下拉输入 */
	HAL_GPIOMode_IPU = 0x48,			/*!< 上拉输入 */
	HAL_GPIOMode_Out_OD = 0x14,		/*!< 开漏输出 */
	HAL_GPIOMode_Out_PP = 0x10,		/*!< 推挽输出 */
	HAL_GPIOMode_AF_OD = 0x1C,		/*!< 复用开漏输出 */
	HAL_GPIOMode_AF_PP = 0x18		/*!< 复用推挽输出 */
}HAL_GPIOMODE_TYPE;

/**
 * @brief: GPIO初始化
 * @param: gpiox GPIO类型
 * @param: pin GPIO端口引脚号
 * @param: mode GPIO工作模式 @ref HAL_GPIOMODE_TYPE
 * @return: void
 */
void HalGpioInit(HANDLE gpiox, UINT32 pin, HAL_GPIOMODE_TYPE mode);

/**
 * @brief: 写GPIO数据
 * @param: gpiox GPIO类型
 * @param: pin GPIO端口引脚号
 * @param: bOn GPIO电平状态（TRUE为高，FALSE为低）
 * @return: void
 */
void HalGpioWrite(HANDLE gpiox, UINT32 pin, BOOL bOn);

/**
 * @brief: 读GPIO输入端口
 * @param: gpiox GPIO类型
 * @param: num GPIO端口引脚号
 * @param: bOut GPIO端口类型，1为输出，0为输入
 * @return: 引脚高电平为TRUE，低电平为FALSE
 */
BOOL HalGpioReadBit(HANDLE gpiox, UINT32 pin, BOOL bOut);

/**
 * @brief: 读GPIO输入端口
 * @param: gpiox GPIO类型
 * @param: num GPIO端口引脚号
 * @param: bOut GPIO端口类型，1为输出，0为输入
 * @return: 引脚高电平为TRUE，低电平为FALSE
 */
UINT16 HalGpioRead(HANDLE gpiox, BOOL bOut);


#endif //CFG_HAL_GPIO
#endif

