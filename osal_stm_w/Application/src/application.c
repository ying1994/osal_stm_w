/**
 * @file    application.c
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
#include "osal_stm_w_cfg.h"

#include "application.h"
#include "debugunit.h"

#if !ENABLE_BOOTLOADER_CODE


/**
 * @brief 系统开始升级消息处理
 * @param pMsg 接收到的消息句柄
 * @return void
 */
static void UpdateUint_OnStart(MsgTypeDef* pMsg)
{
	if (pMsg->functionID != START)
		return;
	if (pMsg->len != 2)
		return;
	HalIapJmp2addr(HAL_BOOTLOADER_BASE_ADDR);
}
/**
 * @brief 系统升级准备完成消息处理
 * @param pMsg 接收到的消息句柄
 * @return void
 */
static void UpdateUint_OnReady(MsgTypeDef* pMsg)
{
}
/**
 * @brief 系统正在升级中消息处理
 * @param pMsg 接收到的消息句柄
 * @return void
 */
static void UpdateUint_OnUPDATING(MsgTypeDef* pMsg)
{
}
/**
 * @brief 单个升级结果反馈 
 * @param pMsg 接收到的消息句柄
 * @return void
 */
static void UpdateUint_OnResult(MsgTypeDef* pMsg)
{
}
/**
 * @brief 反馈最终升级结果
 * @param pMsg 接收到的消息句柄
 * @return void
 */
static void UpdateUint_OnLastResult(MsgTypeDef* pMsg)
{
}
/**
 * @brief 系统升级完成消息处理
 * @param pMsg 接收到的消息句柄
 * @return void
 */
static void UpdateUint_Onend(MsgTypeDef* pMsg)
{
}

/**
 * @brief 通讯结点在线事件回调函数
 * @param uChannel 产生事件通道结点地址
 * @param bIsConneted 指定通道是否在线，TRUE 指定通道上线， FALSE 指定通道掉线
 * @return void
 */
static void CommRouter_OnConnetEnent(UINT16 uAddr, BOOL bIsConneted)
{
}

/**
 * @brief 发送路由消息
 * @param pMsg 待发送的消息句柄
 * @return void
 */
static void CommRouter_SendMsg(MsgTypeDef* pMsg)
{
}

/* 系统升级单元实例 */	
static UpdateUnitCBack_t m_hUpdateInstance = 
{
	UpdateUint_OnStart,
	UpdateUint_OnReady,
	UpdateUint_OnUPDATING,
	UpdateUint_OnResult,
	UpdateUint_OnLastResult,
	UpdateUint_Onend
};

/* 路由单元实例 */
static OSALRouterCBack_t m_hRouterInstance =
{
	CommRouter_OnConnetEnent,
	CommRouter_SendMsg
};

/**
 * @brief 调试单元消息处理函数
 */
static void OnCommMsgEvent(MsgTypeDef* pMsg)
{
	//TODO: Add your codes here.
	switch (pMsg->blockID)
	{
	case DEBUG_UNIT:
		DebugUnit_OnMsgEvent(pMsg);
	break;
	default:
		break;
	}
}

/**
 * @brief 调试单元消息处理函数
 */
static void OnDebugMsgEvent(MsgTypeDef* pMsg)
{
	pMsg->uSerPort = OSAL_ROUTE_PORT0;//记录消息源端口
	if (osal_router_OnCommMsg(pMsg))
	{
		//TODO: Add your codes here.
		OnCommMsgEvent(pMsg);
	}
}

/**
 * @brief application初始化
 * @param None
 * @retval None
 */
void application_init(void)
{
	CommTypeDef* hComm = NULL;
	//UCHAR blocks[] = {UPDATE_UNIT, DEBUG_UNIT, ROUTER_UNIT};
	
	//调试单元初始化 
	hComm = comm_getInstance(COMM_CHANNEL0);
	hComm->init(hal_uart_getinstance(HAL_UART1));
	hComm->add_rx_obser(OnDebugMsgEvent);
	osal_router_setCommPort(hComm, OSAL_ROUTE_PORT0);
	
	osal_router_init(&m_hRouterInstance);
	bd_updateunit_Init(&m_hUpdateInstance);
	
}

#endif //!ENABLE_BOOTLOADER_CODE
