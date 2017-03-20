/**
 * @file    hal_spi.c
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
#include "hal_spi.h"

#ifdef CFG_HAL_SPI

static BYTE m_bySpiInitFlag = 0;
static HALSpiTypeDef m_Instance[HALSpiNumer_SIZE];
static HALSpiTypeDef* m_pthis[HALSpiNumer_SIZE] = {NULL};

/**
 * @brief SPI1初始化
 * @param mode: SPI工作模式 @ref HALSpiMode
 * @param datasize: 传输数据大小 @ref HALSpiDataSize
 * @param cpol: SPI时钟极性 @ref HALSpiCPOL
 * @param cpha: SPI时钟相位 @ref HALSpiCPHA
 * @param prescaler: SPI时钟波特率预分频 @ref HALSpiBaudRate
 * @param firstbit: SPI数据传输顺序 @ref HALSpiFirstBit
 * @retval None
 */
static void spi1_init(HALSpiMode mode, HALSpiDataSize datasize, HALSpiCPOL cpol, HALSpiCPHA cpha, HALSpiBaudRate prescaler, HALSpiFirstBit firstbit)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
  	SPI_InitTypeDef  SPI_InitStructure;

//	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );//PORTB时钟使能 
	RCC_APB1PeriphClockCmd(	RCC_APB2Periph_SPI1,  ENABLE );//SPI1时钟使能  	
 
	GPIO_InitStructure.GPIO_Pin = SPI1_SCLK | SPI1_MOSI;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //SCLK, MOSI复用推挽输出 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI1_GPIO_TYPE, &GPIO_InitStructure);//初始化GPIO
 	GPIO_SetBits(SPI1_GPIO_TYPE, SPI1_SCLK | SPI1_MOSI);  //SCLK, MOSI上拉

	GPIO_InitStructure.GPIO_Pin = SPI1_MISO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //MISO上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI1_GPIO_TYPE, &GPIO_InitStructure);//初始化GPIO

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //SPI设置为全双工模式
	SPI_InitStructure.SPI_Mode = mode;						//设置工作模式
	SPI_InitStructure.SPI_DataSize = datasize;				//数据位
	SPI_InitStructure.SPI_CPOL = cpol;						//极性
	SPI_InitStructure.SPI_CPHA = cpha;						//采样时钟同步
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;				//NSS信号控制方式
	SPI_InitStructure.SPI_BaudRatePrescaler = prescaler;	//时钟预分频
	SPI_InitStructure.SPI_FirstBit = firstbit;				//数据传输顺序
	SPI_InitStructure.SPI_CRCPolynomial = 7;				//CRC校验多项式
	SPI_Init(SPI1, &SPI_InitStructure);
 
	SPI_Cmd(SPI1, ENABLE);//SPI2使能
	
	m_bySpiInitFlag |= 0x01;
}

/**
 * @brief SPI1 资源注销
 * @param None
 * @retval None
 */
static void spi1_deInit(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
  	
	GPIO_InitStructure.GPIO_Pin = SPI1_SCLK | SPI1_MOSI;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //SCLK, MOSI浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI1_GPIO_TYPE, &GPIO_InitStructure);//初始化GPIO

	GPIO_InitStructure.GPIO_Pin = SPI1_MISO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //MISO浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI1_GPIO_TYPE, &GPIO_InitStructure);//初始化GPIO
 
	SPI_Cmd(SPI1, DISABLE);//SPI2使能
	
	m_bySpiInitFlag &= ~0x01;
}

/**
 * @brief SPI1 数据访问
 * @param txdata: 写入的数据
 * @retval 读出的数据
 */
static UINT16 spi1_readwrite(UINT16 txdata)
{
	UINT8 retry=0;		
	if (!(m_bySpiInitFlag & 0x01))//SPI通道未初始化
		return 0;
			 	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //等待发送缓存为空
	{
		retry++;
		if(retry > 200)
			return 0;
	}			  
	SPI_I2S_SendData(SPI1, txdata); //发送数据
	
	retry=0;
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //等待接收缓存已满
	{
		retry++;
		if(retry > 200)
			return 0;
	}	  				
	
	return SPI_I2S_ReceiveData(SPI1); //返回接收到的数据
}

/**
 * @brief SPI1 波特率设置
 * @param uBaudrate: 波特率预分频 @ref HALSpiBaudRate
 * @retval None
 */
static void spi1_setbaudrate(HALSpiBaudRate prescaler)
{
	if (!(m_bySpiInitFlag & 0x01))//SPI通道未初始化
		return;
  	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI1->CR1 &= 0XFFC7;
	SPI1->CR1 |= prescaler;	//设置SPI速度
	SPI_Cmd(SPI1, ENABLE);
}

