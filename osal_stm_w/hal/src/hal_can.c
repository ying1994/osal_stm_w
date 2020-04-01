/**
 * @file    hal_can.c
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
#include "hal_can.h"

#ifdef CFG_HAL_CAN

#define CAN_PCLK 36000	// 72000/2 KHz

/* CAN 类对象*/
static HALCanTypeDef m_Instance[CAN_CHANNEL_SIZE];
static HALCanTypeDef* m_pthis[CAN_CHANNEL_SIZE] = {NULL};

/* CAN 事件回调函数*/
static CAN_RX_BASE_FUNC m_hCanRxCallBlack[CAN_CHANNEL_SIZE] = {NULL};

/* CAN 初始化结构*/
static CAN_InitTypeDef        CAN_InitStructure[CAN_CHANNEL_SIZE];
static CAN_FilterInitTypeDef  CAN_FilterInitStructure[CAN_CHANNEL_SIZE];

/* CAN 过滤器索引*/
static UINT16 m_uFilterNumber[CAN_CHANNEL_SIZE] = {0};

/* CAN1 缓冲区*/
static UCHAR m_chCanMsgBuffer1[CAN_MSG_BUFFER_SIZE * CAN_FRAME_SIZE] = {0};
static UINT8 m_nWritePtr1 = 0;
static UINT8 m_nReadPtr1 = 0;

static BOOL bRecvEn1 = FALSE;

/* CAN2 缓冲区*/
#ifdef STM32F10X_CL
static UCHAR m_chCanMsgBuffer2[CAN_MSG_BUFFER_SIZE * CAN_FRAME_SIZE] = {0};
static UINT8 m_nWritePtr2 = 0;
static UINT8 m_nReadPtr2 = 0;
static BOOL bRecvEn2 = FALSE;
#endif

/*********************************** CAN1 ************************************/

static void enableReceive1(BOOL bIsOn);

/* CAN1 初始化 */
static void init1(UINT8 eMode, UINT32 uBaudrate, CAN_RX_BASE_FUNC hRxFunc)
{
	GPIO_InitTypeDef GPIO_InitStructure; 

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能PORTA时钟
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟	

    GPIO_InitStructure.GPIO_Pin = CAN1_TX;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽
    GPIO_Init(CAN1_GPIO_TYPE, &GPIO_InitStructure);		//初始化IO
   
    GPIO_InitStructure.GPIO_Pin = CAN1_RX;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
    GPIO_Init(CAN1_GPIO_TYPE, &GPIO_InitStructure);//初始化IO
	  
 	//CAN单元设置
	CAN_StructInit(&CAN_InitStructure[CAN_CHANNEL1]);
 	CAN_InitStructure[CAN_CHANNEL1].CAN_TTCM = DISABLE;		//非时间触发通信模式
 	CAN_InitStructure[CAN_CHANNEL1].CAN_ABOM = DISABLE;		//软件自动离线管理
  	CAN_InitStructure[CAN_CHANNEL1].CAN_AWUM = DISABLE;		//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
  	CAN_InitStructure[CAN_CHANNEL1].CAN_NART = DISABLE;		//禁止报文自动传送
  	CAN_InitStructure[CAN_CHANNEL1].CAN_RFLM = DISABLE;		//报文不锁定,新的覆盖旧的 
  	CAN_InitStructure[CAN_CHANNEL1].CAN_TXFP = DISABLE;		//优先级，0：由报文标识符决定 1：由写入FIFO的顺序决定
  	CAN_InitStructure[CAN_CHANNEL1].CAN_Mode = eMode;	    //模式设置
  	//设置波特率
	//CAN Baudrate = PCLK1 / (Prescaler * (SJW + BS1 + BS1))
	//Prescaler = PCLK1 / (Baudrate * (SJW + BS1 + BS1))
  	CAN_InitStructure[CAN_CHANNEL1].CAN_SJW = CAN_SJW_1tq;	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位
  	CAN_InitStructure[CAN_CHANNEL1].CAN_BS1 = CAN_BS2_3tq;	//Tbs1=tbs1+1个时间单位CAN_BS1_1tq ~ CAN_BS1_16tq
  	CAN_InitStructure[CAN_CHANNEL1].CAN_BS2 = CAN_BS1_5tq;	//Tbs2=tbs2+1个时间单位CAN_BS2_1tq ~ CAN_BS2_8tq
  	CAN_InitStructure[CAN_CHANNEL1].CAN_Prescaler = CAN_PCLK / ((1+3+5) * uBaudrate);//分频系数(Fdiv)为brp+1
  	CAN_Init(CAN1, &CAN_InitStructure[CAN_CHANNEL1]);     //初始化CAN1 

 	CAN_FilterInitStructure[CAN_CHANNEL1].CAN_FilterNumber=m_uFilterNumber[CAN_CHANNEL1]++; //过滤器0
 	CAN_FilterInitStructure[CAN_CHANNEL1].CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure[CAN_CHANNEL1].CAN_FilterScale=CAN_FilterScale_32bit; //32位 
  	CAN_FilterInitStructure[CAN_CHANNEL1].CAN_FilterIdHigh=0x0000;////32位ID
  	CAN_FilterInitStructure[CAN_CHANNEL1].CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure[CAN_CHANNEL1].CAN_FilterMaskIdHigh=0x0000;//32位MASK
  	CAN_FilterInitStructure[CAN_CHANNEL1].CAN_FilterMaskIdLow=0x0000;
  	CAN_FilterInitStructure[CAN_CHANNEL1].CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
 	CAN_FilterInitStructure[CAN_CHANNEL1].CAN_FilterActivation=ENABLE; //激活过滤器0
  	CAN_FilterInit(&CAN_FilterInitStructure[CAN_CHANNEL1]);//滤波器初始化
	
	
	m_hCanRxCallBlack[CAN_CHANNEL1] = hRxFunc;
}   

