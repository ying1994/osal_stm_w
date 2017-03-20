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

/** I2C GPIO 初始化结构 */
static GPIO_InitTypeDef  GPIO_InitStructure;

static UINT32 m_uBaudrate[HALI2CNumer_SIZE] = {0};
static UINT16 m_uDeviceID[HALI2CNumer_SIZE] = {0};
static HALI2CTypeDef m_Instance[HALI2CNumer_SIZE];
static HALI2CTypeDef* m_pthis[HALI2CNumer_SIZE] = {NULL};

/**
 * @brief I2C1_SDA方向设置为输出
 */
static void i2c1_sda_out(void)
{
	GPIO_InitStructure.GPIO_Pin = I2C1_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_Init(I2C1_GPIO_TYPE, &GPIO_InitStructure);
}

/**
 * @brief I2C1_SDA方向设置 设置为输入
 */
static void i2c1_sda_in(void)
{
	GPIO_InitStructure.GPIO_Pin = I2C1_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;   //上拉输入
	GPIO_Init(I2C1_GPIO_TYPE, &GPIO_InitStructure);
}

/**
 * @brief I2C1写I2C_SCL端口
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
 * @brief I2C1写I2C_SDA端口
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
 * @brief I2C1读I2C_SDA端口
 */
static BOOL i2c1_sda_read(void)
{
	if ((I2C1_GPIO_TYPE->IDR & I2C1_SDA) != (uint32_t)Bit_RESET)
		 return TRUE;
	 else
		return FALSE;
}

/**
 * @brief I2C1初始化的IO口	
 */
static void i2c1_init(void)
{
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = I2C1_SCL | I2C1_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C1_GPIO_TYPE, &GPIO_InitStructure);
	GPIO_SetBits(I2C1_GPIO_TYPE, I2C1_SCL | I2C1_SDA); 	//PB10,PB11 输出高
}

