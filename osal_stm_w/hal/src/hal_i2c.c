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
 * All rights Reserved
 *
 */
#include "hal_i2c.h"

#ifdef CFG_HAL_I2C

#define WR_WAIT_TIME 200

#define I2C1_SDA_H	GPIO_SetBits(I2C1_GPIO_TYPE, I2C1_SDA)
#define I2C1_SDA_L	GPIO_ResetBits(I2C1_GPIO_TYPE, I2C1_SDA)
#define I2C1_SDA_R	GPIO_ReadInputDataBit(I2C1_GPIO_TYPE, I2C1_SDA)
#define I2C1_SCL_H	GPIO_SetBits(I2C1_GPIO_TYPE, I2C1_SCL)
#define I2C1_SCL_L	GPIO_ResetBits(I2C1_GPIO_TYPE, I2C1_SCL)
#define I2C1_SCL_R	GPIO_ReadInputDataBit(I2C1_GPIO_TYPE, I2C1_SCL)

#define I2C2_SDA_H	GPIO_SetBits(I2C2_GPIO_TYPE, I2C2_SDA)
#define I2C2_SDA_L	GPIO_ResetBits(I2C2_GPIO_TYPE, I2C2_SDA)
#define I2C2_SDA_R	GPIO_ReadInputDataBit(I2C2_GPIO_TYPE, I2C2_SDA)
#define I2C2_SCL_H	GPIO_SetBits(I2C2_GPIO_TYPE, I2C2_SCL)
#define I2C2_SCL_L	GPIO_ResetBits(I2C2_GPIO_TYPE, I2C2_SCL)
#define I2C2_SCL_R	GPIO_ReadInputDataBit(I2C2_GPIO_TYPE, I2C2_SCL)


/** I2C GPIO ��ʼ���ṹ */
static GPIO_InitTypeDef  GPIO_InitStructure[HALI2CNumer_SIZE];

static UINT32 m_uBaudrate[HALI2CNumer_SIZE] = {8, 8};
static UINT16 m_uDeviceID[HALI2CNumer_SIZE] = {0};
static HALI2CTypeDef m_Instance[HALI2CNumer_SIZE];
static HALI2CTypeDef* m_pthis[HALI2CNumer_SIZE] = {NULL};

/**
 * @brief ��ʼ��IIC1��IO��	
 */
static void i2c1_init(void)
{
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	

	GPIO_InitStructure[0].GPIO_Pin = I2C1_SCL | I2C1_SDA;
	GPIO_InitStructure[0].GPIO_Mode = GPIO_Mode_Out_OD ;   //��©���
	GPIO_InitStructure[0].GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C1_GPIO_TYPE, &GPIO_InitStructure[0]);
	
	GPIO_SetBits(I2C1_GPIO_TYPE, I2C1_SCL | I2C1_SDA); 	//I2C1_SCL, I2C1_SDA �����
}

static void i2c1_deInit(void)
{
	GPIO_InitStructure[0].GPIO_Pin = I2C1_SCL | I2C1_SDA;
	GPIO_InitStructure[0].GPIO_Mode = GPIO_Mode_IN_FLOATING ;   //��������
	GPIO_InitStructure[0].GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C1_GPIO_TYPE, &GPIO_InitStructure[0]);
}

static void i2c1_setDeviceID(UINT16 uID)
{
	m_uDeviceID[0] = uID;
}
static void i2c1_setBaudrate(UINT32 baudrate)
{
	m_uBaudrate[0] = (baudrate > 0) ? baudrate : 8;
}
/**
 * @brief I2C1����IIC��ʼ�ź�
 */
static void i2c1_start(void)
{
	I2C1_SDA_H;						//����SDA��
	I2C1_SCL_H;						//����SCL��
	usleep(m_uBaudrate[0]);		//��ʱ���ٶȿ���
	
	I2C1_SDA_L;						//START when CLK is high, DATA change form high to low 
	usleep(m_uBaudrate[0]);		//��ʱ���ٶȿ���
	I2C1_SCL_L;						//ǯסSCL�ߣ��Ա㷢������
}

