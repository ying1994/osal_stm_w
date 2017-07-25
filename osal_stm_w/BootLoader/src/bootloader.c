/**
 * @file    bootloader.c
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
#include "bootloader.h"
#include "osal.h"

#if ENABLE_BOOTLOADER_CODE

/*****************ϵͳ�������� UPDATE_UNIT ********************/
#if (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM) && defined(CFG_OSAL_UPDATEUNIT))
/* �����ļ����� */
static UINT16 m_uUpdateFileCount = 0;
/* ��ǰ�ļ���� */
static UINT16 m_uUpdateFileIndex = 0;
/* ��ǰ�ļ���С */
static UINT32 m_uCurFileSize = 0;
/* ��ǰ�ļ���CRC16У�� */
static UINT16 m_uCurFileCRCValue = 0;
/* ��ǰ�����������׶� */
static UINT16 m_uCurUpdateStage = 0;
/* ��������״̬ */
static UINT16 m_uUpdateResult = ERR_NO_ERROR;

/* �ļ�д���λ��ƫ�� */
static UINT32 m_uOffset = 0;
/* �����ļ�CRC16У�� */
static UINT16 m_uReceiveCRCValue = 0;
/* �ѽ����ļ���С */
static UINT32 m_uReceiveFileSize = 0;
#endif //#if (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM) && defined(CFG_OSAL_UPDATEUNIT))
/************* End of ϵͳ�������� UPDATE_UNIT ****************/

static void OnCommRouterMsg(MsgTypeDef* pMsg)
{
	switch (pMsg->functionID)
	{
	default:
		break;
	}
}

//��ѯBootLoader�汾
static void OnGetBiosVersion(MsgTypeDef* pMsg)
{
	if (OP_GET == pMsg->opType)
	{
		CHAR* pVersion = NULL;
		UINT len = 0;
		UINT i;
		pVersion = BOOTLOADER_VERSION;
		len = sizeof(BOOTLOADER_VERSION);
		pMsg->len = len;
		for (i=0; i < len; ++i)
		{
			pMsg->data[i] =  pVersion[i];
		}
		pMsg->data[i] = '\0';
		pMsg->opType = OP_STATUS;
		
		osal_router_sendMsg(pMsg->uSerPort, pMsg);
	}
}

//��ѯApp�汾
static void OnGetAppVersion(MsgTypeDef* pMsg)
{
	if (OP_GET == pMsg->opType)
	{
		CHAR* pVersion = NULL;
		UINT len = 0;
		UINT i;
		pVersion = FRIMWARE_VERSION;
		len = sizeof(FRIMWARE_VERSION);
		pMsg->len = len;
		for (i=0; i < len; ++i)
		{
			pMsg->data[i] =  pVersion[i];
		}
		pMsg->data[i] = '\0';
		pMsg->opType = OP_STATUS;
		
		osal_router_sendMsg(pMsg->uSerPort, pMsg);
	}
}

//��ѯоƬID
static void OnGetChipID(MsgTypeDef* pMsg)
{
	if (OP_GET == pMsg->opType)
	{
		pMsg->len = HalCpuGetID(pMsg->data);
		pMsg->opType = OP_STATUS;
		
		osal_router_sendMsg(pMsg->uSerPort, pMsg);
	}
}

static void OnDebugMsg(MsgTypeDef* pMsg)
{
	switch (pMsg->functionID)
	{
	case GET_BIOS_VERSION:
		OnGetBiosVersion(pMsg);
		break;
	case GET_APP_VERSION:
		OnGetAppVersion(pMsg);
		break;
	case GET_CHIP_ID:
		OnGetChipID(pMsg);
		break;
	default:
		break;
	}
}

/**
 * @brief ���Ե�Ԫ��Ϣ����
 * @param pMsg ���յ�����Ϣ���
 * @return void
 */
static void OnDebugUnitMsgEvent(MsgTypeDef* pMsg)
{
	//DBG(TRACE("DebugUnit_OnMsgEvent: %04X\r\n", pMsg->blockID));
	switch (pMsg->blockID)
	{
	case ROUTER_UNIT:
		OnCommRouterMsg(pMsg);
		break;
	case DEBUG_UNIT:
		OnDebugMsg(pMsg);
		break;
	default:
		break;
	}
}


/**
 * @brief App������ת�̣߳�����ʱδ���յ�����������ת��APP����
 */
