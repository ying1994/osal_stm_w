/**
 * @file    hal_uart.c
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

#include "hal_uart.h"

#ifdef CFG_HAL_UART

static USART_InitTypeDef USART_InitStructure[5];

static HalUartCBack_t m_hUartObser[5] = {NULL};

static HALUartTypeDef m_hUartInstance[5];
static HALUartTypeDef* pthis[5] = {NULL};

static void uart1_set_baudrate(UINT32 baudrate)
{
	USART_InitStructure[0].USART_BaudRate = baudrate;//一般设置为115200;
    USART_Init(USART1, &USART_InitStructure[0]); //初始化串口
}

static void uart1_set_databits(UINT16 databits)
{
	USART_InitStructure[0].USART_WordLength = databits;//字长为8位数据格式
    USART_Init(USART1, &USART_InitStructure[0]); //初始化串口
}

static void uart1_set_stopbit(UINT16 stopbit)
{
	USART_InitStructure[0].USART_StopBits = stopbit;//一个停止位
    USART_Init(USART1, &USART_InitStructure[0]); //初始化串口
}

static void uart1_set_parity(UINT16 parity)
{
	USART_InitStructure[0].USART_Parity = parity;//无奇偶校验位
    USART_Init(USART1, &USART_InitStructure[0]); //初始化串口
}

static void uart1_writeByte(UCHAR data)
{ 
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC) == RESET); 
	USART_SendData(USART1, data); 
}

static UINT16 uart1_read(UCHAR *pdata, UINT16 len)
{
	return 0;
}

static void uart1_write(UCHAR *pdata, UINT16 len)
{
	UINT16 i = 0;
	if (pdata != NULL)
	{
		for (i = 0; i < len; ++i)
		{
			/* Transmit Data */
			uart1_writeByte(*pdata);
			++pdata;
		}
	}
}

static void uart1_add_rx_obser(HalUartCBack_t hrxobser)
{
	m_hUartObser[0] = hrxobser;
}

static void uart1_init(void)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|
						   RCC_APB2Periph_GPIOA|
						   RCC_APB2Periph_AFIO, 
						   ENABLE);	//使能USART1，GPIOA时钟以及复用功能时钟
	USART_DeInit(USART1);
    //USART1_TX   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //USART1_RX	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

  
   //USART 初始化设置
	USART_InitStructure[0].USART_BaudRate = 115200;//默认为115200;
	USART_InitStructure[0].USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure[0].USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure[0].USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure[0].USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure[0].USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART1, &USART_InitStructure[0]); //初始化串口

   //USART NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART1, ENABLE);                    //使能串口 
}

/**
 * @brief 资源注销
 * @param None
 * @retval None
 */
static void uart1_deInit(void)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	//USART1_TX   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //USART1_RX	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  
	
    USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);//开启中断
    USART_Cmd(USART1, DISABLE);                    //使能串口 
	
	m_hUartObser[0] = NULL;
}

static void uart2_set_baudrate(UINT32 baudrate)
{
	USART_InitStructure[1].USART_BaudRate = baudrate;//一般设置为115200;
    USART_Init(USART2, &USART_InitStructure[1]); //初始化串口
}

static void uart2_set_databits(UINT16 databits)
{
	USART_InitStructure[1].USART_WordLength = databits;//字长为8位数据格式
    USART_Init(USART2, &USART_InitStructure[1]); //初始化串口
}

static void uart2_set_stopbit(UINT16 stopbit)
{
	USART_InitStructure[1].USART_StopBits = stopbit;//一个停止位
    USART_Init(USART2, &USART_InitStructure[1]); //初始化串口
}

static void uart2_set_parity(UINT16 parity)
{
	USART_InitStructure[1].USART_Parity = parity;//无奇偶校验位
    USART_Init(USART2, &USART_InitStructure[1]); //初始化串口
}

static void uart2_writeByte(UCHAR data)
{
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);
	USART_SendData(USART2, data); 
}

static UINT16 uart2_read(UCHAR *pdata, UINT16 len)
{
	return 0;
}

static void uart2_write(UCHAR *pdata, UINT16 len)
{
	UINT16 i = 0;
	if (pdata != NULL)
	{
		for (i = 0; i < len; ++i)
		{
			/* Transmit Data */
			uart2_writeByte(*pdata);
			++pdata;
		}
	}
}

