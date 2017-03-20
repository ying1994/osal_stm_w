/**
 * @file    hal_i2c.c
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
#include "hal_i2c.h"

#ifdef CFG_HAL_I2C

/** I2C GPIO ��ʼ���ṹ */
static GPIO_InitTypeDef  GPIO_InitStructure;

static UINT32 m_uBaudrate[HALI2CNumer_SIZE] = {0};
static UINT16 m_uDeviceID[HALI2CNumer_SIZE] = {0};
static HALI2CTypeDef m_Instance[HALI2CNumer_SIZE];
static HALI2CTypeDef* m_pthis[HALI2CNumer_SIZE] = {NULL};

/**
 * @brief I2C1_SDA��������Ϊ���
 */
static void i2c1_sda_out(void)
{
	GPIO_InitStructure.GPIO_Pin = I2C1_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_Init(I2C1_GPIO_TYPE, &GPIO_InitStructure);
}

/**
 * @brief I2C1_SDA�������� ����Ϊ����
 */
static void i2c1_sda_in(void)
{
	GPIO_InitStructure.GPIO_Pin = I2C1_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;   //��������
	GPIO_Init(I2C1_GPIO_TYPE, &GPIO_InitStructure);
}

/**
 * @brief I2C1дI2C_SCL�˿�
 */
static void i2c1_scl_write(BOOL bOn)
{
	 if (bOn)
	 {
		 I2C1_GPIO_TYPE->BSRR = I2C1_SCL;
	 }
	 else
	 {
		 I2C1_GPIO_TYPE->BRR = I2C1_SCL;
	 }
}

/**
 * @brief I2C1дI2C_SDA�˿�
 */
static void i2c1_sda_write(BOOL bOn)
{
	if (bOn)
	 {
		 I2C1_GPIO_TYPE->BSRR = I2C1_SDA;
	 }
	 else
	 {
		 I2C1_GPIO_TYPE->BRR = I2C1_SDA;
	 }
}

/**
 * @brief I2C1��I2C_SDA�˿�
 */
static BOOL i2c1_sda_read(void)
{
	if ((I2C1_GPIO_TYPE->IDR & I2C1_SDA) != (uint32_t)Bit_RESET)
		 return TRUE;
	 else
		return FALSE;
}

/**
 * @brief I2C1��ʼ����IO��	
 */
static void i2c1_init(void)
{
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = I2C1_SCL | I2C1_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C1_GPIO_TYPE, &GPIO_InitStructure);
	GPIO_SetBits(I2C1_GPIO_TYPE, I2C1_SCL | I2C1_SDA); 	//PB10,PB11 �����
}

static void i2c1_deInit(void)
{
	GPIO_InitStructure.GPIO_Pin = I2C1_SCL | I2C1_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ;   //��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C1_GPIO_TYPE, &GPIO_InitStructure);
}

static void i2c1_setDeviceID(UINT16 uID)
{
	m_uDeviceID[0] = uID;
}
static void i2c1_setBaudrate(UINT32 baudrate)
{
	m_uBaudrate[0] = baudrate;
}
/**
 * @brief I2C1���Ϳ�ʼ�ź�
 */
static void i2c1_start(void)
{
	i2c1_sda_out();     //sda�����
	i2c1_sda_write(TRUE);	  	  
	i2c1_scl_write(TRUE);
	usleep(4);
 	i2c1_sda_write(FALSE);//START:when CLK is high,DATA change form high to low 
	usleep(4);
	i2c1_scl_write(FALSE);//ǯסI2C���ߣ�׼�����ͻ�������� 
}

/**
 * @brief I2C1����ֹͣ�ź�
 */
static void i2c1_stop(void)
{
	i2c1_sda_out();//sda�����
	i2c1_scl_write(FALSE);
	i2c1_sda_write(FALSE);//STOP:when CLK is high DATA change form low to high
 	usleep(4);
	i2c1_scl_write(TRUE); 
	i2c1_sda_write(TRUE);//����I2C���߽����ź�
	usleep(4);	
}

/**
 * @brief I2C1����ACK�ź�
 */
static void i2c1_ack(void)
{
	i2c1_scl_write(FALSE);
	i2c1_sda_out();
	i2c1_sda_write(FALSE);
	usleep(2);
	i2c1_scl_write(TRUE);
	usleep(2);
	i2c1_scl_write(FALSE);
}

/**
 * @brief I2C1������ACK�ź�
 */
