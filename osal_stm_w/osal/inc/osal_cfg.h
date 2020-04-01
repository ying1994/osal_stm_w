/**
 * @file    osal_stm_w_cfg.h
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
 * All rights Reserved
 *
 */
#ifndef OSAL_STM_W_CFG_H
#define OSAL_STM_W_CFG_H

#define CFG_OSAL_COMM
#define CFG_OSAL_COMM2
//#define CFG_OSAL_COMM3
//#define CFG_OSAL_COMM4
//#define CFG_OSAL_COMM5
#define CFG_OSAL_ROUTER
#define CFG_OSAL_UPDATEUNIT


/**
 * App EEPROM 存储区分配
 */
#define APP_DEVICE_ADDRESS_ADDR (HAL_FLASH_EEPROM_ADDR)	/*!< 通讯地址存储地址 */
#define APP_DEVICE_ADDRESS_SIZE 4UL						/*!< 通讯地址存储大小 */

#define APP_DEVICE_KIND_ADDR (APP_DEVICE_ADDRESS_ADDR + APP_DEVICE_ADDRESS_SIZE)	/*!< 设备类型 存储地址 */ 
#define APP_DEVICE_KIND_SIZE (2UL) 												/*!< 设备类型 存储大小*/ 

#define WIFI_SSID_ADDR (APP_DEVICE_KIND_ADDR + APP_DEVICE_KIND_SIZE)	/*!< WiFi 名称 存储地址 */ 
#define WIFI_SSID_SIZE (64UL) 												/*!< WiFi 名称 存储大小*/ 
#define WIFI_PWD_ADDR (WIFI_SSID_ADDR + WIFI_SSID_SIZE)						/*!< WiFi 密码 存储地址 */ 
#define WIFI_PWD_SIZE (64UL) 												/*!< WiFi 密码 存储大小*/ 

#define WIFI_SERVER_IP_ADDR (WIFI_PWD_ADDR + WIFI_PWD_SIZE)					/*!< 服务器IP 存储地址 */ 
#define WIFI_SERVER_IP_SIZE (64UL) 											/*!< 服务器IP 存储大小*/ 
#define WIFI_SERVER_PORT_ADDR (WIFI_SERVER_IP_ADDR + WIFI_SERVER_IP_SIZE)	/*!< 服务器端口 存储地址 */ 
#define WIFI_SERVER_PORT_SIZE (4UL) 										/*!< 服务器端口 存储大小*/ 
#define WIFI_CONNET_TYPE_ADDR (WIFI_SERVER_PORT_ADDR + WIFI_SERVER_PORT_SIZE)	/*!< 连接方式 存储地址 */ 
#define WIFI_CONNET_TYPE_SIZE (4UL) 											/*!< 连接方式 存储大小*/ 

#define WIFI_LOCAL_IP_ADDR (WIFI_CONNET_TYPE_ADDR + WIFI_CONNET_TYPE_SIZE)	/*!< IP 存储地址 */ 
#define WIFI_LOCAL_IP_SIZE (4UL) 											/*!< IP 存储大小*/ 
#define WIFI_LOCAL_GW_ADDR (WIFI_LOCAL_IP_ADDR + WIFI_LOCAL_IP_SIZE)	/*!< 网关 存储地址 */ 
#define WIFI_LOCAL_GW_SIZE (4UL) 										/*!< 网关 存储大小*/ 
#define WIFI_LOCAL_MASK_ADDR (WIFI_LOCAL_GW_ADDR + WIFI_LOCAL_GW_SIZE)	/*!< 掩码 存储地址 */ 
#define WIFI_LOCAL_MASK_SIZE (4UL) 											/*!< 掩码 存储大小*/ 
#define WIFI_LOCAL_MAC_ADDR (WIFI_LOCAL_MASK_ADDR + WIFI_LOCAL_MASK_SIZE)	/*!< MAC地址 存储地址 */ 
#define WIFI_LOCAL_MAC_SIZE (6UL) 											/*!< MAC地址 存储大小*/ 
#define WIFI_LOCAL_DNS_ADDR (WIFI_LOCAL_MAC_ADDR + WIFI_LOCAL_MAC_SIZE)		/*!< DNSμ??· ′?′￠μ??· */ 
#define WIFI_LOCAL_DNS_SIZE (6UL) 											/*!< DNSμ??· ′?′￠′óD?*/ 

#endif //OSAL_STM_W_CFG_H
