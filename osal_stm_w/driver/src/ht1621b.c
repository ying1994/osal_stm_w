/**
 * @file    ht1621b.c
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   HT1621B����
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 * COPYRIGHT NOTICE  
 * Copyright 2016, wsf 
 * All rights res
 *
 */
#include "ht1621b.h"

#ifdef CFG_HT1621B

	
//! CS
#define HT1621B_CS_ON	HalGpioWriteBit(HT1621B_CS_GPIO_TYPE, HT1621B_CS_GPIO_PIN, 1)
#define HT1621B_CS_OFF	HalGpioWriteBit(HT1621B_CS_GPIO_TYPE, HT1621B_CS_GPIO_PIN, 0)

//! RD
#define HT1621B_RD_ON	HalGpioWriteBit(HT1621B_RD_GPIO_TYPE, HT1621B_RD_GPIO_PIN, 1)
#define HT1621B_RD_OFF	HalGpioWriteBit(HT1621B_RD_GPIO_TYPE, HT1621B_RD_GPIO_PIN, 0)

//! WR
#define HT1621B_WR_ON	HalGpioWriteBit(HT1621B_WR_GPIO_TYPE, HT1621B_WR_GPIO_PIN, 1)
#define HT1621B_WR_OFF	HalGpioWriteBit(HT1621B_WR_GPIO_TYPE, HT1621B_WR_GPIO_PIN, 0)

//! DATA
#define HT1621B_DATA_ON		HalGpioWriteBit(HT1621B_DATA_GPIO_TYPE, HT1621B_DATA_GPIO_PIN, 1)
#define HT1621B_DATA_OFF	HalGpioWriteBit(HT1621B_DATA_GPIO_TYPE, HT1621B_DATA_GPIO_PIN, 0)
#define HT1621B_CHK_DATA	HalGpioReadBit(HT1621B_DATA_GPIO_TYPE, HT1621B_DATA_GPIO_PIN, 0)

/**
 * @brief HT1621Bд��ַ
 * @param 
 * @retval ��ʼ���ɹ�����0, ���򷵻�-1
 */
static int write_addr(UINT16 addr, INT16 bitcnt)
{
	bitcnt = (bitcnt > 16) ? 16 : bitcnt;
	bitcnt -= 1;
	while(bitcnt >= 0)
	{
		if(addr & BIT(bitcnt))
			HT1621B_DATA_ON;
		else
			HT1621B_DATA_OFF;
		HT1621B_WR_OFF;
		delay_us(100);
		HT1621B_WR_ON;
		bitcnt -= 1;
	}
	
	return 0;
}

/**
 * @brief HT1621Bд����
 * @param 
 * @retval ��ʼ���ɹ�����0, ���򷵻�-1
 */
static int write_data(UINT16 data, INT16 bitcnt)
{
	INT16 i = 0;
	bitcnt = (bitcnt > 16) ? 16 : bitcnt;
	for(i=0; i < bitcnt; i ++)
	{
		if(data & 0x01)
			HT1621B_DATA_ON;
		else
			HT1621B_DATA_OFF;
		HT1621B_WR_OFF;
		delay_us(20);
		HT1621B_WR_ON;
		delay_us(20);
		data >>= 1;
	}
	
	return 0;
}

/**
 * @brief HT1621B������
 * @param 
 * @retval ���ض�ȡ��������
 */
static UINT8 read_data(UINT8 bitcnt)
{
	UINT8 i;
	UINT8 data = 0;
	
	bitcnt = (bitcnt > 8) ? 8 : bitcnt;
	
	for(i=0; i < bitcnt; i ++)
	{
		HT1621B_RD_OFF;
		if(HT1621B_CHK_DATA)
			data &= 0x80;
		HT1621B_RD_ON;
		delay_us(20);
		data >>= 1;
	}
	data >>= (8-bitcnt);
	
	return data;
}

/**
 * @brief HT1621B ��ʼ��
 * @param 
 * @retval ��ʼ���ɹ�����0, ���򷵻�-1
 */
