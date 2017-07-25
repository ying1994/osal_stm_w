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
#include "global.h"

#if !ENABLE_BOOTLOADER_CODE

#include "osal.h"
#include "osal_net.h"

#include "application.h"
#include "debugunit.h"

#include "esp8266_client.h"
#include "comm_esp8266.h"

#if (defined(CFG_USE_NET) && defined(CFG_WIZCHIP))
#include "wizchip_net.h"
#endif //(defined(CFG_USE_NET) && defined(CFG_WIZCHIP))

#if (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM) && defined(CFG_OSAL_UPDATEUNIT))
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
#endif //#if (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM) && defined(CFG_OSAL_UPDATEUNIT))

#if (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM))
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

/* 路由单元实例 */
static OSALRouterCBack_t m_hRouterInstance =
{
	CommRouter_OnConnetEnent,
	CommRouter_SendMsg
};
#endif //#if (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM))

#if (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM))
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
#endif //#if (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM))

#if (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM) && defined(CFG_USE_WIFI) && defined(CFG_ESP8266_CLIENT) && defined(CFG_USE_COMM_ESP8266))	
/**
 * @brief 调试单元消息处理函数
 */
static void OnEsp8826MsgEvent(MsgTypeDef* pMsg)
{
	pMsg->uSerPort = OSAL_ROUTE_PORT1;//记录消息源端口
	if (osal_router_OnCommMsg(pMsg))
	{
		//TODO: Add your codes here.
		OnCommMsgEvent(pMsg);
	}
}

//检查字符串参数合法性(不能全为0xff)
BOOL checkParamStr(UCHAR* msg, UINT16 len)
{
	UINT16 i = 0;
    for (i=0; i< len; i++)
    {
        if (msg[i] != 0xff)
            return TRUE;
    }
    return FALSE;
}
#endif

#if (defined(CFG_USE_WIFI) && defined(CFG_ESP8266_CLIENT))
//检查Wifi状态
static void taskForCheckWifiStage(void)
{
	g_uWifiState = esp8266_client_check();
	switch (g_uWifiState)
	{
		case ESP8266_CONNET_GETIP: //连接Wifi
			esp8266_client_connet(g_aServerIp, g_uServerPort, g_bTcpConnet);//连接服务器
			break;
		case ESP8266_CONNETED: //建立连接
			esp8266_client_StartTransparent();
			break;
		case ESP8266_CONNET_LOST: //失去连接
			esp8266_client_connet(g_aServerIp, g_uServerPort, g_bTcpConnet);//连接服务器
			break;
		default:				//物理掉线
			esp8266_client_connet_wifi(g_aSsid, g_aPwd);//连接Wifi
			break;
	}
	DBG(TRACE("taskForCheckWifiStage Run: %d\r\n", g_uWifiState));
}

#endif //CFG_ESP8266_CLIENT	

/**
 * @brief application初始化
 * @param None
 * @retval None
 */
void application_init(void)
{
#if (defined(CFG_USE_NET) && defined(CFG_WIZCHIP))
	HALSpiTypeDef *hspi = NULL;
	wiz_NetInfo netinfo;
#endif //(defined(CFG_USE_NET) && defined(CFG_WIZCHIP))
	
#if (defined(CFG_OSAL_COMM))
	CommTypeDef* hComm = NULL;
	//UCHAR blocks[] = {UPDATE_UNIT, DEBUG_UNIT, ROUTER_UNIT};
	
	//调试单元初始化 
	hComm = comm_getInstance(COMM_CHANNEL0);
	hComm->init(hal_uart_getinstance(HAL_UART1));
	hComm->add_rx_obser(OnDebugMsgEvent);
#if defined(CFG_OSAL_ROUTER)
	osal_router_setCommPort(hComm, OSAL_ROUTE_PORT0);
#endif //#if defined(CFG_OSAL_ROUTER)
#endif //#if defined(CFG_OSAL_COMM)
	
#ifdef CFG_USE_NET
	osal_net_init();
#if (defined(CFG_WIZCHIP))
	memcpy(netinfo.ip, g_aLocalIP, 4);
	memcpy(netinfo.gw, g_aLocalGateway, 4);
	memcpy(netinfo.sn, g_aLocalMask, 4);
	memcpy(netinfo.mac, g_aLocalMac, 6);
	memcpy(netinfo.dns, g_aLocalDns, 4);
	netinfo.dhcp = NETINFO_STATIC;
	
	hspi = HalSpiGetInstance(HALSpiNumer1);
	wizchip_net_Init(hspi, &netinfo);
	vizchip_net_start(SOCK_TCP_SERVER, SOCK_TCP_SERVER, g_aServerIp, g_uServerPort);
#endif //CFG_WIZCHIP
#endif //CFG_USE_NET

#if (defined(CFG_USE_WIFI) && defined(CFG_ESP8266_CLIENT))
	//ESP8266初始化
	esp8266_client_Init(hal_uart_getinstance(HAL_UART2), TRUE);
#endif //#if (defined(CFG_USE_WIFI) && defined(CFG_ESP8266_CLIENT))
	
#if (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM) && defined(CFG_USE_WIFI) && defined(CFG_ESP8266_CLIENT) && defined(CFG_USE_COMM_ESP8266))
	comm_registe(comm_esp8266_client_getInstance(), COMM_CHANNEL1);
	hComm = comm_getInstance(COMM_CHANNEL1);
	hComm->init(esp8266_client_getinstance());
	hComm->add_rx_obser(OnEsp8826MsgEvent);
	osal_router_setCommPort(hComm, OSAL_ROUTE_PORT1);
#endif //#if (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM) && defined(CFG_USE_WIFI) && defined(CFG_ESP8266_CLIENT))

#if (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM))
	osal_router_init(&m_hRouterInstance);
#endif

#if (defined(CFG_OSAL_ROUTER) && defined(CFG_OSAL_COMM) && defined(CFG_OSAL_UPDATEUNIT))
	osal_updateunit_Init(&m_hUpdateInstance);
#endif
	
	//创建任务
#if (defined(CFG_USE_WIFI) && defined(CFG_ESP8266_CLIENT))
	osal_task_create(taskForCheckWifiStage, 0x300000);//检查Wifi连接状态 (约 30s)
#endif //CFG_ESP8266_CLIENT
	
}

#endif //!ENABLE_BOOTLOADER_CODE
