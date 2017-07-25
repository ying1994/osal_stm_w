/**
 * @file    global.h
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
#ifndef __GLOBAL_H_
#define __GLOBAL_H_
#include "stdafx.h"
#include "osal.h"


#ifdef CFG_USE_NET
extern UCHAR g_aLocalIP[WIFI_LOCAL_IP_SIZE];		/*!< 本地IP */
extern UCHAR g_aLocalGateway[WIFI_LOCAL_GW_SIZE];	/*!< 网关 */
extern UCHAR g_aLocalMask[WIFI_LOCAL_MASK_SIZE];	/*!< 掩码 */
extern UCHAR g_aLocalMac[WIFI_LOCAL_MAC_SIZE];		/*!< MAC地址 */
extern UCHAR g_aLocalDns[WIFI_LOCAL_MASK_SIZE];	/*!< DNS */

extern UCHAR g_aServerIp[WIFI_SERVER_IP_SIZE];		/*!< 服务器IP */
extern BOOL g_bTcpConnet;							/*!< 连接方式,, TURE: TCP连接  FALSE: UDP连接 */
extern UINT16 g_uServerPort;						/*!< 服务器端口 */

#ifdef CFG_USE_WIFI
extern UCHAR g_aSsid[WIFI_SSID_SIZE];	/*!< Wifi名称 */
extern UCHAR g_aPwd[WIFI_PWD_SIZE];		/*!< Wifi密码 */

extern UINT16 g_uWifiState;
extern UINT16 g_uWifiLostCnt; /*!< Wifi 掉线次数 */
#endif //CFG_USE_WIFI
#endif //CFG_USE_NET

#endif