/**
 * @brief I2C1����IICֹͣ�ź�
 */
static void i2c1_stop(void)
{
	I2C1_SDA_L;						//����SDA��
	I2C1_SCL_L;						//����SCL��
	usleep(m_uBaudrate[0]);		//��ʱ���ٶȿ���
	
	I2C1_SCL_H;						//����SCL��
	I2C1_SDA_H;						//STOP when CLK is high, DATA change form low to high
	usleep(m_uBaudrate[0]);
}

/**
 * @brief I2C1����ACK�ź�
 */
static void i2c1_ack(void)
{
	I2C1_SCL_L;						//����SCL��
	I2C1_SDA_L;						//����SDA��
	usleep(m_uBaudrate[0]);
	I2C1_SCL_H;						//����SCL��
	usleep(m_uBaudrate[0]);
	I2C1_SCL_L;						//����SCL��
}

/**
 * @brief I2C1������ACK�ź�
 */
static void i2c1_nack(void)
{
	I2C1_SCL_L;						//����SCL��
	I2C1_SDA_H;						//����SDA��
	usleep(m_uBaudrate[0]);
	I2C1_SCL_H;						//����SCL��
	usleep(m_uBaudrate[0]);
	I2C1_SCL_L;						//����SCL��
}

/**
 * @brief I2C1�ȴ�ACK�ź�
 * @param wt �ȴ�ACKʱ��
 */
static BOOL i2c1_wait_ack(UINT32 wt)
{
	I2C1_SDA_H; usleep(m_uBaudrate[0]);			//����SDA��
	I2C1_SCL_H; usleep(m_uBaudrate[0]);			//����SCL��
	
	wt = (wt > 0) ? wt : 1;
	while(I2C1_SDA_R)							//�������SDA��Ϊ1����ȴ���Ӧ���ź�Ӧ��0
	{
		if(--wt)
		{
			i2c1_stop();						//��ʱδ�յ�Ӧ����ֹͣ����
			return FALSE;					//����ʧ��
		}
		usleep(m_uBaudrate[0]);
	}
	
	I2C1_SCL_L;									//����SCL�ߣ��Ա�����շ�����
	return TRUE;
}

/**
 * @brief I2C1��˿�һ���ֽ�
 */
static void i2c1_writeByte(UCHAR data)
{
	UINT8 count = 0;
	
    I2C1_SCL_L;							//����ʱ�ӿ�ʼ���ݴ���
	
    for(; count < 8; count++)		//ѭ��8�Σ�ÿ�η���һ��bit
    {
		if(data & 0x80)				//�������λ
			I2C1_SDA_H;
		else
			I2C1_SDA_L;
		
		data <<= 1;					//byte����1λ
		
		usleep(m_uBaudrate[0]);
		I2C1_SCL_H;
		usleep(m_uBaudrate[0]);
		I2C1_SCL_L;
    }
}

/**
 * @brief I2C1�Ӷ˿ڶ�ȡһ���ֽ�
 */
static UCHAR i2c1_readByte(BOOL bAck)
{
	UINT8 count = 0;
	UCHAR rxData = 0;
	
	I2C1_SDA_H;							//����SDA�ߣ���©״̬�£����������Ա��ȡ����
	
    for(; count < 8; count++ )		//ѭ��8�Σ�ÿ�η���һ��bit
	{
		I2C1_SCL_L;
		usleep(m_uBaudrate[0]);
		I2C1_SCL_H;
		
        rxData <<= 1;				//����һλ
		
        if(I2C1_SDA_R)					//���SDA��Ϊ1�����bit0��+1��Ȼ����һ��ѭ����������һ��
			rxData |= 0x01;
		
		usleep(m_uBaudrate[0]);
    }
	
	if (bAck)
		i2c1_ack();
	else
		i2c1_nack();
	
    return rxData;
}

/**
 * @brief 8λ��ַ I2Cд����
 * @param uAddress д�����ݵ�ַ
 * @param pBuff ����ָ�뻺����ָ��
 * @param size д�����ݴ�С
 * @param wt �ȴ�ACKʱ��
 * @retval �����ɹ�����ʵ��д������ݴ�С, ʧ�ܷ���-1
 */