/* CAN1 注销 */
static void deInit1(void)
{
	CAN_StructInit(&CAN_InitStructure[CAN_CHANNEL1]);
	CAN_DeInit(CAN1);
	enableReceive1(FALSE);
	m_hCanRxCallBlack[CAN_CHANNEL1] = NULL;
}

/*
 * 设置波特率 
 * CAN Baudrate = PCLK1 / (Prescaler * (SJW + BS1 + BS1))
 * Prescaler = PCLK1 / (Baudrate * (SJW + BS1 + BS1))
 */
static void setBaudrate1(UINT32 uBaudrate)
{
	CAN_InitStructure[CAN_CHANNEL1].CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure[CAN_CHANNEL1].CAN_BS1 = CAN_BS2_3tq;
	CAN_InitStructure[CAN_CHANNEL1].CAN_BS2 = CAN_BS1_5tq;
	CAN_InitStructure[CAN_CHANNEL1].CAN_Prescaler = CAN_PCLK / ((1+3+5) * uBaudrate);   
	CAN_Init(CAN1, &CAN_InitStructure[CAN_CHANNEL1]);  
}

/* 设置CAN过滤ID. CAN1和CAN2共享14个过滤器组 */
static void setFilterIDs1(UINT32* pIDs, UINT32 size)
{
	UINT i = 0;
	for (i = 0; i < size; ++i)
	{
	#ifndef STM32F10X_CL
		if (m_uFilterNumber[CAN_CHANNEL1] >= 14)//14个32位滤波器
			break;
	#else
		if (m_uFilterNumber[CAN_CHANNEL1] >= 28)//28个16位滤波器
			break;
	#endif /* STM32F10X_CL */ 
		CAN_FilterInitStructure[CAN_CHANNEL1].CAN_FilterNumber = m_uFilterNumber[CAN_CHANNEL1]++;	  //过滤器0
		CAN_FilterInitStructure[CAN_CHANNEL1].CAN_FilterMode = CAN_FilterMode_IdMask; 
		CAN_FilterInitStructure[CAN_CHANNEL1].CAN_FilterScale = CAN_FilterScale_32bit; //32位 
		CAN_FilterInitStructure[CAN_CHANNEL1].CAN_FilterIdHigh = (UINT16)(((pIDs[i] << 3) & 0xFFFF0000) >> 16);//32位时为高位，16位时为第一个 
		CAN_FilterInitStructure[CAN_CHANNEL1].CAN_FilterIdLow = (UINT16)(pIDs[i] << 3);//32位时为低位，16位时为第二个 
		CAN_FilterInitStructure[CAN_CHANNEL1].CAN_FilterMaskIdHigh = 0xFFFF;//32位MASK
		CAN_FilterInitStructure[CAN_CHANNEL1].CAN_FilterMaskIdLow = 0xFFFC & (~CAN_ID_EXT);//同时接收标准帧和扩展帧
		CAN_FilterInitStructure[CAN_CHANNEL1].CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;//过滤器0关联到FIFO0
		CAN_FilterInitStructure[CAN_CHANNEL1].CAN_FilterActivation = ENABLE; //激活过滤器0
		CAN_FilterInit(&CAN_FilterInitStructure[CAN_CHANNEL1]);//滤波器初始化
	}
}

