/**
 * @file    osal_net.c
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

#include "osal_net.h"
#include "osal.h"

#ifdef CFG_USE_NET
/**
 * @brief 检查字符串参数有效性（字符串不能全为0xFF）
 * @param msg 待检查字符串
 * @param len 字符串长度
 * @return 参数有效返回TRUE，否则返回FALSE
 */
BOOL osal_net_CheckParamStr(UCHAR* msg, UINT16 len)
{
	UINT16 i = 0;
    for (i=0; i< len; i++)
    {
        if (msg[i] != 0xff)
            return TRUE;
    }
    return FALSE;
}
/**
 * @brief 网络初始化
 * @param None
 * @return void
 */
void osal_net_init(void)
{
#ifdef CFG_USE_WIFI	
	//读取Wifi参数
	HalFlashRead(WIFI_SSID_ADDR, g_aSsid, WIFI_SSID_SIZE);
	if (!osal_net_CheckParamStr(g_aSsid, WIFI_SSID_SIZE))
		sprintf((char*)g_aSsid, "");
	HalFlashRead(WIFI_PWD_ADDR, g_aPwd, WIFI_PWD_SIZE);
	if (!osal_net_CheckParamStr(g_aPwd, WIFI_PWD_SIZE))
		sprintf((char*)g_aPwd, "");
#endif
		//读取IP数据
	HalFlashRead(WIFI_SERVER_IP_ADDR, g_aServerIp, WIFI_SERVER_IP_SIZE);
	if (!osal_net_CheckParamStr(g_aServerIp, WIFI_SERVER_IP_SIZE))
	{
		memset(g_aServerIp, 0, sizeof(g_aServerIp));
		g_aServerIp[0] = 0xC0;
		g_aServerIp[1] = 0xA8;
		g_aServerIp[2] = 0x01;
		g_aServerIp[3] = 0x0C;
	}
	HalFlashRead(WIFI_SERVER_PORT_ADDR, (UCHAR*)&g_uServerPort, sizeof(g_uServerPort));
	if (0xffff == g_uServerPort)
		g_uServerPort = 1234;
	HalFlashRead(WIFI_CONNET_TYPE_ADDR, (UCHAR*)&g_bTcpConnet, sizeof(g_bTcpConnet));
	if (0xff == g_bTcpConnet)
		g_bTcpConnet = 1;
	HalFlashRead(WIFI_LOCAL_IP_ADDR, g_aLocalIP, WIFI_LOCAL_IP_SIZE);
	if (!osal_net_CheckParamStr(g_aLocalIP, WIFI_LOCAL_IP_SIZE))
	{
		g_aLocalIP[0] = 0xC0;
		g_aLocalIP[1] = 0xA8;
		g_aLocalIP[2] = 0x01;
		g_aLocalIP[3] = 0x0B;
	}
	HalFlashRead(WIFI_LOCAL_GW_ADDR, g_aLocalGateway, WIFI_LOCAL_GW_SIZE);
	if (!osal_net_CheckParamStr(g_aLocalGateway, WIFI_LOCAL_GW_SIZE))
	{
		g_aLocalGateway[0] = 0xC0;
		g_aLocalGateway[1] = 0xA8;
		g_aLocalGateway[2] = 0x01;
		g_aLocalGateway[3] = 0x01;
	}
	HalFlashRead(WIFI_LOCAL_MASK_ADDR, g_aLocalMask, WIFI_LOCAL_MASK_SIZE);
	if (!osal_net_CheckParamStr(g_aLocalMask, WIFI_LOCAL_MASK_SIZE))
	{
		g_aLocalMask[0] = 0xFF;
		g_aLocalMask[1] = 0xFF;
		g_aLocalMask[2] = 0xFF;
		g_aLocalMask[3] = 0x00;
	}
	HalFlashRead(WIFI_LOCAL_MAC_ADDR, g_aLocalMac, WIFI_LOCAL_MAC_SIZE);
	if (!osal_net_CheckParamStr(g_aLocalMac, WIFI_LOCAL_MAC_SIZE))
	{
		g_aLocalMac[0] = 0xEA;
		g_aLocalMac[1] = 0x2D;
		g_aLocalMac[2] = 0x63;
		g_aLocalMac[3] = 0x17;
		g_aLocalMac[4] = 0x9D;
		g_aLocalMac[5] = 0xF5;
	}
	HalFlashRead(WIFI_LOCAL_DNS_ADDR, g_aLocalDns, WIFI_LOCAL_DNS_SIZE);
	if (!osal_net_CheckParamStr(g_aLocalDns, WIFI_LOCAL_DNS_SIZE))
	{
		g_aLocalDns[0] = 0x08;
		g_aLocalDns[1] = 0x08;
		g_aLocalDns[2] = 0x08;
		g_aLocalDns[3] = 0x08;
	}
}

#endif //CFG_USE_NET
