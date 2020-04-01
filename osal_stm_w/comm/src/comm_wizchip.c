/**
 * @file    comm_wizchip.c
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   ͨѶЭ�鴦��ģ�飬ʵ����������֡�Ľ�������Ϣ���͹���
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
#include "comm_wizchip.h"
#include "wizchip_net.h"

#if (defined(CFG_OSAL_COMM) && defined(CFG_USE_NET) && defined(CFG_WIZCHIP) && defined(CFG_OSAL_COMM_WIZCHIP))


#ifdef USE_RT_THREAD
#include <rtthread.h>
#define RT_WAITING_TIME RT_WAITING_FOREVER //��ȡ�������ȴ�ʱ��
/* ���������ƿ� */
static UINT8 m_uMutexInited = 0x00;
static struct rt_mutex mutex_rx;
static struct rt_mutex mutex_tx;

#define RXDATA_TASK_PRIO				1	//���ȼ�
#define RXDATA_TASK_TICK				1	//ʱ��Ƭ
#define RXDATA_TASK_STK_SIZE			32	//����ջ��С
ALIGN(RT_ALIGN_SIZE) static unsigned char RXDATA_TASK_STK[WIZCHIP_NET_SIZE][RXDATA_TASK_STK_SIZE];	//����ջ
static struct rt_thread RXDATA_Task_Handle[WIZCHIP_NET_SIZE];	//������

#if USE_TX_MSG_FIFO
#define TXFIFO_TASK_PRIO				1	//���ȼ�
#define TXFIFO_TASK_TICK				1	//ʱ��Ƭ
#define TXFIFO_TASK_STK_SIZE			32	//����ջ��С
ALIGN(RT_ALIGN_SIZE) static unsigned char TXFIFO_TASK_STK[WIZCHIP_NET_SIZE][TXFIFO_TASK_STK_SIZE];	//����ջ
static struct rt_thread TXFIFO_Task_Handle[WIZCHIP_NET_SIZE];	//������
#endif // USE_TX_MSG_FIFO
#endif //#ifndef USE_RT_THREAD

//��Ϣ�����־
static UINT16 m_uMsgRecvFlag = 0x0000;

/* Comm���ݲ���ʵ�� */
static CommTypeDef m_Instance[WIZCHIP_NET_SIZE];
static CommTypeDef* pthis[WIZCHIP_NET_SIZE] = {NULL};

/* ���ع�����Uart�˿� */
static HALUartTypeDef* m_hUart[WIZCHIP_NET_SIZE] = {NULL};

/* ���ݽ�����ɻص�����ָ�� */
static CommRxCBack_t m_hRxObser[WIZCHIP_NET_SIZE] = {NULL};

/* ���ݽ��ջ����� */
static UCHAR m_rxDataBuffer[MAX_MSG_PHY_DATA_SIZE] = {0};
/* ���ݻ�������ָ�� */
static UINT16 m_uReadPriter = 0;
/* ���ݻ�����дָ�� */
static UINT16 m_uWritePriter = 0;
/* ���ݽ��ջ����� */
static UCHAR m_txDataBuffer[MAX_MSG_PHY_DATA_SIZE] = {0};

#if USE_TX_MSG_FIFO
/* ���ݽ��ջ����� */
static UCHAR m_TX_FIFO_DataBuffer[WIZCHIP_NET_SIZE][MAX_MSG_FIFO_SIZE * COMM_MSG_SISE] = {0};
/* ���ݻ�������ָ�� */
static UINT16 m_uFIFO_ReadPriter = 0;
/* ���ݻ�����дָ�� */
static UINT16 m_uFIFO_WritePriter = 0;
#endif //USE_TX_MSG_FIFO

/**
 * @brief ���ݳ���У��
 * @param nLen ���ݳ���ֵ
 * @return ���ؼ����У����
 */
static UINT16 GetCheckLenght(UINT16 uLen)
{
	UINT16 uCheckLen = ~uLen; 
	uCheckLen <<= 12; //��4λΪ����У��ֵ������4λ���Ƶ���4λ
	
	return uCheckLen | (uLen & 0x0fff); //�ϳ����������ݳ���
}

/**
 * @brief ��������֡У��ֵ(������)
 * @param pMsg ���յ�����Ϣ���
 * @return ���ؼ���У��ֵ
 */
static UCHAR GetCheckValue(MsgTypeDef* pMsg)
{
	UCHAR checkValue = 0;
	UINT16 checkLen = 0;
	UINT16 i = 0;
	
    UINT16 uAddr = osal_sock_htons(pMsg->uAddr);
    UINT16 blockID = osal_sock_htons(pMsg->blockID);
    UINT16 functionID = osal_sock_htons(pMsg->functionID);
	
	checkValue = (UCHAR)(uAddr >> 8);			//��ַ��8λ
	checkValue ^= (UCHAR)(uAddr);				//��ַ��8λ
	checkValue ^= (UCHAR)(blockID >> 8);		//���ܵ�Ԫ��8λ
	checkValue ^= (UCHAR)(blockID);			//���ܵ�Ԫ��8λ
	checkValue ^= (UCHAR)(functionID >> 8);	//����ID��8λ
	checkValue ^= (UCHAR)(functionID);		//����ID��8λ
	checkValue ^= pMsg->opType;						//������
	
	checkLen = osal_sock_htons(GetCheckLenght(pMsg->len));			//���ݳ���У��
	checkValue ^= (UCHAR)(checkLen >> 8);			//���ݳ��ȸ�8λ
	checkValue ^= (UCHAR)(checkLen);					//���ݳ��ȵ�8λ
	
	for (i = 0; i < pMsg->len; i++)					//�û�����
	{
		checkValue ^= pMsg->data[i];
	}
	
	return checkValue;
}


#ifdef WIZCHIP_NET_CH0
/**
 * @brief Uart�˿����ݽ��ջص�����
 * @param data ���յ�������
 * @return void
 */
static void OnComm0(UCHAR data)
{
	m_uMsgRecvFlag |= BIT(0);
}

/**
 * @brief ����֡�����̣߳�����������ݵĽ���
 * @param None
 * @return void
 */
