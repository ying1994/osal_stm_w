/**
 * @file    w25x16.c
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

#include "stdafx.h"
#include "w25x16.h"
#include  "hal_defs.h"
#include  "hal_types.h"

#ifdef CFG_W25X16

static HALSpiTypeDef *m_hspi = NULL;
static UINT32 uID = 0;

/**
 * @brief W25X16 ��ȡоƬ״̬
 * @param None
 * @retval ����оƬ״̬
 */
static UINT8 w25x16_GetStatus()
{
	UINT8 status = 0;
	if (NULL == m_hspi)
		return 0;
	
	HalGpioWriteBit(W25X16_CS_GPIO_TYPE, W25X16_CS_GPIO_PIN, FALSE);
	
    m_hspi->access( W25X16_CMD_RD_ST );
    status=m_hspi->access(0XFF);
	
	HalGpioWriteBit(W25X16_CS_GPIO_TYPE, W25X16_CS_GPIO_PIN, TRUE);
	
	return status;
}

/**
 * @brief W25X16 ��ʼ��
 * @param hspi: SPI�ӿڲ������
 * @retval ��ʼ���ɹ�����0, ���򷵻�-1
 */
int w25x16_Init(HALSpiTypeDef *hspi)
{
	if (NULL == hspi)
		return -1;
	
	HalGpioInit(W25X16_CS_GPIO_TYPE, W25X16_CS_GPIO_PIN, HAL_GPIOMode_Out_PP);
	
	hspi->init(HALSpiMode_Master, HALSpiDataSize_8b, HALSpiCPOL_High, HALSpiCPHA_2Edge, HALSpiBaudRate_128, HALSpiFirstBit_MSB);
	
	m_hspi = hspi;
	
	//��ֹд����
	HalGpioWriteBit(W25X16_CS_GPIO_TYPE, W25X16_CS_GPIO_PIN, FALSE);
	m_hspi->access( W25X16_CMD_WR_DI );
	HalGpioWriteBit(W25X16_CS_GPIO_TYPE, W25X16_CS_GPIO_PIN, TRUE);
	
	//��ȡоƬID
	HalGpioWriteBit(W25X16_CS_GPIO_TYPE, W25X16_CS_GPIO_PIN, FALSE);
	m_hspi->access( W25X16_CMD_JEDEC_ID );
	uID = m_hspi->access(0XFF);
	uID |= (m_hspi->access(0XFF) << 8);
	uID |= (m_hspi->access(0XFF) << 16);
	HalGpioWriteBit(W25X16_CS_GPIO_TYPE, W25X16_CS_GPIO_PIN, TRUE);

	//д״̬
	HalGpioWriteBit(W25X16_CS_GPIO_TYPE, W25X16_CS_GPIO_PIN, FALSE);
	m_hspi->access( W25X16_CMD_WR_ST );
	m_hspi->access(0);
	HalGpioWriteBit(W25X16_CS_GPIO_TYPE, W25X16_CS_GPIO_PIN, TRUE);
	
	return 0;
}

/**
 * @brief W25X16 ��Դ�ͷ�
 * @param None
 * @retval None
 */
void w25x16_DeInit(void)
{
	if (m_hspi != NULL)
	{
		m_hspi->access( W25X16_CMD_WR_DI );
		m_hspi->deInit();
		m_hspi = NULL;
	}
	HalGpioInit(W25X16_CS_GPIO_TYPE, W25X16_CS_GPIO_PIN, HAL_GPIOMode_IN_FLOATING);
}
/**
 * @brief ��ȡ W25X16 оƬID
 * @param None
 * @retval None
 */
UINT32 w25x16_GetID(void)
{
	return uID;
}

#define WAIT_BUSY while( w25x16_GetStatus() & (0x01))

/**
 * @brief ��ȡ W25X16 оƬ
 * @param offset ���ݴ洢��ַ
 * @param buf ���ݴ洢ָ��
 * @param size �������ݴ�С
 * @retval ����ʵ�ʶ�ȡ�����ݴ�С
 */
UINT32 w25x16_Read(UINT32 offset, UINT8* buf, UINT32 size)
{
	UINT32 i = 0;
	if (NULL == m_hspi)
		return 0;
	
	//��ֹд����
	HalGpioWriteBit(W25X16_CS_GPIO_TYPE, W25X16_CS_GPIO_PIN, FALSE);
	m_hspi->access( W25X16_CMD_WR_DI );
	HalGpioWriteBit(W25X16_CS_GPIO_TYPE, W25X16_CS_GPIO_PIN, TRUE);
	
	//������
	HalGpioWriteBit(W25X16_CS_GPIO_TYPE, W25X16_CS_GPIO_PIN, FALSE);
	
	m_hspi->access( W25X16_CMD_READ );
	m_hspi->access( (offset >> 16) & 0xff );
	m_hspi->access( (offset >> 8) & 0xff );
	m_hspi->access( offset & 0xff );
	
	for (i = 0; i < size; i++)
		buf[i] = m_hspi->access(0xff);
	
	HalGpioWriteBit(W25X16_CS_GPIO_TYPE, W25X16_CS_GPIO_PIN, TRUE);
	
	return size;
}