static void taskForJumpToApp(void)
{
	osal_task_kill(taskForJumpToApp);
	HalIapJmp2addr(HAL_APP_BASE_ADDR);
}


#if (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM) && defined(CFG_OSAL_UPDATEUNIT))
/**
 * @brief ϵͳ��ʼ������Ϣ����
 * @param pMsg ���յ�����Ϣ���
 * @return void
 */
static void UpdateUint_OnStart(MsgTypeDef* pMsg)
{
	if (pMsg->functionID != START)
		return;
	if (pMsg->len != 2)
		return;
	
	//bd_timer_killTimerWithFunc(taskForJumpToApp);//�ر�App��ת����
	osal_task_kill(taskForJumpToApp);
	
	m_uCurUpdateStage = START;
	m_uUpdateResult = ERR_UNKNOW_ERROR;
	
	m_uUpdateFileCount = pMsg->data[0];
	m_uUpdateFileCount = (m_uUpdateFileCount << 8) | pMsg->data[1];
	m_uUpdateFileCount = osal_sock_ntohs(m_uUpdateFileCount);
	
	HalIapSetIsAppActive(FALSE);
	
	if (OP_GET == pMsg->opType || OP_SETGET == pMsg->opType)
	{
		pMsg->opType = OP_STATUS;
		osal_router_sendMsg(pMsg->uSerPort, pMsg);
	}
}
/**
 * @brief ϵͳ����׼�������Ϣ����
 * @param pMsg ���յ�����Ϣ���
 * @return void
 */
static void UpdateUint_OnReady(MsgTypeDef* pMsg)
{
	if (pMsg->functionID != READY)
		return;
	if (pMsg->len != 8)
		return;
	
	m_uCurUpdateStage = READY;
	//�ļ����
	m_uUpdateFileIndex = pMsg->data[0];
	m_uUpdateFileIndex = (m_uUpdateFileIndex << 8) | pMsg->data[1];
	m_uUpdateFileIndex = osal_sock_ntohs(m_uUpdateFileIndex);
	//�ļ���С
	m_uCurFileSize = pMsg->data[2];
	m_uCurFileSize = (m_uCurFileSize << 8) | pMsg->data[3];
	m_uCurFileSize = (m_uCurFileSize << 8) | pMsg->data[4];
	m_uCurFileSize = (m_uCurFileSize << 8) | pMsg->data[5];
	m_uCurFileSize = osal_sock_ntohl(m_uCurFileSize);
	//�ļ�CRC16
	m_uCurFileCRCValue = pMsg->data[6];
	m_uCurFileCRCValue = (m_uCurFileCRCValue << 8) | pMsg->data[7];
	m_uCurFileCRCValue = osal_sock_ntohs(m_uCurFileCRCValue);
	
    //���д����
    HalFlashUnlock();
	HalFlashErase(HAL_APP_BASE_ADDR, m_uCurFileSize);
	
	m_uReceiveFileSize = 0;
	m_uReceiveCRCValue = 0;
	m_uOffset = 0;
	
	if (OP_GET == pMsg->opType || OP_SETGET == pMsg->opType)
	{
		UINT16 fileindex = 0;
		UINT32 filesize = 0;
		UINT16 crc16 = 0;
		pMsg->opType = OP_STATUS;
		pMsg->len = 8;
		fileindex = osal_sock_htons(m_uUpdateFileIndex); //�ļ����
		pMsg->data[0] = (UCHAR)(fileindex >> 8);
		pMsg->data[1] = (UCHAR)(fileindex);

		filesize = osal_sock_htonl(m_uCurFileSize); //�ļ���С
		pMsg->data[2] = (UCHAR)(filesize >> 24);
		pMsg->data[3] = (UCHAR)(filesize >> 16);
		pMsg->data[4] = (UCHAR)(filesize >> 8);
		pMsg->data[5] = (UCHAR)(filesize);

		crc16 = osal_sock_htons(m_uCurFileCRCValue); //�ļ�CRC16
		pMsg->data[6] = (UCHAR)(crc16 >> 8);
		pMsg->data[7] = (UCHAR)(crc16);
		osal_router_sendMsg(pMsg->uSerPort, pMsg);
	}
}

/**
 * @brief ϵͳ������������Ϣ����
 * @param pMsg ���յ�����Ϣ���
 * @return void
 */