static void i2c1_nack(void)
{
	i2c1_scl_write(FALSE);
	i2c1_sda_out();
	i2c1_sda_write(TRUE);
	usleep(2);
	i2c1_scl_write(TRUE);
	usleep(2);
	i2c1_scl_write(FALSE);
}

/**
 * @brief I2C1�ȴ�ACK�ź�
 */
static BOOL i2c1_wait_ack(void)
{
	u8 ucErrTime=0;
	i2c1_sda_in();      //SDA����Ϊ����  
	i2c1_sda_write(TRUE);usleep(2);	   
	i2c1_scl_write(TRUE);usleep(2);	 
	while(i2c1_sda_read())
	{
		ucErrTime++;
		if(ucErrTime > 250)
		{
			i2c1_stop();
			return TRUE;
		}
	}
	i2c1_scl_write(FALSE);//ʱ�����0 
	return FALSE;
}

/**
 * @brief I2C1��˿�дһ���ֽ�
 */
static void i2c1_writeByte(UCHAR data)
{
    UCHAR i;   
	i2c1_sda_out(); 	    
    i2c1_scl_write(FALSE);//����ʱ�ӿ�ʼ���ݴ���
    for(i=0; i<8; i++)
    {
		if(data & 0x80)
			i2c1_sda_write(TRUE);
		else
			i2c1_sda_write(FALSE);
		data <<= 1; 	  
		usleep(m_uBaudrate[0]);   //��TEA5767��������ʱ���Ǳ����
		i2c1_scl_write(TRUE);
		usleep(m_uBaudrate[0]); 
		i2c1_scl_write(FALSE);	
		usleep(m_uBaudrate[0]);
    }	 
}

/**
 * @brief I2C1�Ӷ˿ڶ�ȡһ���ֽ�
 */
static UCHAR i2c1_readByte(BOOL bAck)
{
	UCHAR i = 0;
	UCHAR rxData = 0;
	
	i2c1_sda_in();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        i2c1_scl_write(FALSE); 
        usleep(m_uBaudrate[0]);
		i2c1_scl_write(TRUE);
        rxData <<= 1;
        if(i2c1_sda_read()) 
			rxData |= 0x01;   
		usleep(m_uBaudrate[0]); 
    }					 
    if (!bAck)
        i2c1_nack();//����nACK
    else
        i2c1_ack(); //����ACK  
	
	return rxData;
}

/**
 * @brief 8λ��ַ I2Cд����
 * @param uAddress: д�����ݵ�ַ
 * @param pBuff: ����ָ�뻺����ָ��
 * @param size: д�����ݴ�С
 * @retval None
 */
static void i2c1_write(UINT8 uAddress, UCHAR* pBuff, UINT32 size)
{
	UINT32 i;
	
	i2c1_start();
	
	i2c1_writeByte(m_uDeviceID[0]);//����д����
	i2c1_wait_ack();
	
	i2c1_writeByte((UCHAR)uAddress);//д��λ��ַ
	i2c1_wait_ack();
	
	for (i = 0; i < size; ++i)
	{
		i2c1_writeByte(pBuff[i]);//д����
		i2c1_wait_ack();
	}
	
	i2c1_stop();
}

/**
 * @brief 10λ��ַ I2Cд����
 * @param uAddress: д�����ݵ�ַ
 * @param pBuff: ����ָ�뻺����ָ��
 * @param size: д�����ݴ�С
 * @retval None
 */
static void i2c1_writeEx(UINT16 uAddress, UCHAR* pBuff, UINT32 size)
{
	UINT32 i;
	
	i2c1_start();
	
	i2c1_writeByte(m_uDeviceID[0]);//����д����
	i2c1_wait_ack();
	
	i2c1_writeByte((UCHAR)uAddress>>8);//д��λ��ַ
	i2c1_wait_ack();
	i2c1_writeByte((UCHAR)uAddress);//д��λ��ַ
	i2c1_wait_ack();
	
	for (i = 0; i < size; ++i)
	{
		i2c1_writeByte(pBuff[i]);//д����
		i2c1_wait_ack();
	}
	
	i2c1_stop();
}

/**
 * @brief 8λ��ַ I2C������
 * @param uAddress: ���ݴ洢��ַ
 * @param pBuff: ����ָ�뻺����ָ��
 * @param size: �������ݴ�С
 * @retval None
 */
