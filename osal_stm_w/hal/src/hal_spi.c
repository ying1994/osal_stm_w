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
 * All rights Reserved
 *
 */

#include "hal_spi.h"

#ifdef CFG_HAL_SPI

static BYTE m_bySpiInitFlag = 0;
static HALSpiTypeDef m_Instance[HALSpiNumer_SIZE];
static HALSpiTypeDef* m_pthis[HALSpiNumer_SIZE] = {NULL};

/**
 * @brief SPI1��ʼ��
 * @param mode: SPI����ģʽ @ref HALSpiMode
 * @param datasize: �������ݴ�С @ref HALSpiDataSize
 * @param cpol: SPIʱ�Ӽ��� @ref HALSpiCPOL
 * @param cpha: SPIʱ����λ @ref HALSpiCPHA
 * @param prescaler: SPIʱ�Ӳ�����Ԥ��Ƶ @ref HALSpiBaudRate
 * @param firstbit: SPI���ݴ���˳�� @ref HALSpiFirstBit
 * @retval None
 */
static void spi1_init(HALSpiMode mode, HALSpiDataSize datasize, HALSpiCPOL cpol, HALSpiCPHA cpha, HALSpiBaudRate prescaler, HALSpiFirstBit firstbit)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
  	SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA, ENABLE );//PORTBʱ��ʹ�� 
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_SPI1,  ENABLE );//SPI1ʱ��ʹ��  	
 
	GPIO_InitStructure.GPIO_Pin = SPI1_SCLK | SPI1_MOSI | SPI1_MISO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //SCLK, MOSI����������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI1_GPIO_TYPE, &GPIO_InitStructure);//��ʼ��GPIO
 	GPIO_SetBits(SPI1_GPIO_TYPE, SPI1_SCLK | SPI1_MOSI | SPI1_MISO);  //SCLK, MOSI����

	//GPIO_InitStructure.GPIO_Pin = SPI1_MISO;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //MISO��������
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_Init(SPI1_GPIO_TYPE, &GPIO_InitStructure);//��ʼ��GPIO

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //SPI����Ϊȫ˫��ģʽ
	SPI_InitStructure.SPI_Mode = mode;						//���ù���ģʽ
	SPI_InitStructure.SPI_DataSize = datasize;				//����λ
	SPI_InitStructure.SPI_CPOL = cpol;						//����
	SPI_InitStructure.SPI_CPHA = cpha;						//����ʱ��ͬ��
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;				//NSS�źſ��Ʒ�ʽ
	SPI_InitStructure.SPI_BaudRatePrescaler = prescaler;	//ʱ��Ԥ��Ƶ
	SPI_InitStructure.SPI_FirstBit = firstbit;				//���ݴ���˳��
	SPI_InitStructure.SPI_CRCPolynomial = 7;				//CRCУ�����ʽ
	SPI_Init(SPI1, &SPI_InitStructure);
 
	//SPI_SSOutputCmd(SPI1, ENABLE);
	SPI_Cmd(SPI1, ENABLE);//SPI2ʹ��
	
	m_bySpiInitFlag |= 0x01;
}

/**
 * @brief SPI1 ��Դע��
 * @param None
 * @retval None
 */
static void spi1_deInit(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
  	
	GPIO_InitStructure.GPIO_Pin = SPI1_SCLK | SPI1_MOSI;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //SCLK, MOSI��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI1_GPIO_TYPE, &GPIO_InitStructure);//��ʼ��GPIO

	GPIO_InitStructure.GPIO_Pin = SPI1_MISO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //MISO��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI1_GPIO_TYPE, &GPIO_InitStructure);//��ʼ��GPIO
 
	SPI_Cmd(SPI1, DISABLE);//SPI2ʹ��
	
	m_bySpiInitFlag &= ~0x01;
}

/**
 * @brief SPI1 ���ݷ���
 * @param txdata: д�������
 * @retval ����������
 */
static UINT16 spi1_readwrite(UINT16 txdata)
{
	//UINT8 retry=0;		
	if (!(m_bySpiInitFlag & 0x01))//SPIͨ��δ��ʼ��
		return 0;
			 	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //�ȴ����ͻ���Ϊ��
	{
		//retry++;
		//if(retry > 200)
		//	return 0;
	}			  
	SPI_I2S_SendData(SPI1, txdata); //��������
	
	//retry=0;
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //�ȴ����ջ�������
	{
		//retry++;
		//if(retry > 200)
		//	return 0;
	}	  				
	
	return SPI_I2S_ReceiveData(SPI1); //���ؽ��յ�������
}

/**
 * @brief SPI1 ����������
 * @param uBaudrate: ������Ԥ��Ƶ @ref HALSpiBaudRate
 * @retval None
 */
static void spi1_setbaudrate(HALSpiBaudRate prescaler)
{
	if (!(m_bySpiInitFlag & 0x01))//SPIͨ��δ��ʼ��
		return;
  	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI1->CR1 &= 0XFFC7;
	SPI1->CR1 |= prescaler;	//����SPI�ٶ�
	SPI_Cmd(SPI1, ENABLE);
}