int ht1621b_Init(void)
{
	HalGpioInit(HT1621B_CS_GPIO_TYPE, HT1621B_CS_GPIO_PIN, HAL_GPIOMode_Out_PP);
	HalGpioInit(HT1621B_RD_GPIO_TYPE, HT1621B_RD_GPIO_PIN, HAL_GPIOMode_Out_PP);
	HalGpioInit(HT1621B_WR_GPIO_TYPE, HT1621B_WR_GPIO_PIN, HAL_GPIOMode_Out_PP);
	HalGpioInit(HT1621B_DATA_GPIO_TYPE, HT1621B_DATA_GPIO_PIN, HAL_GPIOMode_Out_PP);
	
	ht1621b_Cmd(HT1621B_CMD_BIAS_50);//LCD 1/2ƫѹ, ��ѡ4��������
	ht1621b_Cmd(HT1621B_CMD_SYSEN);//��ϵͳ����
	ht1621b_Cmd(HT1621B_CMD_LCDON);//��LCDƫѹ������
	ht1621b_Cmd(HT1621B_CMD_RC256);//ϵͳʱ��ѡ��Ƭ��ʱ��
	
	return 0;
}

/**
 * @brief HT1621B ��Դ�ͷ�
 * @param None
 * @retval None
 */
void ht1621b_DeInit(void)
{
	HalGpioInit(HT1621B_CS_GPIO_TYPE, HT1621B_CS_GPIO_PIN, HAL_GPIOMode_IN_FLOATING);
	HalGpioInit(HT1621B_RD_GPIO_TYPE, HT1621B_RD_GPIO_PIN, HAL_GPIOMode_IN_FLOATING);
	HalGpioInit(HT1621B_WR_GPIO_TYPE, HT1621B_WR_GPIO_PIN, HAL_GPIOMode_IN_FLOATING);
	HalGpioInit(HT1621B_DATA_GPIO_TYPE, HT1621B_DATA_GPIO_PIN, HAL_GPIOMode_IN_FLOATING);
}

/**
 * @brief д HT1621B ����
 * @param cmd ����
 * @retval ����ʵ�ʶ�ȡ�����ݴ�С
 */
int ht1621b_Cmd(UINT8 cmd)
{
	HT1621B_CS_OFF;
	write_addr(0x04, 3);//д���־λ�롱100��
	write_addr(cmd, 9);
	HT1621B_CS_ON;
	
	return 0;
}

/**
 * @brief ��ȡ HT1621B оƬ
 * @param offset ���ݴ洢��ַ
 * @param buf ���ݴ洢ָ��
 * @param size �������ݴ�С
 * @retval ����ʵ�ʶ�ȡ�����ݴ�С
 */
int ht1621b_Read(UINT8 offset, UINT8* buf, int size)
{
	int i=0;
	HT1621B_CS_OFF;
	write_addr(0x06, 3);//д���־λ�롱110��
	write_addr(offset, 6);//д��ַ
	
	for (i=0; i<size; i++)
		buf[i] = read_data(4);
	HT1621B_CS_ON;
	
	return size;
}

/**
 * @brief д HT1621B оƬ
 * @param offset ���ݴ洢��ַ
 * @param buf ���ݴ洢ָ��(1BYTE: size=2)
 * @param size д�����ݴ�С
 * @retval ����ʵ��д������ݴ�С
 */
int ht1621b_Write(UINT8 offset, const UINT8* buf, int size)
{
	int i = 0;
	int idx = 0;
	HT1621B_CS_OFF;
	write_addr(0x05, 3);//д���־λ�롱101��
	write_addr(offset, 6);//д��ַ
	for (idx=0; idx<size; idx++)
	{
		if (idx & 0x01)
		{
			write_data(buf[i]>>4, 4);
			i++;
		}
		else
		{
			write_data(buf[i] & 0x0f, 4);
		}
	}
	HT1621B_CS_ON;
	
	return size;
}

#endif //CFG_HT1621B