static void uart2_add_rx_obser(HalUartCBack_t hrxobser)
{
	m_hUartObser[1] = hrxobser;
}

static void uart2_init(void)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
						   RCC_APB2Periph_AFIO, 
						   ENABLE);	//使能USART2，GPIOA时钟以及复用功能时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);
	
	USART_DeInit(USART2);
     //USART2_TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //USART2_RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  

  
   //USART 初始化设置
	USART_InitStructure[1].USART_BaudRate = 115200;//默认为115200;
	USART_InitStructure[1].USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure[1].USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure[1].USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure[1].USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure[1].USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART2, &USART_InitStructure[1]); //初始化串口

   //USART NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART2, ENABLE);                    //使能串口 
}

/**
 * @brief 资源注销
 * @param None
 * @retval None
 */
static void uart2_deInit(void)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	//USART2_TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //USART2_RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  
	
    USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);//开启中断
    USART_Cmd(USART2, DISABLE);                    //使能串口 
	
	m_hUartObser[1] = NULL;
}

#if defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)||  defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_XL) || defined (STM32F10X_CL)
static void uart3_set_baudrate(UINT32 baudrate)
{
	USART_InitStructure[2].USART_BaudRate = baudrate;//一般设置为115200;
    USART_Init(USART3, &USART_InitStructure[2]); //初始化串口
}

static void uart3_set_databits(UINT16 databits)
{
	USART_InitStructure[2].USART_WordLength = databits;//字长为8位数据格式
    USART_Init(USART3, &USART_InitStructure[2]); //初始化串口
}

static void uart3_set_stopbit(UINT16 stopbit)
{
	USART_InitStructure[2].USART_StopBits = stopbit;//一个停止位
    USART_Init(USART3, &USART_InitStructure[2]); //初始化串口
}

static void uart3_set_parity(UINT16 parity)
{
	USART_InitStructure[2].USART_Parity = parity;//无奇偶校验位
    USART_Init(USART3, &USART_InitStructure[2]); //初始化串口
}

static void uart3_writeByte(UCHAR data)
{
	while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET);
	USART_SendData(USART3, data); 
}

static UINT16 uart3_read(UCHAR *pdata, UINT16 len)
{
	return 0;
}

static void uart3_write(UCHAR *pdata, UINT16 len)
{
	UINT16 i = 0;
	if (pdata != NULL)
	{
		for (i = 0; i < len; ++i)
		{
			/* Transmit Data */
			uart3_writeByte(*pdata);
			++pdata;
		}
	}
}

static void uart3_add_rx_obser(HalUartCBack_t hrxobser)
{
	m_hUartObser[2] = hrxobser;
}

static void uart3_init(void)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |
						   RCC_APB2Periph_AFIO, 
						   ENABLE);	//使能USART3，GPIOB时钟以及复用功能时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 , ENABLE);
	
	USART_DeInit(USART3);
    //USART3_TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //USART3_RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);  

  
   //USART 初始化设置
	USART_InitStructure[2].USART_BaudRate = 115200;//默认为115200;
	USART_InitStructure[2].USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure[2].USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure[2].USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure[2].USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure[2].USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART3, &USART_InitStructure[2]); //初始化串口

   //USART NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(USART3, ENABLE);                    //使能串口 
}

/**
 * @brief 资源注销
 * @param None
 * @retval None
 */
static void uart3_deInit(void)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	//USART3_TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //USART3_RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);  
	
    USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);//开启中断
    USART_Cmd(USART3, DISABLE);                    //使能串口 
	
	m_hUartObser[2] = NULL;
}
#endif /* defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)||  defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_XL) || defined (STM32F10X_CL) */ 

#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_XL) || defined (STM32F10X_CL)
static void uart4_set_baudrate(UINT32 baudrate)
{
	USART_InitStructure[3].USART_BaudRate = baudrate;//一般设置为115200;
    USART_Init(UART4, &USART_InitStructure[3]); //初始化串口
}

static void uart4_set_databits(UINT16 databits)
{
	USART_InitStructure[3].USART_WordLength = databits;//字长为8位数据格式
    USART_Init(UART4, &USART_InitStructure[3]); //初始化串口
}