/* 允许CAN消息自动接收 */
static void enableReceive1(BOOL bIsOn)
{
   	NVIC_InitTypeDef  NVIC_InitStructure;
	bRecvEn1 = bIsOn;
	if (bIsOn)
	{
		CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);//FIFO0消息挂号中断允许.	
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	#ifdef STM32F10X_CL
		NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
	#else
		NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	#endif //STM32F10X_CL
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
	else
	{
		CAN_ITConfig(CAN1,CAN_IT_FMP0,DISABLE);//FIFO0消息挂号中断允许.	
	}
}

/* 写缓冲区 */
static void writeBuf1(UINT32 uIDE, UINT32 uID, UCHAR* pMsg, UINT8 len)
{
	UINT16 uIndex = 0;
	UINT16 i = 0;
	
	assert_param(NULL != pMsg);
	
	if (len > 8)
	{
		len = 8;
	}
	uIndex = m_nWritePtr1 * CAN_FRAME_SIZE;
	m_chCanMsgBuffer1[uIndex++] = (UCHAR)(uIDE >> 24);
	m_chCanMsgBuffer1[uIndex++] = (UCHAR)(uIDE >> 16);
	m_chCanMsgBuffer1[uIndex++] = (UCHAR)(uIDE >> 8);
	m_chCanMsgBuffer1[uIndex++] = (UCHAR)uIDE;
	m_chCanMsgBuffer1[uIndex++] = (UCHAR)(uID >> 24);
	m_chCanMsgBuffer1[uIndex++] = (UCHAR)(uID >> 16);
	m_chCanMsgBuffer1[uIndex++] = (UCHAR)(uID >> 8);
	m_chCanMsgBuffer1[uIndex++] = (UCHAR)uID;
	m_chCanMsgBuffer1[uIndex++] = len;
	for (i = 0; i < len; i++)
		m_chCanMsgBuffer1[uIndex++] = pMsg[i];
	
	m_nWritePtr1 = (m_nWritePtr1 + 1) %CAN_MSG_BUFFER_SIZE;
	if (m_nWritePtr1 == m_nReadPtr1)
	{
		m_nReadPtr1 = (m_nReadPtr1 + 1) %CAN_MSG_BUFFER_SIZE;
	}
}

/* 读缓冲区 */
static UINT8 readBuf1(UINT32 *pIDE, UINT32 *pID, UCHAR* pMsg)
{
	UINT16 uIndex = 0;
	UINT16 len = 0;
	UINT16 i = 0;
	UINT32 uIDE;
	UINT32 uID;
	
	assert_param(NULL != pIDE);
	assert_param(NULL != pID);
	assert_param(NULL != pMsg);
	
	if (m_nReadPtr1 == m_nWritePtr1)
		return 0;
	
	uIndex = m_nReadPtr1 * CAN_FRAME_SIZE;
	uIDE = m_chCanMsgBuffer1[uIndex++];
	uIDE = (uIDE << 8) | m_chCanMsgBuffer1[uIndex++];
	uIDE = (uIDE << 8) | m_chCanMsgBuffer1[uIndex++];
	uIDE = (uIDE << 8) | m_chCanMsgBuffer1[uIndex++];
	*pIDE = uIDE;
	uID = m_chCanMsgBuffer1[uIndex++];
	uID = (uID << 8) | m_chCanMsgBuffer1[uIndex++];
	uID = (uID << 8) | m_chCanMsgBuffer1[uIndex++];
	uID = (uID << 8) | m_chCanMsgBuffer1[uIndex++];
	*pID = uID;
	len = m_chCanMsgBuffer1[uIndex++];
	for (i = 0; i < len; i++)
		pMsg[i] = m_chCanMsgBuffer1[uIndex++];
	
	m_nReadPtr1 = (m_nReadPtr1 + 1) %CAN_MSG_BUFFER_SIZE;
	
	return len;
}

