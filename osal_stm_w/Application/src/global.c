/**
 * @file    global.c
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
#include "osal.h"

#ifdef CFG_USE_NET
UCHAR g_aLocalIP[WIFI_LOCAL_IP_SIZE] = {0};		/*!< 本地IP */
UCHAR g_aLocalGateway[WIFI_LOCAL_GW_SIZE] = {0};	/*!< 网关 */
UCHAR g_aLocalMask[WIFI_LOCAL_MASK_SIZE] = {0};	/*!< 掩码 */
UCHAR g_aLocalMac[WIFI_LOCAL_MAC_SIZE] = {0};		/*!< MAC地址 */
UCHAR g_aLocalDns[WIFI_LOCAL_MASK_SIZE] = {0};	/*!< DNS */

UCHAR g_aServerIp[WIFI_SERVER_IP_SIZE] = {0};		/*!< 服务器IP */
BOOL g_bTcpConnet = 0;							/*!< 连接方式,, TURE: TCP连接  FALSE: UDP连接 */
UINT16 g_uServerPort = 0;						/*!< 服务器端口 */

#ifdef CFG_USE_WIFI
UCHAR g_aSsid[WIFI_SSID_SIZE] = {0};	/*!< Wifi名称 */
UCHAR g_aPwd[WIFI_PWD_SIZE] = {0};		/*!< Wifi密码 */

UINT16 g_uWifiState = 3;
UINT16 g_uWifiLostCnt = 0; /*!< Wifi 掉线次数 */
#endif //CFG_USE_WIFI
#endif //CFG_USE_NET


