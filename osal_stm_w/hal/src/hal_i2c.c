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


/** I2C GPIO 初始化结构 */
static GPIO_InitTypeDef  GPIO_InitStructure[HALI2CNumer_SIZE];

static UINT32 m_uBaudrate[HALI2CNumer_SIZE] = {8, 8};
static UINT16 m_uDeviceID[HALI2CNumer_SIZE] = {0};
static HALI2CTypeDef m_Instance[HALI2CNumer_SIZE];
static HALI2CTypeDef* m_pthis[HALI2CNumer_SIZE] = {NULL};

/**
 * @brief 初始化IIC1的IO口	
 */
static void i2c1_init(void)
{
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	

	GPIO_InitStructure[0].GPIO_Pin = I2C1_SCL | I2C1_SDA;
	GPIO_InitStructure[0].GPIO_Mode = GPIO_Mode_Out_OD ;   //开漏输出
	GPIO_InitStructure[0].GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C1_GPIO_TYPE, &GPIO_InitStructure[0]);
	
	GPIO_SetBits(I2C1_GPIO_TYPE, I2C1_SCL | I2C1_SDA); 	//I2C1_SCL, I2C1_SDA 输出高
}

static void i2c1_deInit(void)
{
	GPIO_InitStructure[0].GPIO_Pin = I2C1_SCL | I2C1_SDA;
	GPIO_InitStructure[0].GPIO_Mode = GPIO_Mode_IN_FLOATING ;   //浮空输入
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
 * @brief I2C1发送IIC开始信号
 */
static void i2c1_start(void)
{
	I2C1_SDA_H;						//拉高SDA线
	I2C1_SCL_H;						//拉高SCL线
	usleep(m_uBaudrate[0]);		//延时，速度控制
	
	I2C1_SDA_L;						//START when CLK is high, DATA change form high to low 
	usleep(m_uBaudrate[0]);		//延时，速度控制
	I2C1_SCL_L;						//钳住SCL线，以便发送数据
}

/**
 * @brief I2C1发送IIC停止信号
 */
static void i2c1_stop(void)
{
	I2C1_SDA_L;						//拉低SDA线
	I2C1_SCL_L;						//拉低SCL先
	usleep(m_uBaudrate[0]);		//延时，速度控制
	
	I2C1_SCL_H;						//拉高SCL线
	I2C1_SDA_H;						//STOP when CLK is high, DATA change form low to high
	usleep(m_uBaudrate[0]);
}

/**
 * @brief I2C1发送ACK信号
 */
static void i2c1_ack(void)
{
	I2C1_SCL_L;						//拉低SCL线
	I2C1_SDA_L;						//拉低SDA线
	usleep(m_uBaudrate[0]);
	I2C1_SCL_H;						//拉高SCL线
	usleep(m_uBaudrate[0]);
	I2C1_SCL_L;						//拉低SCL线
}

/**
 * @brief I2C1不发送ACK信号
 */
static void i2c1_nack(void)
{
	I2C1_SCL_L;						//拉低SCL线
	I2C1_SDA_H;						//拉高SDA线
	usleep(m_uBaudrate[0]);
	I2C1_SCL_H;						//拉高SCL线
	usleep(m_uBaudrate[0]);
	I2C1_SCL_L;						//拉低SCL线
}

/**
 * @brief I2C1等待ACK信号
 * @param wt 等待ACK时间
 */
static BOOL i2c1_wait_ack(UINT32 wt)
{
	I2C1_SDA_H; usleep(m_uBaudrate[0]);			//拉高SDA线
	I2C1_SCL_H; usleep(m_uBaudrate[0]);			//拉高SCL线
	
	wt = (wt > 0) ? wt : 1;
	while(I2C1_SDA_R)							//如果读到SDA线为1，则等待。应答信号应是0
	{
		if(--wt)
		{
			i2c1_stop();						//超时未收到应答，则停止总线
			return FALSE;					//返回失败
		}
		usleep(m_uBaudrate[0]);
	}
	
	I2C1_SCL_L;									//拉低SCL线，以便继续收发数据
	return TRUE;
}

/**
 * @brief I2C1向端口一个字节
 */
static void i2c1_writeByte(UCHAR data)
{
	UINT8 count = 0;
	
    I2C1_SCL_L;							//拉低时钟开始数据传输
	
    for(; count < 8; count++)		//循环8次，每次发送一个bit
    {
		if(data & 0x80)				//发送最高位
			I2C1_SDA_H;
		else
			I2C1_SDA_L;
		
		data <<= 1;					//byte左移1位
		
		usleep(m_uBaudrate[0]);
		I2C1_SCL_H;
		usleep(m_uBaudrate[0]);
		I2C1_SCL_L;
    }
}

/**
 * @brief I2C1从端口读取一个字节
 */
static UCHAR i2c1_readByte(BOOL bAck)
{
	UINT8 count = 0;
	UCHAR rxData = 0;
	
	I2C1_SDA_H;							//拉高SDA线，开漏状态下，需线拉高以便读取数据
	
    for(; count < 8; count++ )		//循环8次，每次发送一个bit
	{
		I2C1_SCL_L;
		usleep(m_uBaudrate[0]);
		I2C1_SCL_H;
		
        rxData <<= 1;				//左移一位
		
        if(I2C1_SDA_R)					//如果SDA线为1，则对bit0的+1，然后下一次循环会先左移一次
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
 * @brief 8位地址 I2C写数据
 * @param uAddress 写入数据地址
 * @param pBuff 数据指针缓冲区指针
 * @param size 写入数据大小
 * @param wt 等待ACK时间
 * @retval 操作成功返回实际写入的数据大小, 失败返回-1
 */
static int i2c1_write(UINT8 uAddress, UCHAR* pBuff, UINT32 size, UINT32 wt)
{
	UINT32 i;
	if (NULL == pBuff)
		size = 0;
	
	i2c1_start();
	
	i2c1_writeByte(m_uDeviceID[0]);//发送写命令
	if (!i2c1_wait_ack(wt))
		return -1;
	
	i2c1_writeByte((UCHAR)uAddress);//写地址
	if (!i2c1_wait_ack(wt))
		return -1;
	
	for (i = 0; i < size; ++i)
	{
		i2c1_writeByte(pBuff[i]);//写数据
		if (!i2c1_wait_ack(wt))
			return -1;
		usleep(m_uBaudrate[0]);
	}
	
	i2c1_stop();
	return size;
}

/**
 * @brief 10位地址 I2C写数据
 * @param uAddress 写入数据地址
 * @param pBuff 数据指针缓冲区指针
 * @param size 写入数据大小
 * @param wt 等待ACK时间
 * @retval 操作成功返回实际写入的数据大小, 失败返回-1
 */
static int i2c1_writeEx(UINT16 uAddress, UCHAR* pBuff, UINT32 size, UINT32 wt)
{
	UINT32 i;
	if (NULL == pBuff)
		size = 0;
	
	i2c1_start();
	
	i2c1_writeByte(m_uDeviceID[0]);//发送写命令
	if (!i2c1_wait_ack(wt))
		return -1;
	
	i2c1_writeByte((UCHAR)uAddress>>8);//写高位地址
	if (!i2c1_wait_ack(wt))
		return -1;
	i2c1_writeByte((UCHAR)uAddress);//写低位地址
	if (!i2c1_wait_ack(wt))
		return -1;
	
	for (i = 0; i < size; ++i)
	{
		i2c1_writeByte(pBuff[i]);//写数据
		if (!i2c1_wait_ack(wt))
			return -1;
		usleep(m_uBaudrate[0]);
	}
	
	i2c1_stop();
	return size;
}

/**
 * @brief 8位地址 I2C读数据
 * @param uAddress 数据存储地址
 * @param pBuff 数据指针缓冲区指针
 * @param size 读出数据大小
 * @param wt 等待ACK时间
 * @param rdelay 读取数据等待时间, 即发送读命令后等待读取数据的时间
 * @retval 操作成功返回实际读取的数据大小, 失败返回-1
 */
static int i2c1_read(UINT8 uAddress, UCHAR* pBuff, UINT32 size, UINT32 wt, UINT32 rdelay)
{
	UINT32 i;
	INT32 n;
	if (NULL == pBuff)
		return -1;
	
	i2c1_start();
	
	i2c1_writeByte(m_uDeviceID[0]);//发送写命令
	if (!i2c1_wait_ack(wt))
		return -1;
	
	i2c1_writeByte((UCHAR)(uAddress));//写地址
	if (!i2c1_wait_ack(wt))
		return -1;
	
	i = 0;
	i2c1_start();
	i2c1_writeByte(m_uDeviceID[0]+1);//发送读命令
	while (!i2c1_wait_ack(wt))
	{
		if ((i++) > WR_WAIT_TIME)
			return -1;
		usleep(m_uBaudrate[0]);
		i2c1_start();
		i2c1_writeByte(m_uDeviceID[0]+1);//发送读命令
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
 * @brief 10位地址 I2C读数据
 * @param uAddress 数据存储地址
 * @param pBuff 数据指针缓冲区指针
 * @param size 读出数据大小
 * @param wt 等待ACK时间
 * @param rdelay 读取数据等待时间, 即发送读命令后等待读取数据的时间
 * @retval 操作成功返回实际读取的数据大小, 失败返回-1
 */
static int i2c1_readEx(UINT16 uAddress, UCHAR* pBuff, UINT32 size, UINT32 wt, UINT32 rdelay)
{
	UINT32 i;
	INT32 n;
	if (NULL == pBuff)
		return -1;
	
	i2c1_start();
	
	i2c1_writeByte(m_uDeviceID[0]);//发送写命令
	if (!i2c1_wait_ack(wt))
		return -1;
	
	i2c1_writeByte((UCHAR)uAddress>>8);//写高位地址
	if (!i2c1_wait_ack(wt))
		return -1;
	i2c1_writeByte((UCHAR)(uAddress));//写低位地址
	if (!i2c1_wait_ack(wt))
		return -1;
	
	i = 0;
	i2c1_start();
	i2c1_writeByte(m_uDeviceID[0]+1);//发送读命令
	while (!i2c1_wait_ack(wt))
	{
		if ((i++) > WR_WAIT_TIME)
			return -1;
		usleep(m_uBaudrate[0]);
		i2c1_start();
		i2c1_writeByte(m_uDeviceID[0]+1);//发送读命令
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
 * @brief 检查I2C是否产生死锁
 * @param 
 * @retval 死锁返回TRUE，否则返回FALSE
 */
BOOL i2c1_checkLock(void)
{
	return I2C1_SCL_R && !I2C1_SDA_R;
}

/**
 * @brief I2C解锁. 连发9个SCL脉冲解锁
 * @param 
 * @retval None
 */
void i2c1_unlock(void)
{
	int i=0;
	I2C1_SCL_H;						//拉高SCL线
	for (i=0; i<9; i++)
	{
		usleep(m_uBaudrate[0]);
		I2C1_SCL_L;						//拉低SCL线
		usleep(m_uBaudrate[0]);
		I2C1_SCL_H;						//拉高SCL线
	}
}

//>>>>>>>>>>>>>>>>>>>>>>>>> I2C2操作 <<<<<<<<<<<<<<<<<<<<<<<<<<<<

/**
 * @brief 初始化IIC2的IO口	
 */
static void i2c2_init(void)
{
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	

	GPIO_InitStructure[1].GPIO_Pin = I2C2_SCL | I2C2_SDA;
	GPIO_InitStructure[1].GPIO_Mode = GPIO_Mode_Out_OD ;   //开漏输出
	GPIO_InitStructure[1].GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(I2C2_GPIO_TYPE, &GPIO_InitStructure[1]);
	
	GPIO_SetBits(I2C2_GPIO_TYPE, I2C2_SCL | I2C2_SDA); 	//I2C2_SCL, I2C2_SDA 输出高
}

static void i2c2_deInit(void)
{
	GPIO_InitStructure[1].GPIO_Pin = I2C2_SCL | I2C2_SDA;
	GPIO_InitStructure[1].GPIO_Mode = GPIO_Mode_IN_FLOATING ;   //浮空输入
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
 * @brief I2C2发送IIC开始信号
 */
static void i2c2_start(void)
{
	I2C2_SDA_H;						//拉高SDA线
	I2C2_SCL_H;						//拉高SCL线
	usleep(m_uBaudrate[1]);		//延时，速度控制
	
	I2C2_SDA_L;						//START when CLK is high, DATA change form high to low 
	usleep(m_uBaudrate[1]);		//延时，速度控制
	I2C2_SCL_L;						//钳住SCL线，以便发送数据
}

/**
 * @brief I2C2发送IIC停止信号
 */
static void i2c2_stop(void)
{
	I2C2_SDA_L;						//拉低SDA线
	I2C2_SCL_L;						//拉低SCL先
	usleep(m_uBaudrate[1]);		//延时，速度控制
	
	I2C2_SCL_H;						//拉高SCL线
	I2C2_SDA_H;						//STOP when CLK is high, DATA change form low to high
	usleep(m_uBaudrate[1]);
}

/**
 * @brief I2C2发送ACK信号
 */
static void i2c2_ack(void)
{
	I2C2_SCL_L;						//拉低SCL线
	I2C2_SDA_L;						//拉低SDA线
	usleep(m_uBaudrate[1]);
	I2C2_SCL_H;						//拉高SCL线
	usleep(m_uBaudrate[1]);
	I2C2_SCL_L;						//拉低SCL线
}

/**
 * @brief I2C2不发送ACK信号
 */
static void i2c2_nack(void)
{
	I2C2_SCL_L;						//拉低SCL线
	I2C2_SDA_H;						//拉高SDA线
	usleep(m_uBaudrate[1]);
	I2C2_SCL_H;						//拉高SCL线
	usleep(m_uBaudrate[1]);
	I2C2_SCL_L;						//拉低SCL线
}

/**
 * @brief I2C2等待ACK信号
 * @param wt 等待ACK时间
 */
static BOOL i2c2_wait_ack(UINT32 wt)
{
	I2C2_SDA_H;usleep(m_uBaudrate[1]);			//拉高SDA线
	I2C2_SCL_H;usleep(m_uBaudrate[1]);			//拉高SCL线
	
	wt = (wt > 0) ? wt : 1;
	while(I2C2_SDA_R)							//如果读到SDA线为1，则等待。应答信号应是0
	{
		if(--wt)
		{
			i2c2_stop();						//超时未收到应答，则停止总线
			return FALSE;					//返回失败
		}
		usleep(m_uBaudrate[1]);
	}
	
	I2C2_SCL_L;									//拉低SCL线，以便继续收发数据
	return TRUE;
}

/**
 * @brief I2C2向端口一个字节
 */
static void i2c2_writeByte(UCHAR data)
{
	UINT8 count = 0;
	
    I2C2_SCL_L;							//拉低时钟开始数据传输
	
    for(; count < 8; count++)		//循环8次，每次发送一个bit
    {
		if(data & 0x80)				//发送最高位
			I2C2_SDA_H;
		else
			I2C2_SDA_L;
		
		data <<= 1;					//byte左移1位
		
		usleep(m_uBaudrate[1]);
		I2C2_SCL_H;
		usleep(m_uBaudrate[1]);
		I2C2_SCL_L;
    }
}

/**
 * @brief I2C2从端口读取一个字节
 */
static UCHAR i2c2_readByte(BOOL bAck)
{
	UINT8 count = 0;
	UCHAR rxData = 0;
	
	I2C2_SDA_H;							//拉高SDA线，开漏状态下，需线拉高以便读取数据
	
    for(; count < 8; count++ )		//循环8次，每次发送一个bit
	{
		I2C2_SCL_L;
		usleep(m_uBaudrate[1]);
		I2C2_SCL_H;
		
        rxData <<= 1;				//左移一位
		
        if(I2C2_SDA_R)					//如果SDA线为1，则对bit0的+1，然后下一次循环会先左移一次
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
 * @brief 8位地址 I2C写数据
 * @param uAddress 写入数据地址
 * @param pBuff 数据指针缓冲区指针
 * @param size 写入数据大小
 * @param wt 等待ACK时间
 * @retval 操作成功返回实际写入的数据大小, 失败返回-1
 */
static int i2c2_write(UINT8 uAddress, UCHAR* pBuff, UINT32 size, UINT32 wt)
{
	UINT32 i;
	if (NULL == pBuff)
		size = 0;
	
	i2c2_start();
	
	i2c2_writeByte(m_uDeviceID[1]);//发送写命令
	if (!i2c2_wait_ack(wt))
		return -1;
	
	i2c2_writeByte((UCHAR)uAddress);//写地址
	if (!i2c2_wait_ack(wt))
		return -1;
	
	for (i = 0; i < size; ++i)
	{
		i2c2_writeByte(pBuff[i]);//写数据
		if (!i2c2_wait_ack(wt))
			return -1;
		usleep(m_uBaudrate[1]);
	}
	
	i2c2_stop();
	return size;
}

/**
 * @brief 10位地址 I2C写数据
 * @param uAddress 写入数据地址
 * @param pBuff 数据指针缓冲区指针
 * @param size 写入数据大小
 * @param wt 等待ACK时间
 * @retval 操作成功返回实际写入的数据大小, 失败返回-1
 */
static int i2c2_writeEx(UINT16 uAddress, UCHAR* pBuff, UINT32 size, UINT32 wt)
{
	UINT32 i;
	if (NULL == pBuff)
		size = 0;
	
	i2c2_start();
	
	i2c2_writeByte(m_uDeviceID[1]);//发送写命令
	if (!i2c2_wait_ack(wt))
		return -1;
	
	i2c2_writeByte((UCHAR)uAddress>>8);//写高位地址
	if (!i2c2_wait_ack(wt))
		return -1;
	i2c2_writeByte((UCHAR)uAddress);//写低位地址
	if (!i2c2_wait_ack(wt))
		return -1;
	
	for (i = 0; i < size; ++i)
	{
		i2c2_writeByte(pBuff[i]);//写数据
		if (!i2c2_wait_ack(wt))
			return -1;
		usleep(m_uBaudrate[1]);
	}
	
	i2c2_stop();
	return size;
}

/**
 * @brief 8位地址 I2C读数据
 * @param uAddress 数据存储地址
 * @param pBuff 数据指针缓冲区指针
 * @param size 读出数据大小
 * @param wt 等待ACK时间
 * @param rdelay 读取数据等待时间, 即发送读命令后等待读取数据的时间
 * @retval 操作成功返回实际读取的数据大小, 失败返回-1
 */
static int i2c2_read(UINT8 uAddress, UCHAR* pBuff, UINT32 size, UINT32 wt, UINT32 rdelay)
{
	UINT32 i;
	INT32 n;
	if (NULL == pBuff)
		return -1;
	
	i2c2_start();
	
	i2c2_writeByte(m_uDeviceID[1]);//发送写命令
	if (!i2c2_wait_ack(wt))
		return -1;
	
	i2c2_writeByte((UCHAR)(uAddress));//写地址
	if (!i2c2_wait_ack(wt))
		return -1;
	
	i = 0;
	i2c2_start();
	i2c2_writeByte(m_uDeviceID[1]+1);//发送读命令
	while (!i2c2_wait_ack(wt))
	{
		//if ((i++) > WR_WAIT_TIME)
		//	return -1;
		usleep(m_uBaudrate[1]);
		i2c2_start();
		i2c2_writeByte(m_uDeviceID[1]+1);//发送读命令
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
 * @brief 10位地址 I2C读数据
 * @param uAddress 数据存储地址
 * @param pBuff 数据指针缓冲区指针
 * @param size 读出数据大小
 * @param wt 等待ACK时间
 * @param rdelay 读取数据等待时间, 即发送读命令后等待读取数据的时间
 * @retval 操作成功返回实际读取的数据大小, 失败返回-1
 */
static int i2c2_readEx(UINT16 uAddress, UCHAR* pBuff, UINT32 size, UINT32 wt, UINT32 rdelay)
{
	UINT32 i;
	INT32 n;
	if (NULL == pBuff)
		return -1;
	
	i2c2_start();
	
	i2c2_writeByte(m_uDeviceID[1]);//发送写命令
	if (!i2c2_wait_ack(wt))
		return -1;
	
	i2c2_writeByte((UCHAR)uAddress>>8);//写高位地址
	if (!i2c2_wait_ack(wt))
		return -1;
	i2c2_writeByte((UCHAR)(uAddress));//写低位地址
	if (!i2c2_wait_ack(wt))
		return -1;
	
	i = 0;
	i2c2_start();
	i2c2_writeByte(m_uDeviceID[1]+1);//发送读命令
	while (!i2c2_wait_ack(wt))
	{
		if ((i++) > WR_WAIT_TIME)
			return -1;
		usleep(m_uBaudrate[1]);
		i2c2_start();
		i2c2_writeByte(m_uDeviceID[1]+1);//发送读命令
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
 * @brief 检查I2C是否产生死锁
 * @param 
 * @retval 死锁返回TRUE，否则返回FALSE
 */
BOOL i2c2_checkLock(void)
{
	return I2C2_SCL_R && !I2C2_SDA_R;
}

/**
 * @brief I2C解锁. 连发9个SCL脉冲解锁
 * @param 
 * @retval None
 */
void i2c2_unlock(void)
{
	int i=0;
	I2C2_SCL_H;						//拉高SCL线
	for (i=0; i<9; i++)
	{
		usleep(m_uBaudrate[1]);
		I2C2_SCL_L;						//拉低SCL线
		usleep(m_uBaudrate[1]);
		I2C2_SCL_H;						//拉高SCL线
	}
}


/**
 * @brief 申请I2C操作结构对象
 * @param eChannel I2C 通道 @ref HALI2CNumer
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
 * @brief 获取I2C操作结构句柄
 * @param eChannel I2C 通道 @ref HALI2CNumer
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
