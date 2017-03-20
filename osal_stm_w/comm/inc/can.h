/**
 * @file    can.h
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
#ifndef CAN_H
#define CAN_H
#include "types.h"

/** CAN1�˿ڶ��� */
#define CAN1_GPIO_TYPE	GPIOA
#define CAN1_TX		GPIO_Pin_12
#define CAN1_RX		GPIO_Pin_11

/** CAN1�˿ڶ��� */
#define CAN2_GPIO_TYPE	GPIOB
#define CAN2_TX		GPIO_Pin_13
#define CAN2_RX		GPIO_Pin_12


#define CAN_FRAME_SIZE	17		/*!< CAN����֡��С */

#define CAN_MSG_BUFFER_SIZE	20	/*!< CAN��������С(����֡����) */

/** CAN ����ģʽ */
#define HAL_CAN_Mode_Normal          CAN_Mode_Normal           /*!< normal mode */
#define HAL_CAN_Mode_LoopBack        CAN_Mode_LoopBack         /*!< loopback mode */
#define HAL_CAN_Mode_Silent          CAN_Mode_Silent           /*!< silent mode */
#define HAL_CAN_Mode_Silent_LoopBack CAN_Mode_Silent_LoopBack  /*!< loopback combined with silent mode */

/**
 * @brief CANͨ��ö��
 */
typedef enum
{
	CAN_CHANNEL1 = 0,/*!< CANͨ��1 */
#ifdef STM32F10X_CL
	CAN_CHANNEL2,	/*!< CANͨ��2 */
#endif
	CAN_CHANNEL_SIZE,/*!< CANͨ����ͨ���� */
}CAN_CHANNEL;

/**
 * @brief CAN ������
 */
typedef enum
{
	CAN_BAUDRATE_1M = 1000,	/*!< 1MHz */
	CAN_BAUDRATE_800K = 800,	/*!< 800KHz */
	CAN_BAUDRATE_500K = 500,	/*!< 500KHz */
	CAN_BAUDRATE_250K = 250,	/*!< 250KHz */
	CAN_BAUDRATE_200K = 200,	/*!< 200KHz */
	CAN_BAUDRATE_125K = 125,	/*!< 125KHz */
	CAN_BAUDRATE_100K = 100,	/*!< 100KHz */
}CAN_BAUDRATE;

/**
 * @brief CAN �ص�����
 * @param uIDE: ֡���ͱ�ʶ������׼֡����չ֡��
 * @param uID: ����֡��ʶ��
 * @param pMsg: ����֡����
 * @param size: ����֡��С
 * @retval None
 */
typedef void (*CAN_RX_BASE_FUNC)(UINT32 uIDE, UINT32 uID, UCHAR* pMsg, UINT8 size);

/**
 * @brief CAN�����ṹ����
 */
typedef struct _CanTypeDef
{
	/**
	 * @brief CAN��ʼ��
	 * @param eMode: ����ģʽ
	 * @param uBaudrate: ������
	 * @param hRxFunc: CAN���ջص��������
	 * @retval None
	 */
	void (*init)(UINT8 eMode, UINT uBaudrate, CAN_RX_BASE_FUNC hRxFunc);
	
	/**
	 * @brief CAN��Դע��
	 * @param None
	 * @retval None
	 */
	void (*deInit)(void);
	
	/**
	 * @brief CAN ����������
	 * @param uBaudrate: ������ @ref CAN_BAUDRATE
	 * @retval None
	 */
	void (*setBaudrate)(UINT32 uBaudrate);
	
	/**
	 * @brief ����CAN����ID
	 * @param pIDs: ����ID��
	 * @param size: ����ID����
	 * @retval None
	 */
	void (*setFilterIDs)(UINT32* pIDs, UINT32 size);
	
	/**
	 * @brief ����CAN��Ϣ�Զ�����
	 * @param bIsOn: ����(TRUE)���ֹ(FALSE)CAN��Ϣ�Զ�����
	 * @retval None
	 */
	void (*enableReceive)(BOOL bIsOn);
	
	/**
	 * @brief CAN��Ϣ����
	 * @param uIDE: ֡���ͱ�ʶ������׼֡����չ֡��
	 * @param uID: ����֡��ʶ��
	 * @param pMsg: ����֡����
	 * @param len: ����֡��С
	 * @retval  ���ͳɹ�����TRUE��ʧ�ܷ���FALSE
	 */
	BOOL (*transmit)(UINT32 uIDE, UINT32 uID, UCHAR* pMsg, UINT8 len);
	
	/**
	 * @brief CAN��Ϣ����
	 * @param uIDE: ֡���ͱ�ʶ������׼֡����չ֡��
	 * @param uID: ����֡��ʶ��
	 * @param pMsg: ����֡����
	 * @retval ����֡��С
	 */
	UINT8 (*receive)(UINT32 *pIDE, UINT32 *pID, UCHAR* pMsg);
	
}CanTypeDef;

/**
 * @brief ��ȡCAN�����ṹ���
 * @param eChennal: CANͨ��ö�� @ref CAN_CHANNEL
 * @retval CAN�����ṹ���
 */
CanTypeDef* can_getInstance(CAN_CHANNEL eChannel);

#endif