static int i2c1_write(UINT8 uAddress, UCHAR* pBuff, UINT32 size, UINT32 wt)
{
	UINT32 i;
	if (NULL == pBuff)
		size = 0;
	
	i2c1_start();
	
	i2c1_writeByte(m_uDeviceID[0]);//����д����
	if (!i2c1_wait_ack(wt))
		return -1;
	
	i2c1_writeByte((UCHAR)uAddress);//д��ַ
	if (!i2c1_wait_ack(wt))
		return -1;
	
	for (i = 0; i < size; ++i)
	{
		i2c1_writeByte(pBuff[i]);//д����
		if (!i2c1_wait_ack(wt))
			return -1;
		usleep(m_uBaudrate[0]);
	}
	
	i2c1_stop();
	return size;
}

/**
 * @brief 10λ��ַ I2Cд����
 * @param uAddress д�����ݵ�ַ
 * @param pBuff ����ָ�뻺����ָ��
 * @param size д�����ݴ�С
 * @param wt �ȴ�ACKʱ��
 * @retval �����ɹ�����ʵ��д������ݴ�С, ʧ�ܷ���-1
 */
static int i2c1_writeEx(UINT16 uAddress, UCHAR* pBuff, UINT32 size, UINT32 wt)
{
	UINT32 i;
	if (NULL == pBuff)
		size = 0;
	
	i2c1_start();
	
	i2c1_writeByte(m_uDeviceID[0]);//����д����
	if (!i2c1_wait_ack(wt))
		return -1;
	
	i2c1_writeByte((UCHAR)uAddress>>8);//д��λ��ַ
	if (!i2c1_wait_ack(wt))
		return -1;
	i2c1_writeByte((UCHAR)uAddress);//д��λ��ַ
	if (!i2c1_wait_ack(wt))
		return -1;
	
	for (i = 0; i < size; ++i)
	{
		i2c1_writeByte(pBuff[i]);//д����
		if (!i2c1_wait_ack(wt))
			return -1;
		usleep(m_uBaudrate[0]);
	}
	
	i2c1_stop();
	return size;
}

/**
 * @brief 8λ��ַ I2C������
 * @param uAddress ���ݴ洢��ַ
 * @param pBuff ����ָ�뻺����ָ��
 * @param size �������ݴ�С
 * @param wt �ȴ�ACKʱ��
 * @param rdelay ��ȡ���ݵȴ�ʱ��, �����Ͷ������ȴ���ȡ���ݵ�ʱ��
 * @retval �����ɹ�����ʵ�ʶ�ȡ�����ݴ�С, ʧ�ܷ���-1
 */
static int i2c1_read(UINT8 uAddress, UCHAR* pBuff, UINT32 size, UINT32 wt, UINT32 rdelay)
{
	UINT32 i;
	INT32 n;
	if (NULL == pBuff)
		return -1;
	
	i2c1_start();
	
	i2c1_writeByte(m_uDeviceID[0]);//����д����
	if (!i2c1_wait_ack(wt))
		return -1;
	
	i2c1_writeByte((UCHAR)(uAddress));//д��ַ
	if (!i2c1_wait_ack(wt))
		return -1;
	
	i = 0;
	i2c1_start();
	i2c1_writeByte(m_uDeviceID[0]+1);//���Ͷ�����
	while (!i2c1_wait_ack(wt))
	{
		if ((i++) > WR_WAIT_TIME)
			return -1;
		usleep(m_uBaudrate[0]);
		i2c1_start();
		i2c1_writeByte(m_uDeviceID[0]+1);//���Ͷ�����
	}
	//if (!i2c1_wait_ack(wt))
	//	return -1;
	
	usleep(rdelay);
	n = size-1;
	for (i = 0; i < n; ++i)
	{
		pBuff[i] = i2c1_readByte(TRUE);
	}
	pBuff[i] = i2c1_readByte(FALSE);
	
	i2c1_stop();
	return size;
}