static UCHAR i2c1_read(UINT8 uAddress, UCHAR* pBuff, UINT32 size)
{
	UINT32 i, n;
	
	i2c1_start();
	
	i2c1_writeByte(m_uDeviceID[0]);//����д����
	i2c1_wait_ack();
	
	i2c1_writeByte((UCHAR)(uAddress));//д��ַ
	i2c1_wait_ack();
	
	i2c1_start();
	i2c1_writeByte(m_uDeviceID[0]+1);//���Ͷ�����
	i2c1_wait_ack();
	
	n = size-1;
	for (i = 0; i < n; ++i)
	{
		pBuff[i] = i2c1_readByte(TRUE);
	}
	pBuff[i] = i2c1_readByte(FALSE);
	
	i2c1_stop();
	
	return 1;
}

/**
 * @brief 10λ��ַ I2C������
 * @param uAddress: ���ݴ洢��ַ
 * @param pBuff: ����ָ�뻺����ָ��
 * @param size: �������ݴ�С
 * @retval None
 */
static UCHAR i2c1_readEx(UINT16 uAddress, UCHAR* pBuff, UINT32 size)
{
	UINT32 i, n;
	
	i2c1_start();
	
	i2c1_writeByte(m_uDeviceID[0]);//����д����
	i2c1_wait_ack();
	
	i2c1_writeByte((UCHAR)uAddress>>8);//д��λ��ַ
	i2c1_wait_ack();
	i2c1_writeByte((UCHAR)(uAddress));//д��λ��ַ
	i2c1_wait_ack();
	
	i2c1_start();
	i2c1_writeByte(m_uDeviceID[0]+1);//���Ͷ�����
	i2c1_wait_ack();
	
	n = size-1;
	for (i = 0; i < n; ++i)
	{
		pBuff[i] = i2c1_readByte(TRUE);
	}
	pBuff[i] = i2c1_readByte(FALSE);
	
	i2c1_stop();
	
	return 1;
}


//>>>>>>>>>>>>>>>>>>>>>>>>> I2C2���� <<<<<<<<<<<<<<<<<<<<<<<<<<<<

/**
 * @brief I2C2_SDA��������Ϊ���
 */
static void i2c2_sda_out(void)
{
	GPIO_InitStructure.GPIO_Pin = I2C2_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_Init(I2C2_GPIO_TYPE, &GPIO_InitStructure);
}

/**
 * @brief I2C2_SDA�������� ����Ϊ����
 */
static void i2c2_sda_in(void)
{
	GPIO_InitStructure.GPIO_Pin = I2C2_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   //��������
	GPIO_Init(I2C2_GPIO_TYPE, &GPIO_InitStructure);
}

/**
 * @brief дI2C2_SCL�˿�
 */
static void i2c2_scl_write(BOOL bOn)
{
	 if (bOn)
	 {
		 I2C2_GPIO_TYPE->BSRR = I2C2_SCL;
	 }
	 else
	 {
		 I2C2_GPIO_TYPE->BRR = I2C2_SCL;
	 }
}

/**
 * @brief дI2C2_SDA�˿�
 */
static void i2c2_sda_write(BOOL bOn)
{
	if (bOn)
	 {
		 I2C2_GPIO_TYPE->BSRR = I2C2_SDA;
	 }
	 else
	 {
		 I2C2_GPIO_TYPE->BRR = I2C2_SDA;
	 }
}

/**
 * @brief ��I2C2_SDA�˿�
 */
static BOOL i2c2_sda_read(void)
{
	if ((I2C2_GPIO_TYPE->IDR & I2C2_SDA) != (uint32_t)Bit_RESET)
		 return TRUE;
	 else
		return FALSE;
}

/**
 * @brief ��ʼ��IIC2��IO��	
 */
static void i2c2_init(void)
{
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = I2C2_SCL | I2C2_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C2_GPIO_TYPE, &GPIO_InitStructure);
	GPIO_SetBits(I2C2_GPIO_TYPE, I2C2_SCL | I2C2_SDA); 	//PB10,PB11 �����
}

static void i2c2_deInit(void)
{
	GPIO_InitStructure.GPIO_Pin = I2C2_SCL | I2C2_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C2_GPIO_TYPE, &GPIO_InitStructure);
}

static void i2c2_setDeviceID(UINT16 uID)
{
	m_uDeviceID[1] = uID;
}
static void i2c2_setBaudrate(UINT32 baudrate)
{
	m_uBaudrate[0] = baudrate;
}
/**
 * @brief I2C2����IIC��ʼ�ź�
 */
static void i2c2_start(void)
{
	i2c2_sda_out();     //sda�����
	i2c2_sda_write(TRUE);	  	  
	i2c2_scl_write(TRUE);
	usleep(4);
 	i2c2_sda_write(FALSE);//START:when CLK is high,DATA change form high to low 
	usleep(4);
	i2c2_scl_write(FALSE);//ǯסI2C���ߣ�׼�����ͻ�������� 
}

