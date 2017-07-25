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
UCHAR g_aLocalIP[WIFI_LOCAL_IP_SIZE] = {0};		/*!< ����IP */
UCHAR g_aLocalGateway[WIFI_LOCAL_GW_SIZE] = {0};	/*!< ���� */
UCHAR g_aLocalMask[WIFI_LOCAL_MASK_SIZE] = {0};	/*!< ���� */
UCHAR g_aLocalMac[WIFI_LOCAL_MAC_SIZE] = {0};		/*!< MAC��ַ */
UCHAR g_aLocalDns[WIFI_LOCAL_MASK_SIZE] = {0};	/*!< DNS */

UCHAR g_aServerIp[WIFI_SERVER_IP_SIZE] = {0};		/*!< ������IP */
BOOL g_bTcpConnet = 0;							/*!< ���ӷ�ʽ,, TURE: TCP����  FALSE: UDP���� */
UINT16 g_uServerPort = 0;						/*!< �������˿� */

#ifdef CFG_USE_WIFI
UCHAR g_aSsid[WIFI_SSID_SIZE] = {0};	/*!< Wifi���� */
UCHAR g_aPwd[WIFI_PWD_SIZE] = {0};		/*!< Wifi���� */

UINT16 g_uWifiState = 3;
UINT16 g_uWifiLostCnt = 0; /*!< Wifi ���ߴ��� */
#endif //CFG_USE_WIFI
#endif //CFG_USE_NET


