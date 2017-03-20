/**
 * @file    comm2.c
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   通讯协议处理模块，实现数据物理帧的解析及消息发送功能
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
#include "comm2.h"

/* Comm数据操作实例 */
static CommTypeDef m_Instance;
static CommTypeDef* pthis= NULL;

/* 本地关联的Uart端口 */
static HALUartTypeDef* m_hUart = NULL;

/* 数据接收完成回调函数指针 */
static CommRxCBack_t m_hRxObser = NULL;

/* 数据接收缓冲区 */
static UCHAR m_rxDataBuffer[MAX_MSG_PHY_DATA_SIZE] = {0};
/* 数据缓冲区读指针 */
static UINT16 m_uReadPriter = 0;
/* 数据缓冲区写指针 */
static UINT16 m_uWritePriter = 0;

#if USE_TX_MSG_FIFO
/* 数据接收缓冲区 */
static UCHAR m_TX_FIFO_DataBuffer[MAX_MSG_FIFO_SIZE * COMM_MSG_SISE] = {0};
/* 数据缓冲区读指针 */
static UINT16 m_uFIFO_ReadPriter = 0;
/* 数据缓冲区写指针 */
static UINT16 m_uFIFO_WritePriter = 0;
#endif //USE_TX_MSG_FIFO

/**
 * @brief 数据长度校验
 * @param nLen 数据长度值
 * @return 返回计算的校验结果
 */
static UINT16 GetCheckLenght(UINT16 uLen)
{
	UINT16 uCheckLen = ~uLen; 
	uCheckLen <<= 12; //高4位为长度校验值，将低4位左移到高4位
	
	return uCheckLen | (uLen & 0x0fff); //合成完整的数据长度
}

/**
 * @brief 计算数据帧校验值(异或检验)
 * @param pMsg 接收到的消息句柄
 * @return 返回计算校验值
 */
static UCHAR GetCheckValue(MsgTypeDef* pMsg)
{
	UCHAR checkValue = 0;
	UINT16 checkLen = 0;
	UINT16 i = 0;
	
    UINT16 uAddr = osal_sock_htons(pMsg->uAddr);
    UINT16 blockID = osal_sock_htons(pMsg->blockID);
    UINT16 functionID = osal_sock_htons(pMsg->functionID);
	
	checkValue = (UCHAR)(uAddr >> 8);			//地址高8位
	checkValue ^= (UCHAR)(uAddr);				//地址低8位
	checkValue ^= (UCHAR)(blockID >> 8);		//功能单元高8位
	checkValue ^= (UCHAR)(blockID);			//功能单元低8位
	checkValue ^= (UCHAR)(functionID >> 8);	//功能ID高8位
	checkValue ^= (UCHAR)(functionID);		//功能ID低8位
	checkValue ^= pMsg->opType;						//操作码
	
	checkLen = osal_sock_htons(GetCheckLenght(pMsg->len));			//数据长度校验
	checkValue ^= (UCHAR)(checkLen >> 8);			//数据长度高8位
	checkValue ^= (UCHAR)(checkLen);					//数据长度低8位
	
	for (i = 0; i < pMsg->len; i++)					//用户数据
	{
		checkValue ^= pMsg->data[i];
	}
	
	return checkValue;
}

/**
 * @brief Uart端口数据接收回调函数
 * @param data 接收到的数据
 * @return void
 */
static void OnComm(UCHAR data)
{
	m_rxDataBuffer[m_uWritePriter] = data;
	
	++m_uWritePriter;
	m_uWritePriter = m_uWritePriter % MAX_MSG_PHY_DATA_SIZE;
	
	if (m_uWritePriter == m_uReadPriter)
	{
		++m_uReadPriter;
		m_uReadPriter = m_uReadPriter % MAX_MSG_PHY_DATA_SIZE;
	}
}

/**
 * @brief 数据帧解析线程，完成物理数据的解析
 * @param None
 * @return void
 */