/**
 * @brief 10λ��ַ I2C������
 * @param uAddress ���ݴ洢��ַ
 * @param pBuff ����ָ�뻺����ָ��
 * @param size �������ݴ�С
 * @param wt �ȴ�ACKʱ��
 * @param rdelay ��ȡ���ݵȴ�ʱ��, �����Ͷ������ȴ���ȡ���ݵ�ʱ��
 * @retval �����ɹ�����ʵ�ʶ�ȡ�����ݴ�С, ʧ�ܷ���-1
 */
static int i2c1_readEx(UINT16 uAddress, UCHAR* pBuff, UINT32 size, UINT32 wt, UINT32 rdelay)
{
	UINT32 i;
	INT32 n;
	if (NULL == pBuff)
		return -1;
	
	i2c1_start();
	
	i2c1_writeByte(m_uDeviceID[0]);//����д����
	if (!i2c1_wait_ack(wt))
		return -1;
	
	i2c1_writeByte((UCHAR)uAddress>>8);//д��λ��ַ
	if (!i2c1_wait_ack(wt))
		return -1;
	i2c1_writeByte((UCHAR)(uAddress));//д��λ��ַ
	if (!i2c1_wait_ack(wt))
		return -1;
	
	i = 0;
	i2c1_start();
	i2c1_writeByte(m_uDeviceID[0]+1);//���Ͷ�����
	while (!i2c1_wait_ack(wt))
	{
		if ((i++) > WR_WAIT_TIME)
			return -1;
		usleep(m_uBaudrate[0]);
		i2c1_start();
		i2c1_writeByte(m_uDeviceID[0]+1);//���Ͷ�����
	}
	//if (!i2c1_wait_ack(wt))
	//	return -1;
	
	usleep(rdelay);
	n = size-1;
	for (i = 0; i < n; ++i)
	{
		pBuff[i] = i2c1_readByte(TRUE);
	}
	pBuff[i] = i2c1_readByte(FALSE);
	
	i2c1_stop();
	return size;
}

/**
 * @brief ���I2C�Ƿ��������
 * @param 
 * @retval ��������TRUE�����򷵻�FALSE
 */
BOOL i2c1_checkLock(void)
{
	return I2C1_SCL_R && !I2C1_SDA_R;
}

/**
 * @brief I2C����. ����9��SCL�������
 * @param 
 * @retval None
 */
void i2c1_unlock(void)
{
	int i=0;
	I2C1_SCL_H;						//����SCL��
	for (i=0; i<9; i++)
	{
		usleep(m_uBaudrate[0]);
		I2C1_SCL_L;						//����SCL��
		usleep(m_uBaudrate[0]);
		I2C1_SCL_H;						//����SCL��
	}
}

//>>>>>>>>>>>>>>>>>>>>>>>>> I2C2���� <<<<<<<<<<<<<<<<<<<<<<<<<<<<

/**
 * @brief ��ʼ��IIC2��IO��	
 */
static void i2c2_init(void)
{
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	

	GPIO_InitStructure[1].GPIO_Pin = I2C2_SCL | I2C2_SDA;
	GPIO_InitStructure[1].GPIO_Mode = GPIO_Mode_Out_OD ;   //��©���
	GPIO_InitStructure[1].GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C2_GPIO_TYPE, &GPIO_InitStructure[1]);
	
	GPIO_SetBits(I2C2_GPIO_TYPE, I2C2_SCL | I2C2_SDA); 	//I2C2_SCL, I2C2_SDA �����
}

static void i2c2_deInit(void)
{
	GPIO_InitStructure[1].GPIO_Pin = I2C2_SCL | I2C2_SDA;
	GPIO_InitStructure[1].GPIO_Mode = GPIO_Mode_IN_FLOATING ;   //��������
	GPIO_InitStructure[1].GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C2_GPIO_TYPE, &GPIO_InitStructure[1]);
}

