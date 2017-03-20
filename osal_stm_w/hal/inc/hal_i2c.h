/**
 * @file    hal_i2c.h
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
#ifndef _HAL_I2C_H
#define _HAL_I2C_H

#include "hal_board.h"
#include "hal_types.h"

#ifdef CFG_HAL_I2C

#define I2C1_GPIO_TYPE	GPIOB
#define I2C1_SMBA		GPIO_Pin_5
#define I2C1_SCL		GPIO_Pin_6
#define I2C1_SDA		GPIO_Pin_7

#define I2C2_GPIO_TYPE	GPIOE
#define I2C2_SMBA		GPIO_Pin_2
#define I2C2_SCL		GPIO_Pin_3
#define I2C2_SDA		GPIO_Pin_4

/**
 * @brief I2C通道枚举 
 */
typedef enum
{
	HALI2CNumer1 = 0,	/*!< I2C通道1 */
	HALI2CNumer2,		/*!< I2C通道2 */
	HALI2CNumer_SIZE		/*!< I2C通道总数 */
}HALI2CNumer;

/** 
 * @brief I2C操作结构定义
 */
typedef struct
{
	/**
	 * @brief I2C初始化
	 * @param None
	 * @retval None
	 */
	void (*init)(void);
	
	/**
	 * @brief I2C资源注销
	 * @param None
	 * @retval None
	 */
	void (*deInit)(void);
	
	/**
	 * @brief 设置I2C设备ID
	 * @param uID: I2C设备ID
	 * @retval None
	 */
	void (*setDeviceID)(UINT16 uID);
	
	/**
	 * @brief 设置I2C波特率
	 * @param baudrate: I2C波特率
	 * @retval None
	 */
	void (*setBaudrate)(UINT32 baudrate);
	
	/**
	 * @brief 8位地址 I2C写数据
	 * @param uAddress: 写入数据地址
	 * @param pBuff: 数据指针缓冲区指针
	 * @param size: 写入数据大小
	 * @retval None
	 */
	void (*write)(UINT8 uAddress, UCHAR* pBuff, UINT32 size);
	
	/**
	 * @brief 8位地址 I2C读数据
	 * @param uAddress: 数据存储地址
	 * @param pBuff: 数据指针缓冲区指针
	 * @param size: 读出数据大小
	 * @retval None
	 */
	UCHAR (*read)(UINT8 uAddress, UCHAR* pBuff, UINT32 size);
	
	/**
	 * @brief 10位地址 I2C写数据
	 * @param uAddress: 写入数据地址
	 * @param pBuff: 数据指针缓冲区指针
	 * @param size: 写入数据大小
	 * @retval None
	 */
	void (*writeEx)(UINT16 uAddress, UCHAR* pBuff, UINT32 size);
	
	/**
	 * @brief 10位地址 I2C读数据
	 * @param uAddress: 数据存储地址
	 * @param pBuff: 数据指针缓冲区指针
	 * @param size: 读出数据大小
	 * @retval None
	 */
	UCHAR (*readEx)(UINT16 uAddress, UCHAR* pBuff, UINT32 size);
}HALI2CTypeDef;


/**
 * @brief 获取I2C操作结构句柄
 * @param eChannel: I2C 通道 @ref HALI2CNumer
 * @retval I2C操作结构句柄
 */
HALI2CTypeDef* HalI2cGetInstance(HALI2CNumer eChannel);


#endif //CFG_HAL_I2C
#endif
