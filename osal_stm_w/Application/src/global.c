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
#include "osal_stm_w_cfg.h"

#if !ENABLE_BOOTLOADER_CODE

#ifdef CFG_ESP8266
UCHAR g_aSsid[WIFI_SSID_SIZE] = {0};
UCHAR g_aPwd[WIFI_PWD_SIZE] = {0};

UCHAR g_aLocalIP[WIFI_LOCAL_IP_SIZE] = {0};
UCHAR g_aLocalGateway[WIFI_LOCAL_GW_SIZE] = {0};
UCHAR g_aLocalMask[WIFI_LOCAL_MASK_SIZE] = {0};
UCHAR g_aLocalMac[WIFI_LOCAL_MAC_SIZE] = {0};

UCHAR g_aServerIp[WIFI_SERVER_IP_SIZE] = {0};
BOOL g_bTcpConnet = 0;
UINT16 g_uServerPort = 0;

UINT16 g_uWifiState = 3;
#endif //CFG_ESP8266


#endif //!ENABLE_BOOTLOADER_CODE