/**
 * @brief SPI2��ʼ��
 * @param mode: SPI����ģʽ @ref HALSpiMode
 * @param datasize: �������ݴ�С @ref HALSpiDataSize
 * @param cpol: SPIʱ�Ӽ��� @ref HALSpiCPOL
 * @param cpha: SPIʱ����λ @ref HALSpiCPHA
 * @param prescaler: SPIʱ�Ӳ�����Ԥ��Ƶ @ref HALSpiBaudRate
 * @param firstbit: SPI���ݴ���˳�� @ref HALSpiFirstBit
 * @retval None
 */
static void spi2_init(HALSpiMode mode, HALSpiDataSize datasize, HALSpiCPOL cpol, HALSpiCPHA cpha, HALSpiBaudRate prescaler, HALSpiFirstBit firstbit)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
  	SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTBʱ��ʹ�� 
	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI2,  ENABLE );//SPI2ʱ��ʹ��  	
 
	GPIO_InitStructure.GPIO_Pin = SPI2_SCLK | SPI2_MOSI | SPI2_MISO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //SCLK, MOSI����������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI2_GPIO_TYPE, &GPIO_InitStructure);//��ʼ��GPIO
 	GPIO_SetBits(SPI2_GPIO_TYPE, SPI2_SCLK | SPI2_MOSI | SPI2_MISO);  //SCLK, MOSI����

	//GPIO_InitStructure.GPIO_Pin = SPI2_MISO;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //MISO��������
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_Init(SPI2_GPIO_TYPE, &GPIO_InitStructure);//��ʼ��GPIO
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //SPI����Ϊȫ˫��ģʽ
	SPI_InitStructure.SPI_Mode = mode;						//���ù���ģʽ
	SPI_InitStructure.SPI_DataSize = datasize;				//����λ
	SPI_InitStructure.SPI_CPOL = cpol;						//����
	SPI_InitStructure.SPI_CPHA = cpha;						//����ʱ��ͬ��
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;				//NSS�źſ��Ʒ�ʽ
	SPI_InitStructure.SPI_BaudRatePrescaler = prescaler;	//ʱ��Ԥ��Ƶ
	SPI_InitStructure.SPI_FirstBit = firstbit;				//���ݴ���˳��
	SPI_InitStructure.SPI_CRCPolynomial = 7;				//CRCУ�����ʽ
	SPI_Init(SPI2, &SPI_InitStructure);
 
	//SPI_SSOutputCmd(SPI2, ENABLE);
	SPI_Cmd(SPI2, ENABLE);//SPI2ʹ��
	
	m_bySpiInitFlag |= 0x02;
}

/**
 * @brief SPI2 ��Դע��
 * @param None
 * @retval None
 */
static void spi2_deInit(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = SPI2_SCLK | SPI2_MOSI;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //SCLK, MOSI��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI2_GPIO_TYPE, &GPIO_InitStructure);//��ʼ��GPIO

	GPIO_InitStructure.GPIO_Pin = SPI2_MISO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //MISO��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI2_GPIO_TYPE, &GPIO_InitStructure);//��ʼ��GPIO
	
	SPI_Cmd(SPI2, DISABLE);//SPI2ʹ��
	
	m_bySpiInitFlag &= ~0x02;
}

/**
 * @brief SPI2 ���ݷ���
 * @param txdata: д�������
 * @retval ����������
 */
static UINT16 spi2_readwrite(UINT16 txdata)
{
	//UINT8 retry=0;
	if (!(m_bySpiInitFlag & 0x02))//SPIͨ��δ��ʼ��
		return 0;
					 	
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //�ȴ����ͻ���Ϊ��
	{
		//retry++;
		//if(retry > 200)
		//	return 0;
	}			  
	SPI_I2S_SendData(SPI2, txdata); //��������
	
	//retry=0;
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) //�ȴ����ջ��治Ϊ��
	{
		//retry++;
		//if(retry > 200)
		//	return 0;
	}	  		
	
	return SPI_I2S_ReceiveData(SPI2); //���ؽ��յ�������
}

/**
 * @brief SPI2 ����������
 * @param uBaudrate: ������Ԥ��Ƶ @ref HALSpiBaudRate
 * @retval None
 */
static void spi2_setbaudrate(HALSpiBaudRate prescaler)
{
	if (!(m_bySpiInitFlag & 0x02))//SPIͨ��δ��ʼ��
		return;
  	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI2->CR1 &= 0XFFC7;
	SPI2->CR1 |= prescaler;	//����SPI�ٶ�
	SPI_Cmd(SPI2, ENABLE); 
}

