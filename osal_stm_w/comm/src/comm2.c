/**
 * @file    comm2.c
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
#include "comm2.h"

/* Comm���ݲ���ʵ�� */
static CommTypeDef m_Instance;
static CommTypeDef* pthis= NULL;

/* ���ع�����Uart�˿� */
static HALUartTypeDef* m_hUart = NULL;

/* ���ݽ�����ɻص�����ָ�� */
static CommRxCBack_t m_hRxObser = NULL;

/* ���ݽ��ջ����� */
static UCHAR m_rxDataBuffer[MAX_MSG_PHY_DATA_SIZE] = {0};
/* ���ݻ�������ָ�� */
static UINT16 m_uReadPriter = 0;
/* ���ݻ�����дָ�� */
static UINT16 m_uWritePriter = 0;

#if USE_TX_MSG_FIFO
/* ���ݽ��ջ����� */
static UCHAR m_TX_FIFO_DataBuffer[MAX_MSG_FIFO_SIZE * COMM_MSG_SISE] = {0};
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

/**
 * @brief Uart�˿����ݽ��ջص�����
 * @param data ���յ�������
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
 * @brief ����֡�����̣߳�����������ݵĽ���
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
				if (m_hRxObser != NULL)
				{
					m_hRxObser(&RxMsg);
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
}

/**
 * @brief: ע�ᱨ�Ľ��չ۲���
 * @param hrxobser: ���Ľ��չ۲��߾��
 * @retval: void
 */
static void add_rx_obser(CommRxCBack_t hrxobser)
{
	m_hRxObser = hrxobser;
}