/* transmit */
static BOOL transmit1(UINT32 uIDE, UINT32 uID, UCHAR* pMsg, UINT8 len)
{
	UINT8 TransmitMailbox = 0;
	UINT32 i;
	CanTxMsg TxMessage;
	
	if (NULL == pMsg)
		return FALSE;
	
	if (len > 8)
	{
		len = 8;
	}
	
	TxMessage.StdId = uID;
	TxMessage.ExtId = uID;
	
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.IDE = uIDE;
	TxMessage.DLC = len;
	for (i = 0; i < len; ++i)
	{
		TxMessage.Data[i] = pMsg[i];
	}

	TransmitMailbox = CAN_Transmit(CAN1, &TxMessage);
	
	i = 0;
	while((CAN_TransmitStatus(CAN1, TransmitMailbox) == CAN_TxStatus_Failed) && (i <= 0xFFF))
	{
		i++;
	}
	if (i >= 0xFFF)
		return FALSE;

	return TRUE;
}

/* receive */
static UINT8 receive1(UINT32 *pIDE, UINT32 *pID, UCHAR* pMsg)
{
	if (!bRecvEn1)
	{
		UINT32 i;
		CanRxMsg RxMessage;
		
		assert_param(NULL != pIDE);
		assert_param(NULL != pID);
		assert_param(NULL != pMsg);
		
		if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)//没有接收到数据,直接退出 
			return 0;		
		
		CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//读取数据	
		
		*pIDE = RxMessage.IDE;
		if (CAN_ID_EXT == RxMessage.IDE)
		{
			*pID = RxMessage.ExtId;
		}
		else
		{
			*pID = RxMessage.StdId;
		}
		for(i=0;i<8;i++)
		{
			pMsg[i]=RxMessage.Data[i]; 
		}
		return RxMessage.DLC;
	}
	else
	{
		return readBuf1(pIDE, pID, pMsg);
	}
}

//发送中断服务函数
#ifdef  STM32F10X_CL
void CAN1_TX_IRQHandler (void)
#else
void USB_HP_CAN1_TX_IRQHandler (void)
#endif
{
}

//接收中断服务函数
#ifdef  STM32F10X_CL
void CAN1_RX0_IRQHandler (void)
#else
void USB_LP_CAN1_RX0_IRQHandler (void)
#endif
{
  	CanRxMsg RxMessage;
	
	//if (CAN_GetITStatus(CAN1, CAN_IT_FMP0) == SET)//FIFO0消息挂号
	//{
	//	CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
	//}
	//else if (CAN_GetITStatus(CAN1, CAN_IT_FOV0) == SET)//FIFO0溢出
	//{
	//	CAN_ClearITPendingBit(CAN1, CAN_IT_FOV0);
	//}
	//else if (CAN_GetITStatus(CAN1, CAN_IT_BOF) == SET)//进入离线状态
	//{
	//	CAN_ClearITPendingBit(CAN1, CAN_IT_BOF);
	//}
	//else
	{
		CAN_Receive(CAN1, 0, &RxMessage);
		if (m_hCanRxCallBlack[CAN_CHANNEL1] != NULL)
		{
			if (CAN_ID_EXT == RxMessage.IDE)
			{
				writeBuf1(RxMessage.IDE, RxMessage.ExtId, RxMessage.Data, RxMessage.DLC);
			}
			else
			{
				writeBuf1(RxMessage.IDE, RxMessage.StdId, RxMessage.Data, RxMessage.DLC);
			}
			
			if (m_hCanRxCallBlack[CAN_CHANNEL1] != NULL)
			{
				m_hCanRxCallBlack[CAN_CHANNEL1](RxMessage.DLC);
			}
		}
	}
}


/***************************** CAN2 *****************************************/
#ifdef STM32F10X_CL

static void enableReceive2(BOOL bIsOn);