static void uart4_set_stopbit(UINT16 stopbit)
{
	USART_InitStructure[3].USART_StopBits = stopbit;//一个停止位
    USART_Init(UART4, &USART_InitStructure[3]); //初始化串口
}

static void uart4_set_parity(UINT16 parity)
{
	USART_InitStructure[3].USART_Parity = parity;//无奇偶校验位
    USART_Init(UART4, &USART_InitStructure[3]); //初始化串口
}

static void uart4_writeByte(UCHAR data)
{
	while(USART_GetFlagStatus(UART4,USART_FLAG_TC)==RESET);
	USART_SendData(UART4, data); 
}

static UINT16 uart4_read(UCHAR *pdata, UINT16 len)
{
	return 0;
}

static void uart4_write(UCHAR *pdata, UINT16 len)
{
	UINT16 i = 0;
	if (pdata != NULL)
	{
		for (i = 0; i < len; ++i)
		{
			/* Transmit Data */
			uart4_writeByte(*pdata);
			++pdata;
		}
	}
}

static void uart4_add_rx_obser(HalUartCBack_t hrxobser)
{
	m_hUartObser[3] = hrxobser;
}

static void uart4_init(void)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |
						   RCC_APB2Periph_AFIO, 
						   ENABLE);	//使能USART4，GPIOC时钟以及复用功能时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4 , ENABLE);
	
	USART_DeInit(UART4);
    //USART4_TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    //USART4_RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);  

  
   //USART 初始化设置
	USART_InitStructure[3].USART_BaudRate = 115200;//默认为115200;
	USART_InitStructure[3].USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure[3].USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure[3].USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure[3].USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure[3].USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(UART4, &USART_InitStructure[3]); //初始化串口

   //USART NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(UART4, ENABLE);                    //使能串口 
}

/**
 * @brief 资源注销
 * @param None
 * @retval None
 */
static void uart4_deInit(void)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	//USART4_TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    //USART4_RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);  
	
    USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);//开启中断
    USART_Cmd(UART4, DISABLE);                    //使能串口 
	
	m_hUartObser[3] = NULL;
}

static void uart5_set_baudrate(UINT32 baudrate)
{
	USART_InitStructure[4].USART_BaudRate = baudrate;//一般设置为115200;
    USART_Init(UART5, &USART_InitStructure[4]); //初始化串口
}

static void uart5_set_databits(UINT16 databits)
{
	USART_InitStructure[4].USART_WordLength = databits;//字长为8位数据格式
    USART_Init(UART5, &USART_InitStructure[4]); //初始化串口
}

static void uart5_set_stopbit(UINT16 stopbit)
{
	USART_InitStructure[4].USART_StopBits = stopbit;//一个停止位
    USART_Init(UART5, &USART_InitStructure[4]); //初始化串口
}

static void uart5_set_parity(UINT16 parity)
{
	USART_InitStructure[4].USART_Parity = parity;//无奇偶校验位
    USART_Init(UART5, &USART_InitStructure[4]); //初始化串口
}

static void uart5_writeByte(UCHAR data)
{
	while(USART_GetFlagStatus(UART5,USART_FLAG_TC)==RESET);
	USART_SendData(UART5, data); 
}

static UINT16 uart5_read(UCHAR *pdata, UINT16 len)
{
	return 0;
}

static void uart5_write(UCHAR *pdata, UINT16 len)
{
	UINT16 i = 0;
	if (pdata != NULL)
	{
		for (i = 0; i < len; ++i)
		{
			/* Transmit Data */
			uart5_writeByte(*pdata);
			++pdata;
		}
	}
}

static void uart5_add_rx_obser(HalUartCBack_t hrxobser)
{
	m_hUartObser[4] = hrxobser;
}

static void uart5_init(void)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC |
						   RCC_APB2Periph_GPIOD |
						   RCC_APB2Periph_AFIO, 
						   ENABLE);	//使能USART5，GPIOC, GPIOD时钟以及复用功能时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5 , ENABLE);
	
	USART_DeInit(UART5);
    //USART5_TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    //USART5_RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOD, &GPIO_InitStructure);  

  
   //USART 初始化设置
	USART_InitStructure[4].USART_BaudRate = 115200;//默认为115200;
	USART_InitStructure[4].USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure[4].USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure[4].USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure[4].USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure[4].USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(UART5, &USART_InitStructure[4]); //初始化串口

   //USART NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//开启中断
    USART_Cmd(UART5, ENABLE);                    //使能串口 
}