static void taskForRxdata(void)
{
	static UCHAR rxFlag = 0;
	static UINT16 uRxDataIndex = 0;
	static MsgTypeDef RxMsg;
    static UINT16 uAddr = 0;
    static UINT16 blockID = 0;
    static UINT16 funcID = 0;
    static UINT16 msgLen = 0;
	
	UCHAR rxData = 0;

	if (m_uReadPriter == m_uWritePriter)
		return;
	
	rxData = m_rxDataBuffer[m_uReadPriter];
	++m_uReadPriter;
	m_uReadPriter = m_uReadPriter % MAX_MSG_PHY_DATA_SIZE;
	
	switch(rxFlag)
	{
	case 0:			//起始标志1
		if (PHY_DATA_FLAG1 == rxData)
		{
			++rxFlag;
		}
		break;
	case 1:			//起始标志2
		if (PHY_DATA_FLAG2 == rxData)
		{
			++rxFlag;
		}
		else if (PHY_DATA_FLAG1 != rxData)
		{
			rxFlag = 0;
			uRxDataIndex = 0;
		}
		break;
	case 2:			//地址高8位
		uAddr = rxData;
		++rxFlag;
		break;
	case 3:			//地址低8位
		uAddr = (uAddr << 8) | rxData;
		RxMsg.uAddr = osal_sock_ntohs(uAddr);
		++rxFlag;
		break;
	case 4:			//功能单元高8位
		blockID = rxData;
		++rxFlag;
		break;
	case 5:			//功能单元低8位
		blockID = (blockID << 8) | rxData;
		RxMsg.blockID = osal_sock_ntohs(blockID);
		++rxFlag;
		break;
	case 6:			//功能ID高8位
		funcID = rxData;
		++rxFlag;
		break;
	case 7:			//功能ID低8位
		funcID = (funcID << 8) | rxData;
		RxMsg.functionID = osal_sock_ntohs(funcID);
		++rxFlag;
		break;
	case 8:			//操作码
		RxMsg.opType = rxData;
		++rxFlag;
		break;
	case 9:			//数据长度高8位
        msgLen = rxData;
		++rxFlag;
		break;
	case 10:		//数据长度低8位
		msgLen = (msgLen << 8) | rxData;
        RxMsg.len = osal_sock_ntohs(msgLen);
		if (GetCheckLenght(RxMsg.len) == RxMsg.len)
		{
			RxMsg.len = RxMsg.len & 0x0fff;
			if (RxMsg.len > 1024)
			{
				rxFlag = 0;
				uRxDataIndex = 0;
				DBG(TRACE("comm::GetCheckLenght Error: msg length must be less than 1024\r\n"));
			}
			else
			{
				++rxFlag;
				uRxDataIndex = 0;
			}
		}
		else
		{
			rxFlag = 0;
			uRxDataIndex = 0;
			DBG(TRACE("comm::GetCheckLenght Error: RX[%02x], CH[%02x]\r\n", RxMsg.len, GetCheckLenght(RxMsg.len)));
		}
		break;
	case 11:		//用户数据
		if (uRxDataIndex < RxMsg.len)
		{
			RxMsg.data[uRxDataIndex++] = rxData;
		}
		else		//数据接收完成
		{
			rxFlag = 0;
			uRxDataIndex = 0;
			if (rxData == GetCheckValue(&RxMsg)) //接收数据校验
			{
				//DBG(TRACE("comm::GetCheckValue OK"));
				if (m_hRxObser != NULL)
				{
					m_hRxObser(&RxMsg);
				}
			}
			else		//数据校验失败
			{
				DBG(TRACE("comm::GetCheckValue Error: RX[%02x], CH[%02x]\r\n", rxData, GetCheckValue(&RxMsg)));
			}
		}
		break;
	default:
		rxFlag = 0;
		uRxDataIndex = 0;
		break;
	}
}

/**
 * @brief: 注册报文接收观察者
 * @param hrxobser: 报文接收观察者句柄
 * @retval: void
 */
static void add_rx_obser(CommRxCBack_t hrxobser)
{
	m_hRxObser = hrxobser;
}

/**
 * @brief COMM消息发送
 * @param pMsg: 数据帧内容
 * @param len: 数据帧大小
 * @retval  发送成功返回TRUE，失败返回FALSE
 */
