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
#include "osal_stm_w_cfg.h"

#if !ENABLE_BOOTLOADER_CODE

#ifdef CFG_ESP8266
extern UCHAR g_aSsid[WIFI_SSID_SIZE];
extern UCHAR g_aPwd[WIFI_PWD_SIZE];

extern UCHAR g_aLocalIP[WIFI_LOCAL_IP_SIZE];
extern UCHAR g_aLocalGateway[WIFI_LOCAL_GW_SIZE];
extern UCHAR g_aLocalMask[WIFI_LOCAL_MASK_SIZE];
extern UCHAR g_aLocalMac[WIFI_LOCAL_MAC_SIZE];

extern UCHAR g_aServerIp[WIFI_SERVER_IP_SIZE];
extern BOOL g_bTcpConnet;
extern UINT16 g_uServerPort;

extern UINT16 g_uWifiState;

#endif //CFG_ESP8266

#endif //!ENABLE_BOOTLOADER_CODE
#endif