/**
 * @brief д W25X16 оƬ
 * @param offset ���ݴ洢��ַ
 * @param buf ���ݴ洢ָ��
 * @param size д�����ݴ�С
 * @retval ����ʵ��д������ݴ�С
 */
UINT32 w25x16_Write(UINT32 offset, const UINT8* buf, UINT32 size)
{
	static UCHAR byFlashData[W25X16_SECTOR_SIZE] = {0};

	UINT32 uWriteAddr = 0;
	UINT32 uBaseAddr = 0;
	BOOL bValue = FALSE;
	UINT32 idx = 0;
	UINT32 idxbuf = 0;
	UINT32 i;
	
	if (NULL == m_hspi)
		return 0;

	//д������������󣬲�����
	if ((offset + size) > W25X16_EEPROM_SIZE)
		return FALSE;
	
	uBaseAddr = offset % W25X16_SECTOR_SIZE;
	uWriteAddr = offset - (offset % W25X16_SECTOR_SIZE);

	for (; uWriteAddr < (offset+size); uWriteAddr += W25X16_SECTOR_SIZE)
	{
		//������д��ָ��λ��
		w25x16_Read(uWriteAddr, byFlashData, W25X16_SECTOR_SIZE);
		for (i=0; (idxbuf < size) && ((uBaseAddr + i) < W25X16_SECTOR_SIZE); ++i)
		{
			if (byFlashData[uBaseAddr + i] != buf[idxbuf])
			{
				byFlashData[uBaseAddr + i] = buf[idxbuf++];
				bValue = TRUE;
			}
		}
		
		if (bValue)//�����б仯������EEPROM
		{
			//ʹ��д����
			HalGpioWriteBit(W25X16_CS_GPIO_TYPE, W25X16_CS_GPIO_PIN, FALSE);
			m_hspi->access( W25X16_CMD_WR_EN );
			HalGpioWriteBit(W25X16_CS_GPIO_TYPE, W25X16_CS_GPIO_PIN, TRUE);
			
			//��������
			HalGpioWriteBit(W25X16_CS_GPIO_TYPE, W25X16_CS_GPIO_PIN, FALSE);
			m_hspi->access( W25X16_CMD_ERASE_4K );
			m_hspi->access( uWriteAddr );
			m_hspi->access( uWriteAddr );
			m_hspi->access( uWriteAddr );
			HalGpioWriteBit(W25X16_CS_GPIO_TYPE, W25X16_CS_GPIO_PIN, TRUE);

			WAIT_BUSY;
			
			for(idx=0; idx<W25X16_SECTOR_SIZE; )
			{
				//ʹ��д����
				HalGpioWriteBit(W25X16_CS_GPIO_TYPE, W25X16_CS_GPIO_PIN, FALSE);
				m_hspi->access( W25X16_CMD_WR_EN );
				HalGpioWriteBit(W25X16_CS_GPIO_TYPE, W25X16_CS_GPIO_PIN, TRUE);
				
				//ҳ���
				HalGpioWriteBit(W25X16_CS_GPIO_TYPE, W25X16_CS_GPIO_PIN, FALSE);
				m_hspi->access( W25X16_CMD_PAGE_PROG );
				m_hspi->access( uWriteAddr );
				m_hspi->access( uWriteAddr );
				m_hspi->access( uWriteAddr );
				uWriteAddr += W25X16_PAGE_SIZE;
				
				//д����
				for(i=0; i<W25X16_PAGE_SIZE; i++)
					m_hspi->access( byFlashData[idx++] );
				HalGpioWriteBit(W25X16_CS_GPIO_TYPE, W25X16_CS_GPIO_PIN, TRUE);
				
				WAIT_BUSY;
			}
			bValue = FALSE;
		}
		uBaseAddr = 0;
	}
	
	//��ֹд����
	HalGpioWriteBit(W25X16_CS_GPIO_TYPE, W25X16_CS_GPIO_PIN, FALSE);
	m_hspi->access( W25X16_CMD_WR_DI );
	HalGpioWriteBit(W25X16_CS_GPIO_TYPE, W25X16_CS_GPIO_PIN, TRUE);
	
	return size;
}


#endif //CFG_W25X16
