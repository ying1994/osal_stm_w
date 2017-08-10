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


//支持的EEPROM型号
#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767  

#define AT24XX_TYPE AT24C02//板子使用的EEPROM型号

/**
 * @brief AT24CXX 初始化
 * @param hiic I2C操作句柄
 * @param uID 设备ID
 * @retval 
 */
void at24cxx_Init(HALI2CTypeDef* hiic, UINT8 uID);

/**
 * @brief AT24CXX 释放资源
 * @retval 
 */
void at24cxx_deInit(void);

/**
 * @brief AT24CXX 读字节
 * @param offset 读取地址
 * @retval 返回读取到的数据
 */
UCHAR at24cxx_ReadByte(UINT16 offset);

/**
 * @brief AT24CXX 写字节
 * @param offset 写入地址
 * @param data 写入数据
 * @retval 返回写入状态，TRUE表示写入成功，FALSE表示写入失败
 */
int at24cxx_WriteByte(UINT16 offset, UCHAR data);

/**
 * @brief 读取 AT24CXX 芯片
 * @param offset 数据存储地址
 * @param buf 数据存储指针
 * @param size 读出数据大小
 * @retval 返回实际读取的数据大小
 */
int at24cxx_Read(UINT16 offset, UCHAR* buf, UINT32 size);

/**
 * @brief 写 AT24CXX 芯片
 * @param offset 数据存储地址
 * @param buf 数据存储指针
 * @param size 写入数据大小
 * @retval 返回实际写入的数据大小
 */
int at24cxx_Write(UINT16 offset, UCHAR* buf, UINT32 size);

#endif