/**
 * @brief COMM��Ϣ����
 * @param pMsg: ����֡����
 * @param len: ����֡��С
 * @retval  ���ͳɹ�����TRUE��ʧ�ܷ���FALSE
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
	
	txData = PHY_DATA_FLAG1;				//��ʼ��־1
	m_hUart->write(&txData, 1);
	
	txData = PHY_DATA_FLAG2;				//��ʼ��־2
	m_hUart->write(&txData, 1);
	
    uAddr = osal_sock_htons(pMsg->uAddr);
	txData = (UCHAR)(uAddr >> 8);				//��ַ��8λ
	m_hUart->write(&txData, 1);
	txData = (UCHAR)(uAddr);					//��ַ��8λ
	m_hUart->write(&txData, 1);
	
    blockID = osal_sock_htons(pMsg->blockID);
	txData = (UCHAR)(blockID >> 8);			//���ܵ�Ԫ��8λ
	m_hUart->write(&txData, 1);
	txData = (UCHAR)(blockID);			//���ܵ�Ԫ��8λ
	m_hUart->write(&txData, 1);
	
    uFuncID = osal_sock_htons(pMsg->functionID);
	txData = (UCHAR)(uFuncID >> 8);//����ID��8λ
	m_hUart->write(&txData, 1);
	txData = (UCHAR)(uFuncID);		//����ID��8λ
	m_hUart->write(&txData, 1);
	
	txData = pMsg->opType;					//������
	m_hUart->write(&txData, 1);
	
	uDataLen = GetCheckLenght(pMsg->len);	//���ݳ���
    uDataLen = osal_sock_htons(uDataLen);
	txData = (UCHAR)(uDataLen >> 8);		//��8λ
	m_hUart->write(&txData, 1);
	txData = (UCHAR)uDataLen;				//��8λ
	m_hUart->write(&txData, 1);
	
	m_hUart->write(pMsg->data, pMsg->len);	//�û�����
	
	txData = GetCheckValue(pMsg);			//����У��
	m_hUart->write(&txData, 1);
	
	return TRUE;
}

#if USE_TX_MSG_FIFO
/**
 * @brief ����������FIFO
 * @param pMsg �洢���͵���Ϣ���
 * @return ��FIFO�������
 * @retval ��ȡ�ɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL readTxFIFO(MsgTypeDef* pMsg)
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
	pMsg->uAddr = m_TX_FIFO_DataBuffer[index++];								//��ַ��8λ
	pMsg->uAddr = (pMsg->uAddr << 8) | m_TX_FIFO_DataBuffer[index++];			//��ַ��8λ
	pMsg->blockID = m_TX_FIFO_DataBuffer[index++];								//���ܵ�Ԫ��8λ
	pMsg->blockID = (pMsg->blockID << 8) | m_TX_FIFO_DataBuffer[index++];		//���ܵ�Ԫ��8λ
	pMsg->functionID = m_TX_FIFO_DataBuffer[index++];							//����ID��8λ
	pMsg->functionID = (pMsg->functionID << 8) | m_TX_FIFO_DataBuffer[index++];	//����ID��8λ
	pMsg->opType = m_TX_FIFO_DataBuffer[index++];								//������
	pMsg->len = m_TX_FIFO_DataBuffer[index++];									//���ݳ��ȸ�8λ
	pMsg->len = (pMsg->len << 8) | m_TX_FIFO_DataBuffer[index++];				//���ݳ��ȵ�8λ
	for (i = 0; i < pMsg->len; ++i)												//�û�����
	{
		pMsg->data[i] = m_TX_FIFO_DataBuffer[index++];
	}
	
	return TRUE;
}

/**
 * @brief д��������FIFO
 * @param pMsg ������Ϣ���
 * @return дFIFO�������
 * @retval д��ɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL writeTxFIFO(MsgTypeDef* pMsg)
{
	UINT16 i = 0;
	UINT16 index = 0;
	if (NULL == pMsg)	//�����Ƿ�
		return FALSE;
	
	index = m_uFIFO_WritePriter * COMM_MSG_SISE;
	m_TX_FIFO_DataBuffer[index++] = (UCHAR)(pMsg->uAddr >> 8);					//��ַ��8λ
	m_TX_FIFO_DataBuffer[index++] = (UCHAR)(pMsg->uAddr);						//��ַ��8λ
	m_TX_FIFO_DataBuffer[index++] = (UCHAR)(pMsg->blockID >> 8);				//���ܵ�Ԫ��8λ
	m_TX_FIFO_DataBuffer[index++] = (UCHAR)(pMsg->blockID);						//���ܵ�Ԫ��8λ
	m_TX_FIFO_DataBuffer[index++] = (UCHAR)(pMsg->functionID >> 8);				//����ID��8λ
	m_TX_FIFO_DataBuffer[index++] = (UCHAR)(pMsg->functionID);					//����ID��8λ
	m_TX_FIFO_DataBuffer[index++] = pMsg->opType;								//������
	m_TX_FIFO_DataBuffer[index++] = (UCHAR)(pMsg->len >> 8);					//���ݳ��ȸ�8λ
	m_TX_FIFO_DataBuffer[index++] = (UCHAR)(pMsg->len);							//���ݳ��ȵ�8λ
	for (i = 0; i < pMsg->len; ++i)												//�û�����
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
 * @brief COMM��Ϣ����
 * @param pMsg: ����֡����
 * @param len: ����֡��С
 * @retval  ���ͳɹ�����TRUE��ʧ�ܷ���FALSE
 */
static BOOL transmitByFIFO(MsgTypeDef* pMsg)
{
	return writeTxFIFO(pMsg);
}

/**
 * @brief FIFO���ݷ����̣߳����ڷ���FIFO�е�����
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
 * @brief COMM��ʼ��
 * @param hUart:  Uartʵ�����
 * @param hRxFunc: COMM���ջص��������
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
 * @brief COMM��Դע��
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
 * @brief ��ȡComm�����ṹ���
 * @param eChennal: COMMͨ��ö�� @ref CommChannelDef
 * @return ����Comm�����ṹ���
 * @retval COMM�����ṹ���
 */
CommTypeDef* comm2_getInstance(void)
{
	if (NULL == pthis)
	{
		New();
	}
	return pthis;
}

