/**
 * @file    sht2x.c
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
#include "at24cxx.h"
#include "hal_i2c.h"

#define AT24CXX_WAIT_TIME 50000
#define AT24CXX_DELAY_TIME 5000

static HALI2CTypeDef* m_hiic = NULL;

/**
 * @brief AT24CXX 初始化
 * @param hiic I2C操作句柄
 * @param uID 设备ID
 * @retval 
 */
void at24cxx_Init(HALI2CTypeDef* hiic, UINT8 uID)
{
	if (m_hiic != NULL)
	{
		m_hiic->deInit();
	}
	
	m_hiic = hiic;
	if (m_hiic != NULL)
	{
		m_hiic->init();
		m_hiic->setDeviceID(uID);
		m_hiic->setBaudrate(8);
	}
}

/**
 * @brief AT24CXX 释放资源
 * @retval 
 */
void at24cxx_deInit(void)
{
	if (m_hiic != NULL)
	{
		m_hiic->deInit();
		m_hiic = NULL;
	}
}

/**
 * @brief AT24CXX 读字节
 * @param offset 读取地址
 * @retval 返回读取到的数据
 */
UCHAR at24cxx_ReadByte(UINT16 offset)
{
	UCHAR data = 0;
	if (AT24XX_TYPE>AT24C16)
	{
		m_hiic->read(offset, &data, 1, AT24CXX_WAIT_TIME, AT24CXX_DELAY_TIME);
	}
	else
	{
		m_hiic->readEx(offset, &data, 1, AT24CXX_WAIT_TIME, AT24CXX_DELAY_TIME);
	}
	return data;
}

/**
 * @brief AT24CXX 写字节
 * @param offset 写入地址
 * @param data 写入数据
 * @retval 返回写入状态，TRUE表示写入成功，FALSE表示写入失败
 */
int at24cxx_WriteByte(UINT16 offset, UCHAR data)
{
	if (AT24XX_TYPE>AT24C16)
	{
		return m_hiic->write(offset, &data, 1, AT24CXX_WAIT_TIME);
	}
	else
	{
		return m_hiic->writeEx(offset, &data, 1, AT24CXX_WAIT_TIME);
	}
}

/**
 * @brief 读取 AT24CXX 芯片
 * @param offset 数据存储地址
 * @param buf 数据存储指针
 * @param size 读出数据大小
 * @retval 返回实际读取的数据大小
 */
int at24cxx_Read(UINT16 offset, UCHAR* buf, UINT32 size)
{
	if (AT24XX_TYPE>AT24C16)
	{
		return m_hiic->read(offset, buf, size, AT24CXX_WAIT_TIME, AT24CXX_DELAY_TIME);
	}
	else
	{
		return m_hiic->readEx(offset, buf, size, AT24CXX_WAIT_TIME, AT24CXX_DELAY_TIME);
	}
}

/**
 * @brief 写 AT24CXX 芯片
 * @param offset 数据存储地址
 * @param buf 数据存储指针
 * @param size 写入数据大小
 * @retval 返回实际写入的数据大小
 */
int at24cxx_Write(UINT16 offset, UCHAR* buf, UINT32 size)
{
	if (AT24XX_TYPE>AT24C16)
	{
		return m_hiic->write(offset, buf, size, AT24CXX_WAIT_TIME);
	}
	else
	{
		return m_hiic->writeEx(offset, buf, size, AT24CXX_WAIT_TIME);
	}
}
