/**
 * @file    w25x16.h
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
#ifndef W25X16_H
#define W25X16_H

#include "hal_types.h"
#include "hal_board.h"

#include "stdafx.h"

#ifdef CFG_W25X16

#define W25X16_CMD_WR_EN				0x06    //дʹ��  ��ע��xcc
#define W25X16_CMD_WR_DI				0x04    //д����
#define W25X16_CMD_RD_ST				0x05    //��״̬�Ĵ���
#define W25X16_CMD_WR_ST				0x01    //д״̬�Ĵ���
#define W25X16_CMD_READ					0x03    //������
#define W25X16_CMD_FAST_READ			0x0B    //������
#define W25X16_CMD_FAST_READ_DUAL		0x3B    //���˫���
#define W25X16_CMD_PAGE_PROG			0x02    //ҳ���  256Byte
#define W25X16_CMD_ERASE_4K				0x20    //��������:4K
#define W25X16_CMD_ERASE_64K			0xD8    //�����:64K
#define W25X16_CMD_ERASE_ALL			0xC7    //оƬ����
#define W25X16_CMD_POW_DOWN				0xB9    //����
#define W25X16_CMD_DID					0xAB    //����ID
#define W25X16_CMD_MFD_ID				0x90    //����/����ID
#define W25X16_CMD_JEDEC_ID				0x9F    //JEDEC ID

#define W25X16_PAGE_SIZE				(256UL)	//ҳ��С
#define W25X16_SECTOR_SIZE				(4*1024UL)		//������С
#define W25X16_BLOCK_SIZE				(64*1024UL)		//���С
#define W25X16_EEPROM_SIZE				(4*1024*1024UL)	//���С

/* device id define */
typedef enum _W25X_ID
{
    unknow     = 0,
    W25X16 = 0x001530EF,
    W25X32 = 0x001630EF,
    W25X64 = 0x001730EF,
}W25X_ID;

#define W25X16_CS_GPIO_TYPE	GPIOD 
#define W25X16_CS_GPIO_PIN	GPIO_Pin_5 

/**
 * @brief W25X16 ��ʼ��
 * @param hspi: SPI�ӿڲ������
 * @retval ��ʼ���ɹ�����0, ���򷵻�-1
 */
int w25x16_Init(HALSpiTypeDef *hspi);

/**
 * @brief W25X16 ��Դ�ͷ�
 * @param None
 * @retval None
 */
void w25x16_DeInit(void);

/**
 * @brief ��ȡ W25X16 оƬID
 * @param None
 * @retval None
 */
UINT32 w25x16_GetID(void);

/**
 * @brief ��ȡ W25X16 оƬ
 * @param offset ���ݴ洢��ַ
 * @param buf ���ݴ洢ָ��
 * @param size �������ݴ�С
 * @retval ����ʵ�ʶ�ȡ�����ݴ�С
 */
UINT32 w25x16_Read(UINT32 offset, UINT8* buf, UINT32 size);

/**
 * @brief д W25X16 оƬ
 * @param offset ���ݴ洢��ַ
 * @param buf ���ݴ洢ָ��
 * @param size д�����ݴ�С
 * @retval ����ʵ��д������ݴ�С
 */
UINT32 w25x16_Write(UINT32 offset, const UINT8* buf, UINT32 size);

#endif //CFG_W25X16

#endif