/* CAN2初始化 */
static void init2(UINT8 eMode, UINT32 uBaudrate, CAN_RX_BASE_FUNC hRxFunc)
{
	GPIO_InitTypeDef GPIO_InitStructure; 

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能PORTB时钟
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);//使能CAN1时钟	

    GPIO_InitStructure.GPIO_Pin = CAN2_TX;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽
    GPIO_Init(CAN2_GPIO_TYPE, &GPIO_InitStructure);		//初始化IO
   
    GPIO_InitStructure.GPIO_Pin = CAN2_RX;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
    GPIO_Init(CAN2_GPIO_TYPE, &GPIO_InitStructure);//初始化IO
	  
 	//CAN单元设置
	CAN_StructInit(&CAN_InitStructure[CAN_CHANNEL2]);
 	CAN_InitStructure[CAN_CHANNEL2].CAN_TTCM = DISABLE;		//非时间触发通信模式
 	CAN_InitStructure[CAN_CHANNEL2].CAN_ABOM = ENABLE;		//软件自动离线管理
  	CAN_InitStructure[CAN_CHANNEL2].CAN_AWUM = DISABLE;		//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
  	CAN_InitStructure[CAN_CHANNEL2].CAN_NART = DISABLE;		//禁止报文自动传送
  	CAN_InitStructure[CAN_CHANNEL2].CAN_RFLM = DISABLE;		//报文不锁定,新的覆盖旧的 
  	CAN_InitStructure[CAN_CHANNEL2].CAN_TXFP = DISABLE;		//优先级由报文标识符决定
  	CAN_InitStructure[CAN_CHANNEL2].CAN_Mode = eMode;	    //模式设置
  	//设置波特率 
	//CAN Baudrate = PCLK1 / (Prescaler * (SJW + BS1 + BS1))
	//Prescaler = PCLK1 / (Baudrate * (SJW + BS1 + BS1))
  	CAN_InitStructure[CAN_CHANNEL2].CAN_SJW = CAN_SJW_1tq;	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位
  	CAN_InitStructure[CAN_CHANNEL2].CAN_BS1 = CAN_BS2_3tq;	//Tbs1=tbs1+1个时间单位CAN_BS1_1tq ~ CAN_BS1_16tq
  	CAN_InitStructure[CAN_CHANNEL2].CAN_BS2 = CAN_BS1_5tq;	//Tbs2=tbs2+1个时间单位CAN_BS2_1tq ~ CAN_BS2_8tq
  	CAN_InitStructure[CAN_CHANNEL2].CAN_Prescaler = CAN_PCLK / ((1+3+5) * uBaudrate);//分频系数(Fdiv)为brp+1
  	CAN_Init(CAN2, &CAN_InitStructure[CAN_CHANNEL2]);     //初始化CAN2 

 	CAN_FilterInitStructure[CAN_CHANNEL2].CAN_FilterNumber=m_uFilterNumber[CAN_CHANNEL2]++;	  //过滤器0
 	CAN_FilterInitStructure[CAN_CHANNEL2].CAN_FilterMode=CAN_FilterMode_IdMask; 
  	CAN_FilterInitStructure[CAN_CHANNEL2].CAN_FilterScale=CAN_FilterScale_32bit; //32位 
  	CAN_FilterInitStructure[CAN_CHANNEL2].CAN_FilterIdHigh=0x0000;////32位ID
  	CAN_FilterInitStructure[CAN_CHANNEL2].CAN_FilterIdLow=0x0000;
  	CAN_FilterInitStructure[CAN_CHANNEL2].CAN_FilterMaskIdHigh=0x0000;//32位MASK
  	CAN_FilterInitStructure[CAN_CHANNEL2].CAN_FilterMaskIdLow=0x0000;
  	CAN_FilterInitStructure[CAN_CHANNEL2].CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
 	CAN_FilterInitStructure[CAN_CHANNEL2].CAN_FilterActivation=ENABLE; //激活过滤器0
  	CAN_FilterInit(&CAN_FilterInitStructure[CAN_CHANNEL2]);//滤波器初始化
	

	m_hCanRxCallBlack[CAN_CHANNEL2] = hRxFunc;
	
	osal_task_create(taskForReadBuf2, 1);
}   