/**
 * @brief 资源注销
 * @param None
 * @retval None
 */
static void uart5_deInit(void)
{
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	//USART5_TX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    //USART5_RX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOD, &GPIO_InitStructure);  
	
    USART_ITConfig(UART5, USART_IT_RXNE, DISABLE);//开启中断
    USART_Cmd(UART5, DISABLE);                    //使能串口 
	
	m_hUartObser[4] = NULL;
}
#endif /* defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_XL) || defined (STM32F10X_CL) */ 

//串口1中断服务程序
void USART1_IRQHandler(void)
{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断
	{
		Res = USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据
		if (m_hUartObser[0] != NULL)
			m_hUartObser[0](Res);
	} 
}

//串口1中断服务程序
void USART2_IRQHandler(void)
{
	u8 Res;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断
	{
		Res = USART_ReceiveData(USART2);//(USART2->DR);	//读取接收到的数据
		if (m_hUartObser[1] != NULL)
			m_hUartObser[1](Res);
	} 
}

#if defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)||  defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_XL) || defined (STM32F10X_CL)
//串口1中断服务程序
void USART3_IRQHandler(void)
{
	u8 Res;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断
	{
		Res = USART_ReceiveData(USART3);//(USART3->DR);	//读取接收到的数据
		if (m_hUartObser[2] != NULL)
			m_hUartObser[2](Res);
	} 
}
#endif /* defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)||  defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_XL) || defined (STM32F10X_CL) */ 

#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_XL) || defined (STM32F10X_CL)
//串口1中断服务程序
void UART4_IRQHandler(void)
{
	u8 Res;
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //接收中断
	{
		Res = USART_ReceiveData(UART4);//(UART4->DR);	//读取接收到的数据
		if (m_hUartObser[3] != NULL)
			m_hUartObser[3](Res);
	} 
}

//串口1中断服务程序
void UART5_IRQHandler(void)
{
	u8 Res;
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //接收中断
	{
		Res = USART_ReceiveData(UART5);//(UART5->DR);	//读取接收到的数据
		if (m_hUartObser[4] != NULL)
			m_hUartObser[4](Res);
	} 
}
#endif /* defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_XL) || defined (STM32F10X_CL) */ 


/**
 * @brief: 创建端口串口通讯对象
 * @param numer: 串口端口号
 * @retval None
 */