static BOOL transmit(MsgTypeDef* pMsg)
{
	UCHAR txData = 0;
	UINT16 uAddr = 0;
    UINT16 blockID = 0;
	UINT16 uDataLen = 0;
    UINT16 uFuncID = 0;
	
	if (NULL == m_hUart)
		return FALSE;
	
	txData = PHY_DATA_FLAG1;				//起始标志1
	m_hUart->write(&txData, 1);
	
	txData = PHY_DATA_FLAG2;				//起始标志2
	m_hUart->write(&txData, 1);
	
    uAddr = osal_sock_htons(pMsg->uAddr);
	txData = (UCHAR)(uAddr >> 8);				//地址高8位
	m_hUart->write(&txData, 1);
	txData = (UCHAR)(uAddr);					//地址低8位
	m_hUart->write(&txData, 1);
	
    blockID = osal_sock_htons(pMsg->blockID);
	txData = (UCHAR)(blockID >> 8);			//功能单元高8位
	m_hUart->write(&txData, 1);
	txData = (UCHAR)(blockID);			//功能单元低8位
	m_hUart->write(&txData, 1);
	
    uFuncID = osal_sock_htons(pMsg->functionID);
	txData = (UCHAR)(uFuncID >> 8);//功能ID高8位
	m_hUart->write(&txData, 1);
	txData = (UCHAR)(uFuncID);		//功能ID低8位
	m_hUart->write(&txData, 1);
	
	txData = pMsg->opType;					//操作码
	m_hUart->write(&txData, 1);
	
	uDataLen = GetCheckLenght(pMsg->len);	//数据长度
    uDataLen = osal_sock_htons(uDataLen);
	txData = (UCHAR)(uDataLen >> 8);		//高8位
	m_hUart->write(&txData, 1);
	txData = (UCHAR)uDataLen;				//低8位
	m_hUart->write(&txData, 1);
	
	m_hUart->write(pMsg->data, pMsg->len);	//用户数据
	
	txData = GetCheckValue(pMsg);			//数据校验
	m_hUart->write(&txData, 1);
	
	return TRUE;
}

#if USE_TX_MSG_FIFO
/**
 * @brief 读发送数据FIFO
 * @param pMsg 存储发送的消息句柄
 * @return 读FIFO操作结果
 * @retval 读取成功返回TRUE，失败返回FALSE
 */
static BOOL readTxFIFO(MsgTypeDef* pMsg)
{
	UINT16 i = 0;
	UINT16 index = 0;
	if (NULL == pMsg)		//参数非法
		return FALSE;
	if (m_uFIFO_ReadPriter == m_uFIFO_WritePriter)//FIFO为空
		return FALSE;
	++m_uFIFO_ReadPriter;
	m_uFIFO_ReadPriter = m_uFIFO_ReadPriter % MAX_MSG_FIFO_SIZE;
	
	index = m_uFIFO_ReadPriter * COMM_MSG_SISE;
	pMsg->uAddr = m_TX_FIFO_DataBuffer[index++];								//地址高8位
	pMsg->uAddr = (pMsg->uAddr << 8) | m_TX_FIFO_DataBuffer[index++];			//地址低8位
	pMsg->blockID = m_TX_FIFO_DataBuffer[index++];								//功能单元高8位
	pMsg->blockID = (pMsg->blockID << 8) | m_TX_FIFO_DataBuffer[index++];		//功能单元低8位
	pMsg->functionID = m_TX_FIFO_DataBuffer[index++];							//功能ID高8位
	pMsg->functionID = (pMsg->functionID << 8) | m_TX_FIFO_DataBuffer[index++];	//功能ID低8位
	pMsg->opType = m_TX_FIFO_DataBuffer[index++];								//操作码
	pMsg->len = m_TX_FIFO_DataBuffer[index++];									//数据长度高8位
	pMsg->len = (pMsg->len << 8) | m_TX_FIFO_DataBuffer[index++];				//数据长度低8位
	for (i = 0; i < pMsg->len; ++i)												//用户数据
	{
		pMsg->data[i] = m_TX_FIFO_DataBuffer[index++];
	}
	
	return TRUE;
}

/**
 * @brief 写发送数据FIFO
 * @param pMsg 发送消息句柄
 * @return 写FIFO操作结果
 * @retval 写入成功返回TRUE，失败返回FALSE
 */
