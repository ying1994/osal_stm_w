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
 * @brief AT24CXX ��ʼ��
 * @param hiic I2C�������
 * @param uID �豸ID
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
 * @brief AT24CXX �ͷ���Դ
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
 * @brief AT24CXX ���ֽ�
 * @param offset ��ȡ��ַ
 * @retval ���ض�ȡ��������
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
 * @brief AT24CXX д�ֽ�
 * @param offset д���ַ
 * @param data д������
 * @retval ����д��״̬��TRUE��ʾд��ɹ���FALSE��ʾд��ʧ��
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
 * @brief ��ȡ AT24CXX оƬ
 * @param offset ���ݴ洢��ַ
 * @param buf ���ݴ洢ָ��
 * @param size �������ݴ�С
 * @retval ����ʵ�ʶ�ȡ�����ݴ�С
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
 * @brief д AT24CXX оƬ
 * @param offset ���ݴ洢��ַ
 * @param buf ���ݴ洢ָ��
 * @param size д�����ݴ�С
 * @retval ����ʵ��д������ݴ�С
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
