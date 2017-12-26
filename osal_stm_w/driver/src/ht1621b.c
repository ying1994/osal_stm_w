/**
 * @file    ht1621b.c
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
 * @brief HT1621B写地址
 * @param 
 * @retval 初始化成功返回0, 否则返回-1
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
 * @brief HT1621B写数据
 * @param 
 * @retval 初始化成功返回0, 否则返回-1
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
 * @brief HT1621B读数据
 * @param 
 * @retval 返回读取到的数据
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
 * @brief HT1621B 初始化
 * @param 
 * @retval 初始化成功返回0, 否则返回-1
 */
int ht1621b_Init(void)
{
	HalGpioInit(HT1621B_CS_GPIO_TYPE, HT1621B_CS_GPIO_PIN, HAL_GPIOMode_Out_PP);
	HalGpioInit(HT1621B_RD_GPIO_TYPE, HT1621B_RD_GPIO_PIN, HAL_GPIOMode_Out_PP);
	HalGpioInit(HT1621B_WR_GPIO_TYPE, HT1621B_WR_GPIO_PIN, HAL_GPIOMode_Out_PP);
	HalGpioInit(HT1621B_DATA_GPIO_TYPE, HT1621B_DATA_GPIO_PIN, HAL_GPIOMode_Out_PP);
	
	ht1621b_Cmd(HT1621B_CMD_BIAS_50);//LCD 1/2偏压, 可选4个公共口
	ht1621b_Cmd(HT1621B_CMD_SYSEN);//打开系统振荡器
	ht1621b_Cmd(HT1621B_CMD_LCDON);//打开LCD偏压发生器
	ht1621b_Cmd(HT1621B_CMD_RC256);//系统时钟选择片内时钟
	
	return 0;
}

/**
 * @brief HT1621B 资源释放
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
 * @brief 写 HT1621B 命令
 * @param cmd 命令
 * @retval 返回实际读取的数据大小
 */
int ht1621b_Cmd(UINT8 cmd)
{
	HT1621B_CS_OFF;
	write_addr(0x04, 3);//写入标志位码”100”
	write_addr(cmd, 9);
	HT1621B_CS_ON;
	
	return 0;
}

/**
 * @brief 读取 HT1621B 芯片
 * @param offset 数据存储地址
 * @param buf 数据存储指针
 * @param size 读出数据大小
 * @retval 返回实际读取的数据大小
 */
int ht1621b_Read(UINT8 offset, UINT8* buf, int size)
{
	int i=0;
	HT1621B_CS_OFF;
	write_addr(0x06, 3);//写入标志位码”110”
	write_addr(offset, 6);//写地址
	
	for (i=0; i<size; i++)
		buf[i] = read_data(4);
	HT1621B_CS_ON;
	
	return size;
}

/**
 * @brief 写 HT1621B 芯片
 * @param offset 数据存储地址
 * @param buf 数据存储指针(1BYTE: size=2)
 * @param size 写入数据大小
 * @retval 返回实际写入的数据大小
 */
int ht1621b_Write(UINT8 offset, const UINT8* buf, int size)
{
	int i = 0;
	int idx = 0;
	HT1621B_CS_OFF;
	write_addr(0x05, 3);//写入标志位码”101”
	write_addr(offset, 6);//写地址
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