/* CAN2 注销 */
static void deInit2(void)
{
	CAN_StructInit(&CAN_InitStructure[CAN_CHANNEL2]);
	CAN_DeInit(CAN2);
	enableReceive2(FALSE);
	m_hCanRxCallBlack[CAN_CHANNEL2] = NULL;
	osal_task_kill(taskForReadBuf2);
}


/*
 * 设置波特率 (KHz)
 * CAN Baudrate = PCLK1 / (Prescaler * (SJW + BS1 + BS1))
 * Prescaler = PCLK1 / (Baudrate * (SJW + BS1 + BS1))
 */
static void setBaudrate2(UINT32 uBaudrate)
{
	CAN_InitStructure[CAN_CHANNEL2].CAN_SJW = CAN_SJW_1tq;
	CAN_InitStructure[CAN_CHANNEL2].CAN_BS1 = CAN_BS2_3tq;
	CAN_InitStructure[CAN_CHANNEL2].CAN_BS2 = CAN_BS1_5tq;
	CAN_InitStructure[CAN_CHANNEL2].CAN_Prescaler = CAN_PCLK / ((1+3+5) * uBaudrate);    
	CAN_Init(CAN2, &CAN_InitStructure[CAN_CHANNEL2]);     
}

/* 设置CAN过滤ID. CAN1和CAN2共享14个过滤器组 */
static void setFilterIDs2(UINT32* pIDs, UINT32 size)
{
	UINT i = 0;
	
	for (i = 0; i < size; ++i)
	{
	#ifndef STM32F10X_CL
		if (m_uFilterNumber[CAN_CHANNEL2] >= 14)//14个32位滤波器
			break;
	#else
		if (m_uFilterNumber[CAN_CHANNEL2] >= 28)//28个16位滤波器
			break;
	#endif /* STM32F10X_CL */  
		CAN_FilterInitStructure[CAN_CHANNEL2].CAN_FilterNumber = m_uFilterNumber[CAN_CHANNEL2]++;	  //过滤器0
		CAN_FilterInitStructure[CAN_CHANNEL2].CAN_FilterMode = CAN_FilterMode_IdMask; 
		CAN_FilterInitStructure[CAN_CHANNEL2].CAN_FilterScale = CAN_FilterScale_32bit; //32位 
		CAN_FilterInitStructure[CAN_CHANNEL1].CAN_FilterIdHigh = (UINT16)(((pIDs[i] << 3) & 0xFFFF0000) >> 16);//32位时为高位，16位时为第一个 
		CAN_FilterInitStructure[CAN_CHANNEL1].CAN_FilterIdLow = (UINT16)(pIDs[i] << 3);//32位时为低位，16位时为第二个 
		CAN_FilterInitStructure[CAN_CHANNEL2].CAN_FilterMaskIdHigh = 0xFFFF;//32位时为高位，16位时为第一个 
		CAN_FilterInitStructure[CAN_CHANNEL2].CAN_FilterMaskIdLow = 0xFFFC & (~CAN_ID_EXT);//32位时为低位，16位时为第二个 
		CAN_FilterInitStructure[CAN_CHANNEL2].CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;//过滤器0关联到FIFO0
		CAN_FilterInitStructure[CAN_CHANNEL2].CAN_FilterActivation = ENABLE; //激活过滤器0
		CAN_FilterInit(&CAN_FilterInitStructure[CAN_CHANNEL2]);//滤波器初始化
	}
}

/* 允许CAN消息自动接收 */
static void enableReceive2(BOOL bIsOn)
{
   	NVIC_InitTypeDef  NVIC_InitStructure;
	bRecvEn2 = bIsOn;
	if (bIsOn)
	{
		CAN_ITConfig(CAN2,CAN_IT_FMP0,ENABLE);//FIFO0消息挂号中断允许.	
	
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	}
	else
	{
		CAN_ITConfig(CAN2,CAN_IT_FMP0,DISABLE);//FIFO0消息挂号中断允许.	
	}
}