static void UpdateUint_OnUpdating(MsgTypeDef* pMsg)
{
	UINT32 uDataLen = 0;
	UINT32 uOffset = 0;
	BOOL bWriteStatus = FALSE;
	if (pMsg->functionID != UPDATING)
		return;
	if (pMsg->len < 4)
		return;
	
	m_uCurUpdateStage = UPDATING;
	//�ļ�ƫ��
	uOffset = pMsg->data[0];
	uOffset = (uOffset << 8) | pMsg->data[1];
	uOffset = (uOffset << 8) | pMsg->data[2];
	uOffset = (uOffset << 8) | pMsg->data[3];
	m_uOffset = osal_sock_ntohl(uOffset);

	//���ݴ�С
	uDataLen = pMsg->len - 4;
	if (HalIapProgramApp(HAL_APP_BASE_ADDR + m_uOffset, &pMsg->data[4], uDataLen))
	{
		bWriteStatus = TRUE;
		m_uReceiveFileSize += uDataLen;
	}
	
	if (OP_GET == pMsg->opType || OP_SETGET == pMsg->opType)
	{
		pMsg->opType = OP_STATUS;
		uOffset = osal_sock_htonl(m_uOffset);
		pMsg->data[0] = (UCHAR)(uOffset >> 24);
		pMsg->data[1] = (UCHAR)(uOffset >> 16);
		pMsg->data[2] = (UCHAR)(uOffset >> 8);
		pMsg->data[3] = (UCHAR)(uOffset);
		pMsg->data[4] = (UCHAR)bWriteStatus;
		pMsg->len = 5;
		osal_router_sendMsg(pMsg->uSerPort, pMsg);
	}
}
/**
 * @brief ��������������� 
 * @param pMsg ���յ�����Ϣ���
 * @return void
 */
static void UpdateUint_OnResult(MsgTypeDef* pMsg)
{
	enum {READ_DATA_MAX = 256};
	static UCHAR data[READ_DATA_MAX] = {0};
//	UINT16 uReadSize = 0;
	INT32 uLen = m_uReceiveFileSize;
//	UINT32 uReadAddr = HAL_APP_BASE_ADDR;
	INT32 i = 0;
	
	m_uCurUpdateStage = RESULT;
	
	//����CRC16
	for (i = 0; i < uLen; i += READ_DATA_MAX)
	{
		if ((i + READ_DATA_MAX) > uLen)
			break;
		HalIapRead(HAL_APP_BASE_ADDR+i, data, READ_DATA_MAX);
		m_uReceiveCRCValue = HalCRC16(m_uReceiveCRCValue, data, READ_DATA_MAX);
	}
	//����ʣ������CRC
	HalIapRead(HAL_APP_BASE_ADDR+i, data, uLen-i);
	m_uReceiveCRCValue = HalCRC16(m_uReceiveCRCValue, data, uLen-i);
	
	//�ж�CRCУ���Ƿ�һ��
	m_uUpdateResult = ERR_NO_ERROR;
	if (m_uReceiveCRCValue != m_uCurFileCRCValue)
	{
		m_uUpdateResult = ERR_CHECKCRC_ERROR;
	}
	if (m_uReceiveFileSize != m_uCurFileSize)
	{
		m_uUpdateResult = ERR_CHECKSIZE_ERROR;
	}
	
	if (OP_GET == pMsg->opType || OP_SETGET == pMsg->opType)
	{
		UINT16 uUpdateResult = 0;
		pMsg->opType = OP_STATUS;
		uUpdateResult = osal_sock_htons(m_uUpdateResult);
		pMsg->data[0] = (UCHAR)(uUpdateResult >> 8);
		pMsg->data[1] = (UCHAR)uUpdateResult;
		uUpdateResult = osal_sock_htons(m_uReceiveCRCValue);
		//uUpdateResult = osal_sock_htons(m_uCurFileCRCValue);
		pMsg->data[2] = (UCHAR)(uUpdateResult >> 8);
		pMsg->data[3] = (UCHAR)uUpdateResult;
		pMsg->len = 4;
		osal_router_sendMsg(pMsg->uSerPort, pMsg);
	}
}
/**
 * @brief ���������������
 * @param pMsg ���յ�����Ϣ���
 * @return void
 */
static void UpdateUint_OnLastResult(MsgTypeDef* pMsg)
{
	m_uCurUpdateStage = LAST_RESULT;

	if (OP_GET == pMsg->opType || OP_SETGET == pMsg->opType)
	{
		UINT16 uUpdateResult = osal_sock_htons(m_uUpdateResult);
		pMsg->opType = OP_STATUS;
		pMsg->data[0] = (UCHAR)(uUpdateResult >> 8);
		pMsg->data[1] = (UCHAR)uUpdateResult;
		pMsg->len = 2;
		osal_router_sendMsg(pMsg->uSerPort, pMsg);
	}
}

