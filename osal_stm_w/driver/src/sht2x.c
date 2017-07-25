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
#include "sht2x.h"
#include "hal_i2c.h"

#define SHT2X_WAIT_TIME 50000

static const UINT16 POLYNOMIAL = 0x131;

static HALI2CTypeDef* m_hiic = NULL;
static UCHAR m_szDataBuf[4] = {0};

/**
 * @brief SHT2X 检查数据正确性
 * @param data 读取到的数据
 * @param nb 需要校验的数量
 * @retval 返回校验值
 */
UINT8 sht2x_CalCRC(UCHAR *data, INT16 nb)
{
    UINT8 crc = 0;
    UCHAR bit = 0;
    INT16 i = 0;
	
    //calculates 8-Bit checksum with given polynomial
    for(i = 0; i < nb; ++i)
    {
        crc ^= (data[i]);
        for ( bit = 8; bit > 0; --bit)
        {
            if (crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
            else crc = (crc << 1);
        }
    }
	
	return crc;
}

/**
 * @brief SHT2X 复位
 */
void sht2x_reset(void)
{
	if (m_hiic != NULL)
		m_hiic->write(SHT2X_SOFT_RESET, NULL, 0, 5000);
}

/**
 * @brief SHT2X 读取温度值
 * @param[out] tem 温度值
 * @retval 操作成功返回0, 失败返回-1
 */
int sht2x_GetTemperture(float *tem)
{
	UINT16 uTem = 0;
	if ((m_hiic != NULL) && (m_hiic->read(SHT2X_Measurement_T_HM, m_szDataBuf, 3, SHT2X_WAIT_TIME, 70000) > 0))
	{
		//if (sht2x_CalCRC(m_szDataBuf, 2) == m_szDataBuf[2])
		{
			uTem = (m_szDataBuf[0] << 8) + m_szDataBuf[1];
			uTem &= ~0x0003;           // clear bits [1..0] (status bits)
			
			//T= -46.85 + 175.72 * ST/2^16
			*tem = -46.85 + 175.72 / 65536 * (float)uTem; 
			return 0;
		}
	}

	return -1;
}

/**
 * @brief SHT2X 读取湿度值
 * @param[out] hum 湿度值
 * @retval 操作成功返回0, 失败返回-1
 */
int sht2x_GetHumidity(float *hum)
{
	UINT16 uHum = 0;
	if ((m_hiic != NULL) && (m_hiic->read(SHT2X_Measurement_RH_HM, m_szDataBuf, 3, SHT2X_WAIT_TIME, 30000) > 0))
	{
		//if (sht2x_CalCRC(m_szDataBuf, 2) == m_szDataBuf[2])
		{
			uHum = (m_szDataBuf[0] << 8) + m_szDataBuf[1];
			uHum &= ~0x0003;          // clear bits [1..0] (status bits)
			
			//Hum = -6.0 + 125.0/65536 * (float)uHum; // RH= -6 + 125 * SRH/2^16
			*hum = ((float)uHum * 0.00190735) - 6;
	
			return 0;
		}
	}

	return -1;
}

/**
 * @brief SHT2X 读取用户寄存器
 */
UINT8 sht2x_read_user_reg(void)
{
	UINT8 val = 0;
	if (NULL != m_hiic)
		m_hiic->read(SHT2X_READ_REG, &val, 1, SHT2X_WAIT_TIME, 3000);
	return val;
}

/**
 * @brief SHT2X 初始化
 * @param hiic I2C操作句柄
 * @param uID 设备ID
 * @retval 
 */
void sht2x_Init(HALI2CTypeDef* hiic, UINT8 uID)
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
		sht2x_reset();
	}
}

/**
 * @brief SHT2X 释放资源
 * @retval 
 */
void sht2x_deInit(void)
{
	if (m_hiic != NULL)
	{
		m_hiic->deInit();
		m_hiic = NULL;
	}
}
