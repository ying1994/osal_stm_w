/**
 * @file    hal_spi.h
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
 * All rights Reserved
 *
 */
#ifndef _HAL_SPI_H
#define _HAL_SPI_H

#include "hal_cfg.h"
#include "hal_types.h"

#ifdef CFG_HAL_SPI

/** SPI1端口定义 */
#define SPI1_GPIO_TYPE	GPIOA
#define SPI1_NSS		GPIO_Pin_4
#define SPI1_SCLK		GPIO_Pin_5
#define SPI1_MISO		GPIO_Pin_6
#define SPI1_MOSI		GPIO_Pin_7

/** SPI2端口定义 */
#define SPI2_GPIO_TYPE	GPIOB
#define SPI2_NSS		GPIO_Pin_12
#define SPI2_SCLK		GPIO_Pin_13
#define SPI2_MISO		GPIO_Pin_14
#define SPI2_MOSI		GPIO_Pin_15

/** SPI3端口定义 */
#define SPI3_GPIO_TYPE		GPIOB
#define SPI3_NSS_GPIO_TYPE	GPIOA
#define SPI3_NSS			GPIO_Pin_15
#define SPI3_SCLK			GPIO_Pin_3
#define SPI3_MISO			GPIO_Pin_4
#define SPI3_MOSI			GPIO_Pin_5

/** 
 * @brief SPI通道 
 */
typedef enum
{
	HALSpiNumer1 = 0,	/*!< SPI通道1 */
	HALSpiNumer2,		/*!< SPI通道2 */
	HALSpiNumer3,		/*!< SPI通道3 */
	HALSpiNumer_SIZE		/*!< SPI通道总数 */
}HALSpiNumer;

/** 
 * @brief SPI工作模式
 */
typedef enum
{
	HALSpiMode_Master = SPI_Mode_Master,	/*!< 主模式 */
	HALSpiMode_Slave = SPI_Mode_Slave		/*!< 从模式 */
}HALSpiMode;

/** 
 * @brief SPI数据位
 */
typedef enum
{
	HALSpiDataSize_8b = SPI_DataSize_8b,	/*!< 8位数据位 */
	HALSpiDataSize_16b = SPI_DataSize_16b	/*!< 16位数据位 */
}HALSpiDataSize;

/**
 * @brief SPI时钟极性
 */
typedef enum
{
	HALSpiCPOL_High = SPI_CPOL_High,	/*!< 时钟悬空高 */
	HALSpiCPOL_Low = SPI_CPOL_Low		/*!< 时钟悬空低 */
}HALSpiCPOL;

/** 
 * @brief SPI时钟相位
 */
typedef enum
{
	HALSpiCPHA_1Edge = SPI_CPHA_1Edge,	/*!< 第1个时钟沿采样 */
	HALSpiCPHA_2Edge = SPI_CPHA_2Edge,	/*!< 第2个时钟沿采样 */
}HALSpiCPHA;

/** 
 * @brief SPI波特率预分频
 */
typedef enum
{
	HALSpiBaudRate_2   = SPI_BaudRatePrescaler_2,		/*!< 波特率2分频 */
	HALSpiBaudRate_4   = SPI_BaudRatePrescaler_4,  	/*!< 波特率3分频 */
	HALSpiBaudRate_8   = SPI_BaudRatePrescaler_8,  	/*!< 波特率4分频 */
	HALSpiBaudRate_16  = SPI_BaudRatePrescaler_16, 	/*!< 波特率16分频 */
	HALSpiBaudRate_32  = SPI_BaudRatePrescaler_32, 	/*!< 波特率32分频 */
	HALSpiBaudRate_64  = SPI_BaudRatePrescaler_64, 	/*!< 波特率64分频 */
	HALSpiBaudRate_128 = SPI_BaudRatePrescaler_128,	/*!< 波特率128分频 */
	HALSpiBaudRate_256 = SPI_BaudRatePrescaler_256,	/*!< 波特率256分频 */
}HALSpiBaudRate;

/**
 * @brief SPI数据传输顺序 
 */
typedef enum
{
	HALSpiFirstBit_MSB = SPI_FirstBit_MSB, /*!< 数据传输从MSB开始 */
	HALSpiFirstBit_LSB = SPI_FirstBit_LSB	/*!< 数据传输从LSB开始 */
}HALSpiFirstBit;

/**
 * @brief SPI操作结构定义
 */
typedef struct _HALSpiTypeDef
{
	/**
	 * @brief SPI初始化
	 * @param mode: SPI工作模式 @ref HALSpiMode
	 * @param datasize: 传输数据大小 @ref HALSpiDataSize
	 * @param cpol: SPI时钟极性 @ref HALSpiCPOL
	 * @param cpha: SPI时钟相位 @ref HALSpiCPHA
	 * @param prescaler: SPI时钟波特率预分频 @ref HALSpiBaudRate
	 * @param firstbit: SPI数据传输顺序 @ref HALSpiFirstBit
	 * @retval None
	 */
	void (*init)(HALSpiMode mode, HALSpiDataSize datasize, HALSpiCPOL cpol, HALSpiCPHA cpha, HALSpiBaudRate prescaler, HALSpiFirstBit firstbit);
	
	/**
	 * @brief SPI资源注销
	 * @param None
	 * @retval None
	 */
	void (*deInit)(void);
	
	/**
	 * @brief SPI 波特率设置
	 * @param uBaudrate: 波特率预分频 @ref HALSpiBaudRate
	 * @retval None
	 */
	void (*setBaudrate)(HALSpiBaudRate prescaler);
	
	/**
	 * @brief SPI 数据访问
	 * @param txdata: 写入的数据
	 * @retval 读出的数据
	 */
	UINT16 (*access)(UINT16 txdata);
}HALSpiTypeDef;

/**
 * @brief 获取SPI操作结构句柄
 * @param eChennal: SPI通道枚举 @ref HALSpiNumer
 * @retval SPI操作结构句柄
 */
HALSpiTypeDef* HalSpiGetInstance(HALSpiNumer eChannel);

#endif //CFG_HAL_SPI
#endif