/**
 * @brief I2C2����IICֹͣ�ź�
 */
static void i2c2_stop(void)
{
	i2c2_sda_out();//sda�����
	i2c2_scl_write(FALSE);
	i2c2_sda_write(FALSE);//STOP:when CLK is high DATA change form low to high
 	usleep(4);
	i2c2_scl_write(TRUE); 
	i2c2_sda_write(TRUE);//����I2C���߽����ź�
	usleep(4);	
}

/**
 * @brief I2C2����ACK�ź�
 */
static void i2c2_ack(void)
{
	i2c2_scl_write(FALSE);
	i2c2_sda_out();
	i2c2_sda_write(FALSE);
	usleep(2);
	i2c2_scl_write(TRUE);
	usleep(2);
	i2c2_scl_write(FALSE);
}

/**
 * @brief I2C2������ACK�ź�
 */
static void i2c2_nack(void)
{
	i2c2_scl_write(FALSE);
	i2c2_sda_out();
	i2c2_sda_write(TRUE);
	usleep(2);
	i2c2_scl_write(TRUE);
	usleep(2);
	i2c2_scl_write(FALSE);
}

/**
 * @brief I2C2�ȴ�ACK�ź�
 */
static BOOL i2c2_wait_ack(void)
{
	u8 ucErrTime=0;
	i2c2_sda_in();      //SDA����Ϊ����  
	i2c2_sda_write(TRUE);usleep(2);	   
	i2c2_scl_write(TRUE);usleep(2);	 
	while(i2c2_sda_read())
	{
		ucErrTime++;
		if(ucErrTime > 250)
		{
			i2c2_stop();
			return TRUE;
		}
	}
	i2c2_scl_write(FALSE);//ʱ�����0 
	return FALSE;
}

/**
 * @brief I2C2��˿�һ���ֽ�
 */
static void i2c2_writeByte(UCHAR data)
{
    UCHAR i;   
	i2c2_sda_out(); 	    
    i2c2_scl_write(FALSE);//����ʱ�ӿ�ʼ���ݴ���
    for(i=0; i<8; i++)
    {
		if(data & 0x80)
			i2c2_sda_write(TRUE);
		else
			i2c2_sda_write(FALSE);
		data <<= 1; 	  
		usleep(m_uBaudrate[0]);   //��TEA5767��������ʱ���Ǳ����
		i2c2_scl_write(TRUE);
		usleep(m_uBaudrate[0]); 
		i2c2_scl_write(FALSE);	
		usleep(m_uBaudrate[0]);
    }	 
}

/**
 * @brief I2C2�Ӷ˿ڶ�ȡһ���ֽ�
 */
static UCHAR i2c2_readByte(BOOL bAck)
{
	UCHAR i = 0;
	UCHAR rxData = 0;
	
	i2c2_sda_in();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        i2c2_scl_write(FALSE); 
        usleep(m_uBaudrate[0]);
		i2c2_scl_write(TRUE);
        rxData <<= 1;
        if(i2c2_sda_read()) 
			rxData |= 0x01;   
		usleep(m_uBaudrate[0]); 
    }					 
    if (!bAck)
        i2c2_nack();//����nACK
    else
        i2c2_ack(); //����ACK  
	
	return rxData;
}

/**
 * @brief 8λ��ַ I2Cд����
 * @param uAddress: д�����ݵ�ַ
 * @param pBuff: ����ָ�뻺����ָ��
 * @param size: д�����ݴ�С
 * @retval None
 */
static void i2c2_write(UINT8 uAddress, UCHAR* pBuff, UINT32 size)
{
	UINT32 i;
	i2c2_start();
	
	i2c2_writeByte(m_uDeviceID[0]);//����д����
	i2c2_wait_ack();
	
	i2c2_writeByte((UCHAR)uAddress);//д��ַ
	i2c2_wait_ack();
	
	for (i = 0; i < size; ++i)
	{
		i2c2_writeByte(pBuff[i]);//д����
		i2c2_wait_ack();
	}
	
	i2c2_stop();
}

/**
 * @brief 10λ��ַ I2Cд����
 * @param uAddress: д�����ݵ�ַ
 * @param pBuff: ����ָ�뻺����ָ��
 * @param size: д�����ݴ�С
 * @retval None
 */
