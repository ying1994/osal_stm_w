/**
 * @file    ht1621b.h
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   HT1621B驱动
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 * COPYRIGHT NOTICE  
 * Copyright 2016, wsf 
 * All rights res
 *
 */
#ifndef HT1621B_H
#define HT1621B_H
#include "stdafx.h"
#include "stm32f10x.h"
#include "hal_types.h"
#include "hal_board.h"

#include "hal_gpio.h"

#ifdef CFG_HT1621B

//! CS
#define HT1621B_CS_GPIO_TYPE	GPIOB 
#define HT1621B_CS_GPIO_PIN	GPIO_Pin_5 

//! RD
#define HT1621B_RD_GPIO_TYPE	GPIOB 
#define HT1621B_RD_GPIO_PIN	GPIO_Pin_3 

//! WR
#define HT1621B_WR_GPIO_TYPE	GPIOB
#define HT1621B_WR_GPIO_PIN	GPIO_Pin_8

//! DATA
#define HT1621B_DATA_GPIO_TYPE	GPIOB 
#define HT1621B_DATA_GPIO_PIN	GPIO_Pin_9 

/**
 * @brief HT1621B 命令
 */
typedef enum
{
	HT1621B_CMD_BIAS_40 = 0x40,	//LCD 1/2偏压, 可选2个公共口
	HT1621B_CMD_BIAS_42 = 0x42,	//LCD 1/3偏压, 可选2个公共口
	HT1621B_CMD_BIAS_48 = 0x48,	//LCD 1/2偏压, 可选3个公共口
	HT1621B_CMD_BIAS_4a = 0x4a,	//LCD 1/3偏压, 可选3个公共口
	HT1621B_CMD_BIAS_50 = 0x50,	//LCD 1/2偏压, 可选4个公共口
	HT1621B_CMD_BIAS_52 = 0x52,	//LCD 1/3偏压, 可选4个公共口
	
	HT1621B_CMD_SYSEN 	= 0x02,	//打开系统振荡器
	HT1621B_CMD_SYSDIS 	= 0x00,	//关闭系统振荡器
	
	HT1621B_CMD_LCDON 	= 0x06,	//打开LCD偏压发生器
	HT1621B_CMD_LCDOFF 	= 0x04,	//关闭LCD偏压发生器
	
	HT1621B_CMD_RC256  	= 0X30,	//系统时钟选择片内时钟
	HT1621B_CMD_EXT256  = 0X38,	//系统时钟选择外部时钟
}HT1621B_CMD;

/**
 * @brief HT1621B 初始化
 * @param 
 * @retval 初始化成功返回0, 否则返回-1
 */
int ht1621b_Init(void);

/**
 * @brief HT1621B 资源释放
 * @param None
 * @retval None
 */
void ht1621b_DeInit(void);

/**
 * @brief 写 HT1621B 命令
 * @param cmd 命令
 * @retval 返回实际读取的数据大小
 */
int ht1621b_Cmd(UINT8 cmd);

/**
 * @brief 读取 HT1621B 芯片
 * @param offset 数据存储地址
 * @param buf 数据存储指针
 * @param size 读出数据大小
 * @retval 返回实际读取的数据大小
 */
int ht1621b_Read(UINT8 offset, UINT8* buf, int size);

/**
 * @brief 写 HT1621B 芯片
 * @param offset 数据存储地址
 * @param buf 数据存储指针(1BYTE: size=2)
 * @param size 写入数据大小
 * @retval 返回实际写入的数据大小
 */
int ht1621b_Write(UINT8 offset, const UINT8* buf, int size);

#endif //CFG_HT1621B

#endif