static BOOL writeTxFIFO(MsgTypeDef* pMsg)
{
	UINT16 i = 0;
	UINT16 index = 0;
	if (NULL == pMsg)	//参数非法
		return FALSE;
	
	index = m_uFIFO_WritePriter * COMM_MSG_SISE;
	m_TX_FIFO_DataBuffer[index++] = (UCHAR)(pMsg->uAddr >> 8);					//地址高8位
	m_TX_FIFO_DataBuffer[index++] = (UCHAR)(pMsg->uAddr);						//地址低8位
	m_TX_FIFO_DataBuffer[index++] = (UCHAR)(pMsg->blockID >> 8);				//功能单元高8位
	m_TX_FIFO_DataBuffer[index++] = (UCHAR)(pMsg->blockID);						//功能单元低8位
	m_TX_FIFO_DataBuffer[index++] = (UCHAR)(pMsg->functionID >> 8);				//功能ID高8位
	m_TX_FIFO_DataBuffer[index++] = (UCHAR)(pMsg->functionID);					//功能ID低8位
	m_TX_FIFO_DataBuffer[index++] = pMsg->opType;								//操作码
	m_TX_FIFO_DataBuffer[index++] = (UCHAR)(pMsg->len >> 8);					//数据长度高8位
	m_TX_FIFO_DataBuffer[index++] = (UCHAR)(pMsg->len);							//数据长度低8位
	for (i = 0; i < pMsg->len; ++i)												//用户数据
	{
		m_TX_FIFO_DataBuffer[index++] = pMsg->data[i];
	}
	
	++m_uFIFO_WritePriter;
	m_uFIFO_WritePriter = m_uFIFO_WritePriter % MAX_MSG_FIFO_SIZE;
	if (m_uFIFO_WritePriter == m_uFIFO_ReadPriter)
	{
		++m_uFIFO_ReadPriter;
		m_uFIFO_ReadPriter = m_uFIFO_ReadPriter % MAX_MSG_FIFO_SIZE;
	}
	
	return TRUE;
}

/**
 * @brief COMM消息发送
 * @param pMsg: 数据帧内容
 * @param len: 数据帧大小
 * @retval  发送成功返回TRUE，失败返回FALSE
 */
static BOOL transmitByFIFO(MsgTypeDef* pMsg)
{
	return writeTxFIFO(pMsg);
}

/**
 * @brief FIFO数据发送线程，用于发送FIFO中的数据
 */
static void taskForTxFiFoMsg(void)
{
	static MsgTypeDef TxMsg;
	
	if (readTxFIFO(&TxMsg))
	{
		transmit(&TxMsg);
	}
}
#endif

/**
 * @brief COMM初始化
 * @param hUart:  Uart实例句柄
 * @param hRxFunc: COMM接收回调函数句柄
 * @retval None
 */
static void init(HALUartTypeDef* hUart)
{
	if (NULL == hUart)
		return;
	if (m_hUart != NULL)
		m_hUart->deInit();
	
	m_hUart = hUart;
	m_hUart->add_rx_obser(OnComm);
	osal_task_create(taskForRxdata, 1);
#if USE_TX_MSG_FIFO
	osal_task_create(taskForTxFiFoMsg, 1);
#endif
}

/**
 * @brief COMM资源注销
 * @param None
 * @retval None
 */
static void deInit(void)
{
	pthis = NULL;
	if (m_hUart != NULL)
	{
		m_hUart->deInit();
	}
	m_hUart = NULL;
	osal_task_kill(taskForRxdata);
	
#if USE_TX_MSG_FIFO
	osal_task_kill(taskForTxFiFoMsg);
#endif
}

static void New(void)
{
	m_Instance.init = init;
	m_Instance.deInit = deInit;
	m_Instance.add_rx_obser = add_rx_obser;
	m_Instance.transmit = transmit;
#if USE_TX_MSG_FIFO
	m_Instance.transmitByFIFO = transmitByFIFO;
#endif
	pthis = &m_Instance;
}

/**
 * @brief 获取Comm操作结构句柄
 * @param eChennal: COMM通道枚举 @ref CommChannelDef
 * @return 返回Comm操作结构句柄
 * @retval COMM操作结构句柄
 */
CommTypeDef* comm2_getInstance(void)
{
	if (NULL == pthis)
	{
		New();
	}
	return pthis;
}

