/**
 * @file    debugunit.c
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   调试单元
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
#include "esp8266_client.h"
#include "debugunit.h"

//查询BootLoader版本
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

//查询App版本
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

//查询芯片ID
static void OnGetChipID(MsgTypeDef* pMsg)
{
	if (OP_GET == pMsg->opType)
	{
		pMsg->len = HalCpuGetID(pMsg->data);
		pMsg->opType = OP_STATUS;
		
		osal_router_sendMsg(pMsg->uSerPort, pMsg);
	}
}

#ifdef CFG_ESP8266_CLIENT
//本地IP消息
static void OnNetLocalIpEvent(MsgTypeDef* pMsg)
{
	if (OP_GET == pMsg->opType)
	{
		memcpy(&pMsg->data[0], g_aLocalIP, WIFI_LOCAL_IP_SIZE);
		memcpy(&pMsg->data[WIFI_LOCAL_IP_SIZE], g_aLocalGateway, WIFI_LOCAL_GW_SIZE);
		memcpy(&pMsg->data[WIFI_LOCAL_IP_SIZE+WIFI_LOCAL_GW_SIZE], g_aLocalMask, WIFI_LOCAL_MASK_SIZE);
		memcpy(&pMsg->data[WIFI_LOCAL_IP_SIZE+WIFI_LOCAL_GW_SIZE+WIFI_LOCAL_MASK_SIZE], g_aLocalMac, WIFI_LOCAL_MAC_SIZE);
		
		pMsg->len = WIFI_LOCAL_IP_SIZE + WIFI_LOCAL_GW_SIZE + WIFI_LOCAL_MASK_SIZE + WIFI_LOCAL_MAC_SIZE;
		pMsg->opType = OP_STATUS;
		
		osal_router_sendMsg(pMsg->uSerPort, pMsg);
	}
	else if (OP_SETGET == pMsg->opType)
	{
		memcpy(g_aLocalIP, &pMsg->data[0], WIFI_LOCAL_IP_SIZE);
		memcpy(g_aLocalGateway, &pMsg->data[WIFI_LOCAL_IP_SIZE], WIFI_LOCAL_GW_SIZE);
		memcpy(g_aLocalMask, &pMsg->data[WIFI_LOCAL_IP_SIZE+WIFI_LOCAL_GW_SIZE], WIFI_LOCAL_MASK_SIZE);
		memcpy(g_aLocalMac, &pMsg->data[WIFI_LOCAL_IP_SIZE+WIFI_LOCAL_GW_SIZE+WIFI_LOCAL_MASK_SIZE], WIFI_LOCAL_MAC_SIZE);
		HalFlashReadWriteE2PROW(WIFI_LOCAL_IP_ADDR, g_aLocalIP, WIFI_LOCAL_IP_SIZE);
		HalFlashReadWriteE2PROW(WIFI_LOCAL_GW_ADDR, g_aLocalGateway, WIFI_LOCAL_GW_SIZE);
		HalFlashReadWriteE2PROW(WIFI_LOCAL_MASK_ADDR, g_aLocalMask, WIFI_LOCAL_MASK_SIZE);
		HalFlashReadWriteE2PROW(WIFI_LOCAL_MAC_ADDR, g_aLocalMac, WIFI_LOCAL_MAC_SIZE);
		pMsg->opType = OP_STATUS;
		
		osal_router_sendMsg(pMsg->uSerPort, pMsg);
	}
}

//服务器IP参数消息
static void OnNetServerIpEvent(MsgTypeDef* pMsg)
{
	UINT16 uIpLen = 0;
	UINT16 uServerPort = 0;
	if (OP_GET == pMsg->opType)
	{
		uIpLen = strlen((const char*)g_aServerIp);
		memcpy(&pMsg->data[2], g_aServerIp, uIpLen);
		
		pMsg->len = uIpLen+5;
		
		uServerPort = osal_sock_htons(g_uServerPort);
		pMsg->data[uIpLen+2] = (UCHAR)(uServerPort >> 8);
		pMsg->data[uIpLen+3] = (UCHAR)(uServerPort);
		
		pMsg->data[uIpLen+4] = (UCHAR)(g_bTcpConnet);
		
		uIpLen = osal_sock_htons(uIpLen);
		pMsg->data[0] = (UCHAR)(uIpLen >> 8);
		pMsg->data[1] = (UCHAR)(uIpLen);
		
		pMsg->opType = OP_STATUS;
		osal_router_sendMsg(pMsg->uSerPort, pMsg);
	}
	else if (OP_SETGET == pMsg->opType)
	{
		uIpLen = (pMsg->data[0] << 8) | pMsg->data[1];
		uIpLen = osal_sock_ntohs(uIpLen);
		
		memset(g_aServerIp, 0, sizeof(g_aServerIp));
		memcpy(g_aServerIp, &pMsg->data[2], uIpLen);
		
		uServerPort = (pMsg->data[uIpLen+2] << 8) | pMsg->data[uIpLen+3];
		g_uServerPort = osal_sock_ntohs(uServerPort);
		
		g_bTcpConnet = pMsg->data[uIpLen+4];
		
		HalFlashReadWriteE2PROW(WIFI_SERVER_IP_ADDR, g_aServerIp, WIFI_SERVER_IP_SIZE);
		HalFlashReadWriteE2PROW(WIFI_SERVER_PORT_ADDR, (UCHAR*)&g_uServerPort, sizeof(g_uServerPort));
		HalFlashReadWriteE2PROW(WIFI_CONNET_TYPE_ADDR, (UCHAR*)&g_bTcpConnet, sizeof(g_bTcpConnet));
		
		pMsg->opType = OP_STATUS;
		
		osal_router_sendMsg(pMsg->uSerPort, pMsg);
	}
}

//Wifi 参数消息
static void OnNetWifiParamEvent(MsgTypeDef* pMsg)
{
	UINT16 uSsidLen = 0;
	UINT16 uPwdLen = 0;
	if (OP_GET == pMsg->opType)
	{
		uSsidLen = strlen((const char*)g_aSsid);
		memcpy(&pMsg->data[2], g_aSsid, uSsidLen);
		
		uPwdLen = strlen((const char*)g_aPwd);
		memcpy(&pMsg->data[uSsidLen+4], g_aPwd, uPwdLen);
		
		pMsg->len = uSsidLen + uPwdLen +4;
		
		uPwdLen = osal_sock_htons(uPwdLen);
		pMsg->data[2+uSsidLen] = (UCHAR)(uPwdLen>>8);
		pMsg->data[3+uSsidLen] = (UCHAR)(uPwdLen);
		
		uSsidLen = osal_sock_htons(uSsidLen);
		pMsg->data[0] = (UCHAR)(uSsidLen>>8);
		pMsg->data[1] = (UCHAR)(uSsidLen);
		
		
		pMsg->opType = OP_STATUS;
		osal_router_sendMsg(pMsg->uSerPort, pMsg);
	}
	else if (OP_SETGET == pMsg->opType)
	{
		uSsidLen = (pMsg->data[0] << 8) | pMsg->data[1];
		uSsidLen = osal_sock_ntohs(uSsidLen);
		uPwdLen = (pMsg->data[uSsidLen+2] << 8) | pMsg->data[uSsidLen+3];
		uPwdLen = osal_sock_ntohs(uPwdLen);
		memset(g_aSsid, 0, sizeof(g_aSsid));
		memset(g_aPwd, 0, sizeof(g_aPwd));
		memcpy(g_aSsid, &pMsg->data[2], uSsidLen);
		memcpy(g_aPwd, &pMsg->data[2+uSsidLen+2], uPwdLen);
		HalFlashReadWriteE2PROW(WIFI_SSID_ADDR, g_aSsid, WIFI_SSID_SIZE);
		HalFlashReadWriteE2PROW(WIFI_PWD_ADDR, g_aPwd, WIFI_PWD_SIZE);
		
		pMsg->opType = OP_STATUS;
		
		osal_router_sendMsg(pMsg->uSerPort, pMsg);
	}
}

//网络连接 参数消息
static void OnNetWifiConnetEvent(MsgTypeDef* pMsg)
{
	UCHAR u8ConnetType;
	UINT16 u16ConnetStatus = g_uWifiState;
	if (OP_SETGET == pMsg->opType)
	{
		u8ConnetType = pMsg->data[0];
		if (1 == u8ConnetType)
		{
			esp8266_client_connet_wifi(g_aSsid, g_aPwd);
		}
		else if (2 == u8ConnetType)
		{
			esp8266_client_connet(g_aServerIp, g_uServerPort, g_bTcpConnet);
		}
		g_uWifiState = esp8266_client_check();
	}
	
	if (OP_GET & pMsg->opType)
	{
		u16ConnetStatus = g_uWifiState;
		u16ConnetStatus = osal_sock_htons(u16ConnetStatus);
		pMsg->data[0] = (UCHAR)(u16ConnetStatus>>8);
		pMsg->data[1] = (UCHAR)(u16ConnetStatus);
		pMsg->len = 2;
		
		pMsg->opType = OP_STATUS;
		
		osal_router_sendMsg(pMsg->uSerPort, pMsg);
	}
}

//网络数据发送测试 参数消息
static void OnNetMsgTestEvent(MsgTypeDef* pMsg)
{
	UINT16 ret = 0;
	if (pMsg->len > 0)
	{
		ret = esp8266_client_write(pMsg->data, pMsg->len);
	}
	
	if (OP_GET & pMsg->opType)
	{
		pMsg->opType = (ret > 0) ? OP_STATUS : OP_ERROR;
		
		osal_router_sendMsg(pMsg->uSerPort, pMsg);
	}
}
#endif //CFG_ESP8266_CLIENT

/**
 * @brief 调试单元消息处理
 * @param pMsg 接收到的消息句柄
 * @return void
 */
void DebugUnit_OnMsgEvent(MsgTypeDef* pMsg)
{
	//DBG(TRACE("DebugUnit_OnMsgEvent\r\n"));
	if (pMsg->blockID == DEBUG_UNIT)
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
#ifdef CFG_ESP8266_CLIENT
		case NET_LOCAL_IP:
			OnNetLocalIpEvent(pMsg);
			break;
		case NET_SERVER_IP:
			OnNetServerIpEvent(pMsg);
			break;
		case NET_WIFI_PARAM:
			OnNetWifiParamEvent(pMsg);
			break;
		case NET_WIFI_CONNET:
			OnNetWifiConnetEvent(pMsg);
			break;
		case NET_MSG_TEST:
			OnNetMsgTestEvent(pMsg);
			break;
#endif //CFG_ESP8266_CLIENT
		default:
			break;
		}
	}
}