/**
 * @brief SPI3��ʼ��
 * @param mode: SPI����ģʽ @ref HALSpiMode
 * @param datasize: �������ݴ�С @ref HALSpiDataSize
 * @param cpol: SPIʱ�Ӽ��� @ref HALSpiCPOL
 * @param cpha: SPIʱ����λ @ref HALSpiCPHA
 * @param prescaler: SPIʱ�Ӳ�����Ԥ��Ƶ @ref HALSpiBaudRate
 * @param firstbit: SPI���ݴ���˳�� @ref HALSpiFirstBit
 * @retval None
 */
static void spi3_init(HALSpiMode mode, HALSpiDataSize datasize, HALSpiCPOL cpol, HALSpiCPHA cpha, HALSpiBaudRate prescaler, HALSpiFirstBit firstbit)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
  	SPI_InitTypeDef  SPI_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );//PORTBʱ��ʹ�� 
	RCC_APB1PeriphClockCmd(	RCC_APB1Periph_SPI3,  ENABLE );//SPI3ʱ��ʹ��  	
 
	GPIO_InitStructure.GPIO_Pin = SPI3_SCLK | SPI3_MOSI | SPI3_MISO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //SCLK, MOSI����������� 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI3_GPIO_TYPE, &GPIO_InitStructure);//��ʼ��GPIO
 	GPIO_SetBits(SPI3_GPIO_TYPE, SPI3_SCLK | SPI3_MOSI | SPI3_MISO);  //SCLK, MOSI����

	//GPIO_InitStructure.GPIO_Pin = SPI3_MISO;
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //MISO��������
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_Init(SPI3_GPIO_TYPE, &GPIO_InitStructure);//��ʼ��GPIO
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //SPI����Ϊȫ˫��ģʽ
	SPI_InitStructure.SPI_Mode = mode;						//���ù���ģʽ
	SPI_InitStructure.SPI_DataSize = datasize;				//����λ
	SPI_InitStructure.SPI_CPOL = cpol;						//����
	SPI_InitStructure.SPI_CPHA = cpha;						//����ʱ��ͬ��
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;				//NSS�źſ��Ʒ�ʽ
	SPI_InitStructure.SPI_BaudRatePrescaler = prescaler;	//ʱ��Ԥ��Ƶ
	SPI_InitStructure.SPI_FirstBit = firstbit;				//���ݴ���˳��
	SPI_InitStructure.SPI_CRCPolynomial = 7;				//CRCУ�����ʽ
	SPI_Init(SPI3, &SPI_InitStructure);
 
	//SPI_SSOutputCmd(SPI3, ENABLE);
	SPI_Cmd(SPI3, ENABLE);//SPI3ʹ��
	
	m_bySpiInitFlag |= 0x04;
}

/**
 * @brief SPI��Դע��
 * @param None
 * @retval None
 */
static void spi3_deInit(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = SPI3_SCLK | SPI3_MOSI;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//SCLK, MOSI��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI3_GPIO_TYPE, &GPIO_InitStructure);//��ʼ��GPIO

	GPIO_InitStructure.GPIO_Pin = SPI3_MISO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI3_GPIO_TYPE, &GPIO_InitStructure);//��ʼ��GPIO
	
	SPI_Cmd(SPI3, DISABLE);//SPI3ʹ��
	
	m_bySpiInitFlag &= ~0x04;
}

/**
 * @brief SPI3 ���ݷ���
 * @param txdata: д�������
 * @retval ����������
 */
static UINT16 spi3_readwrite(UINT16 txdata)
{
	//UINT8 retry=0;	
	if (!(m_bySpiInitFlag & 0x04))//SPIͨ��δ��ʼ��
		return 0;
	
	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_TXE) == RESET) //�ȴ����ͻ���Ϊ��
	{
		//retry++;
		//if(retry > 200)
		//	return 0;
	}			  
	SPI_I2S_SendData(SPI3, txdata); //��������
	
	//retry=0;
	while (SPI_I2S_GetFlagStatus(SPI3, SPI_I2S_FLAG_RXNE) == RESET) //�ȴ����ջ�������
	{
		//retry++;
		//if(retry > 200)
		//	return 0;
	}	  			
	
	return SPI_I2S_ReceiveData(SPI3); //���ؽ��յ�������
}

/**
 * @brief SPI3 ����������
 * @param uBaudrate: ������Ԥ��Ƶ @ref HALSpiBaudRate
 * @retval None
 */
static void spi3_setbaudrate(HALSpiBaudRate prescaler)
{
	if (!(m_bySpiInitFlag & 0x04))//SPIͨ��δ��ʼ��
		return;
  	assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI3->CR1 &= 0XFFC7;
	SPI3->CR1 |= prescaler;	//����SPI�ٶ�
	SPI_Cmd(SPI3, ENABLE);
}

/**
 * @brief ����SPI�����ṹ���
 * @param eChennal: SPIͨ��ö�� @ref HALSpiNumer
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
 * @brief ��ȡSPI�����ṹ���
 * @param eChennal: SPIͨ��ö�� @ref HALSpiNumer
 * @retval SPI�����ṹ���
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