/* 写缓冲区 */
static void writeBuf2(UINT32 uIDE, UINT32 uID, UCHAR* pMsg, UINT8 len)
{
	UINT16 uIndex = 0;
	UINT16 i = 0;
	
	assert_param(NULL != pMsg);
	
	if (len > 8)
	{
		len = 8;
	}
	uIndex = m_nWritePtr2 * CAN_FRAME_SIZE;
	m_chCanMsgBuffer2[uIndex++] = (UCHAR)(uIDE >> 24);
	m_chCanMsgBuffer2[uIndex++] = (UCHAR)(uIDE >> 16);
	m_chCanMsgBuffer2[uIndex++] = (UCHAR)(uIDE >> 8);
	m_chCanMsgBuffer2[uIndex++] = (UCHAR)uIDE;
	m_chCanMsgBuffer2[uIndex++] = (UCHAR)(uID >> 24);
	m_chCanMsgBuffer2[uIndex++] = (UCHAR)(uID >> 16);
	m_chCanMsgBuffer2[uIndex++] = (UCHAR)(uID >> 8);
	m_chCanMsgBuffer2[uIndex++] = (UCHAR)uID;
	m_chCanMsgBuffer2[uIndex++] = len;
	for (i = 0; i < len; i++)
		m_chCanMsgBuffer2[uIndex++] = pMsg[i];
	
	m_nWritePtr2++;
	if (m_nWritePtr2 == m_nReadPtr2)
	{
		m_nReadPtr2++;
	}
}

/* 读缓冲区 */
static UINT8 readBuf2(UINT32 *pIDE, UINT32 *pID, UCHAR* pMsg)
{
	UINT16 uIndex = 0;
	UINT16 len = 0;
	UINT16 i = 0;
	UINT32 uIDE;
	UINT32 uID;
	
	assert_param(NULL != pIDE);
	assert_param(NULL != pID);
	assert_param(NULL != pMsg);
	
	if (m_nReadPtr2 == m_nWritePtr2)
		return 0;
	
	uIndex = m_nReadPtr2 * CAN_FRAME_SIZE;
	uIDE = m_chCanMsgBuffer2[uIndex++];
	uIDE = (uIDE << 8) | m_chCanMsgBuffer2[uIndex++];
	uIDE = (uIDE << 8) | m_chCanMsgBuffer2[uIndex++];
	uIDE = (uIDE << 8) | m_chCanMsgBuffer2[uIndex++];
	*pIDE = uIDE;
	uID = m_chCanMsgBuffer2[uIndex++];
	uID = (uID << 8) | m_chCanMsgBuffer2[uIndex++];
	uID = (uID << 8) | m_chCanMsgBuffer2[uIndex++];
	uID = (uID << 8) | m_chCanMsgBuffer2[uIndex++];
	*pID = uID;
	len = m_chCanMsgBuffer2[uIndex++];
	for (i = 0; i < len; i++)
		pMsg[i] = m_chCanMsgBuffer2[uIndex++];
	
	m_nReadPtr2++;
	
	return len;
}

/* transmit */
static BOOL transmit2(UINT32 uIDE, UINT32 uID, UCHAR* pMsg, UINT8 len)
{
	UINT8 TransmitMailbox = 0;
	UINT32 i;
	CanTxMsg TxMessage;
	
	if (NULL == pMsg)
		return FALSE;
	
	if (len > 8)
	{
		len = 8;
	}
	
	if (CAN_ID_EXT == uIDE)
	{
		TxMessage.StdId = uID;
	}
	else
	{
		TxMessage.ExtId = uID;
	}
	TxMessage.RTR = CAN_RTR_DATA;
	TxMessage.IDE = uIDE;
	TxMessage.DLC = len;
	for (i = 0; i < len; ++i)
	{
		TxMessage.Data[i] = pMsg[i];
	}

	TransmitMailbox = CAN_Transmit(CAN2, &TxMessage);
	
	i = 0;
	while((CAN_TransmitStatus(CAN2, TransmitMailbox) == CAN_TxStatus_Failed) && (i <= 0xFFF))
	{
		i++;
	}
	if (i >= 0xFFF)
		return FALSE;

	return TRUE;
}

