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
#include "osal_stm_w_cfg.h"

#include "application.h"
#include "debugunit.h"

#include "esp8266.h"
#include "comm_esp8266.h"


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

#ifdef CFG_ESP8266	
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

//检查Wifi状态
static void taskForCheckWifiStage(void)
{
	g_uWifiState = esp8266_check();
	switch (g_uWifiState)
	{
		case ESP8266_CONNET_GETIP: //连接Wifi
			esp8266_connet(g_aServerIp, g_uServerPort, g_bTcpConnet);//连接服务器
			break;
		case ESP8266_CONNETED: //建立连接
			esp8266_StartTransparent();
			break;
		case ESP8266_CONNET_LOST: //失去连接
			esp8266_connet(g_aServerIp, g_uServerPort, g_bTcpConnet);//连接服务器
			break;
		default:				//物理掉线
			esp8266_connet_wifi(g_aSsid, g_aPwd);//连接Wifi
			break;
	}
	DBG(TRACE("taskForCheckWifiStage Run: %d\r\n", g_uWifiState));
}

#endif //CFG_ESP8266	

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
	
#ifdef CFG_ESP8266	
	//读取IP数据
	HalFlashRead(WIFI_SSID_ADDR, g_aSsid, WIFI_SSID_SIZE);
	if (!checkParamStr(g_aSsid, WIFI_SSID_SIZE))
		memset(g_aSsid, 0, WIFI_SSID_SIZE);
	HalFlashRead(WIFI_PWD_ADDR, g_aPwd, WIFI_PWD_SIZE);
	if (!checkParamStr(g_aPwd, WIFI_PWD_SIZE))
		memset(g_aPwd, 0, WIFI_PWD_SIZE);
	HalFlashRead(WIFI_SERVER_IP_ADDR, g_aServerIp, WIFI_SERVER_IP_SIZE);
	if (!checkParamStr(g_aServerIp, WIFI_SERVER_IP_SIZE))
		sprintf((char*)g_aServerIp, "192.168.1.102");
	HalFlashRead(WIFI_SERVER_PORT_ADDR, (UCHAR*)&g_uServerPort, sizeof(g_uServerPort));
	if (0xffff == g_uServerPort)
		g_uServerPort = 1234;
	HalFlashRead(WIFI_CONNET_TYPE_ADDR, (UCHAR*)&g_bTcpConnet, sizeof(g_bTcpConnet));
	if (0xff == g_bTcpConnet)
		g_bTcpConnet = 1;
	HalFlashRead(WIFI_LOCAL_IP_ADDR, g_aLocalIP, WIFI_LOCAL_IP_SIZE);
	if (!checkParamStr(g_aLocalIP, WIFI_LOCAL_IP_SIZE))
	{
		g_aLocalIP[0] = 0xC0;
		g_aLocalIP[1] = 0xA8;
		g_aLocalIP[2] = 0x01;
		g_aLocalIP[3] = 0x0B;
	}
	HalFlashRead(WIFI_LOCAL_GW_ADDR, g_aLocalGateway, WIFI_LOCAL_GW_SIZE);
	if (!checkParamStr(g_aLocalGateway, WIFI_LOCAL_GW_SIZE))
	{
		g_aLocalGateway[0] = 0xC0;
		g_aLocalGateway[1] = 0xA8;
		g_aLocalGateway[2] = 0x01;
		g_aLocalGateway[3] = 0x01;
	}
	HalFlashRead(WIFI_LOCAL_MASK_ADDR, g_aLocalMask, WIFI_LOCAL_MASK_SIZE);
	//DBG(TRACE("Mask3: %x\r\n",g_aLocalMask[3]));
	if (!checkParamStr(g_aLocalMask, WIFI_LOCAL_MASK_SIZE))
	{
		g_aLocalMask[0] = 0xFF;
		g_aLocalMask[1] = 0xFF;
		g_aLocalMask[2] = 0xFF;
		g_aLocalMask[3] = 0x00;
	}
	HalFlashRead(WIFI_LOCAL_MAC_ADDR, g_aLocalMac, WIFI_LOCAL_MAC_SIZE);
	if (!checkParamStr(g_aLocalMac, WIFI_LOCAL_MAC_SIZE))
	{
		g_aLocalMac[0] = 0xEA;
		g_aLocalMac[1] = 0x2D;
		g_aLocalMac[2] = 0x63;
		g_aLocalMac[3] = 0x17;
		g_aLocalMac[4] = 0x9D;
		g_aLocalMac[5] = 0xF5;
	}
	
	//ESP8266初始化
	esp8266_Init(hal_uart_getinstance(HAL_UART2));
	esp8266_connet_wifi(g_aSsid, g_aPwd);//连接Wifi
	esp8266_setIp(g_aLocalIP, g_aLocalGateway, g_aLocalMask);//设置IP
	esp8266_setMac(g_aLocalMac);//设置MAC地址
	esp8266_connet(g_aServerIp, g_uServerPort, g_bTcpConnet);//连接服务器
	esp8266_StartTransparent();//ESP8266进入透传模式
	
	comm_registe(comm_esp8266_getInstance(), COMM_CHANNEL1);
	hComm = comm_getInstance(COMM_CHANNEL1);
	hComm->init(esp8266_getinstance());
	hComm->add_rx_obser(OnEsp8826MsgEvent);
	osal_router_setCommPort(hComm, OSAL_ROUTE_PORT1);
#endif //CFG_ESP8266

	osal_router_init(&m_hRouterInstance);
	bd_updateunit_Init(&m_hUpdateInstance);
	
	//创建任务
	osal_task_create(taskForCheckWifiStage, 0x300000);//检查Wifi连接状态 (约 30s)
	
}

#endif //!ENABLE_BOOTLOADER_CODE