static void i2c1_deInit(void)
{
	GPIO_InitStructure.GPIO_Pin = I2C1_SCL | I2C1_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ;   //浮空输入
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
 * @brief I2C1发送开始信号
 */
static void i2c1_start(void)
{
	i2c1_sda_out();     //sda线输出
	i2c1_sda_write(TRUE);	  	  
	i2c1_scl_write(TRUE);
	usleep(4);
 	i2c1_sda_write(FALSE);//START:when CLK is high,DATA change form high to low 
	usleep(4);
	i2c1_scl_write(FALSE);//钳住I2C总线，准备发送或接收数据 
}

/**
 * @brief I2C1发送停止信号
 */
static void i2c1_stop(void)
{
	i2c1_sda_out();//sda线输出
	i2c1_scl_write(FALSE);
	i2c1_sda_write(FALSE);//STOP:when CLK is high DATA change form low to high
 	usleep(4);
	i2c1_scl_write(TRUE); 
	i2c1_sda_write(TRUE);//发送I2C总线结束信号
	usleep(4);	
}

/**
 * @brief I2C1发送ACK信号
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
 * @brief I2C1不发送ACK信号
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
 * @brief I2C1等待ACK信号
 */
static BOOL i2c1_wait_ack(void)
{
	u8 ucErrTime=0;
	i2c1_sda_in();      //SDA设置为输入  
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
	i2c1_scl_write(FALSE);//时钟输出0 
	return FALSE;
}

/**
 * @brief I2C1向端口写一个字节
 */
static void i2c1_writeByte(UCHAR data)
{
    UCHAR i;   
	i2c1_sda_out(); 	    
    i2c1_scl_write(FALSE);//拉低时钟开始数据传输
    for(i=0; i<8; i++)
    {
		if(data & 0x80)
			i2c1_sda_write(TRUE);
		else
			i2c1_sda_write(FALSE);
		data <<= 1; 	  
		usleep(m_uBaudrate[0]);   //对TEA5767这三个延时都是必须的
		i2c1_scl_write(TRUE);
		usleep(m_uBaudrate[0]); 
		i2c1_scl_write(FALSE);	
		usleep(m_uBaudrate[0]);
    }	 
}

/**
 * @brief I2C1从端口读取一个字节
 */
static UCHAR i2c1_readByte(BOOL bAck)
{
	UCHAR i = 0;
	UCHAR rxData = 0;
	
	i2c1_sda_in();//SDA设置为输入
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
        i2c1_nack();//发送nACK
    else
        i2c1_ack(); //发送ACK  
	
	return rxData;
}

/**
 * @brief 8位地址 I2C写数据
 * @param uAddress: 写入数据地址
 * @param pBuff: 数据指针缓冲区指针
 * @param size: 写入数据大小
 * @retval None
 */
static void i2c1_write(UINT8 uAddress, UCHAR* pBuff, UINT32 size)
{
	UINT32 i;
	
	i2c1_start();
	
	i2c1_writeByte(m_uDeviceID[0]);//发送写命令
	i2c1_wait_ack();
	
	i2c1_writeByte((UCHAR)uAddress);//写低位地址
	i2c1_wait_ack();
	
	for (i = 0; i < size; ++i)
	{
		i2c1_writeByte(pBuff[i]);//写数据
		i2c1_wait_ack();
	}
	
	i2c1_stop();
}

/**
 * @brief 10位地址 I2C写数据
 * @param uAddress: 写入数据地址
 * @param pBuff: 数据指针缓冲区指针
 * @param size: 写入数据大小
 * @retval None
 */
static void i2c1_writeEx(UINT16 uAddress, UCHAR* pBuff, UINT32 size)
{
	UINT32 i;
	
	i2c1_start();
	
	i2c1_writeByte(m_uDeviceID[0]);//发送写命令
	i2c1_wait_ack();
	
	i2c1_writeByte((UCHAR)uAddress>>8);//写高位地址
	i2c1_wait_ack();
	i2c1_writeByte((UCHAR)uAddress);//写低位地址
	i2c1_wait_ack();
	
	for (i = 0; i < size; ++i)
	{
		i2c1_writeByte(pBuff[i]);//写数据
		i2c1_wait_ack();
	}
	
	i2c1_stop();
}

/**
 * @brief 8位地址 I2C读数据
 * @param uAddress: 数据存储地址
 * @param pBuff: 数据指针缓冲区指针
 * @param size: 读出数据大小
 * @retval None
 */
static UCHAR i2c1_read(UINT8 uAddress, UCHAR* pBuff, UINT32 size)
{
	UINT32 i, n;
	
	i2c1_start();
	
	i2c1_writeByte(m_uDeviceID[0]);//发送写命令
	i2c1_wait_ack();
	
	i2c1_writeByte((UCHAR)(uAddress));//写地址
	i2c1_wait_ack();
	
	i2c1_start();
	i2c1_writeByte(m_uDeviceID[0]+1);//发送读命令
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
 * @brief 10位地址 I2C读数据
 * @param uAddress: 数据存储地址
 * @param pBuff: 数据指针缓冲区指针
 * @param size: 读出数据大小
 * @retval None
 */
static UCHAR i2c1_readEx(UINT16 uAddress, UCHAR* pBuff, UINT32 size)
{
	UINT32 i, n;
	
	i2c1_start();
	
	i2c1_writeByte(m_uDeviceID[0]);//发送写命令
	i2c1_wait_ack();
	
	i2c1_writeByte((UCHAR)uAddress>>8);//写高位地址
	i2c1_wait_ack();
	i2c1_writeByte((UCHAR)(uAddress));//写低位地址
	i2c1_wait_ack();
	
	i2c1_start();
	i2c1_writeByte(m_uDeviceID[0]+1);//发送读命令
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


//>>>>>>>>>>>>>>>>>>>>>>>>> I2C2操作 <<<<<<<<<<<<<<<<<<<<<<<<<<<<

/**
 * @brief I2C2_SDA方向设置为输出
 */
static void i2c2_sda_out(void)
{
	GPIO_InitStructure.GPIO_Pin = I2C2_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_Init(I2C2_GPIO_TYPE, &GPIO_InitStructure);
}

/**
 * @brief I2C2_SDA方向设置 设置为输入
 */
static void i2c2_sda_in(void)
{
	GPIO_InitStructure.GPIO_Pin = I2C2_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   //上拉输入
	GPIO_Init(I2C2_GPIO_TYPE, &GPIO_InitStructure);
}

/**
 * @brief 写I2C2_SCL端口
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
 * @brief 写I2C2_SDA端口
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
 * @brief 读I2C2_SDA端口
 */
static BOOL i2c2_sda_read(void)
{
	if ((I2C2_GPIO_TYPE->IDR & I2C2_SDA) != (uint32_t)Bit_RESET)
		 return TRUE;
	 else
		return FALSE;
}

/**
 * @brief 初始化IIC2的IO口	
 */
static void i2c2_init(void)
{
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	
	   
	GPIO_InitStructure.GPIO_Pin = I2C2_SCL | I2C2_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C2_GPIO_TYPE, &GPIO_InitStructure);
	GPIO_SetBits(I2C2_GPIO_TYPE, I2C2_SCL | I2C2_SDA); 	//PB10,PB11 输出高
}

static void i2c2_deInit(void)
{
	GPIO_InitStructure.GPIO_Pin = I2C2_SCL | I2C2_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //浮空输入
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
 * @brief I2C2发送IIC开始信号
 */
static void i2c2_start(void)
{
	i2c2_sda_out();     //sda线输出
	i2c2_sda_write(TRUE);	  	  
	i2c2_scl_write(TRUE);
	usleep(4);
 	i2c2_sda_write(FALSE);//START:when CLK is high,DATA change form high to low 
	usleep(4);
	i2c2_scl_write(FALSE);//钳住I2C总线，准备发送或接收数据 
}

/**
 * @brief I2C2发送IIC停止信号
 */
static void i2c2_stop(void)
{
	i2c2_sda_out();//sda线输出
	i2c2_scl_write(FALSE);
	i2c2_sda_write(FALSE);//STOP:when CLK is high DATA change form low to high
 	usleep(4);
	i2c2_scl_write(TRUE); 
	i2c2_sda_write(TRUE);//发送I2C总线结束信号
	usleep(4);	
}

/**
 * @brief I2C2发送ACK信号
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
 * @brief I2C2不发送ACK信号
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
 * @brief I2C2等待ACK信号
 */
static BOOL i2c2_wait_ack(void)
{
	u8 ucErrTime=0;
	i2c2_sda_in();      //SDA设置为输入  
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
	i2c2_scl_write(FALSE);//时钟输出0 
	return FALSE;
}

/**
 * @brief I2C2向端口一个字节
 */
static void i2c2_writeByte(UCHAR data)
{
    UCHAR i;   
	i2c2_sda_out(); 	    
    i2c2_scl_write(FALSE);//拉低时钟开始数据传输
    for(i=0; i<8; i++)
    {
		if(data & 0x80)
			i2c2_sda_write(TRUE);
		else
			i2c2_sda_write(FALSE);
		data <<= 1; 	  
		usleep(m_uBaudrate[0]);   //对TEA5767这三个延时都是必须的
		i2c2_scl_write(TRUE);
		usleep(m_uBaudrate[0]); 
		i2c2_scl_write(FALSE);	
		usleep(m_uBaudrate[0]);
    }	 
}

/**
 * @brief I2C2从端口读取一个字节
 */
static UCHAR i2c2_readByte(BOOL bAck)
{
	UCHAR i = 0;
	UCHAR rxData = 0;
	
	i2c2_sda_in();//SDA设置为输入
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
        i2c2_nack();//发送nACK
    else
        i2c2_ack(); //发送ACK  
	
	return rxData;
}

/**
 * @brief 8位地址 I2C写数据
 * @param uAddress: 写入数据地址
 * @param pBuff: 数据指针缓冲区指针
 * @param size: 写入数据大小
 * @retval None
 */
static void i2c2_write(UINT8 uAddress, UCHAR* pBuff, UINT32 size)
{
	UINT32 i;
	i2c2_start();
	
	i2c2_writeByte(m_uDeviceID[0]);//发送写命令
	i2c2_wait_ack();
	
	i2c2_writeByte((UCHAR)uAddress);//写地址
	i2c2_wait_ack();
	
	for (i = 0; i < size; ++i)
	{
		i2c2_writeByte(pBuff[i]);//写数据
		i2c2_wait_ack();
	}
	
	i2c2_stop();
}

/**
 * @brief 10位地址 I2C写数据
 * @param uAddress: 写入数据地址
 * @param pBuff: 数据指针缓冲区指针
 * @param size: 写入数据大小
 * @retval None
 */
static void i2c2_writeEx(UINT16 uAddress, UCHAR* pBuff, UINT32 size)
{
	UINT32 i;
	i2c2_start();
	
	i2c2_writeByte(m_uDeviceID[0]);//发送写命令
	i2c2_wait_ack();
	
	i2c2_writeByte((UCHAR)uAddress>>8);//写高位地址
	i2c2_wait_ack();
	i2c2_writeByte((UCHAR)uAddress);//写低位地址
	i2c2_wait_ack();
	
	for (i = 0; i < size; ++i)
	{
		i2c2_writeByte(pBuff[i]);//写数据
		i2c2_wait_ack();
	}
	
	i2c2_stop();
}

/**
 * @brief 8位地址 I2C读数据
 * @param uAddress: 数据存储地址
 * @param pBuff: 数据指针缓冲区指针
 * @param size: 读出数据大小
 * @retval None
 */
static UCHAR i2c2_read(UINT8 uAddress, UCHAR* pBuff, UINT32 size)
{
	UINT32 i, n;
	
	i2c2_start();
	
	i2c2_writeByte(m_uDeviceID[0]);//发送写命令
	i2c2_wait_ack();
	
	i2c2_writeByte((UCHAR)(uAddress));//写地址
	i2c2_wait_ack();
	
	i2c2_start();
	i2c2_writeByte(m_uDeviceID[0]+1);//发送读命令
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
 * @brief 10位地址 I2C读数据
 * @param uAddress: 数据存储地址
 * @param pBuff: 数据指针缓冲区指针
 * @param size: 读出数据大小
 * @retval None
 */
static UCHAR i2c2_readEx(UINT16 uAddress, UCHAR* pBuff, UINT32 size)
{
	UINT32 i, n;
	
	i2c2_start();
	
	i2c2_writeByte(m_uDeviceID[0]);//发送写命令
	i2c2_wait_ack();
	
	i2c2_writeByte((UCHAR)uAddress>>8);//写高位地址
	i2c2_wait_ack();
	i2c2_writeByte((UCHAR)(uAddress));//写低位地址
	i2c2_wait_ack();
	
	i2c2_start();
	i2c2_writeByte(m_uDeviceID[0]+1);//发送读命令
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
 * @brief 申请I2C操作结构对象
 * @param eChannel: I2C 通道 @ref HALI2CNumer
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
 * @brief 获取I2C操作结构句柄
 * @param eChannel: I2C 通道 @ref HALI2CNumer
 * @retval I2C操作结构句柄
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