/* receive */
static UINT8 receive2(UINT32 *pIDE, UINT32 *pID, UCHAR* pMsg)
{
	if (!bRecvEn2)
	{
		UINT32 i;
		CanRxMsg RxMessage;
		
		assert_param(NULL != pIDE);
		assert_param(NULL != pID);
		assert_param(NULL != pMsg);
		
		if( CAN_MessagePending(CAN2,CAN_FIFO0)==0)//没有接收到数据,直接退出 
			return 0;		
		
		CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);//读取数据	
		
		*pIDE = RxMessage.IDE;
		if (CAN_ID_EXT == RxMessage.IDE)
		{
			*pID = RxMessage.ExtId;
		}
		else
		{
			*pID = RxMessage.StdId;
		}
		for(i=0;i<8;i++)
		{
			pMsg[i]=RxMessage.Data[i]; 
		}
		return RxMessage.DLC;
	}
	else
	{
		return readBuf2(pIDE, pID, pMsg);
	}
}

//发送中断服务函数
void CAN2_TX_IRQHandler(void)
{
}
 
//接收中断服务函数
void CAN2_RX0_IRQHandler(void)
{
  	CanRxMsg RxMessage;
	
	//if (CAN_GetITStatus(CAN2, CAN_IT_FMP0) == SET)//FIFO0消息挂号
	//{
	//	CAN_ClearITPendingBit(CAN2, CAN_IT_FMP0);
	//}
	//else if (CAN_GetITStatus(CAN2, CAN_IT_FOV0) == SET)//FIFO0溢出
	//{
	//	CAN_ClearITPendingBit(CAN2, CAN_IT_FOV0);
	//}
	//else if (CAN_GetITStatus(CAN2, CAN_IT_BOF) == SET)//进入离线状态
	//{
	//	CAN_ClearITPendingBit(CAN2, CAN_IT_BOF);
	//}
	//else
	{
		CAN_Receive(CAN2, 0, &RxMessage);
		if (m_hCanRxCallBlack[CAN_CHANNEL2] != NULL)
		{
			if (CAN_ID_EXT == RxMessage.IDE)
			{
				writeBuf2(RxMessage.IDE, RxMessage.ExtId, RxMessage.Data, RxMessage.DLC);
			}
			else
			{
				writeBuf2(RxMessage.IDE, RxMessage.StdId, RxMessage.Data, RxMessage.DLC);
			}
			
			if (m_hCanRxCallBlack[CAN_CHANNEL2] != NULL)
			{
				m_hCanRxCallBlack[CAN_CHANNEL2](RxMessage.DLC);
			}
		}
	}
}


#endif //STM32F10X_CL


void New(CAN_CHANNEL eChannel)
{
	switch (eChannel)
	{
	case CAN_CHANNEL1:
		m_Instance[CAN_CHANNEL1].init = init1;
		m_Instance[CAN_CHANNEL1].deInit = deInit1;
		m_Instance[CAN_CHANNEL1].setBaudrate = setBaudrate1;
		m_Instance[CAN_CHANNEL1].setFilterIDs = setFilterIDs1;
		m_Instance[CAN_CHANNEL1].enableReceive = enableReceive1;
		m_Instance[CAN_CHANNEL1].transmit = transmit1;
		m_Instance[CAN_CHANNEL1].receive = receive1;
		m_pthis[CAN_CHANNEL1] = &m_Instance[CAN_CHANNEL1];
		break;
#ifdef STM32F10X_CL
	case CAN_CHANNEL2:
		m_Instance[CAN_CHANNEL2].init = init2;
		m_Instance[CAN_CHANNEL2].deInit = deInit2;
		m_Instance[CAN_CHANNEL2].setBaudrate = setBaudrate2;
		m_Instance[CAN_CHANNEL1].setFilterIDs = setFilterIDs2;
		m_Instance[CAN_CHANNEL1].enableReceive = enableReceive2;
		m_Instance[CAN_CHANNEL2].transmit = transmit2;
		m_Instance[CAN_CHANNEL2].receive = receive2;
		m_pthis[CAN_CHANNEL2] = &m_Instance[CAN_CHANNEL2];
		break;
#endif //STM32F10X_CL
	default:
		break;
	}
}

HALCanTypeDef* hal_can_getInstance(CAN_CHANNEL eChannel)
{
	if (NULL == m_pthis[eChannel])
	{
		New(eChannel);
	}
	return m_pthis[eChannel];
}

#endif //CFG_HAL_CAN