/**
 * @brief SPI2初始化
 * @param mode: SPI工作模式 @ref HALSpiMode
 * @param datasize: 传输数据大小 @ref HALSpiDataSize
 * @param cpol: SPI时钟极性 @ref HALSpiCPOL
 * @param cpha: SPI时钟相位 @ref HALSpiCPHA
 * @param prescaler: SPI时钟波特率预分频 @ref HALSpiBaudRate
 * @param firstbit: SPI数据传输顺序 @ref HALSpiFirstBit
 * @retval None
 */
static void spi2_init(HALSpiMode mode, HALSpiDataSize datasize, HALSpiCPOL cpol, HALSpiCPHA cpha, HALSpiBaudRate prescaler, HALSpiFirstBit firstbit)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
  	SPI_InitTypeDef  SPI_InitStructure;

//	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTB时钟使能 
	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2,  ENABLE );//SPI2时钟使能  	
 
	GPIO_InitStructure.GPIO_Pin = SPI2_SCLK | SPI2_MOSI;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //SCLK, MOSI复用推挽输出 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI2_GPIO_TYPE, &GPIO_InitStructure);//初始化GPIO
 	GPIO_SetBits(SPI2_GPIO_TYPE, SPI2_SCLK | SPI2_MOSI);  //SCLK, MOSI上拉

	GPIO_InitStructure.GPIO_Pin = SPI2_MISO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //MISO上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI2_GPIO_TYPE, &GPIO_InitStructure);//初始化GPIO
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //SPI设置为全双工模式
	SPI_InitStructure.SPI_Mode = mode;						//设置工作模式
	SPI_InitStructure.SPI_DataSize = datasize;				//数据位
	SPI_InitStructure.SPI_CPOL = cpol;						//极性
	SPI_InitStructure.SPI_CPHA = cpha;						//采样时钟同步
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;				//NSS信号控制方式
	SPI_InitStructure.SPI_BaudRatePrescaler = prescaler;	//时钟预分频
	SPI_InitStructure.SPI_FirstBit = firstbit;				//数据传输顺序
	SPI_InitStructure.SPI_CRCPolynomial = 7;				//CRC校验多项式
	SPI_Init(SPI2, &SPI_InitStructure);
 
	SPI_Cmd(SPI2, ENABLE);//SPI2使能
	
	m_bySpiInitFlag |= 0x02;
}

/**
 * @brief SPI2 资源注销
 * @param None
 * @retval None
 */
static void spi2_deInit(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = SPI2_SCLK | SPI2_MOSI;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //SCLK, MOSI浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI2_GPIO_TYPE, &GPIO_InitStructure);//初始化GPIO

	GPIO_InitStructure.GPIO_Pin = SPI2_MISO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //MISO浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI2_GPIO_TYPE, &GPIO_InitStructure);//初始化GPIO
	
	SPI_Cmd(SPI2, DISABLE);//SPI2使能
	
	m_bySpiInitFlag &= ~0x02;
}

/**
 * @brief SPI2 数据访问
 * @param txdata: 写入的数据
 * @retval 读出的数据
 */
static UINT16 spi2_readwrite(UINT16 txdata)
{
	UINT8 retry=0;
	if (!(m_bySpiInitFlag & 0x02))//SPI通道未初始化
		return 0;
					 	
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //等待发送缓存为空
	{
		retry++;
		if(retry > 200)
			return 0;
	}			  
	SPI_I2S_SendData(SPI2, txdata); //发送数据
	
	retry=0;
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) //等待接收缓存不为空
	{
		retry++;
		if(retry > 200)
			return 0;
	}	  		
	
	return SPI_I2S_ReceiveData(SPI2); //返回接收到的数据
}

/**
 * @brief SPI2 波特率设置
 * @param uBaudrate: 波特率预分频 @ref HALSpiBaudRate
 * @retval None
 */
static void spi2_setbaudrate(HALSpiBaudRate prescaler)
{
	if (!(m_bySpiInitFlag & 0x02))//SPI通道未初始化
		return;
  	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI2->CR1 &= 0XFFC7;
	SPI2->CR1 |= prescaler;	//设置SPI速度
	SPI_Cmd(SPI2, ENABLE); 
}

/**
 * @brief SPI3初始化
 * @param mode: SPI工作模式 @ref HALSpiMode
 * @param datasize: 传输数据大小 @ref HALSpiDataSize
 * @param cpol: SPI时钟极性 @ref HALSpiCPOL
 * @param cpha: SPI时钟相位 @ref HALSpiCPHA
 * @param prescaler: SPI时钟波特率预分频 @ref HALSpiBaudRate
 * @param firstbit: SPI数据传输顺序 @ref HALSpiFirstBit
 * @retval None
 */
static void spi3_init(HALSpiMode mode, HALSpiDataSize datasize, HALSpiCPOL cpol, HALSpiCPHA cpha, HALSpiBaudRate prescaler, HALSpiFirstBit firstbit)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
  	SPI_InitTypeDef  SPI_InitStructure;