static void i2c2_writeEx(UINT16 uAddress, UCHAR* pBuff, UINT32 size)
{
	UINT32 i;
	i2c2_start();
	
	i2c2_writeByte(m_uDeviceID[0]);//����д����
	i2c2_wait_ack();
	
	i2c2_writeByte((UCHAR)uAddress>>8);//д��λ��ַ
	i2c2_wait_ack();
	i2c2_writeByte((UCHAR)uAddress);//д��λ��ַ
	i2c2_wait_ack();
	
	for (i = 0; i < size; ++i)
	{
		i2c2_writeByte(pBuff[i]);//д����
		i2c2_wait_ack();
	}
	
	i2c2_stop();
}

/**
 * @brief 8λ��ַ I2C������
 * @param uAddress: ���ݴ洢��ַ
 * @param pBuff: ����ָ�뻺����ָ��
 * @param size: �������ݴ�С
 * @retval None
 */
static UCHAR i2c2_read(UINT8 uAddress, UCHAR* pBuff, UINT32 size)
{
	UINT32 i, n;
	
	i2c2_start();
	
	i2c2_writeByte(m_uDeviceID[0]);//����д����
	i2c2_wait_ack();
	
	i2c2_writeByte((UCHAR)(uAddress));//д��ַ
	i2c2_wait_ack();
	
	i2c2_start();
	i2c2_writeByte(m_uDeviceID[0]+1);//���Ͷ�����
	i2c2_wait_ack();
	
	n = size-1;
	for (i = 0; i < n; ++i)
	{
		pBuff[i] = i2c2_readByte(TRUE);
	}
	pBuff[i] = i2c2_readByte(FALSE);
	
	i2c2_stop();
	
	return 1;
}

/**
 * @brief 10λ��ַ I2C������
 * @param uAddress: ���ݴ洢��ַ
 * @param pBuff: ����ָ�뻺����ָ��
 * @param size: �������ݴ�С
 * @retval None
 */
static UCHAR i2c2_readEx(UINT16 uAddress, UCHAR* pBuff, UINT32 size)
{
	UINT32 i, n;
	
	i2c2_start();
	
	i2c2_writeByte(m_uDeviceID[0]);//����д����
	i2c2_wait_ack();
	
	i2c2_writeByte((UCHAR)uAddress>>8);//д��λ��ַ
	i2c2_wait_ack();
	i2c2_writeByte((UCHAR)(uAddress));//д��λ��ַ
	i2c2_wait_ack();
	
	i2c2_start();
	i2c2_writeByte(m_uDeviceID[0]+1);//���Ͷ�����
	i2c2_wait_ack();
	
	n = size-1;
	for (i = 0; i < n; ++i)
	{
		pBuff[i] = i2c2_readByte(TRUE);
	}
	pBuff[i] = i2c2_readByte(FALSE);
	
	i2c2_stop();
	
	return 1;
}


/**
 * @brief ����I2C�����ṹ����
 * @param eChannel: I2C ͨ�� @ref HALI2CNumer
 * @retval None
 */
static void New(HALI2CNumer eChannel)
{
	switch (eChannel)
	{
	case HALI2CNumer1:
		m_Instance[0].init = i2c1_init;
		m_Instance[0].deInit = i2c1_deInit;
		m_Instance[0].setDeviceID = i2c1_setDeviceID;
		m_Instance[0].setBaudrate = i2c1_setBaudrate;
		m_Instance[0].write = i2c1_write;
		m_Instance[0].read = i2c1_read;
		m_Instance[0].writeEx = i2c1_writeEx;
		m_Instance[0].readEx = i2c1_readEx;
		m_pthis[0] = &m_Instance[0];
		m_pthis[0]->init();
		break;
	case HALI2CNumer2:
		m_Instance[1].init = i2c2_init;
		m_Instance[1].deInit = i2c2_deInit;
		m_Instance[1].setDeviceID = i2c2_setDeviceID;
		m_Instance[1].setBaudrate = i2c2_setBaudrate;
		m_Instance[1].write = i2c2_write;
		m_Instance[1].read = i2c2_read;
		m_Instance[1].writeEx = i2c2_writeEx;
		m_Instance[1].readEx = i2c2_readEx;
		m_pthis[1] = &m_Instance[1];
		m_pthis[1]->init();
		break;
	default:
		break;
	}
}

/**
 * @brief ��ȡI2C�����ṹ���
 * @param eChannel: I2C ͨ�� @ref HALI2CNumer
 * @retval I2C�����ṹ���
 */
HALI2CTypeDef* HalI2cGetInstance(HALI2CNumer eChannel)
{
	if (NULL == m_pthis[eChannel])
	{
		New(eChannel);
	}
	return m_pthis[eChannel];
}

#endif //CFG_HAL_I2C