static void New(HALUartNumer numer)
{
	switch (numer)
	{
	case HAL_UART1:
		m_hUartInstance[HAL_UART1].add_rx_obser = uart1_add_rx_obser;
		m_hUartInstance[HAL_UART1].init = uart1_init;
		m_hUartInstance[HAL_UART1].deInit = uart1_deInit;
		m_hUartInstance[HAL_UART1].set_baudrate = uart1_set_baudrate;
		m_hUartInstance[HAL_UART1].set_databits = uart1_set_databits;
		m_hUartInstance[HAL_UART1].set_stopbit = uart1_set_stopbit;
		m_hUartInstance[HAL_UART1].set_parity = uart1_set_parity;
		m_hUartInstance[HAL_UART1].read = uart1_read;
		m_hUartInstance[HAL_UART1].write = uart1_write;
		pthis[HAL_UART1] = &m_hUartInstance[HAL_UART1];
		pthis[HAL_UART1]->init();
		break;
	case HAL_UART2:
		m_hUartInstance[HAL_UART2].add_rx_obser = uart2_add_rx_obser;
		m_hUartInstance[HAL_UART2].init = uart2_init;
		m_hUartInstance[HAL_UART2].deInit = uart2_deInit;
		m_hUartInstance[HAL_UART2].set_baudrate = uart2_set_baudrate;
		m_hUartInstance[HAL_UART2].set_databits = uart2_set_databits;
		m_hUartInstance[HAL_UART2].set_stopbit = uart2_set_stopbit;
		m_hUartInstance[HAL_UART2].set_parity = uart2_set_parity;
		m_hUartInstance[HAL_UART2].read = uart2_read;
		m_hUartInstance[HAL_UART2].write = uart2_write;
		pthis[HAL_UART2] = &m_hUartInstance[HAL_UART2];
		pthis[HAL_UART2]->init();
		break;
#if defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)||  defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_XL) || defined (STM32F10X_CL)
	case HAL_UART3:
		m_hUartInstance[HAL_UART3].add_rx_obser = uart3_add_rx_obser;
		m_hUartInstance[HAL_UART3].init = uart3_init;
		m_hUartInstance[HAL_UART3].deInit = uart3_deInit;
		m_hUartInstance[HAL_UART3].set_baudrate = uart3_set_baudrate;
		m_hUartInstance[HAL_UART3].set_databits = uart3_set_databits;
		m_hUartInstance[HAL_UART3].set_stopbit = uart3_set_stopbit;
		m_hUartInstance[HAL_UART3].set_parity = uart3_set_parity;
		m_hUartInstance[HAL_UART3].read = uart3_read;
		m_hUartInstance[HAL_UART3].write = uart3_write;
		pthis[HAL_UART3] = &m_hUartInstance[HAL_UART3];
		pthis[HAL_UART3]->init();
		break;
#endif /* defined (STM32F10X_MD) || defined (STM32F10X_MD_VL)||  defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_XL) || defined (STM32F10X_CL) */ 

#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_XL) || defined (STM32F10X_CL)
	case HAL_UART4:
		m_hUartInstance[HAL_UART4].add_rx_obser = uart4_add_rx_obser;
		m_hUartInstance[HAL_UART4].init = uart4_init;
		m_hUartInstance[HAL_UART4].deInit = uart4_deInit;
		m_hUartInstance[HAL_UART4].set_baudrate = uart4_set_baudrate;
		m_hUartInstance[HAL_UART4].set_databits = uart4_set_databits;
		m_hUartInstance[HAL_UART4].set_stopbit = uart4_set_stopbit;
		m_hUartInstance[HAL_UART4].set_parity = uart4_set_parity;
		m_hUartInstance[HAL_UART4].read = uart4_read;
		m_hUartInstance[HAL_UART4].write = uart4_write;
		pthis[HAL_UART4] = &m_hUartInstance[HAL_UART4];
		pthis[HAL_UART4]->init();
		break;
	case HAL_UART5:
		m_hUartInstance[HAL_UART5].add_rx_obser = uart5_add_rx_obser;
		m_hUartInstance[HAL_UART5].init = uart5_init;
		m_hUartInstance[HAL_UART5].deInit = uart5_deInit;
		m_hUartInstance[HAL_UART5].set_baudrate = uart5_set_baudrate;
		m_hUartInstance[HAL_UART5].set_databits = uart5_set_databits;
		m_hUartInstance[HAL_UART5].set_stopbit = uart5_set_stopbit;
		m_hUartInstance[HAL_UART5].set_parity = uart5_set_parity;
		m_hUartInstance[HAL_UART5].read = uart5_read;
		m_hUartInstance[HAL_UART5].write = uart5_write;
		pthis[HAL_UART5] = &m_hUartInstance[HAL_UART5];
		pthis[HAL_UART5]->init();
		break;
#endif /* defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_XL) || defined (STM32F10X_CL) */ 
	default:
		break;
	}
}

/**
 * @brief: 获取指定端口串口通讯句柄
 * @param numer: 串口端口号
 * @retval: 指定端口串口通讯句柄
 */
HALUartTypeDef* hal_uart_getinstance(HALUartNumer numer)
{
	if (NULL == pthis[numer])
	{
		New(numer);
	}
	
	return pthis[numer];
}

#include "stdio.h"
/**
 * @function: 重定义fputc函数 
 */
int fputc(int ch, FILE *f)
{      
#ifdef DEBUG_UART1
	uart1_writeByte(ch); 
#endif    
      
#ifdef DEBUG_UART2
	uart2_writeByte(ch);  
#endif 
      
#ifdef DEBUG_UART3
	uart3_writeByte(ch);  
#endif 
      
#ifdef DEBUG_UART4
	uart4_writeByte(ch);  
#endif 
      
#ifdef DEBUG_UART5
	uart5_writeByte(ch);  
#endif 
	return ch;
}

#endif //CFG_HAL_UART