#ifndef USE_RT_THREAD
static void taskForRxdata0(void)
#else
static void taskForRxdata0(void* arg)
#endif
{
	static UCHAR rxFlag = 0;
	static UINT16 uRxDataIndex = 0;
	static MsgTypeDef RxMsg;
    static UINT16 uAddr = 0;
    static UINT16 blockID = 0;
    static UINT16 funcID = 0;
    static UINT16 msgLen = 0;
	
	UCHAR rxData = 0;
#ifdef USE_RT_THREAD
	while (1)
	{
		if (m_uMsgRecvFlag & BIT(0))
			continue;
		m_uMsgRecvFlag &= ~BIT(0);
		if (rt_mutex_take(&mutex_rx, RT_WAITING_TIME) != RT_EOK)
			continue;
		m_uWritePriter = m_hUart[0]->read(m_rxDataBuffer, MAX_MSG_PHY_DATA_SIZE);
		for (m_uReadPriter=0; m_uReadPriter<m_uWritePriter;m_uReadPriter++)
		{
			rxData = m_rxDataBuffer[m_uReadPriter];
#else
	if (m_uReadPriter == m_uWritePriter)
		return;
		rxData = m_rxDataBuffer[m_uReadPriter];
		++m_uReadPriter;
		m_uReadPriter = m_uReadPriter % MAX_MSG_PHY_DATA_SIZE;
#endif
		
		switch(rxFlag)
		{
		case 0:			//��ʼ��־1
			if (PHY_DATA_FLAG1 == rxData)
			{
				++rxFlag;
			}
			break;
		case 1:			//��ʼ��־2
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
		case 2:			//��ַ��8λ
			uAddr = rxData;
			++rxFlag;
			break;
		case 3:			//��ַ��8λ
			uAddr = (uAddr << 8) | rxData;
			RxMsg.uAddr = osal_sock_ntohs(uAddr);
			++rxFlag;
			break;
		case 4:			//���ܵ�Ԫ��8λ
			blockID = rxData;
			++rxFlag;
			break;
		case 5:			//���ܵ�Ԫ��8λ
			blockID = (blockID << 8) | rxData;
			RxMsg.blockID = osal_sock_ntohs(blockID);
			++rxFlag;
			break;
		case 6:			//����ID��8λ
			funcID = rxData;
			++rxFlag;
			break;
		case 7:			//����ID��8λ
			funcID = (funcID << 8) | rxData;
			RxMsg.functionID = osal_sock_ntohs(funcID);
			++rxFlag;
			break;
		case 8:			//������
			RxMsg.opType = rxData;
			++rxFlag;
			break;
		case 9:			//���ݳ��ȸ�8λ
			msgLen = rxData;
			++rxFlag;
			break;
		case 10:		//���ݳ��ȵ�8λ
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
		case 11:		//�û�����
			if (uRxDataIndex < RxMsg.len)
			{
				RxMsg.data[uRxDataIndex++] = rxData;
			}
			else		//���ݽ������
			{
				rxFlag = 0;
				uRxDataIndex = 0;
				if (rxData == GetCheckValue(&RxMsg)) //��������У��
				{
					//DBG(TRACE("comm::GetCheckValue OK"));
					if (m_hRxObser[0] != NULL)
					{
						m_hRxObser[0](&RxMsg);
#ifdef USE_RT_THREAD
						m_uReadPriter = m_uWritePriter;
#endif
					}
				}
				else		//����У��ʧ��
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

#ifdef USE_RT_THREAD
		}//for
		rt_mutex_release(&mutex_rx);
	}
#endif
}

/**
 * @brief: ע�ᱨ�Ľ��չ۲���
 * @param hrxobser: ���Ľ��չ۲��߾��
 * @retval: void
 */
static void add_rx_obser0(CommRxCBack_t hrxobser)
{
	m_hRxObser[0] = hrxobser;
}

/**
 * @brief COMM��Ϣ����
 * @param pMsg: ����֡����
 * @param len: ����֡��С
 * @retval  ���ͳɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL transmit0(MsgTypeDef* pMsg)
{
	UCHAR txData = 0;
	UINT16 uAddr = 0;
    UINT16 blockID = 0;
	UINT16 uDataLen = 0;
    UINT16 uFuncID = 0;
	UINT16 txLen = 0;
	
	if (NULL == m_hUart[0])
		return FALSE;
	
#ifdef USE_RT_THREAD
	if (rt_mutex_take(&mutex_tx, RT_WAITING_TIME) != RT_EOK)
		return FALSE;
#endif
	txData = PHY_DATA_FLAG1;				//��ʼ��־1
	m_txDataBuffer[txLen++] = txData;
	
	txData = PHY_DATA_FLAG2;				//��ʼ��־2
	m_txDataBuffer[txLen++] = txData;
	
    uAddr = osal_sock_htons(pMsg->uAddr);
	txData = (UCHAR)(uAddr >> 8);				//��ַ��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)(uAddr);					//��ַ��8λ
	m_txDataBuffer[txLen++] = txData;
	
    blockID = osal_sock_htons(pMsg->blockID);
	txData = (UCHAR)(blockID >> 8);			//���ܵ�Ԫ��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)(blockID);			//���ܵ�Ԫ��8λ
	m_txDataBuffer[txLen++] = txData;
	
    uFuncID = osal_sock_htons(pMsg->functionID);
	txData = (UCHAR)(uFuncID >> 8);//����ID��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)(uFuncID);		//����ID��8λ
	m_txDataBuffer[txLen++] = txData;
	
	txData = pMsg->opType;					//������
	m_txDataBuffer[txLen++] = txData;
	
	uDataLen = GetCheckLenght(pMsg->len);	//���ݳ���
    uDataLen = osal_sock_htons(uDataLen);
	txData = (UCHAR)(uDataLen >> 8);		//��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)uDataLen;				//��8λ
	m_txDataBuffer[txLen++] = txData;
	
	memcpy(&m_txDataBuffer[txLen], pMsg->data, pMsg->len);	//�û�����
	txLen += pMsg->len;
	
	txData = GetCheckValue(pMsg);			//����У��
	m_txDataBuffer[txLen++] = txData;
	
	m_hUart[0]->write(m_txDataBuffer, txLen);
	
#ifdef USE_RT_THREAD
	rt_mutex_release(&mutex_tx);
#endif
	return TRUE;
}

#if USE_TX_MSG_FIFO
/**
 * @brief ����������FIFO
 * @param pMsg �洢���͵���Ϣ���
 * @return ��FIFO�������
 * @retval ��ȡ�ɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL readTxFIFO0(MsgTypeDef* pMsg)
{
	UINT16 i = 0;
	UINT16 index = 0;
	if (NULL == pMsg)		//�����Ƿ�
		return FALSE;
	if (m_uFIFO_ReadPriter == m_uFIFO_WritePriter)//FIFOΪ��
		return FALSE;
	++m_uFIFO_ReadPriter;
	m_uFIFO_ReadPriter = m_uFIFO_ReadPriter % MAX_MSG_FIFO_SIZE;
	
	index = m_uFIFO_ReadPriter * COMM_MSG_SISE;
	pMsg->uAddr = m_TX_FIFO_DataBuffer[0][index++];								//��ַ��8λ
	pMsg->uAddr = (pMsg->uAddr << 8) | m_TX_FIFO_DataBuffer[0][index++];			//��ַ��8λ
	pMsg->blockID = m_TX_FIFO_DataBuffer[0][index++];								//���ܵ�Ԫ��8λ
	pMsg->blockID = (pMsg->blockID << 8) | m_TX_FIFO_DataBuffer[0][index++];		//���ܵ�Ԫ��8λ
	pMsg->functionID = m_TX_FIFO_DataBuffer[0][index++];							//����ID��8λ
	pMsg->functionID = (pMsg->functionID << 8) | m_TX_FIFO_DataBuffer[0][index++];	//����ID��8λ
	pMsg->opType = m_TX_FIFO_DataBuffer[0][index++];								//������
	pMsg->len = m_TX_FIFO_DataBuffer[0][index++];									//���ݳ��ȸ�8λ
	pMsg->len = (pMsg->len << 8) | m_TX_FIFO_DataBuffer[0][index++];				//���ݳ��ȵ�8λ
	for (i = 0; i < pMsg->len; ++i)												//�û�����
	{
		pMsg->data[i] = m_TX_FIFO_DataBuffer[0][index++];
	}
	
	return TRUE;
}

/**
 * @brief д��������FIFO
 * @param pMsg ������Ϣ���
 * @return дFIFO�������
 * @retval д��ɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL writeTxFIFO0(MsgTypeDef* pMsg)
{
	UINT16 i = 0;
	UINT16 index = 0;
	if (NULL == pMsg)	//�����Ƿ�
		return FALSE;
	
	index = m_uFIFO_WritePriter * COMM_MSG_SISE;
	m_TX_FIFO_DataBuffer[0][index++] = (UCHAR)(pMsg->uAddr >> 8);					//��ַ��8λ
	m_TX_FIFO_DataBuffer[0][index++] = (UCHAR)(pMsg->uAddr);						//��ַ��8λ
	m_TX_FIFO_DataBuffer[0][index++] = (UCHAR)(pMsg->blockID >> 8);				//���ܵ�Ԫ��8λ
	m_TX_FIFO_DataBuffer[0][index++] = (UCHAR)(pMsg->blockID);						//���ܵ�Ԫ��8λ
	m_TX_FIFO_DataBuffer[0][index++] = (UCHAR)(pMsg->functionID >> 8);				//����ID��8λ
	m_TX_FIFO_DataBuffer[0][index++] = (UCHAR)(pMsg->functionID);					//����ID��8λ
	m_TX_FIFO_DataBuffer[0][index++] = pMsg->opType;								//������
	m_TX_FIFO_DataBuffer[0][index++] = (UCHAR)(pMsg->len >> 8);					//���ݳ��ȸ�8λ
	m_TX_FIFO_DataBuffer[0][index++] = (UCHAR)(pMsg->len);							//���ݳ��ȵ�8λ
	for (i = 0; i < pMsg->len; ++i)												//�û�����
	{
		m_TX_FIFO_DataBuffer[0][index++] = pMsg->data[i];
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
 * @brief COMM��Ϣ����
 * @param pMsg: ����֡����
 * @param len: ����֡��С
 * @retval  ���ͳɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL transmitByFIFO0(MsgTypeDef* pMsg)
{
	return writeTxFIFO0(pMsg);
}

/**
 * @brief FIFO���ݷ����̣߳����ڷ���FIFO�е�����
 */
#ifndef USE_RT_THREAD
static void taskForTxFiFoMsg0(void)
#else
static void taskForTxFiFoMsg0(void* arg)
#endif
{
	static MsgTypeDef TxMsg;
#ifdef USE_RT_THREAD
	while (1)
#endif
	{
		if (readTxFIFO0(&TxMsg))
		{
			transmit0(&TxMsg);
		}
	}
}
#endif

/**
 * @brief COMM��ʼ��
 * @param hUart:  Uartʵ�����
 * @param hRxFunc: COMM���ջص��������
 * @retval None
 */
static void init0(HALUartTypeDef* hUart)
{
	if (NULL == hUart)
		return;
	if (m_hUart[0] != NULL)
		m_hUart[0]->deInit();
	
	m_hUart[0] = hUart;
	m_hUart[0]->add_rx_obser(OnComm0);
	
#ifndef USE_RT_THREAD
	m_hUart[0]->add_rx_obser(OnComm0);
	osal_task_create(taskForRxdata0, 1);
#if USE_TX_MSG_FIFO
	osal_task_create(taskForTxFiFoMsg0, 1);
#endif
#else
	if(rt_thread_init(&RXDATA_Task_Handle[0], "CommEsp8266RxData", taskForRxdata0, RT_NULL,
						(unsigned char *)&RXDATA_TASK_STK[0][0], RXDATA_TASK_STK_SIZE, RXDATA_TASK_PRIO, RXDATA_TASK_TICK) == RT_EOK)
		rt_thread_startup(&RXDATA_Task_Handle[0]);
#if USE_TX_MSG_FIFO
	if(rt_thread_init(&TXFIFO_Task_Handle[0], "CommEsp8266TxFIFO", taskForTxFiFoMsg0, RT_NULL,
						(unsigned char *)&TXFIFO_TASK_STK[0][0], TXFIFO_TASK_STK_SIZE, TXFIFO_TASK_PRIO, TXFIFO_TASK_TICK) == RT_EOK)
		rt_thread_startup(&TXFIFO_Task_Handle[0]);
#endif
#endif
}

/**
 * @brief COMM��Դע��
 * @param None
 * @retval None
 */
static void deInit0(void)
{
	pthis[0] = NULL;
	if (m_hUart[0] != NULL)
	{
		m_hUart[0]->deInit();
	}
	m_hUart[0] = NULL;
#ifndef USE_RT_THREAD
	osal_task_kill(taskForRxdata0);
	
#if USE_TX_MSG_FIFO
	osal_task_kill(taskForTxFiFoMsg0);
#endif
#else
	rt_thread_detach(&RXDATA_Task_Handle[0]);
	
#if USE_TX_MSG_FIFO
	rt_thread_detach(&TXFIFO_Task_Handle[0]);
#endif
#endif

}
#endif

#ifdef WIZCHIP_NET_CH1
/**
 * @brief Uart�˿����ݽ��ջص�����
 * @param data ���յ�������
 * @return void
 */
static void OnComm1(UCHAR data)
{
	m_uMsgRecvFlag |= BIT(1);
}

/**
 * @brief ����֡�����̣߳�����������ݵĽ���
 * @param None
 * @return void
 */
#ifndef USE_RT_THREAD
static void taskForRxdata1(void)
#else
static void taskForRxdata1(void* arg)
#endif
{
	static UCHAR rxFlag = 0;
	static UINT16 uRxDataIndex = 0;
	static MsgTypeDef RxMsg;
    static UINT16 uAddr = 0;
    static UINT16 blockID = 0;
    static UINT16 funcID = 0;
    static UINT16 msgLen = 0;
	
	UCHAR rxData = 0;
#ifdef USE_RT_THREAD
	while (1)
	{
		if (m_uMsgRecvFlag & BIT(1))
			continue;
		m_uMsgRecvFlag &= ~BIT(1);
		if (rt_mutex_take(&mutex_rx, RT_WAITING_TIME) != RT_EOK)
			continue;
		m_uWritePriter = m_hUart[1]->read(m_rxDataBuffer, MAX_MSG_PHY_DATA_SIZE);
		for (m_uReadPriter=0; m_uReadPriter<m_uWritePriter;m_uReadPriter++)
		{
			rxData = m_rxDataBuffer[m_uReadPriter];
#else
	if (m_uReadPriter == m_uWritePriter)
		return;
		rxData = m_rxDataBuffer[m_uReadPriter];
		++m_uReadPriter;
		m_uReadPriter = m_uReadPriter % MAX_MSG_PHY_DATA_SIZE;
#endif
		
		switch(rxFlag)
		{
		case 0:			//��ʼ��־1
			if (PHY_DATA_FLAG1 == rxData)
			{
				++rxFlag;
			}
			break;
		case 1:			//��ʼ��־2
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
		case 2:			//��ַ��8λ
			uAddr = rxData;
			++rxFlag;
			break;
		case 3:			//��ַ��8λ
			uAddr = (uAddr << 8) | rxData;
			RxMsg.uAddr = osal_sock_ntohs(uAddr);
			++rxFlag;
			break;
		case 4:			//���ܵ�Ԫ��8λ
			blockID = rxData;
			++rxFlag;
			break;
		case 5:			//���ܵ�Ԫ��8λ
			blockID = (blockID << 8) | rxData;
			RxMsg.blockID = osal_sock_ntohs(blockID);
			++rxFlag;
			break;
		case 6:			//����ID��8λ
			funcID = rxData;
			++rxFlag;
			break;
		case 7:			//����ID��8λ
			funcID = (funcID << 8) | rxData;
			RxMsg.functionID = osal_sock_ntohs(funcID);
			++rxFlag;
			break;
		case 8:			//������
			RxMsg.opType = rxData;
			++rxFlag;
			break;
		case 9:			//���ݳ��ȸ�8λ
			msgLen = rxData;
			++rxFlag;
			break;
		case 10:		//���ݳ��ȵ�8λ
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
		case 11:		//�û�����
			if (uRxDataIndex < RxMsg.len)
			{
				RxMsg.data[uRxDataIndex++] = rxData;
			}
			else		//���ݽ������
			{
				rxFlag = 0;
				uRxDataIndex = 0;
				if (rxData == GetCheckValue(&RxMsg)) //��������У��
				{
					//DBG(TRACE("comm::GetCheckValue OK"));
					if (m_hRxObser[1] != NULL)
					{
						m_hRxObser[1](&RxMsg);
#ifdef USE_RT_THREAD
						m_uReadPriter = m_uWritePriter;
#endif
					}
				}
				else		//����У��ʧ��
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

#ifdef USE_RT_THREAD
		}//for
		rt_mutex_release(&mutex_rx);
	}
#endif
}

/**
 * @brief: ע�ᱨ�Ľ��չ۲���
 * @param hrxobser: ���Ľ��չ۲��߾��
 * @retval: void
 */
static void add_rx_obser1(CommRxCBack_t hrxobser)
{
	m_hRxObser[1] = hrxobser;
}

/**
 * @brief COMM��Ϣ����
 * @param pMsg: ����֡����
 * @param len: ����֡��С
 * @retval  ���ͳɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL transmit1(MsgTypeDef* pMsg)
{
	UCHAR txData = 0;
	UINT16 uAddr = 0;
    UINT16 blockID = 0;
	UINT16 uDataLen = 0;
    UINT16 uFuncID = 0;
	UINT16 txLen = 0;
	
	if (NULL == m_hUart[1])
		return FALSE;
	
#ifdef USE_RT_THREAD
	if (rt_mutex_take(&mutex_tx, RT_WAITING_TIME) != RT_EOK)
		return FALSE;
#endif
	txData = PHY_DATA_FLAG1;				//��ʼ��־1
	m_txDataBuffer[txLen++] = txData;
	
	txData = PHY_DATA_FLAG2;				//��ʼ��־2
	m_txDataBuffer[txLen++] = txData;
	
    uAddr = osal_sock_htons(pMsg->uAddr);
	txData = (UCHAR)(uAddr >> 8);				//��ַ��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)(uAddr);					//��ַ��8λ
	m_txDataBuffer[txLen++] = txData;
	
    blockID = osal_sock_htons(pMsg->blockID);
	txData = (UCHAR)(blockID >> 8);			//���ܵ�Ԫ��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)(blockID);			//���ܵ�Ԫ��8λ
	m_txDataBuffer[txLen++] = txData;
	
    uFuncID = osal_sock_htons(pMsg->functionID);
	txData = (UCHAR)(uFuncID >> 8);//����ID��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)(uFuncID);		//����ID��8λ
	m_txDataBuffer[txLen++] = txData;
	
	txData = pMsg->opType;					//������
	m_txDataBuffer[txLen++] = txData;
	
	uDataLen = GetCheckLenght(pMsg->len);	//���ݳ���
    uDataLen = osal_sock_htons(uDataLen);
	txData = (UCHAR)(uDataLen >> 8);		//��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)uDataLen;				//��8λ
	m_txDataBuffer[txLen++] = txData;
	
	memcpy(&m_txDataBuffer[txLen], pMsg->data, pMsg->len);	//�û�����
	txLen += pMsg->len;
	
	txData = GetCheckValue(pMsg);			//����У��
	m_txDataBuffer[txLen++] = txData;
	
	m_hUart[1]->write(m_txDataBuffer, txLen);
	
#ifdef USE_RT_THREAD
	rt_mutex_release(&mutex_tx);
#endif
	return TRUE;
}

#if USE_TX_MSG_FIFO
/**
 * @brief ����������FIFO
 * @param pMsg �洢���͵���Ϣ���
 * @return ��FIFO�������
 * @retval ��ȡ�ɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL readTxFIFO1(MsgTypeDef* pMsg)
{
	UINT16 i = 0;
	UINT16 index = 0;
	if (NULL == pMsg)		//�����Ƿ�
		return FALSE;
	if (m_uFIFO_ReadPriter == m_uFIFO_WritePriter)//FIFOΪ��
		return FALSE;
	++m_uFIFO_ReadPriter;
	m_uFIFO_ReadPriter = m_uFIFO_ReadPriter % MAX_MSG_FIFO_SIZE;
	
	index = m_uFIFO_ReadPriter * COMM_MSG_SISE;
	pMsg->uAddr = m_TX_FIFO_DataBuffer[1][index++];								//��ַ��8λ
	pMsg->uAddr = (pMsg->uAddr << 8) | m_TX_FIFO_DataBuffer[1][index++];			//��ַ��8λ
	pMsg->blockID = m_TX_FIFO_DataBuffer[1][index++];								//���ܵ�Ԫ��8λ
	pMsg->blockID = (pMsg->blockID << 8) | m_TX_FIFO_DataBuffer[1][index++];		//���ܵ�Ԫ��8λ
	pMsg->functionID = m_TX_FIFO_DataBuffer[1][index++];							//����ID��8λ
	pMsg->functionID = (pMsg->functionID << 8) | m_TX_FIFO_DataBuffer[1][index++];	//����ID��8λ
	pMsg->opType = m_TX_FIFO_DataBuffer[1][index++];								//������
	pMsg->len = m_TX_FIFO_DataBuffer[1][index++];									//���ݳ��ȸ�8λ
	pMsg->len = (pMsg->len << 8) | m_TX_FIFO_DataBuffer[1][index++];				//���ݳ��ȵ�8λ
	for (i = 0; i < pMsg->len; ++i)												//�û�����
	{
		pMsg->data[i] = m_TX_FIFO_DataBuffer[1][index++];
	}
	
	return TRUE;
}

/**
 * @brief д��������FIFO
 * @param pMsg ������Ϣ���
 * @return дFIFO�������
 * @retval д��ɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL writeTxFIFO1(MsgTypeDef* pMsg)
{
	UINT16 i = 0;
	UINT16 index = 0;
	if (NULL == pMsg)	//�����Ƿ�
		return FALSE;
	
	index = m_uFIFO_WritePriter * COMM_MSG_SISE;
	m_TX_FIFO_DataBuffer[1][index++] = (UCHAR)(pMsg->uAddr >> 8);					//��ַ��8λ
	m_TX_FIFO_DataBuffer[1][index++] = (UCHAR)(pMsg->uAddr);						//��ַ��8λ
	m_TX_FIFO_DataBuffer[1][index++] = (UCHAR)(pMsg->blockID >> 8);				//���ܵ�Ԫ��8λ
	m_TX_FIFO_DataBuffer[1][index++] = (UCHAR)(pMsg->blockID);						//���ܵ�Ԫ��8λ
	m_TX_FIFO_DataBuffer[1][index++] = (UCHAR)(pMsg->functionID >> 8);				//����ID��8λ
	m_TX_FIFO_DataBuffer[1][index++] = (UCHAR)(pMsg->functionID);					//����ID��8λ
	m_TX_FIFO_DataBuffer[1][index++] = pMsg->opType;								//������
	m_TX_FIFO_DataBuffer[1][index++] = (UCHAR)(pMsg->len >> 8);					//���ݳ��ȸ�8λ
	m_TX_FIFO_DataBuffer[1][index++] = (UCHAR)(pMsg->len);							//���ݳ��ȵ�8λ
	for (i = 0; i < pMsg->len; ++i)												//�û�����
	{
		m_TX_FIFO_DataBuffer[1][index++] = pMsg->data[i];
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
 * @brief COMM��Ϣ����
 * @param pMsg: ����֡����
 * @param len: ����֡��С
 * @retval  ���ͳɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL transmitByFIFO1(MsgTypeDef* pMsg)
{
	return writeTxFIFO1(pMsg);
}

/**
 * @brief FIFO���ݷ����̣߳����ڷ���FIFO�е�����
 */
#ifndef USE_RT_THREAD
static void taskForTxFiFoMsg1(void)
#else
static void taskForTxFiFoMsg1(void* arg)
#endif
{
	static MsgTypeDef TxMsg;
#ifdef USE_RT_THREAD
	while (1)
#endif
	{
		if (readTxFIFO1(&TxMsg))
		{
			transmit1(&TxMsg);
		}
	}
}
#endif

/**
 * @brief COMM��ʼ��
 * @param hUart:  Uartʵ�����
 * @param hRxFunc: COMM���ջص��������
 * @retval None
 */
static void init1(HALUartTypeDef* hUart)
{
	if (NULL == hUart)
		return;
	if (m_hUart[1] != NULL)
		m_hUart[1]->deInit();
	
	m_hUart[1] = hUart;
	m_hUart[1]->add_rx_obser(OnComm1);
	
#ifndef USE_RT_THREAD
	m_hUart[1]->add_rx_obser(OnComm1);
	osal_task_create(taskForRxdata1, 1);
#if USE_TX_MSG_FIFO
	osal_task_create(taskForTxFiFoMsg1, 1);
#endif
#else
	if(rt_thread_init(&RXDATA_Task_Handle[1], "CommEsp8266RxData", taskForRxdata1, RT_NULL,
						(unsigned char *)&RXDATA_TASK_STK[1][0], RXDATA_TASK_STK_SIZE, RXDATA_TASK_PRIO, RXDATA_TASK_TICK) == RT_EOK)
		rt_thread_startup(&RXDATA_Task_Handle[1]);
#if USE_TX_MSG_FIFO
	if(rt_thread_init(&TXFIFO_Task_Handle[1], "CommEsp8266TxFIFO", taskForTxFiFoMsg1, RT_NULL,
						(unsigned char *)&TXFIFO_TASK_STK[1][0], TXFIFO_TASK_STK_SIZE, TXFIFO_TASK_PRIO, TXFIFO_TASK_TICK) == RT_EOK)
		rt_thread_startup(&TXFIFO_Task_Handle[1]);
#endif
#endif
}

/**
 * @brief COMM��Դע��
 * @param None
 * @retval None
 */
static void deInit1(void)
{
	pthis[1] = NULL;
	if (m_hUart[1] != NULL)
	{
		m_hUart[1]->deInit();
	}
	m_hUart[1] = NULL;
#ifndef USE_RT_THREAD
	osal_task_kill(taskForRxdata1);
	
#if USE_TX_MSG_FIFO
	osal_task_kill(taskForTxFiFoMsg1);
#endif
#else
	rt_thread_detach(&RXDATA_Task_Handle[1]);
	
#if USE_TX_MSG_FIFO
	rt_thread_detach(&TXFIFO_Task_Handle[1]);
#endif
#endif

}

#endif

#ifdef WIZCHIP_NET_CH2
/**
 * @brief Uart�˿����ݽ��ջص�����
 * @param data ���յ�������
 * @return void
 */
static void OnComm2(UCHAR data)
{
	m_uMsgRecvFlag |= BIT(2);
}

/**
 * @brief ����֡�����̣߳�����������ݵĽ���
 * @param None
 * @return void
 */
#ifndef USE_RT_THREAD
static void taskForRxdata2(void)
#else
static void taskForRxdata2(void* arg)
#endif
{
	static UCHAR rxFlag = 0;
	static UINT16 uRxDataIndex = 0;
	static MsgTypeDef RxMsg;
    static UINT16 uAddr = 0;
    static UINT16 blockID = 0;
    static UINT16 funcID = 0;
    static UINT16 msgLen = 0;
	
	UCHAR rxData = 0;
#ifdef USE_RT_THREAD
	while (1)
	{
		if (m_uMsgRecvFlag & BIT(2))
			continue;
		m_uMsgRecvFlag &= ~BIT(2);
		if (rt_mutex_take(&mutex_rx, RT_WAITING_TIME) != RT_EOK)
			continue;
		m_uWritePriter = m_hUart[2]->read(m_rxDataBuffer, MAX_MSG_PHY_DATA_SIZE);
		for (m_uReadPriter=0; m_uReadPriter<m_uWritePriter;m_uReadPriter++)
		{
			rxData = m_rxDataBuffer[m_uReadPriter];
#else
	if (m_uReadPriter == m_uWritePriter)
		return;
		rxData = m_rxDataBuffer[m_uReadPriter];
		++m_uReadPriter;
		m_uReadPriter = m_uReadPriter % MAX_MSG_PHY_DATA_SIZE;
#endif
		
		switch(rxFlag)
		{
		case 0:			//��ʼ��־1
			if (PHY_DATA_FLAG1 == rxData)
			{
				++rxFlag;
			}
			break;
		case 1:			//��ʼ��־2
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
		case 2:			//��ַ��8λ
			uAddr = rxData;
			++rxFlag;
			break;
		case 3:			//��ַ��8λ
			uAddr = (uAddr << 8) | rxData;
			RxMsg.uAddr = osal_sock_ntohs(uAddr);
			++rxFlag;
			break;
		case 4:			//���ܵ�Ԫ��8λ
			blockID = rxData;
			++rxFlag;
			break;
		case 5:			//���ܵ�Ԫ��8λ
			blockID = (blockID << 8) | rxData;
			RxMsg.blockID = osal_sock_ntohs(blockID);
			++rxFlag;
			break;
		case 6:			//����ID��8λ
			funcID = rxData;
			++rxFlag;
			break;
		case 7:			//����ID��8λ
			funcID = (funcID << 8) | rxData;
			RxMsg.functionID = osal_sock_ntohs(funcID);
			++rxFlag;
			break;
		case 8:			//������
			RxMsg.opType = rxData;
			++rxFlag;
			break;
		case 9:			//���ݳ��ȸ�8λ
			msgLen = rxData;
			++rxFlag;
			break;
		case 10:		//���ݳ��ȵ�8λ
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
		case 11:		//�û�����
			if (uRxDataIndex < RxMsg.len)
			{
				RxMsg.data[uRxDataIndex++] = rxData;
			}
			else		//���ݽ������
			{
				rxFlag = 0;
				uRxDataIndex = 0;
				if (rxData == GetCheckValue(&RxMsg)) //��������У��
				{
					//DBG(TRACE("comm::GetCheckValue OK"));
					if (m_hRxObser[2] != NULL)
					{
						m_hRxObser[2](&RxMsg);
#ifdef USE_RT_THREAD
						m_uReadPriter = m_uWritePriter;
#endif
					}
				}
				else		//����У��ʧ��
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

#ifdef USE_RT_THREAD
		}//for
		rt_mutex_release(&mutex_rx);
	}
#endif
}

/**
 * @brief: ע�ᱨ�Ľ��չ۲���
 * @param hrxobser: ���Ľ��չ۲��߾��
 * @retval: void
 */
static void add_rx_obser2(CommRxCBack_t hrxobser)
{
	m_hRxObser[2] = hrxobser;
}

/**
 * @brief COMM��Ϣ����
 * @param pMsg: ����֡����
 * @param len: ����֡��С
 * @retval  ���ͳɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL transmit2(MsgTypeDef* pMsg)
{
	UCHAR txData = 0;
	UINT16 uAddr = 0;
    UINT16 blockID = 0;
	UINT16 uDataLen = 0;
    UINT16 uFuncID = 0;
	UINT16 txLen = 0;
	
	if (NULL == m_hUart[2])
		return FALSE;
	
#ifdef USE_RT_THREAD
	if (rt_mutex_take(&mutex_tx, RT_WAITING_TIME) != RT_EOK)
		return FALSE;
#endif
	txData = PHY_DATA_FLAG1;				//��ʼ��־1
	m_txDataBuffer[txLen++] = txData;
	
	txData = PHY_DATA_FLAG2;				//��ʼ��־2
	m_txDataBuffer[txLen++] = txData;
	
    uAddr = osal_sock_htons(pMsg->uAddr);
	txData = (UCHAR)(uAddr >> 8);				//��ַ��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)(uAddr);					//��ַ��8λ
	m_txDataBuffer[txLen++] = txData;
	
    blockID = osal_sock_htons(pMsg->blockID);
	txData = (UCHAR)(blockID >> 8);			//���ܵ�Ԫ��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)(blockID);			//���ܵ�Ԫ��8λ
	m_txDataBuffer[txLen++] = txData;
	
    uFuncID = osal_sock_htons(pMsg->functionID);
	txData = (UCHAR)(uFuncID >> 8);//����ID��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)(uFuncID);		//����ID��8λ
	m_txDataBuffer[txLen++] = txData;
	
	txData = pMsg->opType;					//������
	m_txDataBuffer[txLen++] = txData;
	
	uDataLen = GetCheckLenght(pMsg->len);	//���ݳ���
    uDataLen = osal_sock_htons(uDataLen);
	txData = (UCHAR)(uDataLen >> 8);		//��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)uDataLen;				//��8λ
	m_txDataBuffer[txLen++] = txData;
	
	memcpy(&m_txDataBuffer[txLen], pMsg->data, pMsg->len);	//�û�����
	txLen += pMsg->len;
	
	txData = GetCheckValue(pMsg);			//����У��
	m_txDataBuffer[txLen++] = txData;
	
	m_hUart[2]->write(m_txDataBuffer, txLen);
	
#ifdef USE_RT_THREAD
	rt_mutex_release(&mutex_tx);
#endif
	return TRUE;
}

#if USE_TX_MSG_FIFO
/**
 * @brief ����������FIFO
 * @param pMsg �洢���͵���Ϣ���
 * @return ��FIFO�������
 * @retval ��ȡ�ɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL readTxFIFO2(MsgTypeDef* pMsg)
{
	UINT16 i = 0;
	UINT16 index = 0;
	if (NULL == pMsg)		//�����Ƿ�
		return FALSE;
	if (m_uFIFO_ReadPriter == m_uFIFO_WritePriter)//FIFOΪ��
		return FALSE;
	++m_uFIFO_ReadPriter;
	m_uFIFO_ReadPriter = m_uFIFO_ReadPriter % MAX_MSG_FIFO_SIZE;
	
	index = m_uFIFO_ReadPriter * COMM_MSG_SISE;
	pMsg->uAddr = m_TX_FIFO_DataBuffer[2][index++];								//��ַ��8λ
	pMsg->uAddr = (pMsg->uAddr << 8) | m_TX_FIFO_DataBuffer[2][index++];			//��ַ��8λ
	pMsg->blockID = m_TX_FIFO_DataBuffer[2][index++];								//���ܵ�Ԫ��8λ
	pMsg->blockID = (pMsg->blockID << 8) | m_TX_FIFO_DataBuffer[2][index++];		//���ܵ�Ԫ��8λ
	pMsg->functionID = m_TX_FIFO_DataBuffer[2][index++];							//����ID��8λ
	pMsg->functionID = (pMsg->functionID << 8) | m_TX_FIFO_DataBuffer[2][index++];	//����ID��8λ
	pMsg->opType = m_TX_FIFO_DataBuffer[2][index++];								//������
	pMsg->len = m_TX_FIFO_DataBuffer[2][index++];									//���ݳ��ȸ�8λ
	pMsg->len = (pMsg->len << 8) | m_TX_FIFO_DataBuffer[2][index++];				//���ݳ��ȵ�8λ
	for (i = 0; i < pMsg->len; ++i)												//�û�����
	{
		pMsg->data[i] = m_TX_FIFO_DataBuffer[2][index++];
	}
	
	return TRUE;
}

/**
 * @brief д��������FIFO
 * @param pMsg ������Ϣ���
 * @return дFIFO�������
 * @retval д��ɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL writeTxFIFO2(MsgTypeDef* pMsg)
{
	UINT16 i = 0;
	UINT16 index = 0;
	if (NULL == pMsg)	//�����Ƿ�
		return FALSE;
	
	index = m_uFIFO_WritePriter * COMM_MSG_SISE;
	m_TX_FIFO_DataBuffer[2][index++] = (UCHAR)(pMsg->uAddr >> 8);					//��ַ��8λ
	m_TX_FIFO_DataBuffer[2][index++] = (UCHAR)(pMsg->uAddr);						//��ַ��8λ
	m_TX_FIFO_DataBuffer[2][index++] = (UCHAR)(pMsg->blockID >> 8);				//���ܵ�Ԫ��8λ
	m_TX_FIFO_DataBuffer[2][index++] = (UCHAR)(pMsg->blockID);						//���ܵ�Ԫ��8λ
	m_TX_FIFO_DataBuffer[2][index++] = (UCHAR)(pMsg->functionID >> 8);				//����ID��8λ
	m_TX_FIFO_DataBuffer[2][index++] = (UCHAR)(pMsg->functionID);					//����ID��8λ
	m_TX_FIFO_DataBuffer[2][index++] = pMsg->opType;								//������
	m_TX_FIFO_DataBuffer[2][index++] = (UCHAR)(pMsg->len >> 8);					//���ݳ��ȸ�8λ
	m_TX_FIFO_DataBuffer[2][index++] = (UCHAR)(pMsg->len);							//���ݳ��ȵ�8λ
	for (i = 0; i < pMsg->len; ++i)												//�û�����
	{
		m_TX_FIFO_DataBuffer[2][index++] = pMsg->data[i];
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
 * @brief COMM��Ϣ����
 * @param pMsg: ����֡����
 * @param len: ����֡��С
 * @retval  ���ͳɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL transmitByFIFO2(MsgTypeDef* pMsg)
{
	return writeTxFIFO2(pMsg);
}

/**
 * @brief FIFO���ݷ����̣߳����ڷ���FIFO�е�����
 */
#ifndef USE_RT_THREAD
static void taskForTxFiFoMsg2(void)
#else
static void taskForTxFiFoMsg2(void* arg)
#endif
{
	static MsgTypeDef TxMsg;
#ifdef USE_RT_THREAD
	while (1)
#endif
	{
		if (readTxFIFO2(&TxMsg))
		{
			transmit2(&TxMsg);
		}
	}
}
#endif

/**
 * @brief COMM��ʼ��
 * @param hUart:  Uartʵ�����
 * @param hRxFunc: COMM���ջص��������
 * @retval None
 */
static void init2(HALUartTypeDef* hUart)
{
	if (NULL == hUart)
		return;
	if (m_hUart[2] != NULL)
		m_hUart[2]->deInit();
	
	m_hUart[2] = hUart;
	m_hUart[2]->add_rx_obser(OnComm2);
	
#ifndef USE_RT_THREAD
	m_hUart[2]->add_rx_obser(OnComm2);
	osal_task_create(taskForRxdata2, 1);
#if USE_TX_MSG_FIFO
	osal_task_create(taskForTxFiFoMsg2, 1);
#endif
#else
	if(rt_thread_init(&RXDATA_Task_Handle[2], "CommEsp8266RxData", taskForRxdata2, RT_NULL,
						(unsigned char *)&RXDATA_TASK_STK[2][0], RXDATA_TASK_STK_SIZE, RXDATA_TASK_PRIO, RXDATA_TASK_TICK) == RT_EOK)
		rt_thread_startup(&RXDATA_Task_Handle[2]);
#if USE_TX_MSG_FIFO
	if(rt_thread_init(&TXFIFO_Task_Handle[2], "CommEsp8266TxFIFO", taskForTxFiFoMsg2, RT_NULL,
						(unsigned char *)&TXFIFO_TASK_STK[2][0], TXFIFO_TASK_STK_SIZE, TXFIFO_TASK_PRIO, TXFIFO_TASK_TICK) == RT_EOK)
		rt_thread_startup(&TXFIFO_Task_Handle[2]);
#endif
#endif
}

/**
 * @brief COMM��Դע��
 * @param None
 * @retval None
 */
static void deInit2(void)
{
	pthis[2] = NULL;
	if (m_hUart[2] != NULL)
	{
		m_hUart[2]->deInit();
	}
	m_hUart[2] = NULL;
#ifndef USE_RT_THREAD
	osal_task_kill(taskForRxdata2);
	
#if USE_TX_MSG_FIFO
	osal_task_kill(taskForTxFiFoMsg2);
#endif
#else
	rt_thread_detach(&RXDATA_Task_Handle[2]);
	
#if USE_TX_MSG_FIFO
	rt_thread_detach(&TXFIFO_Task_Handle[2]);
#endif
#endif

}

#endif

#ifdef WIZCHIP_NET_CH3
/**
 * @brief Uart�˿����ݽ��ջص�����
 * @param data ���յ�������
 * @return void
 */
static void OnComm3(UCHAR data)
{
	m_uMsgRecvFlag |= BIT(3);
}

/**
 * @brief ����֡�����̣߳�����������ݵĽ���
 * @param None
 * @return void
 */
#ifndef USE_RT_THREAD
static void taskForRxdata3(void)
#else
static void taskForRxdata3(void* arg)
#endif
{
	static UCHAR rxFlag = 0;
	static UINT16 uRxDataIndex = 0;
	static MsgTypeDef RxMsg;
    static UINT16 uAddr = 0;
    static UINT16 blockID = 0;
    static UINT16 funcID = 0;
    static UINT16 msgLen = 0;
	
	UCHAR rxData = 0;
#ifdef USE_RT_THREAD
	while (1)
	{
		if (m_uMsgRecvFlag & BIT(3))
			continue;
		m_uMsgRecvFlag &= ~BIT(3);
		if (rt_mutex_take(&mutex_rx, RT_WAITING_TIME) != RT_EOK)
			continue;
		m_uWritePriter = m_hUart[3]->read(m_rxDataBuffer, MAX_MSG_PHY_DATA_SIZE);
		for (m_uReadPriter=0; m_uReadPriter<m_uWritePriter;m_uReadPriter++)
		{
			rxData = m_rxDataBuffer[m_uReadPriter];
#else
	if (m_uReadPriter == m_uWritePriter)
		return;
		rxData = m_rxDataBuffer[m_uReadPriter];
		++m_uReadPriter;
		m_uReadPriter = m_uReadPriter % MAX_MSG_PHY_DATA_SIZE;
#endif
		
		switch(rxFlag)
		{
		case 0:			//��ʼ��־1
			if (PHY_DATA_FLAG1 == rxData)
			{
				++rxFlag;
			}
			break;
		case 1:			//��ʼ��־2
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
		case 2:			//��ַ��8λ
			uAddr = rxData;
			++rxFlag;
			break;
		case 3:			//��ַ��8λ
			uAddr = (uAddr << 8) | rxData;
			RxMsg.uAddr = osal_sock_ntohs(uAddr);
			++rxFlag;
			break;
		case 4:			//���ܵ�Ԫ��8λ
			blockID = rxData;
			++rxFlag;
			break;
		case 5:			//���ܵ�Ԫ��8λ
			blockID = (blockID << 8) | rxData;
			RxMsg.blockID = osal_sock_ntohs(blockID);
			++rxFlag;
			break;
		case 6:			//����ID��8λ
			funcID = rxData;
			++rxFlag;
			break;
		case 7:			//����ID��8λ
			funcID = (funcID << 8) | rxData;
			RxMsg.functionID = osal_sock_ntohs(funcID);
			++rxFlag;
			break;
		case 8:			//������
			RxMsg.opType = rxData;
			++rxFlag;
			break;
		case 9:			//���ݳ��ȸ�8λ
			msgLen = rxData;
			++rxFlag;
			break;
		case 10:		//���ݳ��ȵ�8λ
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
		case 11:		//�û�����
			if (uRxDataIndex < RxMsg.len)
			{
				RxMsg.data[uRxDataIndex++] = rxData;
			}
			else		//���ݽ������
			{
				rxFlag = 0;
				uRxDataIndex = 0;
				if (rxData == GetCheckValue(&RxMsg)) //��������У��
				{
					//DBG(TRACE("comm::GetCheckValue OK"));
					if (m_hRxObser[3] != NULL)
					{
						m_hRxObser[3](&RxMsg);
#ifdef USE_RT_THREAD
						m_uReadPriter = m_uWritePriter;
#endif
					}
				}
				else		//����У��ʧ��
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

#ifdef USE_RT_THREAD
		}//for
		rt_mutex_release(&mutex_rx);
	}
#endif
}

/**
 * @brief: ע�ᱨ�Ľ��չ۲���
 * @param hrxobser: ���Ľ��չ۲��߾��
 * @retval: void
 */
static void add_rx_obser3(CommRxCBack_t hrxobser)
{
	m_hRxObser[3] = hrxobser;
}

/**
 * @brief COMM��Ϣ����
 * @param pMsg: ����֡����
 * @param len: ����֡��С
 * @retval  ���ͳɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL transmit3(MsgTypeDef* pMsg)
{
	UCHAR txData = 0;
	UINT16 uAddr = 0;
    UINT16 blockID = 0;
	UINT16 uDataLen = 0;
    UINT16 uFuncID = 0;
	UINT16 txLen = 0;
	
	if (NULL == m_hUart[3])
		return FALSE;
	
#ifdef USE_RT_THREAD
	if (rt_mutex_take(&mutex_tx, RT_WAITING_TIME) != RT_EOK)
		return FALSE;
#endif
	txData = PHY_DATA_FLAG1;				//��ʼ��־1
	m_txDataBuffer[txLen++] = txData;
	
	txData = PHY_DATA_FLAG2;				//��ʼ��־2
	m_txDataBuffer[txLen++] = txData;
	
    uAddr = osal_sock_htons(pMsg->uAddr);
	txData = (UCHAR)(uAddr >> 8);				//��ַ��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)(uAddr);					//��ַ��8λ
	m_txDataBuffer[txLen++] = txData;
	
    blockID = osal_sock_htons(pMsg->blockID);
	txData = (UCHAR)(blockID >> 8);			//���ܵ�Ԫ��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)(blockID);			//���ܵ�Ԫ��8λ
	m_txDataBuffer[txLen++] = txData;
	
    uFuncID = osal_sock_htons(pMsg->functionID);
	txData = (UCHAR)(uFuncID >> 8);//����ID��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)(uFuncID);		//����ID��8λ
	m_txDataBuffer[txLen++] = txData;
	
	txData = pMsg->opType;					//������
	m_txDataBuffer[txLen++] = txData;
	
	uDataLen = GetCheckLenght(pMsg->len);	//���ݳ���
    uDataLen = osal_sock_htons(uDataLen);
	txData = (UCHAR)(uDataLen >> 8);		//��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)uDataLen;				//��8λ
	m_txDataBuffer[txLen++] = txData;
	
	memcpy(&m_txDataBuffer[txLen], pMsg->data, pMsg->len);	//�û�����
	txLen += pMsg->len;
	
	txData = GetCheckValue(pMsg);			//����У��
	m_txDataBuffer[txLen++] = txData;
	
	m_hUart[3]->write(m_txDataBuffer, txLen);
	
#ifdef USE_RT_THREAD
	rt_mutex_release(&mutex_tx);
#endif
	return TRUE;
}

#if USE_TX_MSG_FIFO
/**
 * @brief ����������FIFO
 * @param pMsg �洢���͵���Ϣ���
 * @return ��FIFO�������
 * @retval ��ȡ�ɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL readTxFIFO3(MsgTypeDef* pMsg)
{
	UINT16 i = 0;
	UINT16 index = 0;
	if (NULL == pMsg)		//�����Ƿ�
		return FALSE;
	if (m_uFIFO_ReadPriter == m_uFIFO_WritePriter)//FIFOΪ��
		return FALSE;
	++m_uFIFO_ReadPriter;
	m_uFIFO_ReadPriter = m_uFIFO_ReadPriter % MAX_MSG_FIFO_SIZE;
	
	index = m_uFIFO_ReadPriter * COMM_MSG_SISE;
	pMsg->uAddr = m_TX_FIFO_DataBuffer[3][index++];								//��ַ��8λ
	pMsg->uAddr = (pMsg->uAddr << 8) | m_TX_FIFO_DataBuffer[3][index++];			//��ַ��8λ
	pMsg->blockID = m_TX_FIFO_DataBuffer[3][index++];								//���ܵ�Ԫ��8λ
	pMsg->blockID = (pMsg->blockID << 8) | m_TX_FIFO_DataBuffer[3][index++];		//���ܵ�Ԫ��8λ
	pMsg->functionID = m_TX_FIFO_DataBuffer[3][index++];							//����ID��8λ
	pMsg->functionID = (pMsg->functionID << 8) | m_TX_FIFO_DataBuffer[3][index++];	//����ID��8λ
	pMsg->opType = m_TX_FIFO_DataBuffer[3][index++];								//������
	pMsg->len = m_TX_FIFO_DataBuffer[3][index++];									//���ݳ��ȸ�8λ
	pMsg->len = (pMsg->len << 8) | m_TX_FIFO_DataBuffer[3][index++];				//���ݳ��ȵ�8λ
	for (i = 0; i < pMsg->len; ++i)												//�û�����
	{
		pMsg->data[i] = m_TX_FIFO_DataBuffer[3][index++];
	}
	
	return TRUE;
}

/**
 * @brief д��������FIFO
 * @param pMsg ������Ϣ���
 * @return дFIFO�������
 * @retval д��ɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL writeTxFIFO3(MsgTypeDef* pMsg)
{
	UINT16 i = 0;
	UINT16 index = 0;
	if (NULL == pMsg)	//�����Ƿ�
		return FALSE;
	
	index = m_uFIFO_WritePriter * COMM_MSG_SISE;
	m_TX_FIFO_DataBuffer[3][index++] = (UCHAR)(pMsg->uAddr >> 8);					//��ַ��8λ
	m_TX_FIFO_DataBuffer[3][index++] = (UCHAR)(pMsg->uAddr);						//��ַ��8λ
	m_TX_FIFO_DataBuffer[3][index++] = (UCHAR)(pMsg->blockID >> 8);				//���ܵ�Ԫ��8λ
	m_TX_FIFO_DataBuffer[3][index++] = (UCHAR)(pMsg->blockID);						//���ܵ�Ԫ��8λ
	m_TX_FIFO_DataBuffer[3][index++] = (UCHAR)(pMsg->functionID >> 8);				//����ID��8λ
	m_TX_FIFO_DataBuffer[3][index++] = (UCHAR)(pMsg->functionID);					//����ID��8λ
	m_TX_FIFO_DataBuffer[3][index++] = pMsg->opType;								//������
	m_TX_FIFO_DataBuffer[3][index++] = (UCHAR)(pMsg->len >> 8);					//���ݳ��ȸ�8λ
	m_TX_FIFO_DataBuffer[3][index++] = (UCHAR)(pMsg->len);							//���ݳ��ȵ�8λ
	for (i = 0; i < pMsg->len; ++i)												//�û�����
	{
		m_TX_FIFO_DataBuffer[3][index++] = pMsg->data[i];
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
 * @brief COMM��Ϣ����
 * @param pMsg: ����֡����
 * @param len: ����֡��С
 * @retval  ���ͳɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL transmitByFIFO3(MsgTypeDef* pMsg)
{
	return writeTxFIFO3(pMsg);
}

/**
 * @brief FIFO���ݷ����̣߳����ڷ���FIFO�е�����
 */
#ifndef USE_RT_THREAD
static void taskForTxFiFoMsg3(void)
#else
static void taskForTxFiFoMsg3(void* arg)
#endif
{
	static MsgTypeDef TxMsg;
#ifdef USE_RT_THREAD
	while (1)
#endif
	{
		if (readTxFIFO3(&TxMsg))
		{
			transmit3(&TxMsg);
		}
	}
}
#endif

/**
 * @brief COMM��ʼ��
 * @param hUart:  Uartʵ�����
 * @param hRxFunc: COMM���ջص��������
 * @retval None
 */
static void init3(HALUartTypeDef* hUart)
{
	if (NULL == hUart)
		return;
	if (m_hUart[3] != NULL)
		m_hUart[3]->deInit();
	
	m_hUart[3] = hUart;
	m_hUart[3]->add_rx_obser(OnComm3);
	
#ifndef USE_RT_THREAD
	m_hUart[3]->add_rx_obser(OnComm3);
	osal_task_create(taskForRxdata3, 1);
#if USE_TX_MSG_FIFO
	osal_task_create(taskForTxFiFoMsg3, 1);
#endif
#else
	if(rt_thread_init(&RXDATA_Task_Handle[3], "CommEsp8266RxData", taskForRxdata3, RT_NULL,
						(unsigned char *)&RXDATA_TASK_STK[3][0], RXDATA_TASK_STK_SIZE, RXDATA_TASK_PRIO, RXDATA_TASK_TICK) == RT_EOK)
		rt_thread_startup(&RXDATA_Task_Handle[3]);
#if USE_TX_MSG_FIFO
	if(rt_thread_init(&TXFIFO_Task_Handle[3], "CommEsp8266TxFIFO", taskForTxFiFoMsg3, RT_NULL,
						(unsigned char *)&TXFIFO_TASK_STK[3][0], TXFIFO_TASK_STK_SIZE, TXFIFO_TASK_PRIO, TXFIFO_TASK_TICK) == RT_EOK)
		rt_thread_startup(&TXFIFO_Task_Handle[3]);
#endif
#endif
}

/**
 * @brief COMM��Դע��
 * @param None
 * @retval None
 */
static void deInit3(void)
{
	pthis[3] = NULL;
	if (m_hUart[3] != NULL)
	{
		m_hUart[3]->deInit();
	}
	m_hUart[3] = NULL;
#ifndef USE_RT_THREAD
	osal_task_kill(taskForRxdata3);
	
#if USE_TX_MSG_FIFO
	osal_task_kill(taskForTxFiFoMsg3);
#endif
#else
	rt_thread_detach(&RXDATA_Task_Handle[3]);
	
#if USE_TX_MSG_FIFO
	rt_thread_detach(&TXFIFO_Task_Handle[3]);
#endif
#endif

}

#endif

#if _WIZCHIP_ > 5100
#ifdef WIZCHIP_NET_CH4
/**
 * @brief Uart�˿����ݽ��ջص�����
 * @param data ���յ�������
 * @return void
 */
static void OnComm4(UCHAR data)
{
	m_uMsgRecvFlag |= BIT(4);
}

/**
 * @brief ����֡�����̣߳�����������ݵĽ���
 * @param None
 * @return void
 */
#ifndef USE_RT_THREAD
static void taskForRxdata4(void)
#else
static void taskForRxdata4(void* arg)
#endif
{
	static UCHAR rxFlag = 0;
	static UINT16 uRxDataIndex = 0;
	static MsgTypeDef RxMsg;
    static UINT16 uAddr = 0;
    static UINT16 blockID = 0;
    static UINT16 funcID = 0;
    static UINT16 msgLen = 0;
	
	UCHAR rxData = 0;
#ifdef USE_RT_THREAD
	while (1)
	{
		if (m_uMsgRecvFlag & BIT(4))
			continue;
		m_uMsgRecvFlag &= ~BIT(4);
		if (rt_mutex_take(&mutex_rx, RT_WAITING_TIME) != RT_EOK)
			continue;
		m_uWritePriter = m_hUart[4]->read(m_rxDataBuffer, MAX_MSG_PHY_DATA_SIZE);
		for (m_uReadPriter=0; m_uReadPriter<m_uWritePriter;m_uReadPriter++)
		{
			rxData = m_rxDataBuffer[m_uReadPriter];
#else
	if (m_uReadPriter == m_uWritePriter)
		return;
		rxData = m_rxDataBuffer[m_uReadPriter];
		++m_uReadPriter;
		m_uReadPriter = m_uReadPriter % MAX_MSG_PHY_DATA_SIZE;
#endif
		
		switch(rxFlag)
		{
		case 0:			//��ʼ��־1
			if (PHY_DATA_FLAG1 == rxData)
			{
				++rxFlag;
			}
			break;
		case 1:			//��ʼ��־2
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
		case 2:			//��ַ��8λ
			uAddr = rxData;
			++rxFlag;
			break;
		case 3:			//��ַ��8λ
			uAddr = (uAddr << 8) | rxData;
			RxMsg.uAddr = osal_sock_ntohs(uAddr);
			++rxFlag;
			break;
		case 4:			//���ܵ�Ԫ��8λ
			blockID = rxData;
			++rxFlag;
			break;
		case 5:			//���ܵ�Ԫ��8λ
			blockID = (blockID << 8) | rxData;
			RxMsg.blockID = osal_sock_ntohs(blockID);
			++rxFlag;
			break;
		case 6:			//����ID��8λ
			funcID = rxData;
			++rxFlag;
			break;
		case 7:			//����ID��8λ
			funcID = (funcID << 8) | rxData;
			RxMsg.functionID = osal_sock_ntohs(funcID);
			++rxFlag;
			break;
		case 8:			//������
			RxMsg.opType = rxData;
			++rxFlag;
			break;
		case 9:			//���ݳ��ȸ�8λ
			msgLen = rxData;
			++rxFlag;
			break;
		case 10:		//���ݳ��ȵ�8λ
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
		case 11:		//�û�����
			if (uRxDataIndex < RxMsg.len)
			{
				RxMsg.data[uRxDataIndex++] = rxData;
			}
			else		//���ݽ������
			{
				rxFlag = 0;
				uRxDataIndex = 0;
				if (rxData == GetCheckValue(&RxMsg)) //��������У��
				{
					//DBG(TRACE("comm::GetCheckValue OK"));
					if (m_hRxObser[4] != NULL)
					{
						m_hRxObser[4](&RxMsg);
#ifdef USE_RT_THREAD
						m_uReadPriter = m_uWritePriter;
#endif
					}
				}
				else		//����У��ʧ��
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

#ifdef USE_RT_THREAD
		}//for
		rt_mutex_release(&mutex_rx);
	}
#endif
}

/**
 * @brief: ע�ᱨ�Ľ��չ۲���
 * @param hrxobser: ���Ľ��չ۲��߾��
 * @retval: void
 */
static void add_rx_obser4(CommRxCBack_t hrxobser)
{
	m_hRxObser[4] = hrxobser;
}

/**
 * @brief COMM��Ϣ����
 * @param pMsg: ����֡����
 * @param len: ����֡��С
 * @retval  ���ͳɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL transmit4(MsgTypeDef* pMsg)
{
	UCHAR txData = 0;
	UINT16 uAddr = 0;
    UINT16 blockID = 0;
	UINT16 uDataLen = 0;
    UINT16 uFuncID = 0;
	UINT16 txLen = 0;
	
	if (NULL == m_hUart[4])
		return FALSE;
	
#ifdef USE_RT_THREAD
	if (rt_mutex_take(&mutex_tx, RT_WAITING_TIME) != RT_EOK)
		return FALSE;
#endif
	txData = PHY_DATA_FLAG1;				//��ʼ��־1
	m_txDataBuffer[txLen++] = txData;
	
	txData = PHY_DATA_FLAG2;				//��ʼ��־2
	m_txDataBuffer[txLen++] = txData;
	
    uAddr = osal_sock_htons(pMsg->uAddr);
	txData = (UCHAR)(uAddr >> 8);				//��ַ��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)(uAddr);					//��ַ��8λ
	m_txDataBuffer[txLen++] = txData;
	
    blockID = osal_sock_htons(pMsg->blockID);
	txData = (UCHAR)(blockID >> 8);			//���ܵ�Ԫ��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)(blockID);			//���ܵ�Ԫ��8λ
	m_txDataBuffer[txLen++] = txData;
	
    uFuncID = osal_sock_htons(pMsg->functionID);
	txData = (UCHAR)(uFuncID >> 8);//����ID��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)(uFuncID);		//����ID��8λ
	m_txDataBuffer[txLen++] = txData;
	
	txData = pMsg->opType;					//������
	m_txDataBuffer[txLen++] = txData;
	
	uDataLen = GetCheckLenght(pMsg->len);	//���ݳ���
    uDataLen = osal_sock_htons(uDataLen);
	txData = (UCHAR)(uDataLen >> 8);		//��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)uDataLen;				//��8λ
	m_txDataBuffer[txLen++] = txData;
	
	memcpy(&m_txDataBuffer[txLen], pMsg->data, pMsg->len);	//�û�����
	txLen += pMsg->len;
	
	txData = GetCheckValue(pMsg);			//����У��
	m_txDataBuffer[txLen++] = txData;
	
	m_hUart[4]->write(m_txDataBuffer, txLen);
	
#ifdef USE_RT_THREAD
	rt_mutex_release(&mutex_tx);
#endif
	return TRUE;
}

#if USE_TX_MSG_FIFO
/**
 * @brief ����������FIFO
 * @param pMsg �洢���͵���Ϣ���
 * @return ��FIFO�������
 * @retval ��ȡ�ɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL readTxFIFO4(MsgTypeDef* pMsg)
{
	UINT16 i = 0;
	UINT16 index = 0;
	if (NULL == pMsg)		//�����Ƿ�
		return FALSE;
	if (m_uFIFO_ReadPriter == m_uFIFO_WritePriter)//FIFOΪ��
		return FALSE;
	++m_uFIFO_ReadPriter;
	m_uFIFO_ReadPriter = m_uFIFO_ReadPriter % MAX_MSG_FIFO_SIZE;
	
	index = m_uFIFO_ReadPriter * COMM_MSG_SISE;
	pMsg->uAddr = m_TX_FIFO_DataBuffer[4][index++];								//��ַ��8λ
	pMsg->uAddr = (pMsg->uAddr << 8) | m_TX_FIFO_DataBuffer[4][index++];			//��ַ��8λ
	pMsg->blockID = m_TX_FIFO_DataBuffer[4][index++];								//���ܵ�Ԫ��8λ
	pMsg->blockID = (pMsg->blockID << 8) | m_TX_FIFO_DataBuffer[4][index++];		//���ܵ�Ԫ��8λ
	pMsg->functionID = m_TX_FIFO_DataBuffer[4][index++];							//����ID��8λ
	pMsg->functionID = (pMsg->functionID << 8) | m_TX_FIFO_DataBuffer[4][index++];	//����ID��8λ
	pMsg->opType = m_TX_FIFO_DataBuffer[4][index++];								//������
	pMsg->len = m_TX_FIFO_DataBuffer[4][index++];									//���ݳ��ȸ�8λ
	pMsg->len = (pMsg->len << 8) | m_TX_FIFO_DataBuffer[4][index++];				//���ݳ��ȵ�8λ
	for (i = 0; i < pMsg->len; ++i)												//�û�����
	{
		pMsg->data[i] = m_TX_FIFO_DataBuffer[4][index++];
	}
	
	return TRUE;
}

/**
 * @brief д��������FIFO
 * @param pMsg ������Ϣ���
 * @return дFIFO�������
 * @retval д��ɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL writeTxFIFO4(MsgTypeDef* pMsg)
{
	UINT16 i = 0;
	UINT16 index = 0;
	if (NULL == pMsg)	//�����Ƿ�
		return FALSE;
	
	index = m_uFIFO_WritePriter * COMM_MSG_SISE;
	m_TX_FIFO_DataBuffer[4][index++] = (UCHAR)(pMsg->uAddr >> 8);					//��ַ��8λ
	m_TX_FIFO_DataBuffer[4][index++] = (UCHAR)(pMsg->uAddr);						//��ַ��8λ
	m_TX_FIFO_DataBuffer[4][index++] = (UCHAR)(pMsg->blockID >> 8);				//���ܵ�Ԫ��8λ
	m_TX_FIFO_DataBuffer[4][index++] = (UCHAR)(pMsg->blockID);						//���ܵ�Ԫ��8λ
	m_TX_FIFO_DataBuffer[4][index++] = (UCHAR)(pMsg->functionID >> 8);				//����ID��8λ
	m_TX_FIFO_DataBuffer[4][index++] = (UCHAR)(pMsg->functionID);					//����ID��8λ
	m_TX_FIFO_DataBuffer[4][index++] = pMsg->opType;								//������
	m_TX_FIFO_DataBuffer[4][index++] = (UCHAR)(pMsg->len >> 8);					//���ݳ��ȸ�8λ
	m_TX_FIFO_DataBuffer[4][index++] = (UCHAR)(pMsg->len);							//���ݳ��ȵ�8λ
	for (i = 0; i < pMsg->len; ++i)												//�û�����
	{
		m_TX_FIFO_DataBuffer[4][index++] = pMsg->data[i];
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
 * @brief COMM��Ϣ����
 * @param pMsg: ����֡����
 * @param len: ����֡��С
 * @retval  ���ͳɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL transmitByFIFO4(MsgTypeDef* pMsg)
{
	return writeTxFIFO4(pMsg);
}

/**
 * @brief FIFO���ݷ����̣߳����ڷ���FIFO�е�����
 */
#ifndef USE_RT_THREAD
static void taskForTxFiFoMsg4(void)
#else
static void taskForTxFiFoMsg4(void* arg)
#endif
{
	static MsgTypeDef TxMsg;
#ifdef USE_RT_THREAD
	while (1)
#endif
	{
		if (readTxFIFO4(&TxMsg))
		{
			transmit4(&TxMsg);
		}
	}
}
#endif

/**
 * @brief COMM��ʼ��
 * @param hUart:  Uartʵ�����
 * @param hRxFunc: COMM���ջص��������
 * @retval None
 */
static void init4(HALUartTypeDef* hUart)
{
	if (NULL == hUart)
		return;
	if (m_hUart[4] != NULL)
		m_hUart[4]->deInit();
	
	m_hUart[4] = hUart;
	m_hUart[4]->add_rx_obser(OnComm4);
	
#ifndef USE_RT_THREAD
	m_hUart[4]->add_rx_obser(OnComm4);
	osal_task_create(taskForRxdata4, 1);
#if USE_TX_MSG_FIFO
	osal_task_create(taskForTxFiFoMsg4, 1);
#endif
#else
	if(rt_thread_init(&RXDATA_Task_Handle[4], "CommEsp8266RxData", taskForRxdata4, RT_NULL,
						(unsigned char *)&RXDATA_TASK_STK[4][0], RXDATA_TASK_STK_SIZE, RXDATA_TASK_PRIO, RXDATA_TASK_TICK) == RT_EOK)
		rt_thread_startup(&RXDATA_Task_Handle[4]);
#if USE_TX_MSG_FIFO
	if(rt_thread_init(&TXFIFO_Task_Handle[4], "CommEsp8266TxFIFO", taskForTxFiFoMsg4, RT_NULL,
						(unsigned char *)&TXFIFO_TASK_STK[4][0], TXFIFO_TASK_STK_SIZE, TXFIFO_TASK_PRIO, TXFIFO_TASK_TICK) == RT_EOK)
		rt_thread_startup(&TXFIFO_Task_Handle[4]);
#endif
#endif
}

/**
 * @brief COMM��Դע��
 * @param None
 * @retval None
 */
static void deInit4(void)
{
	pthis[4] = NULL;
	if (m_hUart[4] != NULL)
	{
		m_hUart[4]->deInit();
	}
	m_hUart[4] = NULL;
#ifndef USE_RT_THREAD
	osal_task_kill(taskForRxdata4);
	
#if USE_TX_MSG_FIFO
	osal_task_kill(taskForTxFiFoMsg4);
#endif
#else
	rt_thread_detach(&RXDATA_Task_Handle[4]);
	
#if USE_TX_MSG_FIFO
	rt_thread_detach(&TXFIFO_Task_Handle[4]);
#endif
#endif

}

#endif

#ifdef WIZCHIP_NET_CH5
/**
 * @brief Uart�˿����ݽ��ջص�����
 * @param data ���յ�������
 * @return void
 */
static void OnComm5(UCHAR data)
{
	m_uMsgRecvFlag |= BIT(5);
}

/**
 * @brief ����֡�����̣߳�����������ݵĽ���
 * @param None
 * @return void
 */
#ifndef USE_RT_THREAD
static void taskForRxdata5(void)
#else
static void taskForRxdata5(void* arg)
#endif
{
	static UCHAR rxFlag = 0;
	static UINT16 uRxDataIndex = 0;
	static MsgTypeDef RxMsg;
    static UINT16 uAddr = 0;
    static UINT16 blockID = 0;
    static UINT16 funcID = 0;
    static UINT16 msgLen = 0;
	
	UCHAR rxData = 0;
#ifdef USE_RT_THREAD
	while (1)
	{
		if (m_uMsgRecvFlag & BIT(5))
			continue;
		m_uMsgRecvFlag &= ~BIT(5);
		if (rt_mutex_take(&mutex_rx, RT_WAITING_TIME) != RT_EOK)
			continue;
		m_uWritePriter = m_hUart[5]->read(m_rxDataBuffer, MAX_MSG_PHY_DATA_SIZE);
		for (m_uReadPriter=0; m_uReadPriter<m_uWritePriter;m_uReadPriter++)
		{
			rxData = m_rxDataBuffer[m_uReadPriter];
#else
	if (m_uReadPriter == m_uWritePriter)
		return;
		rxData = m_rxDataBuffer[m_uReadPriter];
		++m_uReadPriter;
		m_uReadPriter = m_uReadPriter % MAX_MSG_PHY_DATA_SIZE;
#endif
		
		switch(rxFlag)
		{
		case 0:			//��ʼ��־1
			if (PHY_DATA_FLAG1 == rxData)
			{
				++rxFlag;
			}
			break;
		case 1:			//��ʼ��־2
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
		case 2:			//��ַ��8λ
			uAddr = rxData;
			++rxFlag;
			break;
		case 3:			//��ַ��8λ
			uAddr = (uAddr << 8) | rxData;
			RxMsg.uAddr = osal_sock_ntohs(uAddr);
			++rxFlag;
			break;
		case 4:			//���ܵ�Ԫ��8λ
			blockID = rxData;
			++rxFlag;
			break;
		case 5:			//���ܵ�Ԫ��8λ
			blockID = (blockID << 8) | rxData;
			RxMsg.blockID = osal_sock_ntohs(blockID);
			++rxFlag;
			break;
		case 6:			//����ID��8λ
			funcID = rxData;
			++rxFlag;
			break;
		case 7:			//����ID��8λ
			funcID = (funcID << 8) | rxData;
			RxMsg.functionID = osal_sock_ntohs(funcID);
			++rxFlag;
			break;
		case 8:			//������
			RxMsg.opType = rxData;
			++rxFlag;
			break;
		case 9:			//���ݳ��ȸ�8λ
			msgLen = rxData;
			++rxFlag;
			break;
		case 10:		//���ݳ��ȵ�8λ
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
		case 11:		//�û�����
			if (uRxDataIndex < RxMsg.len)
			{
				RxMsg.data[uRxDataIndex++] = rxData;
			}
			else		//���ݽ������
			{
				rxFlag = 0;
				uRxDataIndex = 0;
				if (rxData == GetCheckValue(&RxMsg)) //��������У��
				{
					//DBG(TRACE("comm::GetCheckValue OK"));
					if (m_hRxObser[5] != NULL)
					{
						m_hRxObser[5](&RxMsg);
#ifdef USE_RT_THREAD
						m_uReadPriter = m_uWritePriter;
#endif
					}
				}
				else		//����У��ʧ��
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

#ifdef USE_RT_THREAD
		}//for
		rt_mutex_release(&mutex_rx);
	}
#endif
}

/**
 * @brief: ע�ᱨ�Ľ��չ۲���
 * @param hrxobser: ���Ľ��չ۲��߾��
 * @retval: void
 */
static void add_rx_obser5(CommRxCBack_t hrxobser)
{
	m_hRxObser[5] = hrxobser;
}

/**
 * @brief COMM��Ϣ����
 * @param pMsg: ����֡����
 * @param len: ����֡��С
 * @retval  ���ͳɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL transmit5(MsgTypeDef* pMsg)
{
	UCHAR txData = 0;
	UINT16 uAddr = 0;
    UINT16 blockID = 0;
	UINT16 uDataLen = 0;
    UINT16 uFuncID = 0;
	UINT16 txLen = 0;
	
	if (NULL == m_hUart[5])
		return FALSE;
	
#ifdef USE_RT_THREAD
	if (rt_mutex_take(&mutex_tx, RT_WAITING_TIME) != RT_EOK)
		return FALSE;
#endif
	txData = PHY_DATA_FLAG1;				//��ʼ��־1
	m_txDataBuffer[txLen++] = txData;
	
	txData = PHY_DATA_FLAG2;				//��ʼ��־2
	m_txDataBuffer[txLen++] = txData;
	
    uAddr = osal_sock_htons(pMsg->uAddr);
	txData = (UCHAR)(uAddr >> 8);				//��ַ��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)(uAddr);					//��ַ��8λ
	m_txDataBuffer[txLen++] = txData;
	
    blockID = osal_sock_htons(pMsg->blockID);
	txData = (UCHAR)(blockID >> 8);			//���ܵ�Ԫ��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)(blockID);			//���ܵ�Ԫ��8λ
	m_txDataBuffer[txLen++] = txData;
	
    uFuncID = osal_sock_htons(pMsg->functionID);
	txData = (UCHAR)(uFuncID >> 8);//����ID��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)(uFuncID);		//����ID��8λ
	m_txDataBuffer[txLen++] = txData;
	
	txData = pMsg->opType;					//������
	m_txDataBuffer[txLen++] = txData;
	
	uDataLen = GetCheckLenght(pMsg->len);	//���ݳ���
    uDataLen = osal_sock_htons(uDataLen);
	txData = (UCHAR)(uDataLen >> 8);		//��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)uDataLen;				//��8λ
	m_txDataBuffer[txLen++] = txData;
	
	memcpy(&m_txDataBuffer[txLen], pMsg->data, pMsg->len);	//�û�����
	txLen += pMsg->len;
	
	txData = GetCheckValue(pMsg);			//����У��
	m_txDataBuffer[txLen++] = txData;
	
	m_hUart[5]->write(m_txDataBuffer, txLen);
	
#ifdef USE_RT_THREAD
	rt_mutex_release(&mutex_tx);
#endif
	return TRUE;
}

#if USE_TX_MSG_FIFO
/**
 * @brief ����������FIFO
 * @param pMsg �洢���͵���Ϣ���
 * @return ��FIFO�������
 * @retval ��ȡ�ɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL readTxFIFO5(MsgTypeDef* pMsg)
{
	UINT16 i = 0;
	UINT16 index = 0;
	if (NULL == pMsg)		//�����Ƿ�
		return FALSE;
	if (m_uFIFO_ReadPriter == m_uFIFO_WritePriter)//FIFOΪ��
		return FALSE;
	++m_uFIFO_ReadPriter;
	m_uFIFO_ReadPriter = m_uFIFO_ReadPriter % MAX_MSG_FIFO_SIZE;
	
	index = m_uFIFO_ReadPriter * COMM_MSG_SISE;
	pMsg->uAddr = m_TX_FIFO_DataBuffer[5][index++];								//��ַ��8λ
	pMsg->uAddr = (pMsg->uAddr << 8) | m_TX_FIFO_DataBuffer[5][index++];			//��ַ��8λ
	pMsg->blockID = m_TX_FIFO_DataBuffer[5][index++];								//���ܵ�Ԫ��8λ
	pMsg->blockID = (pMsg->blockID << 8) | m_TX_FIFO_DataBuffer[5][index++];		//���ܵ�Ԫ��8λ
	pMsg->functionID = m_TX_FIFO_DataBuffer[5][index++];							//����ID��8λ
	pMsg->functionID = (pMsg->functionID << 8) | m_TX_FIFO_DataBuffer[5][index++];	//����ID��8λ
	pMsg->opType = m_TX_FIFO_DataBuffer[5][index++];								//������
	pMsg->len = m_TX_FIFO_DataBuffer[5][index++];									//���ݳ��ȸ�8λ
	pMsg->len = (pMsg->len << 8) | m_TX_FIFO_DataBuffer[5][index++];				//���ݳ��ȵ�8λ
	for (i = 0; i < pMsg->len; ++i)												//�û�����
	{
		pMsg->data[i] = m_TX_FIFO_DataBuffer[5][index++];
	}
	
	return TRUE;
}

/**
 * @brief д��������FIFO
 * @param pMsg ������Ϣ���
 * @return дFIFO�������
 * @retval д��ɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL writeTxFIFO5(MsgTypeDef* pMsg)
{
	UINT16 i = 0;
	UINT16 index = 0;
	if (NULL == pMsg)	//�����Ƿ�
		return FALSE;
	
	index = m_uFIFO_WritePriter * COMM_MSG_SISE;
	m_TX_FIFO_DataBuffer[5][index++] = (UCHAR)(pMsg->uAddr >> 8);					//��ַ��8λ
	m_TX_FIFO_DataBuffer[5][index++] = (UCHAR)(pMsg->uAddr);						//��ַ��8λ
	m_TX_FIFO_DataBuffer[5][index++] = (UCHAR)(pMsg->blockID >> 8);				//���ܵ�Ԫ��8λ
	m_TX_FIFO_DataBuffer[5][index++] = (UCHAR)(pMsg->blockID);						//���ܵ�Ԫ��8λ
	m_TX_FIFO_DataBuffer[5][index++] = (UCHAR)(pMsg->functionID >> 8);				//����ID��8λ
	m_TX_FIFO_DataBuffer[5][index++] = (UCHAR)(pMsg->functionID);					//����ID��8λ
	m_TX_FIFO_DataBuffer[5][index++] = pMsg->opType;								//������
	m_TX_FIFO_DataBuffer[5][index++] = (UCHAR)(pMsg->len >> 8);					//���ݳ��ȸ�8λ
	m_TX_FIFO_DataBuffer[5][index++] = (UCHAR)(pMsg->len);							//���ݳ��ȵ�8λ
	for (i = 0; i < pMsg->len; ++i)												//�û�����
	{
		m_TX_FIFO_DataBuffer[5][index++] = pMsg->data[i];
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
 * @brief COMM��Ϣ����
 * @param pMsg: ����֡����
 * @param len: ����֡��С
 * @retval  ���ͳɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL transmitByFIFO5(MsgTypeDef* pMsg)
{
	return writeTxFIFO5(pMsg);
}

/**
 * @brief FIFO���ݷ����̣߳����ڷ���FIFO�е�����
 */
#ifndef USE_RT_THREAD
static void taskForTxFiFoMsg5(void)
#else
static void taskForTxFiFoMsg5(void* arg)
#endif
{
	static MsgTypeDef TxMsg;
#ifdef USE_RT_THREAD
	while (1)
#endif
	{
		if (readTxFIFO5(&TxMsg))
		{
			transmit5(&TxMsg);
		}
	}
}
#endif

/**
 * @brief COMM��ʼ��
 * @param hUart:  Uartʵ�����
 * @param hRxFunc: COMM���ջص��������
 * @retval None
 */
static void init5(HALUartTypeDef* hUart)
{
	if (NULL == hUart)
		return;
	if (m_hUart[5] != NULL)
		m_hUart[5]->deInit();
	
	m_hUart[5] = hUart;
	m_hUart[5]->add_rx_obser(OnComm5);
	
#ifndef USE_RT_THREAD
	m_hUart[5]->add_rx_obser(OnComm5);
	osal_task_create(taskForRxdata5, 1);
#if USE_TX_MSG_FIFO
	osal_task_create(taskForTxFiFoMsg5, 1);
#endif
#else
	if(rt_thread_init(&RXDATA_Task_Handle[5], "CommEsp8266RxData", taskForRxdata5, RT_NULL,
						(unsigned char *)&RXDATA_TASK_STK[5][0], RXDATA_TASK_STK_SIZE, RXDATA_TASK_PRIO, RXDATA_TASK_TICK) == RT_EOK)
		rt_thread_startup(&RXDATA_Task_Handle[5]);
#if USE_TX_MSG_FIFO
	if(rt_thread_init(&TXFIFO_Task_Handle[5], "CommEsp8266TxFIFO", taskForTxFiFoMsg5, RT_NULL,
						(unsigned char *)&TXFIFO_TASK_STK[5][0], TXFIFO_TASK_STK_SIZE, TXFIFO_TASK_PRIO, TXFIFO_TASK_TICK) == RT_EOK)
		rt_thread_startup(&TXFIFO_Task_Handle[5]);
#endif
#endif
}

/**
 * @brief COMM��Դע��
 * @param None
 * @retval None
 */
static void deInit5(void)
{
	pthis[5] = NULL;
	if (m_hUart[5] != NULL)
	{
		m_hUart[5]->deInit();
	}
	m_hUart[5] = NULL;
#ifndef USE_RT_THREAD
	osal_task_kill(taskForRxdata5);
	
#if USE_TX_MSG_FIFO
	osal_task_kill(taskForTxFiFoMsg5);
#endif
#else
	rt_thread_detach(&RXDATA_Task_Handle[5]);
	
#if USE_TX_MSG_FIFO
	rt_thread_detach(&TXFIFO_Task_Handle[5]);
#endif
#endif

}

#endif

#ifdef WIZCHIP_NET_CH6
/**
 * @brief Uart�˿����ݽ��ջص�����
 * @param data ���յ�������
 * @return void
 */
static void OnComm6(UCHAR data)
{
	m_uMsgRecvFlag |= BIT(6);
}

/**
 * @brief ����֡�����̣߳�����������ݵĽ���
 * @param None
 * @return void
 */
#ifndef USE_RT_THREAD
static void taskForRxdata6(void)
#else
static void taskForRxdata6(void* arg)
#endif
{
	static UCHAR rxFlag = 0;
	static UINT16 uRxDataIndex = 0;
	static MsgTypeDef RxMsg;
    static UINT16 uAddr = 0;
    static UINT16 blockID = 0;
    static UINT16 funcID = 0;
    static UINT16 msgLen = 0;
	
	UCHAR rxData = 0;
#ifdef USE_RT_THREAD
	while (1)
	{
		if (m_uMsgRecvFlag & BIT(6))
			continue;
		m_uMsgRecvFlag &= ~BIT(6);
		if (rt_mutex_take(&mutex_rx, RT_WAITING_TIME) != RT_EOK)
			continue;
		m_uWritePriter = m_hUart[6]->read(m_rxDataBuffer, MAX_MSG_PHY_DATA_SIZE);
		for (m_uReadPriter=0; m_uReadPriter<m_uWritePriter;m_uReadPriter++)
		{
			rxData = m_rxDataBuffer[m_uReadPriter];
#else
	if (m_uReadPriter == m_uWritePriter)
		return;
		rxData = m_rxDataBuffer[m_uReadPriter];
		++m_uReadPriter;
		m_uReadPriter = m_uReadPriter % MAX_MSG_PHY_DATA_SIZE;
#endif
		
		switch(rxFlag)
		{
		case 0:			//��ʼ��־1
			if (PHY_DATA_FLAG1 == rxData)
			{
				++rxFlag;
			}
			break;
		case 1:			//��ʼ��־2
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
		case 2:			//��ַ��8λ
			uAddr = rxData;
			++rxFlag;
			break;
		case 3:			//��ַ��8λ
			uAddr = (uAddr << 8) | rxData;
			RxMsg.uAddr = osal_sock_ntohs(uAddr);
			++rxFlag;
			break;
		case 4:			//���ܵ�Ԫ��8λ
			blockID = rxData;
			++rxFlag;
			break;
		case 5:			//���ܵ�Ԫ��8λ
			blockID = (blockID << 8) | rxData;
			RxMsg.blockID = osal_sock_ntohs(blockID);
			++rxFlag;
			break;
		case 6:			//����ID��8λ
			funcID = rxData;
			++rxFlag;
			break;
		case 7:			//����ID��8λ
			funcID = (funcID << 8) | rxData;
			RxMsg.functionID = osal_sock_ntohs(funcID);
			++rxFlag;
			break;
		case 8:			//������
			RxMsg.opType = rxData;
			++rxFlag;
			break;
		case 9:			//���ݳ��ȸ�8λ
			msgLen = rxData;
			++rxFlag;
			break;
		case 10:		//���ݳ��ȵ�8λ
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
		case 11:		//�û�����
			if (uRxDataIndex < RxMsg.len)
			{
				RxMsg.data[uRxDataIndex++] = rxData;
			}
			else		//���ݽ������
			{
				rxFlag = 0;
				uRxDataIndex = 0;
				if (rxData == GetCheckValue(&RxMsg)) //��������У��
				{
					//DBG(TRACE("comm::GetCheckValue OK"));
					if (m_hRxObser[6] != NULL)
					{
						m_hRxObser[6](&RxMsg);
#ifdef USE_RT_THREAD
						m_uReadPriter = m_uWritePriter;
#endif
					}
				}
				else		//����У��ʧ��
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

#ifdef USE_RT_THREAD
		}//for
		rt_mutex_release(&mutex_rx);
	}
#endif
}

/**
 * @brief: ע�ᱨ�Ľ��չ۲���
 * @param hrxobser: ���Ľ��չ۲��߾��
 * @retval: void
 */
static void add_rx_obser6(CommRxCBack_t hrxobser)
{
	m_hRxObser[6] = hrxobser;
}

/**
 * @brief COMM��Ϣ����
 * @param pMsg: ����֡����
 * @param len: ����֡��С
 * @retval  ���ͳɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL transmit6(MsgTypeDef* pMsg)
{
	UCHAR txData = 0;
	UINT16 uAddr = 0;
    UINT16 blockID = 0;
	UINT16 uDataLen = 0;
    UINT16 uFuncID = 0;
	UINT16 txLen = 0;
	
	if (NULL == m_hUart[6])
		return FALSE;
	
#ifdef USE_RT_THREAD
	if (rt_mutex_take(&mutex_tx, RT_WAITING_TIME) != RT_EOK)
		return FALSE;
#endif
	txData = PHY_DATA_FLAG1;				//��ʼ��־1
	m_txDataBuffer[txLen++] = txData;
	
	txData = PHY_DATA_FLAG2;				//��ʼ��־2
	m_txDataBuffer[txLen++] = txData;
	
    uAddr = osal_sock_htons(pMsg->uAddr);
	txData = (UCHAR)(uAddr >> 8);				//��ַ��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)(uAddr);					//��ַ��8λ
	m_txDataBuffer[txLen++] = txData;
	
    blockID = osal_sock_htons(pMsg->blockID);
	txData = (UCHAR)(blockID >> 8);			//���ܵ�Ԫ��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)(blockID);			//���ܵ�Ԫ��8λ
	m_txDataBuffer[txLen++] = txData;
	
    uFuncID = osal_sock_htons(pMsg->functionID);
	txData = (UCHAR)(uFuncID >> 8);//����ID��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)(uFuncID);		//����ID��8λ
	m_txDataBuffer[txLen++] = txData;
	
	txData = pMsg->opType;					//������
	m_txDataBuffer[txLen++] = txData;
	
	uDataLen = GetCheckLenght(pMsg->len);	//���ݳ���
    uDataLen = osal_sock_htons(uDataLen);
	txData = (UCHAR)(uDataLen >> 8);		//��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)uDataLen;				//��8λ
	m_txDataBuffer[txLen++] = txData;
	
	memcpy(&m_txDataBuffer[txLen], pMsg->data, pMsg->len);	//�û�����
	txLen += pMsg->len;
	
	txData = GetCheckValue(pMsg);			//����У��
	m_txDataBuffer[txLen++] = txData;
	
	m_hUart[6]->write(m_txDataBuffer, txLen);

#ifdef USE_RT_THREAD
	rt_mutex_release(&mutex_tx);
#endif
	return TRUE;
}

#if USE_TX_MSG_FIFO
/**
 * @brief ����������FIFO
 * @param pMsg �洢���͵���Ϣ���
 * @return ��FIFO�������
 * @retval ��ȡ�ɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL readTxFIFO6(MsgTypeDef* pMsg)
{
	UINT16 i = 0;
	UINT16 index = 0;
	if (NULL == pMsg)		//�����Ƿ�
		return FALSE;
	if (m_uFIFO_ReadPriter == m_uFIFO_WritePriter)//FIFOΪ��
		return FALSE;
	++m_uFIFO_ReadPriter;
	m_uFIFO_ReadPriter = m_uFIFO_ReadPriter % MAX_MSG_FIFO_SIZE;
	
	index = m_uFIFO_ReadPriter * COMM_MSG_SISE;
	pMsg->uAddr = m_TX_FIFO_DataBuffer[6][index++];								//��ַ��8λ
	pMsg->uAddr = (pMsg->uAddr << 8) | m_TX_FIFO_DataBuffer[6][index++];			//��ַ��8λ
	pMsg->blockID = m_TX_FIFO_DataBuffer[6][index++];								//���ܵ�Ԫ��8λ
	pMsg->blockID = (pMsg->blockID << 8) | m_TX_FIFO_DataBuffer[6][index++];		//���ܵ�Ԫ��8λ
	pMsg->functionID = m_TX_FIFO_DataBuffer[6][index++];							//����ID��8λ
	pMsg->functionID = (pMsg->functionID << 8) | m_TX_FIFO_DataBuffer[6][index++];	//����ID��8λ
	pMsg->opType = m_TX_FIFO_DataBuffer[6][index++];								//������
	pMsg->len = m_TX_FIFO_DataBuffer[6][index++];									//���ݳ��ȸ�8λ
	pMsg->len = (pMsg->len << 8) | m_TX_FIFO_DataBuffer[6][index++];				//���ݳ��ȵ�8λ
	for (i = 0; i < pMsg->len; ++i)												//�û�����
	{
		pMsg->data[i] = m_TX_FIFO_DataBuffer[6][index++];
	}
	
	return TRUE;
}

/**
 * @brief д��������FIFO
 * @param pMsg ������Ϣ���
 * @return дFIFO�������
 * @retval д��ɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL writeTxFIFO6(MsgTypeDef* pMsg)
{
	UINT16 i = 0;
	UINT16 index = 0;
	if (NULL == pMsg)	//�����Ƿ�
		return FALSE;
	
	index = m_uFIFO_WritePriter * COMM_MSG_SISE;
	m_TX_FIFO_DataBuffer[6][index++] = (UCHAR)(pMsg->uAddr >> 8);					//��ַ��8λ
	m_TX_FIFO_DataBuffer[6][index++] = (UCHAR)(pMsg->uAddr);						//��ַ��8λ
	m_TX_FIFO_DataBuffer[6][index++] = (UCHAR)(pMsg->blockID >> 8);				//���ܵ�Ԫ��8λ
	m_TX_FIFO_DataBuffer[6][index++] = (UCHAR)(pMsg->blockID);						//���ܵ�Ԫ��8λ
	m_TX_FIFO_DataBuffer[6][index++] = (UCHAR)(pMsg->functionID >> 8);				//����ID��8λ
	m_TX_FIFO_DataBuffer[6][index++] = (UCHAR)(pMsg->functionID);					//����ID��8λ
	m_TX_FIFO_DataBuffer[6][index++] = pMsg->opType;								//������
	m_TX_FIFO_DataBuffer[6][index++] = (UCHAR)(pMsg->len >> 8);					//���ݳ��ȸ�8λ
	m_TX_FIFO_DataBuffer[6][index++] = (UCHAR)(pMsg->len);							//���ݳ��ȵ�8λ
	for (i = 0; i < pMsg->len; ++i)												//�û�����
	{
		m_TX_FIFO_DataBuffer[6][index++] = pMsg->data[i];
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
 * @brief COMM��Ϣ����
 * @param pMsg: ����֡����
 * @param len: ����֡��С
 * @retval  ���ͳɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL transmitByFIFO6(MsgTypeDef* pMsg)
{
	return writeTxFIFO6(pMsg);
}

/**
 * @brief FIFO���ݷ����̣߳����ڷ���FIFO�е�����
 */
#ifndef USE_RT_THREAD
static void taskForTxFiFoMsg6(void)
#else
static void taskForTxFiFoMsg6(void* arg)
#endif
{
	static MsgTypeDef TxMsg;
#ifdef USE_RT_THREAD
	while (1)
#endif
	{
		if (readTxFIFO6(&TxMsg))
		{
			transmit6(&TxMsg);
		}
	}
}
#endif

/**
 * @brief COMM��ʼ��
 * @param hUart:  Uartʵ�����
 * @param hRxFunc: COMM���ջص��������
 * @retval None
 */
static void init6(HALUartTypeDef* hUart)
{
	if (NULL == hUart)
		return;
	if (m_hUart[6] != NULL)
		m_hUart[6]->deInit();
	
	m_hUart[6] = hUart;
	m_hUart[6]->add_rx_obser(OnComm6);
	
#ifndef USE_RT_THREAD
	m_hUart[6]->add_rx_obser(OnComm6);
	osal_task_create(taskForRxdata6, 1);
#if USE_TX_MSG_FIFO
	osal_task_create(taskForTxFiFoMsg6, 1);
#endif
#else
	if(rt_thread_init(&RXDATA_Task_Handle[6], "CommEsp8266RxData", taskForRxdata6, RT_NULL,
						(unsigned char *)&RXDATA_TASK_STK[6][0], RXDATA_TASK_STK_SIZE, RXDATA_TASK_PRIO, RXDATA_TASK_TICK) == RT_EOK)
		rt_thread_startup(&RXDATA_Task_Handle[6]);
#if USE_TX_MSG_FIFO
	if(rt_thread_init(&TXFIFO_Task_Handle[6], "CommEsp8266TxFIFO", taskForTxFiFoMsg6, RT_NULL,
						(unsigned char *)&TXFIFO_TASK_STK[6][0], TXFIFO_TASK_STK_SIZE, TXFIFO_TASK_PRIO, TXFIFO_TASK_TICK) == RT_EOK)
		rt_thread_startup(&TXFIFO_Task_Handle[6]);
#endif
#endif
}

/**
 * @brief COMM��Դע��
 * @param None
 * @retval None
 */
static void deInit6(void)
{
	pthis[6] = NULL;
	if (m_hUart[6] != NULL)
	{
		m_hUart[6]->deInit();
	}
	m_hUart[6] = NULL;
#ifndef USE_RT_THREAD
	osal_task_kill(taskForRxdata6);
	
#if USE_TX_MSG_FIFO
	osal_task_kill(taskForTxFiFoMsg6);
#endif
#else
	rt_thread_detach(&RXDATA_Task_Handle[6]);
	
#if USE_TX_MSG_FIFO
	rt_thread_detach(&TXFIFO_Task_Handle[6]);
#endif
#endif

}
#endif

#ifdef WIZCHIP_NET_CH7
/**
 * @brief Uart�˿����ݽ��ջص�����
 * @param data ���յ�������
 * @return void
 */
static void OnComm7(UCHAR data)
{
	m_uMsgRecvFlag |= BIT(7);
}

/**
 * @brief ����֡�����̣߳�����������ݵĽ���
 * @param None
 * @return void
 */
#ifndef USE_RT_THREAD
static void taskForRxdata7(void)
#else
static void taskForRxdata7(void* arg)
#endif
{
	static UCHAR rxFlag = 0;
	static UINT16 uRxDataIndex = 0;
	static MsgTypeDef RxMsg;
    static UINT16 uAddr = 0;
    static UINT16 blockID = 0;
    static UINT16 funcID = 0;
    static UINT16 msgLen = 0;
	
	UCHAR rxData = 0;
#ifdef USE_RT_THREAD
	while (1)
	{
		if (m_uMsgRecvFlag & BIT(7))
			continue;
		m_uMsgRecvFlag &= ~BIT(7);
		if (rt_mutex_take(&mutex_rx, RT_WAITING_TIME) != RT_EOK)
			continue;
		m_uWritePriter = m_hUart[7]->read(m_rxDataBuffer, MAX_MSG_PHY_DATA_SIZE);
		for (m_uReadPriter=0; m_uReadPriter<m_uWritePriter;m_uReadPriter++)
		{
			rxData = m_rxDataBuffer[m_uReadPriter];
#else
	if (m_uReadPriter == m_uWritePriter)
		return;
		rxData = m_rxDataBuffer[m_uReadPriter];
		++m_uReadPriter;
		m_uReadPriter = m_uReadPriter % MAX_MSG_PHY_DATA_SIZE;
#endif
		
		switch(rxFlag)
		{
		case 0:			//��ʼ��־1
			if (PHY_DATA_FLAG1 == rxData)
			{
				++rxFlag;
			}
			break;
		case 1:			//��ʼ��־2
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
		case 2:			//��ַ��8λ
			uAddr = rxData;
			++rxFlag;
			break;
		case 3:			//��ַ��8λ
			uAddr = (uAddr << 8) | rxData;
			RxMsg.uAddr = osal_sock_ntohs(uAddr);
			++rxFlag;
			break;
		case 4:			//���ܵ�Ԫ��8λ
			blockID = rxData;
			++rxFlag;
			break;
		case 5:			//���ܵ�Ԫ��8λ
			blockID = (blockID << 8) | rxData;
			RxMsg.blockID = osal_sock_ntohs(blockID);
			++rxFlag;
			break;
		case 6:			//����ID��8λ
			funcID = rxData;
			++rxFlag;
			break;
		case 7:			//����ID��8λ
			funcID = (funcID << 8) | rxData;
			RxMsg.functionID = osal_sock_ntohs(funcID);
			++rxFlag;
			break;
		case 8:			//������
			RxMsg.opType = rxData;
			++rxFlag;
			break;
		case 9:			//���ݳ��ȸ�8λ
			msgLen = rxData;
			++rxFlag;
			break;
		case 10:		//���ݳ��ȵ�8λ
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
		case 11:		//�û�����
			if (uRxDataIndex < RxMsg.len)
			{
				RxMsg.data[uRxDataIndex++] = rxData;
			}
			else		//���ݽ������
			{
				rxFlag = 0;
				uRxDataIndex = 0;
				if (rxData == GetCheckValue(&RxMsg)) //��������У��
				{
					//DBG(TRACE("comm::GetCheckValue OK"));
					if (m_hRxObser[7] != NULL)
					{
						m_hRxObser[7](&RxMsg);
#ifdef USE_RT_THREAD
						m_uReadPriter = m_uWritePriter;
#endif
					}
				}
				else		//����У��ʧ��
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

#ifdef USE_RT_THREAD
		}//for
		rt_mutex_release(&mutex_rx);
	}
#endif
}

/**
 * @brief: ע�ᱨ�Ľ��չ۲���
 * @param hrxobser: ���Ľ��չ۲��߾��
 * @retval: void
 */
static void add_rx_obser7(CommRxCBack_t hrxobser)
{
	m_hRxObser[7] = hrxobser;
}

/**
 * @brief COMM��Ϣ����
 * @param pMsg: ����֡����
 * @param len: ����֡��С
 * @retval  ���ͳɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL transmit7(MsgTypeDef* pMsg)
{
	UCHAR txData = 0;
	UINT16 uAddr = 0;
    UINT16 blockID = 0;
	UINT16 uDataLen = 0;
    UINT16 uFuncID = 0;
	UINT16 txLen = 0;
	
	if (NULL == m_hUart[7])
		return FALSE;
	
#ifdef USE_RT_THREAD
	if (rt_mutex_take(&mutex_tx, RT_WAITING_TIME) != RT_EOK)
		return FALSE;
#endif
	txData = PHY_DATA_FLAG1;				//��ʼ��־1
	m_txDataBuffer[txLen++] = txData;
	
	txData = PHY_DATA_FLAG2;				//��ʼ��־2
	m_txDataBuffer[txLen++] = txData;
	
    uAddr = osal_sock_htons(pMsg->uAddr);
	txData = (UCHAR)(uAddr >> 8);				//��ַ��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)(uAddr);					//��ַ��8λ
	m_txDataBuffer[txLen++] = txData;
	
    blockID = osal_sock_htons(pMsg->blockID);
	txData = (UCHAR)(blockID >> 8);			//���ܵ�Ԫ��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)(blockID);			//���ܵ�Ԫ��8λ
	m_txDataBuffer[txLen++] = txData;
	
    uFuncID = osal_sock_htons(pMsg->functionID);
	txData = (UCHAR)(uFuncID >> 8);//����ID��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)(uFuncID);		//����ID��8λ
	m_txDataBuffer[txLen++] = txData;
	
	txData = pMsg->opType;					//������
	m_txDataBuffer[txLen++] = txData;
	
	uDataLen = GetCheckLenght(pMsg->len);	//���ݳ���
    uDataLen = osal_sock_htons(uDataLen);
	txData = (UCHAR)(uDataLen >> 8);		//��8λ
	m_txDataBuffer[txLen++] = txData;
	txData = (UCHAR)uDataLen;				//��8λ
	m_txDataBuffer[txLen++] = txData;
	
	memcpy(&m_txDataBuffer[txLen], pMsg->data, pMsg->len);	//�û�����
	txLen += pMsg->len;
	
	txData = GetCheckValue(pMsg);			//����У��
	m_txDataBuffer[txLen++] = txData;
	
	m_hUart[7]->write(m_txDataBuffer, txLen);
	
#ifdef USE_RT_THREAD
	rt_mutex_release(&mutex_tx);
#endif
	return TRUE;
}

#if USE_TX_MSG_FIFO
/**
 * @brief ����������FIFO
 * @param pMsg �洢���͵���Ϣ���
 * @return ��FIFO�������
 * @retval ��ȡ�ɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL readTxFIFO7(MsgTypeDef* pMsg)
{
	UINT16 i = 0;
	UINT16 index = 0;
	if (NULL == pMsg)		//�����Ƿ�
		return FALSE;
	if (m_uFIFO_ReadPriter == m_uFIFO_WritePriter)//FIFOΪ��
		return FALSE;
	++m_uFIFO_ReadPriter;
	m_uFIFO_ReadPriter = m_uFIFO_ReadPriter % MAX_MSG_FIFO_SIZE;
	
	index = m_uFIFO_ReadPriter * COMM_MSG_SISE;
	pMsg->uAddr = m_TX_FIFO_DataBuffer[7][index++];								//��ַ��8λ
	pMsg->uAddr = (pMsg->uAddr << 8) | m_TX_FIFO_DataBuffer[7][index++];			//��ַ��8λ
	pMsg->blockID = m_TX_FIFO_DataBuffer[7][index++];								//���ܵ�Ԫ��8λ
	pMsg->blockID = (pMsg->blockID << 8) | m_TX_FIFO_DataBuffer[7][index++];		//���ܵ�Ԫ��8λ
	pMsg->functionID = m_TX_FIFO_DataBuffer[7][index++];							//����ID��8λ
	pMsg->functionID = (pMsg->functionID << 8) | m_TX_FIFO_DataBuffer[7][index++];	//����ID��8λ
	pMsg->opType = m_TX_FIFO_DataBuffer[7][index++];								//������
	pMsg->len = m_TX_FIFO_DataBuffer[7][index++];									//���ݳ��ȸ�8λ
	pMsg->len = (pMsg->len << 8) | m_TX_FIFO_DataBuffer[7][index++];				//���ݳ��ȵ�8λ
	for (i = 0; i < pMsg->len; ++i)												//�û�����
	{
		pMsg->data[i] = m_TX_FIFO_DataBuffer[7][index++];
	}
	
	return TRUE;
}

/**
 * @brief д��������FIFO
 * @param pMsg ������Ϣ���
 * @return дFIFO�������
 * @retval д��ɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL writeTxFIFO7(MsgTypeDef* pMsg)
{
	UINT16 i = 0;
	UINT16 index = 0;
	if (NULL == pMsg)	//�����Ƿ�
		return FALSE;
	
	index = m_uFIFO_WritePriter * COMM_MSG_SISE;
	m_TX_FIFO_DataBuffer[7][index++] = (UCHAR)(pMsg->uAddr >> 8);					//��ַ��8λ
	m_TX_FIFO_DataBuffer[7][index++] = (UCHAR)(pMsg->uAddr);						//��ַ��8λ
	m_TX_FIFO_DataBuffer[7][index++] = (UCHAR)(pMsg->blockID >> 8);				//���ܵ�Ԫ��8λ
	m_TX_FIFO_DataBuffer[7][index++] = (UCHAR)(pMsg->blockID);						//���ܵ�Ԫ��8λ
	m_TX_FIFO_DataBuffer[7][index++] = (UCHAR)(pMsg->functionID >> 8);				//����ID��8λ
	m_TX_FIFO_DataBuffer[7][index++] = (UCHAR)(pMsg->functionID);					//����ID��8λ
	m_TX_FIFO_DataBuffer[7][index++] = pMsg->opType;								//������
	m_TX_FIFO_DataBuffer[7][index++] = (UCHAR)(pMsg->len >> 8);					//���ݳ��ȸ�8λ
	m_TX_FIFO_DataBuffer[7][index++] = (UCHAR)(pMsg->len);							//���ݳ��ȵ�8λ
	for (i = 0; i < pMsg->len; ++i)												//�û�����
	{
		m_TX_FIFO_DataBuffer[7][index++] = pMsg->data[i];
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
 * @brief COMM��Ϣ����
 * @param pMsg: ����֡����
 * @param len: ����֡��С
 * @retval  ���ͳɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL transmitByFIFO7(MsgTypeDef* pMsg)
{
	return writeTxFIFO7(pMsg);
}

/**
 * @brief FIFO���ݷ����̣߳����ڷ���FIFO�е�����
 */
#ifndef USE_RT_THREAD
static void taskForTxFiFoMsg7(void)
#else
static void taskForTxFiFoMsg7(void* arg)
#endif
{
	static MsgTypeDef TxMsg;
#ifdef USE_RT_THREAD
	while (1)
#endif
	{
		if (readTxFIFO7(&TxMsg))
		{
			transmit7(&TxMsg);
		}
	}
}
#endif

/**
 * @brief COMM��ʼ��
 * @param hUart:  Uartʵ�����
 * @param hRxFunc: COMM���ջص��������
 * @retval None
 */
static void init7(HALUartTypeDef* hUart)
{
	if (NULL == hUart)
		return;
	if (m_hUart[7] != NULL)
		m_hUart[7]->deInit();
	
	m_hUart[7] = hUart;
	m_hUart[7]->add_rx_obser(OnComm7);
	
#ifndef USE_RT_THREAD
	m_hUart[7]->add_rx_obser(OnComm7);
	osal_task_create(taskForRxdata7, 1);
#if USE_TX_MSG_FIFO
	osal_task_create(taskForTxFiFoMsg7, 1);
#endif
#else
	if(rt_thread_init(&RXDATA_Task_Handle[7], "CommEsp8266RxData", taskForRxdata7, RT_NULL,
						(unsigned char *)&RXDATA_TASK_STK[7][0], RXDATA_TASK_STK_SIZE, RXDATA_TASK_PRIO, RXDATA_TASK_TICK) == RT_EOK)
		rt_thread_startup(&RXDATA_Task_Handle[7]);
#if USE_TX_MSG_FIFO
	if(rt_thread_init(&TXFIFO_Task_Handle[7], "CommEsp8266TxFIFO", taskForTxFiFoMsg7, RT_NULL,
						(unsigned char *)&TXFIFO_TASK_STK[7][0], TXFIFO_TASK_STK_SIZE, TXFIFO_TASK_PRIO, TXFIFO_TASK_TICK) == RT_EOK)
		rt_thread_startup(&TXFIFO_Task_Handle[7]);
#endif
#endif
}

/**
 * @brief COMM��Դע��
 * @param None
 * @retval None
 */
static void deInit7(void)
{
	pthis[7] = NULL;
	if (m_hUart[7] != NULL)
	{
		m_hUart[7]->deInit();
	}
	m_hUart[7] = NULL;
#ifndef USE_RT_THREAD
	osal_task_kill(taskForRxdata7);
	
#if USE_TX_MSG_FIFO
	osal_task_kill(taskForTxFiFoMsg7);
#endif
#else
	rt_thread_detach(&RXDATA_Task_Handle[7]);
	
#if USE_TX_MSG_FIFO
	rt_thread_detach(&TXFIFO_Task_Handle[7]);
#endif
#endif

}
#endif//WIZCHIP_NET_CH
#endif //_WIZCHIP_ > 5100

static void New(UINT8 sn)
{
	if (sn >= WIZCHIP_NET_SIZE)
		return;
	
	if (NULL == pthis[sn])
	{
		switch (sn)
		{
#ifdef WIZCHIP_NET_CH0
		case WIZCHIP_NET_CH0:
		m_Instance[sn].init = init0;
		m_Instance[sn].deInit = deInit0;
		m_Instance[sn].add_rx_obser = add_rx_obser0;
		m_Instance[sn].transmit = transmit0;
#if USE_TX_MSG_FIFO
		m_Instance[sn].transmitByFIFO = transmitByFIFO0;
#endif
		break;
#endif
#ifdef WIZCHIP_NET_CH1
		case WIZCHIP_NET_CH1:
		m_Instance[sn].init = init1;
		m_Instance[sn].deInit = deInit1;
		m_Instance[sn].add_rx_obser = add_rx_obser1;
		m_Instance[sn].transmit = transmit1;
#if USE_TX_MSG_FIFO
		m_Instance[sn].transmitByFIFO = transmitByFIFO1;
#endif
		break;
#endif
#ifdef WIZCHIP_NET_CH2
		case WIZCHIP_NET_CH2:
		m_Instance[sn].init = init2;
		m_Instance[sn].deInit = deInit2;
		m_Instance[sn].add_rx_obser = add_rx_obser2;
		m_Instance[sn].transmit = transmit2;
#if USE_TX_MSG_FIFO
		m_Instance[sn].transmitByFIFO = transmitByFIFO2;
#endif
		break;
#endif
#ifdef WIZCHIP_NET_CH3
		case WIZCHIP_NET_CH3:
		m_Instance[sn].init = init3;
		m_Instance[sn].deInit = deInit3;
		m_Instance[sn].add_rx_obser = add_rx_obser3;
		m_Instance[sn].transmit = transmit3;
#if USE_TX_MSG_FIFO
		m_Instance[sn].transmitByFIFO = transmitByFIFO3;
#endif
		break;
#endif
#if _WIZCHIP_ > 5100
#ifdef WIZCHIP_NET_CH4
		case WIZCHIP_NET_CH4:
		m_Instance[sn].init = init4;
		m_Instance[sn].deInit = deInit4;
		m_Instance[sn].add_rx_obser = add_rx_obser4;
		m_Instance[sn].transmit = transmit4;
#if USE_TX_MSG_FIFO
		m_Instance[sn].transmitByFIFO = transmitByFIFO4;
#endif
		break;
#endif
#ifdef WIZCHIP_NET_CH5
		case WIZCHIP_NET_CH5:
		m_Instance[sn].init = init5;
		m_Instance[sn].deInit = deInit5;
		m_Instance[sn].add_rx_obser = add_rx_obser5;
		m_Instance[sn].transmit = transmit5;
#if USE_TX_MSG_FIFO
		m_Instance[sn].transmitByFIFO = transmitByFIFO5;
#endif
		break;
#endif
#ifdef WIZCHIP_NET_CH6
		case WIZCHIP_NET_CH6:
		m_Instance[sn].init = init6;
		m_Instance[sn].deInit = deInit6;
		m_Instance[sn].add_rx_obser = add_rx_obser6;
		m_Instance[sn].transmit = transmit6;
#if USE_TX_MSG_FIFO
		m_Instance[sn].transmitByFIFO = transmitByFIFO6;
#endif
		break;
#endif
#ifdef WIZCHIP_NET_CH7
		case WIZCHIP_NET_CH7:
		m_Instance[sn].init = init7;
		m_Instance[sn].deInit = deInit7;
		m_Instance[sn].add_rx_obser = add_rx_obser7;
		m_Instance[sn].transmit = transmit7;
#if USE_TX_MSG_FIFO
		m_Instance[sn].transmitByFIFO = transmitByFIFO7;
#endif
		break;
#endif // WIZCHIP_NET_CH
#endif
		}
		pthis[sn] = &m_Instance[sn];
	}
}

/**
 * @brief ��ȡComm�����ṹ���
 * @param sn: COMMͨ��ö�� @ref EnWizchipNetChannel
 * @return ����Comm�����ṹ���
 * @retval COMM�����ṹ���
 */
CommTypeDef* comm_wizchip_getInstance(UINT8 sn)
{
#ifdef USE_RT_THREAD
	if (!(m_uMutexInited & 0x01))
	{
		if (rt_mutex_init(&mutex_rx, "mutex_rx", RT_IPC_FLAG_FIFO) != RT_EOK)
			m_uMutexInited |= 0x01;
	}
	
	if (!(m_uMutexInited & 0x02))
	{
		if (rt_mutex_init(&mutex_tx, "mutex_tx", RT_IPC_FLAG_FIFO) != RT_EOK)
			m_uMutexInited |= 0x02;
	}
#endif
	
	if (NULL == pthis[sn])
	{
		New(sn);
	}
	return pthis[sn];
}

#endif //CFG_WIZCHIP

