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

/** SPI1�˿ڶ��� */
#define SPI1_GPIO_TYPE	GPIOA
#define SPI1_NSS		GPIO_Pin_4
#define SPI1_SCLK		GPIO_Pin_5
#define SPI1_MISO		GPIO_Pin_6
#define SPI1_MOSI		GPIO_Pin_7

/** SPI2�˿ڶ��� */
#define SPI2_GPIO_TYPE	GPIOB
#define SPI2_NSS		GPIO_Pin_12
#define SPI2_SCLK		GPIO_Pin_13
#define SPI2_MISO		GPIO_Pin_14
#define SPI2_MOSI		GPIO_Pin_15

/** SPI3�˿ڶ��� */
#define SPI3_GPIO_TYPE		GPIOB
#define SPI3_NSS_GPIO_TYPE	GPIOA
#define SPI3_NSS			GPIO_Pin_15
#define SPI3_SCLK			GPIO_Pin_3
#define SPI3_MISO			GPIO_Pin_4
#define SPI3_MOSI			GPIO_Pin_5

/** 
 * @brief SPIͨ�� 
 */
typedef enum
{
	HALSpiNumer1 = 0,	/*!< SPIͨ��1 */
	HALSpiNumer2,		/*!< SPIͨ��2 */
	HALSpiNumer3,		/*!< SPIͨ��3 */
	HALSpiNumer_SIZE		/*!< SPIͨ������ */
}HALSpiNumer;

/** 
 * @brief SPI����ģʽ
 */
typedef enum
{
	HALSpiMode_Master = SPI_Mode_Master,	/*!< ��ģʽ */
	HALSpiMode_Slave = SPI_Mode_Slave		/*!< ��ģʽ */
}HALSpiMode;

/** 
 * @brief SPI����λ
 */
typedef enum
{
	HALSpiDataSize_8b = SPI_DataSize_8b,	/*!< 8λ����λ */
	HALSpiDataSize_16b = SPI_DataSize_16b	/*!< 16λ����λ */
}HALSpiDataSize;

/**
 * @brief SPIʱ�Ӽ���
 */
typedef enum
{
	HALSpiCPOL_High = SPI_CPOL_High,	/*!< ʱ�����ո� */
	HALSpiCPOL_Low = SPI_CPOL_Low		/*!< ʱ�����յ� */
}HALSpiCPOL;

/** 
 * @brief SPIʱ����λ
 */
typedef enum
{
	HALSpiCPHA_1Edge = SPI_CPHA_1Edge,	/*!< ��1��ʱ���ز��� */
	HALSpiCPHA_2Edge = SPI_CPHA_2Edge,	/*!< ��2��ʱ���ز��� */
}HALSpiCPHA;

/** 
 * @brief SPI������Ԥ��Ƶ
 */
typedef enum
{
	HALSpiBaudRate_2   = SPI_BaudRatePrescaler_2,		/*!< ������2��Ƶ */
	HALSpiBaudRate_4   = SPI_BaudRatePrescaler_4,  	/*!< ������3��Ƶ */
	HALSpiBaudRate_8   = SPI_BaudRatePrescaler_8,  	/*!< ������4��Ƶ */
	HALSpiBaudRate_16  = SPI_BaudRatePrescaler_16, 	/*!< ������16��Ƶ */
	HALSpiBaudRate_32  = SPI_BaudRatePrescaler_32, 	/*!< ������32��Ƶ */
	HALSpiBaudRate_64  = SPI_BaudRatePrescaler_64, 	/*!< ������64��Ƶ */
	HALSpiBaudRate_128 = SPI_BaudRatePrescaler_128,	/*!< ������128��Ƶ */
	HALSpiBaudRate_256 = SPI_BaudRatePrescaler_256,	/*!< ������256��Ƶ */
}HALSpiBaudRate;

/**
 * @brief SPI���ݴ���˳�� 
 */
typedef enum
{
	HALSpiFirstBit_MSB = SPI_FirstBit_MSB, /*!< ���ݴ����MSB��ʼ */
	HALSpiFirstBit_LSB = SPI_FirstBit_LSB	/*!< ���ݴ����LSB��ʼ */
}HALSpiFirstBit;

/**
 * @brief SPI�����ṹ����
 */
typedef struct _HALSpiTypeDef
{
	/**
	 * @brief SPI��ʼ��
	 * @param mode: SPI����ģʽ @ref HALSpiMode
	 * @param datasize: �������ݴ�С @ref HALSpiDataSize
	 * @param cpol: SPIʱ�Ӽ��� @ref HALSpiCPOL
	 * @param cpha: SPIʱ����λ @ref HALSpiCPHA
	 * @param prescaler: SPIʱ�Ӳ�����Ԥ��Ƶ @ref HALSpiBaudRate
	 * @param firstbit: SPI���ݴ���˳�� @ref HALSpiFirstBit
	 * @retval None
	 */
	void (*init)(HALSpiMode mode, HALSpiDataSize datasize, HALSpiCPOL cpol, HALSpiCPHA cpha, HALSpiBaudRate prescaler, HALSpiFirstBit firstbit);
	
	/**
	 * @brief SPI��Դע��
	 * @param None
	 * @retval None
	 */
	void (*deInit)(void);
	
	/**
	 * @brief SPI ����������
	 * @param uBaudrate: ������Ԥ��Ƶ @ref HALSpiBaudRate
	 * @retval None
	 */
	void (*setBaudrate)(HALSpiBaudRate prescaler);
	
	/**
	 * @brief SPI ���ݷ���
	 * @param txdata: д�������
	 * @retval ����������
	 */
	UINT16 (*access)(UINT16 txdata);
}HALSpiTypeDef;

/**
 * @brief ��ȡSPI�����ṹ���
 * @param eChennal: SPIͨ��ö�� @ref HALSpiNumer
 * @retval SPI�����ṹ���
 */
HALSpiTypeDef* HalSpiGetInstance(HALSpiNumer eChannel);

#endif //CFG_HAL_SPI
#endif