static void i2c2_setDeviceID(UINT16 uID)
{
	m_uDeviceID[1] = uID;
}
static void i2c2_setBaudrate(UINT32 baudrate)
{
	m_uBaudrate[1] = (baudrate > 0) ? baudrate : 8;
}
/**
 * @brief I2C2����IIC��ʼ�ź�
 */
static void i2c2_start(void)
{
	I2C2_SDA_H;						//����SDA��
	I2C2_SCL_H;						//����SCL��
	usleep(m_uBaudrate[1]);		//��ʱ���ٶȿ���
	
	I2C2_SDA_L;						//START when CLK is high, DATA change form high to low 
	usleep(m_uBaudrate[1]);		//��ʱ���ٶȿ���
	I2C2_SCL_L;						//ǯסSCL�ߣ��Ա㷢������
}

/**
 * @brief I2C2����IICֹͣ�ź�
 */
static void i2c2_stop(void)
{
	I2C2_SDA_L;						//����SDA��
	I2C2_SCL_L;						//����SCL��
	usleep(m_uBaudrate[1]);		//��ʱ���ٶȿ���
	
	I2C2_SCL_H;						//����SCL��
	I2C2_SDA_H;						//STOP when CLK is high, DATA change form low to high
	usleep(m_uBaudrate[1]);
}

/**
 * @brief I2C2����ACK�ź�
 */
static void i2c2_ack(void)
{
	I2C2_SCL_L;						//����SCL��
	I2C2_SDA_L;						//����SDA��
	usleep(m_uBaudrate[1]);
	I2C2_SCL_H;						//����SCL��
	usleep(m_uBaudrate[1]);
	I2C2_SCL_L;						//����SCL��
}

/**
 * @brief I2C2������ACK�ź�
 */
static void i2c2_nack(void)
{
	I2C2_SCL_L;						//����SCL��
	I2C2_SDA_H;						//����SDA��
	usleep(m_uBaudrate[1]);
	I2C2_SCL_H;						//����SCL��
	usleep(m_uBaudrate[1]);
	I2C2_SCL_L;						//����SCL��
}

/**
 * @brief I2C2�ȴ�ACK�ź�
 * @param wt �ȴ�ACKʱ��
 */
static BOOL i2c2_wait_ack(UINT32 wt)
{
	I2C2_SDA_H;usleep(m_uBaudrate[1]);			//����SDA��
	I2C2_SCL_H;usleep(m_uBaudrate[1]);			//����SCL��
	
	wt = (wt > 0) ? wt : 1;
	while(I2C2_SDA_R)							//�������SDA��Ϊ1����ȴ���Ӧ���ź�Ӧ��0
	{
		if(--wt)
		{
			i2c2_stop();						//��ʱδ�յ�Ӧ����ֹͣ����
			return FALSE;					//����ʧ��
		}
		usleep(m_uBaudrate[1]);
	}
	
	I2C2_SCL_L;									//����SCL�ߣ��Ա�����շ�����
	return TRUE;
}

/**
 * @brief I2C2��˿�һ���ֽ�
 */
static void i2c2_writeByte(UCHAR data)
{
	UINT8 count = 0;
	
    I2C2_SCL_L;							//����ʱ�ӿ�ʼ���ݴ���
	
    for(; count < 8; count++)		//ѭ��8�Σ�ÿ�η���һ��bit
    {
		if(data & 0x80)				//�������λ
			I2C2_SDA_H;
		else
			I2C2_SDA_L;
		
		data <<= 1;					//byte����1λ
		
		usleep(m_uBaudrate[1]);
		I2C2_SCL_H;
		usleep(m_uBaudrate[1]);
		I2C2_SCL_L;
    }
}

/**
 * @brief I2C2�Ӷ˿ڶ�ȡһ���ֽ�
 */