//	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTB时钟使能 
	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI3,  ENABLE );//SPI3时钟使能  	
 
	GPIO_InitStructure.GPIO_Pin = SPI3_SCLK | SPI3_MOSI;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //SCLK, MOSI复用推挽输出 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI3_GPIO_TYPE, &GPIO_InitStructure);//初始化GPIO
 	GPIO_SetBits(SPI3_GPIO_TYPE, SPI3_SCLK | SPI3_MOSI);  //SCLK, MOSI上拉

	GPIO_InitStructure.GPIO_Pin = SPI3_MISO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //MISO上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI3_GPIO_TYPE, &GPIO_InitStructure);//初始化GPIO
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //SPI设置为全双工模式
	SPI_InitStructure.SPI_Mode = mode;						//设置工作模式
	SPI_InitStructure.SPI_DataSize = datasize;				//数据位
	SPI_InitStructure.SPI_CPOL = cpol;						//极性
	SPI_InitStructure.SPI_CPHA = cpha;						//采样时钟同步
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;				//NSS信号控制方式
	SPI_InitStructure.SPI_BaudRatePrescaler = prescaler;	//时钟预分频
	SPI_InitStructure.SPI_FirstBit = firstbit;				//数据传输顺序
	SPI_InitStructure.SPI_CRCPolynomial = 7;				//CRC校验多项式
	SPI_Init(SPI3, &SPI_InitStructure);
 
	SPI_Cmd(SPI3, ENABLE);//SPI3使能
	
	m_bySpiInitFlag |= 0x04;
}

/**
 * @brief SPI资源注销
 * @param None
 * @retval None
 */
static void spi3_deInit(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = SPI3_SCLK | SPI3_MOSI;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//SCLK, MOSI浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI3_GPIO_TYPE, &GPIO_InitStructure);//初始化GPIO

	GPIO_InitStructure.GPIO_Pin = SPI3_MISO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI3_GPIO_TYPE, &GPIO_InitStructure);//初始化GPIO
	
	SPI_Cmd(SPI3, DISABLE);//SPI3使能
	
	m_bySpiInitFlag &= ~0x04;
}

/**
 * @brief SPI3 数据访问
 * @param txdata: 写入的数据
 * @retval 读出的数据
 */
static UINT16 spi3_readwrite(UINT16 txdata)
{
	UINT8 retry=0;	
	if (!(m_bySpiInitFlag & 0x04))//SPI通道未初始化
		return 0;
	
	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET) //等待发送缓存为空
	{
		retry++;
		if(retry > 200)
			return 0;
	}			  
	SPI_I2S_SendData(SPI3, txdata); //发送数据
	
	retry=0;
	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET) //等待接收缓存已满
	{
		retry++;
		if(retry > 200)
			return 0;
	}	  			
	
	return SPI_I2S_ReceiveData(SPI3); //返回接收到的数据
}

/**
 * @brief SPI3 波特率设置
 * @param uBaudrate: 波特率预分频 @ref HALSpiBaudRate
 * @retval None
 */
static void spi3_setbaudrate(HALSpiBaudRate prescaler)
{
	if (!(m_bySpiInitFlag & 0x04))//SPI通道未初始化
		return;
  	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI3->CR1 &= 0XFFC7;
	SPI3->CR1 |= prescaler;	//设置SPI速度
	SPI_Cmd(SPI3, ENABLE);
}

/**
 * @brief 创建SPI操作结构句柄
 * @param eChennal: SPI通道枚举 @ref HALSpiNumer
 * @retval None
 */
static void New(HALSpiNumer eChannel)
{
	switch (eChannel)
	{
	case HALSpiNumer1:
		m_Instance[0].init = spi1_init;
		m_Instance[0].deInit = spi1_deInit;
		m_Instance[0].access = spi1_readwrite;
		m_Instance[0].setBaudrate = spi1_setbaudrate;
		m_pthis[0] = &m_Instance[0];
		break;
	case HALSpiNumer2:
		m_Instance[1].init = spi2_init;
		m_Instance[1].deInit = spi2_deInit;
		m_Instance[1].access = spi2_readwrite;
		m_Instance[1].setBaudrate = spi2_setbaudrate;
		m_pthis[1] = &m_Instance[1];
		break;
	case HALSpiNumer3:
		m_Instance[2].init = spi3_init;
		m_Instance[2].deInit = spi3_deInit;
		m_Instance[2].access = spi3_readwrite;
		m_Instance[2].setBaudrate = spi3_setbaudrate;
		m_pthis[2] = &m_Instance[2];
		break;
	default:
		break;
	}
}

/**
 * @brief 获取SPI操作结构句柄
 * @param eChennal: SPI通道枚举 @ref HALSpiNumer
 * @retval SPI操作结构句柄
 */
HALSpiTypeDef* HalSpiGetInstance(HALSpiNumer eChannel)
{
	if (NULL == m_pthis[eChannel])
	{
		New(eChannel);
	}
	return m_pthis[eChannel];
}

#endif //CFG_HAL_SPI
