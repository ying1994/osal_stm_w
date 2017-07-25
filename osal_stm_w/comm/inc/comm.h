/**
 * @file    comm.h
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
 * All rights Reserved
 */
#ifndef COMM_H
#define COMM_H
#include "osal.h"

#ifdef CFG_OSAL_COMM

#include "hal_board.h"

#include "unit.h"
#include "optype.h"
#include "function.h"

/** ��������֡��ʼ��־ */
#define PHY_DATA_FLAG1 0x55
#define PHY_DATA_FLAG2 0xee

/** ʹ�÷������ݻ��������� */
#define USE_TX_MSG_FIFO 1

/** ��������������ݳ��� */
#define MAX_MSG_DATA_SIZE 1024
/** �������������������ݳ��� */
#define MAX_MSG_PHY_DATA_SIZE MAX_MSG_DATA_SIZE + 20

/** ���ݷ���FIFO��С */
#define MAX_MSG_FIFO_SIZE 10
/** ��Ϣ֡��С */
#define COMM_MSG_SISE MAX_MSG_DATA_SIZE + 8

/**
 * @brief COMMͨ��ö��
 */
typedef enum
{
	COMM_CHANNEL0 = 0,	/*!< COMMͨ��0 */
	COMM_CHANNEL1,		/*!< COMMͨ��1 */
	COMM_CHANNEL2,		/*!< COMMͨ��2 */
	COMM_CHANNEL3,		/*!< COMMͨ��3 */
	COMM_CHANNEL4,		/*!< COMMͨ��4 */
	COMM_CHANNEL_SIZE,	/*!< COMMͨ����ͨ���� */
}CommChannelDef;

/**
 * @brief BD����Э�������֡�ṹ
 */
typedef struct _MsgTypeDef
{
	UCHAR uSerPort;					/*!< ���ݶ˿� */
	UINT16 uAddr;					/*!< �豸��ַ */
	UINT16 blockID;				/*!< ��Ϣ���ܵ�Ԫ */
	UINT16 functionID;				/*!< ��Ϣ��������ID */
	UCHAR opType;					/*!< ����״̬������ */
	UINT16 len;					/*!< ���ݳ��� */
	UCHAR data[MAX_MSG_DATA_SIZE];	/*!< �û����� */
}MsgTypeDef;


/**
 * @brief COMM �ص�����
 * @param uIDE: ֡���ͱ�ʶ������׼֡����չ֡��
 * @param uID: ����֡��ʶ��
 * @param pMsg: ����֡����
 * @param size: ����֡��С
 * @retval None
 */
typedef void (*CommRxCBack_t)(MsgTypeDef* pMsg);


/**
 * @brief COMM�����ṹ����
 */
typedef struct _CommTypeDef
{
	/**
	 * @brief COMM��ʼ��
	 * @param hUart:  Uartʵ�����
	 * @retval None
	 */
	void (*init)(HALUartTypeDef* hUart);
	
	/**
	 * @brief COMM��Դע��
	 * @param None
	 * @retval None
	 */
	void (*deInit)(void);
	
	/**
	 * @brief: ע�ᱨ�Ľ��չ۲���
	 * @param hrxobser: ���Ľ��չ۲��߾��
	 * @retval: void
	 */
	void (*add_rx_obser)(CommRxCBack_t hrxobser);
	
	/**
	 * @brief COMM��Ϣ����
	 * @param pMsg: ����֡����
	 * @param len: ����֡��С
	 * @retval  ���ͳɹ�����TRUE��ʧ�ܷ���FALSE
	 */
	BOOL (*transmit)(MsgTypeDef* pMsg);
	
	/**
	 * @brief ͨ��FIFO�ķ�ʽ����COMM��Ϣ
	 * @param pMsg: ����֡����
	 * @param len: ����֡��С
	 * @retval  ���ͳɹ�����TRUE��ʧ�ܷ���FALSE
	 */
#if USE_TX_MSG_FIFO
	BOOL (*transmitByFIFO)(MsgTypeDef* pMsg);
#endif
	
}CommTypeDef;



/**
 * @brief ��ȡComm�����ṹ���
 * @param eChennal: COMMͨ��ö�� @ref CommChannelDef
 * @retval COMM�����ṹ���
 */
CommTypeDef* comm_getInstance(CommChannelDef eChannel);

/**
 * @brief ע��һ��Comm�����ṹ
 * @param hCommHandle Comm�����ṹ���
 * @param eChennal COMMͨ��ö�� @ref CommChannelDef
 * @return ���ز������
 * @retval ע��ɹ�����TRUE��ʧ�ܷ���FALSE
 */
BOOL comm_registe(CommTypeDef* hCommHandle, CommChannelDef eChannel);

#endif // CFG_OSAL_COMM
#endif //__COMM_H_