/**
 * @brief ϵͳ���������Ϣ����
 * @param pMsg ���յ�����Ϣ���
 * @return void
 */
static void UpdateUint_Onend(MsgTypeDef* pMsg)
{
	BOOL bIsJumpToApp = FALSE;
	BOOL bUpdateResult = FALSE;
	if (1 == pMsg->len || LAST_RESULT == m_uCurUpdateStage)
	{
		bIsJumpToApp = (BOOL)pMsg->data[0];
	}
	bUpdateResult = (ERR_NO_ERROR == m_uUpdateResult ? TRUE : FALSE);
	//bUpdateResult = TRUE;
	HalIapSetIsAppActive(bUpdateResult);
	
	if (OP_SETGET == pMsg->opType || OP_GET == pMsg->opType)
	{
		pMsg->opType = OP_STATUS;
		pMsg->data[0] = (CHAR)bUpdateResult;
		pMsg->len = 1;
		osal_router_sendMsg(pMsg->uSerPort, pMsg);
	}
	
	m_uCurUpdateStage = 0;
	
	if (bIsJumpToApp)
	{
		HalIapJmp2addr(HAL_APP_BASE_ADDR);
	}
}

/* ϵͳ������Ԫʵ�� */	
static UpdateUnitCBack_t m_hUpdateInstance = 
{
	UpdateUint_OnStart,
	UpdateUint_OnReady,
	UpdateUint_OnUpdating,
	UpdateUint_OnResult,
	UpdateUint_OnLastResult,
	UpdateUint_Onend
};

#endif //#if (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM) && defined(CFG_OSAL_UPDATEUNIT))

#if (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM))
/**
 * @brief ͨѶ��������¼��ص�����
 * @param uChannel �����¼�ͨ������ַ
 * @param bIsConneted ָ��ͨ���Ƿ����ߣ�TRUE ָ��ͨ�����ߣ� FALSE ָ��ͨ������
 * @return void
 */
static void CommRouter_OnConnetEvent(UINT16 uPort, BOOL bIsConneted)
{
}

/**
 * @brief ����·����Ϣ
 * @param pMsg �����͵���Ϣ���
 * @return void
 */
static void CommRouter_SendMsg(MsgTypeDef* pMsg)
{
}

/* ·�ɵ�Ԫʵ�� */
static OSALRouterCBack_t m_hRouterInstance =
{
	CommRouter_OnConnetEvent,
	CommRouter_SendMsg
};

/**
 * @brief ���Ե�Ԫ��Ϣ������
 */
static void OnDebugMsgEvent(MsgTypeDef* pMsg)
{
	//DBG(TRACE("OnDebugMsgEvent\r\n"));
	pMsg->uSerPort = OSAL_ROUTE_PORT0;//��¼��ϢԴ�˿�
	if (osal_router_OnCommMsg(pMsg))
	{
		//TODO: Add your codes here.
		OnDebugUnitMsgEvent(pMsg);
	}
}
#endif //#if (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM) && defined(CFG_OSAL_UPDATEUNIT))

/**
 * @brief BootLoader��ʼ��
 * @param None
 * @retval None
 */
void bootloader_init(void)
{
#if (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM))
	CommTypeDef* hComm = NULL;
	//UCHAR blocks[] = {UPDATE_UNIT, DEBUG_UNIT, ROUTER_UNIT};
	
	//���Ե�Ԫ��ʼ�� 
	hComm = comm_getInstance(COMM_CHANNEL0);
	hComm->init(hal_uart_getinstance(HAL_UART1));
	hComm->add_rx_obser(OnDebugMsgEvent);
	osal_router_setCommPort(hComm, OSAL_ROUTE_PORT0);
	
	osal_router_init(&m_hRouterInstance);
#endif //#if (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM))
	
#if (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM) && defined(CFG_OSAL_UPDATEUNIT))
	osal_updateunit_Init(&m_hUpdateInstance);
#endif //#if (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM) && defined(CFG_OSAL_UPDATEUNIT))
	
	//����App��ת����ʵ��App�����Զ���תִ�У���תʱ��Ϊ300ms��
	osal_task_create(taskForJumpToApp, 0x40000);
}


#endif //ENABLE_BOOTLOADER_CODE