static UCHAR i2c2_readByte(BOOL bAck)
{
	UINT8 count = 0;
	UCHAR rxData = 0;
	
	I2C2_SDA_H;							//����SDA�ߣ���©״̬�£����������Ա��ȡ����
	
    for(; count < 8; count++ )		//ѭ��8�Σ�ÿ�η���һ��bit
	{
		I2C2_SCL_L;
		usleep(m_uBaudrate[1]);
		I2C2_SCL_H;
		
        rxData <<= 1;				//����һλ
		
        if(I2C2_SDA_R)					//���SDA��Ϊ1�����bit0��+1��Ȼ����һ��ѭ����������һ��
			rxData |= 0x01;
		
		usleep(m_uBaudrate[1]);
    }
	
	if (bAck)
		i2c2_ack();
	else
		i2c2_nack();
	
    return rxData;
}

/**
 * @brief 8λ��ַ I2Cд����
 * @param uAddress д�����ݵ�ַ
 * @param pBuff ����ָ�뻺����ָ��
 * @param size д�����ݴ�С
 * @param wt �ȴ�ACKʱ��
 * @retval �����ɹ�����ʵ��д������ݴ�С, ʧ�ܷ���-1
 */
static int i2c2_write(UINT8 uAddress, UCHAR* pBuff, UINT32 size, UINT32 wt)
{
	UINT32 i;
	if (NULL == pBuff)
		size = 0;
	
	i2c2_start();
	
	i2c2_writeByte(m_uDeviceID[1]);//����д����
	if (!i2c2_wait_ack(wt))
		return -1;
	
	i2c2_writeByte((UCHAR)uAddress);//д��ַ
	if (!i2c2_wait_ack(wt))
		return -1;
	
	for (i = 0; i < size; ++i)
	{
		i2c2_writeByte(pBuff[i]);//д����
		if (!i2c2_wait_ack(wt))
			return -1;
		usleep(m_uBaudrate[1]);
	}
	
	i2c2_stop();
	return size;
}

/**
 * @brief 10λ��ַ I2Cд����
 * @param uAddress д�����ݵ�ַ
 * @param pBuff ����ָ�뻺����ָ��
 * @param size д�����ݴ�С
 * @param wt �ȴ�ACKʱ��
 * @retval �����ɹ�����ʵ��д������ݴ�С, ʧ�ܷ���-1
 */
static int i2c2_writeEx(UINT16 uAddress, UCHAR* pBuff, UINT32 size, UINT32 wt)
{
	UINT32 i;
	if (NULL == pBuff)
		size = 0;
	
	i2c2_start();
	
	i2c2_writeByte(m_uDeviceID[1]);//����д����
	if (!i2c2_wait_ack(wt))
		return -1;
	
	i2c2_writeByte((UCHAR)uAddress>>8);//д��λ��ַ
	if (!i2c2_wait_ack(wt))
		return -1;
	i2c2_writeByte((UCHAR)uAddress);//д��λ��ַ
	if (!i2c2_wait_ack(wt))
		return -1;
	
	for (i = 0; i < size; ++i)
	{
		i2c2_writeByte(pBuff[i]);//д����
		if (!i2c2_wait_ack(wt))
			return -1;
		usleep(m_uBaudrate[1]);
	}
	
	i2c2_stop();
	return size;
}

/**
 * @brief 8λ��ַ I2C������
 * @param uAddress ���ݴ洢��ַ
 * @param pBuff ����ָ�뻺����ָ��
 * @param size �������ݴ�С
 * @param wt �ȴ�ACKʱ��
 * @param rdelay ��ȡ���ݵȴ�ʱ��, �����Ͷ������ȴ���ȡ���ݵ�ʱ��
 * @retval �����ɹ�����ʵ�ʶ�ȡ�����ݴ�С, ʧ�ܷ���-1
 */
