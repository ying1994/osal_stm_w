/**
 * @file    osal_router.c
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   系统路由转发功能模块
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 * COPYRIGHT NOTICE  
 * Copyright 2016, wsf 
 * All rights res
 *
 */


#include "hal_board.h"
#include "osal.h"

#include "comm.h"
#include "osal_error.h"
#include "osal_router.h"
#include "osal_updateunit.h"


#if (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM))


/* 路由的端口收发实例句柄 */
static CommTypeDef* m_hCommHandle[COMM_CHANNEL_SIZE] = {NULL};
/* 本地消息处理函数集 */
static OSALRouterCBack_t* m_hRouterInstance = NULL;
/* 结点地址 */
static UINT16 m_uAddress = 0;

//设备地址
static void OnDeviceAddress(MsgTypeDef* pMsg)
{
	UINT16 uAddr = 0;
	if (OP_SETGET == pMsg->opType)
	{
		uAddr = (pMsg->data[0] << 8) | pMsg->data[1];
		uAddr = osal_sock_ntohs(uAddr);
		uAddr = osal_router_setAddress(uAddr);
	}
	if (OP_GET & pMsg->opType)
	{
		uAddr = osal_router_getAddress();
		uAddr = osal_sock_htons(uAddr);
		pMsg->data[0] = (UCHAR)(uAddr >> 8);
		pMsg->data[1] = (UCHAR)(uAddr);
		pMsg->len = 2;
		pMsg->opType = OP_STATUS;
		
		osal_router_sendMsg(pMsg->uSerPort, pMsg);
	}
}

static void OnCommMsg(MsgTypeDef* pMsg)
{
	switch(pMsg->functionID)
	{
	case COMM_HANDLE:
		//if (pMsg->opType & OP_GET)
		{
			pMsg->opType = OP_STATUS;
			osal_router_sendMsg(pMsg->uSerPort, pMsg);
		}
		break;
	case DEV_ADDR:
		OnDeviceAddress(pMsg);
		break;
	default:
		break;
	}
}
/**
 * @brief 接收消息路由转发
 * @param pMsg 接收到的消息句柄
 * @return 是否是本地消息
 * @retval TRUE 本地消息， FALSE 非本地消息
 */
BOOL osal_router_OnCommMsg(MsgTypeDef* pMsg)
{
	if (osal_router_checkAddress(pMsg->uAddr))
	{
		if (m_hRouterInstance != NULL)
		{
			if (m_hRouterInstance->OnConnetEnent != NULL)
			{
				m_hRouterInstance->OnConnetEnent(pMsg->uSerPort, TRUE);
			}
		}
		switch(pMsg->blockID)
		{
		case UPDATE_UNIT:
			osal_updateunit_OnCommMsg(pMsg);
			break;
		case ROUTER_UNIT:
			OnCommMsg(pMsg);
			break;
		default:
			break;
		}
		return TRUE;
	}
	
	return FALSE;
}

/**
 * @brief 添加一个路由协议转发端口
 * @param hCommHandle 路由协议转发器句柄
 * @param uPortAddr 协议转发的端口地址
 * @return void
 */
BOOL osal_router_setCommPort(CommTypeDef* hCommHandle, UINT16 uPortAddr)
{
	if (uPortAddr >= COMM_CHANNEL_SIZE)
	{
		osal_error_setLastError(ERR_CHANNEL_NOEXIT);
		return FALSE;
	}
	if (m_hCommHandle[uPortAddr] != NULL)
		m_hCommHandle[uPortAddr]->deInit();
	
	m_hCommHandle[uPortAddr] = hCommHandle;
	
	return TRUE;
}

/**
 * @brief 获得一个可用的消息存储结构句柄
 * @param None
 * @return 返回一个可用的消息句柄
 */
MsgTypeDef* osal_router_getFreeMsgHandle(void)
{
	static MsgTypeDef msg;
	
	return &msg;
}

/**
 * @brief 路由消息发送
 * @param uAddr 发送消息地址
 * @param pMsg 预发送的消息存储句柄
 * @return BOOL 消息发送是否成功
 * @retval TRUE 消息发送成功，FALSE 消息发送失败
 */
BOOL osal_router_sendMsg(UINT8 uPortAddr, MsgTypeDef* pMsg)
{
	if (uPortAddr >= COMM_CHANNEL_SIZE)//非法地址
	{
		osal_error_setLastError(ERR_ADDR_NOTEXIT);
		return FALSE;
	}
	
	if (m_hCommHandle[uPortAddr] != NULL)
	{
		pMsg->uAddr = osal_router_getAddress();
		m_hCommHandle[uPortAddr]->transmit(pMsg);
	}
	
	return TRUE;
}

/**
 * @brief 通讯结点心跳包发送计时器
 */
static void OnRouterCommHandleTimer(void)
{
	//TODO: 在此添加握手心跳包通讯代码
	//MsgTypeDef* pMsg = osal_router_getFreeMsgHandle();
	//pMsg->uAddr = 0;
	//pMsg->blockID = ROUTER_UNIT;
	//pMsg->functionID = COMM_HANDLE;
	//pMsg->opType = OP_STATUS;
	//pMsg->len = 0;
	
	//osal_router_sendMsg(0, pMsg);
}

/**
 * @brief 通讯结点心跳计数器
 */
static void OnRouterTimer(void)
{
	//TODO: 在此添加握手心跳包通讯代码
}

/**
 * @brief 设置结点地址
 * @param uAddr 本地结点地址
 * @return 返回本地结点地址
 */
UINT16 osal_router_setAddress(UINT16 uAddr)
{
	m_uAddress = uAddr;
	HalFlashReadWriteE2PROW(APP_DEVICE_ADDRESS_ADDR, (UCHAR*)&uAddr, sizeof(uAddr));
	return m_uAddress;
}
/**
 * @brief 获取结点地址
 * @param None
 * @return 返回本地结点地址
 */
UINT16 osal_router_getAddress(void)
{
	return m_uAddress;
}

/**
 * @brief 检查结点地址是否是本机地址
 * @param uAddr 结点地址
 * @return 地址合法返回TRUE, 否则返回FALSE
 */
BOOL osal_router_checkAddress(UINT16 uAddr)
{
	if ((uAddr == 0xffff) || (uAddr == m_uAddress))
		return TRUE;
	
	return FALSE;
}

/**
 * @brief 路由功能单元初始化，注册路由基本处理回调函数
 * @param hRouterBase 路由消息处理回调函数集
 * @return void
 */
void osal_router_init(OSALRouterCBack_t* hRouterBase)
{
	UINT16 uAddress = 0;
	HalFlashRead(APP_DEVICE_ADDRESS_ADDR, (UCHAR*)&uAddress, sizeof(uAddress));
	if ((uAddress != 0xffff))
		m_uAddress = uAddress;
	TRACE("Device Address: %d\r\n", m_uAddress);
		
	HalSetShareTimer(OnRouterCommHandleTimer, 1000);//创建独立定时器用于发送心跳包
	HalSetShareTimer(OnRouterTimer, 1000);//创建独立定时器监控结点心在线事件
	m_hRouterInstance = hRouterBase;
}

#endif // (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM))

