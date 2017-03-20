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

#include "hal_spi.h"

#ifdef CFG_W25X16

#define W25X16_CMD_WR_EN				0x06    //写使能  备注：xcc
#define W25X16_CMD_WR_DI				0x04    //写禁能
#define W25X16_CMD_RD_ST				0x05    //读状态寄存器
#define W25X16_CMD_WR_ST				0x01    //写状态寄存器
#define W25X16_CMD_READ					0x03    //读数据
#define W25X16_CMD_FAST_READ			0x0B    //读数据
#define W25X16_CMD_FAST_READ_DUAL		0x3B    //快读双输出
#define W25X16_CMD_PAGE_PROG			0x02    //页编程  256Byte
#define W25X16_CMD_ERASE_4K				0x20    //扇区擦除:4K
#define W25X16_CMD_ERASE_64K			0xD8    //块擦除:64K
#define W25X16_CMD_ERASE_ALL			0xC7    //芯片擦除
#define W25X16_CMD_POW_DOWN				0xB9    //掉电
#define W25X16_CMD_DID					0xAB    //器件ID
#define W25X16_CMD_MFD_ID				0x90    //制造/器件ID
#define W25X16_CMD_JEDEC_ID				0x9F    //JEDEC ID

#define W25X16_PAGE_SIZE				(256UL)	//页大小
#define W25X16_SECTOR_SIZE				(4*1024UL)		//扇区大小
#define W25X16_BLOCK_SIZE				(64*1024UL)		//块大小
#define W25X16_EEPROM_SIZE				(4*1024*1024UL)	//块大小

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
 * @brief W25X16 初始化
 * @param hspi: SPI接口操作句柄
 * @retval 初始化成功返回0, 否则返回-1
 */
int w25x16_Init(HALSpiTypeDef *hspi);

/**
 * @brief W25X16 资源释放
 * @param None
 * @retval None
 */
void w25x16_DeInit(void);

/**
 * @brief 读取 W25X16 芯片ID
 * @param None
 * @retval None
 */
UINT32 w25x16_GetID(void);

/**
 * @brief 读取 W25X16 芯片
 * @param offset 数据存储地址
 * @param buf 数据存储指针
 * @param size 读出数据大小
 * @retval 返回实际读取的数据大小
 */
UINT32 w25x16_Read(UINT32 offset, UINT8* buf, UINT32 size);

/**
 * @brief 写 W25X16 芯片
 * @param offset 数据存储地址
 * @param buf 数据存储指针
 * @param size 写入数据大小
 * @retval 返回实际写入的数据大小
 */
UINT32 w25x16_Write(UINT32 offset, const UINT8* buf, UINT32 size);

#endif //CFG_W25X16

#endif