static int i2c2_read(UINT8 uAddress, UCHAR* pBuff, UINT32 size, UINT32 wt, UINT32 rdelay)
{
	UINT32 i;
	INT32 n;
	if (NULL == pBuff)
		return -1;
	
	i2c2_start();
	
	i2c2_writeByte(m_uDeviceID[1]);//����д����
	if (!i2c2_wait_ack(wt))
		return -1;
	
	i2c2_writeByte((UCHAR)(uAddress));//д��ַ
	if (!i2c2_wait_ack(wt))
		return -1;
	
	i = 0;
	i2c2_start();
	i2c2_writeByte(m_uDeviceID[1]+1);//���Ͷ�����
	while (!i2c2_wait_ack(wt))
	{
		//if ((i++) > WR_WAIT_TIME)
		//	return -1;
		usleep(m_uBaudrate[1]);
		i2c2_start();
		i2c2_writeByte(m_uDeviceID[1]+1);//���Ͷ�����
	}
	//if (!i2c2_wait_ack(wt))
	//	return -1;
	
	usleep(rdelay);
	n = size-1;
	for (i = 0; i < n; ++i)
	{
		pBuff[i] = i2c2_readByte(TRUE);
	}
	pBuff[i] = i2c2_readByte(FALSE);
	
	i2c2_stop();
	return size;
}

/**
 * @brief 10λ��ַ I2C������
 * @param uAddress ���ݴ洢��ַ
 * @param pBuff ����ָ�뻺����ָ��
 * @param size �������ݴ�С
 * @param wt �ȴ�ACKʱ��
 * @param rdelay ��ȡ���ݵȴ�ʱ��, �����Ͷ������ȴ���ȡ���ݵ�ʱ��
 * @retval �����ɹ�����ʵ�ʶ�ȡ�����ݴ�С, ʧ�ܷ���-1
 */
static int i2c2_readEx(UINT16 uAddress, UCHAR* pBuff, UINT32 size, UINT32 wt, UINT32 rdelay)
{
	UINT32 i;
	INT32 n;
	if (NULL == pBuff)
		return -1;
	
	i2c2_start();
	
	i2c2_writeByte(m_uDeviceID[1]);//����д����
	if (!i2c2_wait_ack(wt))
		return -1;
	
	i2c2_writeByte((UCHAR)uAddress>>8);//д��λ��ַ
	if (!i2c2_wait_ack(wt))
		return -1;
	i2c2_writeByte((UCHAR)(uAddress));//д��λ��ַ
	if (!i2c2_wait_ack(wt))
		return -1;
	
	i = 0;
	i2c2_start();
	i2c2_writeByte(m_uDeviceID[1]+1);//���Ͷ�����
	while (!i2c2_wait_ack(wt))
	{
		if ((i++) > WR_WAIT_TIME)
			return -1;
		usleep(m_uBaudrate[1]);
		i2c2_start();
		i2c2_writeByte(m_uDeviceID[1]+1);//���Ͷ�����
	}
	//if (!i2c2_wait_ack(wt))
	//	return -1;

	usleep(rdelay);
	n = size-1;
	for (i = 0; i < n; ++i)
	{
		pBuff[i] = i2c2_readByte(TRUE);
	}
	pBuff[i] = i2c2_readByte(FALSE);
	
	i2c2_stop();
	return size;
}

/**
 * @brief ���I2C�Ƿ��������
 * @param 
 * @retval ��������TRUE�����򷵻�FALSE
 */
BOOL i2c2_checkLock(void)
{
	return I2C2_SCL_R && !I2C2_SDA_R;
}

/**
 * @brief I2C����. ����9��SCL�������
 * @param 
 * @retval None
 */
void i2c2_unlock(void)
{
	int i=0;
	I2C2_SCL_H;						//����SCL��
	for (i=0; i<9; i++)
	{
		usleep(m_uBaudrate[1]);
		I2C2_SCL_L;						//����SCL��
		usleep(m_uBaudrate[1]);
		I2C2_SCL_H;						//����SCL��
	}
}


/**
 * @brief ����I2C�����ṹ����
 * @param eChannel I2C ͨ�� @ref HALI2CNumer
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
		m_Instance[0].checkLock = i2c1_checkLock;
		m_Instance[0].unlock = i2c1_unlock;
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
		m_Instance[1].checkLock = i2c2_checkLock;
		m_Instance[1].unlock = i2c2_unlock;
		m_pthis[1] = &m_Instance[1];
		m_pthis[1]->init();
		break;
	default:
		break;
	}
}

/**
 * @brief ��ȡI2C�����ṹ���
 * @param eChannel I2C ͨ�� @ref HALI2CNumer
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
