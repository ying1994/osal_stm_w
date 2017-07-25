/**
 * @file    osal_router.h
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   ϵͳ·��ת������ģ��
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 * COPYRIGHT NOTICE  
 * Copyright 2016, wsf 
 * All rights Reserved
 *
 */


#ifndef OSAL_ROUTER_H
#define OSAL_ROUTER_H

#include "types.h"
#include "comm.h"

#if (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM))

#define MAX_BLOCK_NAME_LEN 128	/*!< ����ģ��������󳤶ȣ�����"\n" */
#define MAX_BLOCK_SIZE 32 		/*!< ģ������ܵ�Ԫ���� */

/**
 * @brief ���ڶ˿ں�
 */
typedef enum 
{
	OSAL_ROUTE_PORT0 = 0,	/*!< Port0 */
	OSAL_ROUTE_PORT1,		/*!< Port1 */
	OSAL_ROUTE_PORT2,		/*!< Port2 */
	OSAL_ROUTE_PORT3,		/*!< Port3 */
	OSAL_ROUTE_PORT4,		/*!< Port4 */
	OSAL_ROUTE_PORT5,		/*!< Port5 */
	OSAL_ROUTE_PORT_SIZE	/*!< �˿����� */
}OsalRoutePortNumer;

/**
 * @brief ·��ͨ�������ƻ�����Ϣ
 */
typedef struct _OSALRouterNodeDef
{
	UINT8 u8ChannelMode;	/*!< ͨ��ͨѶģʽ */
	BOOL bIsConneted;		/*!< ͨ���Ƿ����� */
	UINT16 u16TimeCounter;	/*!< �������߼�����������Ϊ��λ */
	UINT16 u16TimerPeriod;	/*!< ������߼������ڣ�����Ϊ��λ */
}OSALRouterNodeDef;

/**
 * @brief ע��Ĺ���ģ����Ϣע���
 */
typedef struct _OSALRouterReg_t
{
	UINT8 blockAddr;						/*!< ����ģ�����ַ */
	UCHAR blockName[MAX_BLOCK_NAME_LEN];	/*!< ����ģ������ */
	UCHAR functionCount;					/*!< ģ�鹦�ܵ�Ԫ�ĸ��� */
	UCHAR functionIDs[MAX_BLOCK_SIZE];		/*!< ģ�鹦�ܵ�ԪID���� */
}OSALRouterReg_t;

/**
 * @brief ·�ɹ��ܵ�Ԫʵ�ֻص���������
 */
typedef struct _OSALRouterCBack_t
{
	/**
	 * @brief ͨѶ��������¼��ص�����
	 * @param uChannel �����¼�ͨ������ַ
	 * @param bIsConneted ָ��ͨ���Ƿ����ߣ�TRUE ָ��ͨ�����ߣ� FALSE ָ��ͨ������
	 * @return void
	 */
	void (*OnConnetEnent)(UINT16 uPortAddr, BOOL bIsConneted);
	
	/**
	 * @brief ����·����Ϣ
	 * @param pMsg �����͵���Ϣ���
	 * @return void
	 */
	void (*SendMsg)(MsgTypeDef* pMsg);
}OSALRouterCBack_t;

/**
 * @brief ·�ɹ��ܵ�Ԫ��ʼ����ע��·�ɻ�������ص�����
 * @param hRouterBase ·����Ϣ����ص�������
 * @return void
 */
void osal_router_init(OSALRouterCBack_t* hRouterBase);

/**
 * @brief ������Ϣ·��ת��
 * @param pMsg ���յ�����Ϣ���
 * @return �Ƿ��Ǳ�����Ϣ
 * @retval TRUE ������Ϣ�� FALSE �Ǳ�����Ϣ
 */
BOOL osal_router_OnCommMsg(MsgTypeDef* pMsg);

/**
 * @brief ���һ��·��Э��ת���˿�
 * @param hCommHandle ·��Э��ת�������
 * @param uPortAddr Э��ת���Ķ˿ڵ�ַ
 * @return BOOL ·������Ƿ�ɹ�
 * @retval TRUE ·����ӳɹ��� FALSE ·�����ʧ��
 */
BOOL osal_router_setCommPort(CommTypeDef* hCommHandle, UINT16 uPortAddr);

/**
 * @brief ���һ�����õ���Ϣ�洢�ṹ���
 * @param None
 * @return ����һ�����õ���Ϣ���
 */
MsgTypeDef* osal_router_getFreeMsgHandle(void);

/**
 * @brief ·����Ϣ����
 * @param uAddr ������Ϣ��ַ
 * @param pMsg Ԥ���͵���Ϣ�洢���
 * @return BOOL ��Ϣ�����Ƿ�ɹ�
 * @retval TRUE ��Ϣ���ͳɹ���FALSE ��Ϣ����ʧ��
 */
BOOL osal_router_sendMsg(UINT8 uPortAddr, MsgTypeDef* pMsg);

/**
 * @brief ���ý���ַ
 * @param uAddr ���ؽ���ַ
 * @return ���ر��ؽ���ַ
 */
UINT16 osal_router_setAddress(UINT16 uAddr);
/**
 * @brief ��ȡ����ַ
 * @param None
 * @return ���ر��ؽ���ַ
 */
UINT16 osal_router_getAddress(void);
/**
 * @brief ������ַ�Ƿ��Ǳ�����ַ
 * @param uAddr ����ַ
 * @return ��ַ�Ϸ�����TRUE, ���򷵻�FALSE
 */
BOOL osal_router_checkAddress(UINT16 uAddr);

#endif //(defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM))
#endif
