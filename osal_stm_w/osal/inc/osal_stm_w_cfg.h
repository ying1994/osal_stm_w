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
 * All rights res
 *
 */
#ifndef OSAL_STM_W_CFG_H
#define OSAL_STM_W_CFG_H
#include "types.h"
#include "hal_flash.h"


/**
 * App EEPROM �洢������
 */
#define APP_DEVICE_ADDRESS_ADDR (HAL_FLASH_EEPROM_ADDR)	/*!< ͨѶ��ַ�洢��ַ */
#define APP_DEVICE_ADDRESS_SIZE 4UL						/*!< ͨѶ��ַ�洢��С */


#define WIFI_SSID_ADDR (APP_DEVICE_ADDRESS_ADDR + APP_DEVICE_ADDRESS_SIZE)	/*!< WiFi ���� �洢��ַ */ 
#define WIFI_SSID_SIZE (64UL) 												/*!< WiFi ���� �洢��С*/ 
#define WIFI_PWD_ADDR (WIFI_SSID_ADDR + WIFI_SSID_SIZE)						/*!< WiFi ���� �洢��ַ */ 
#define WIFI_PWD_SIZE (64UL) 												/*!< WiFi ���� �洢��С*/ 

#define WIFI_SERVER_IP_ADDR (WIFI_PWD_ADDR + WIFI_PWD_SIZE)					/*!< ������IP �洢��ַ */ 
#define WIFI_SERVER_IP_SIZE (64UL) 											/*!< ������IP �洢��С*/ 
#define WIFI_SERVER_PORT_ADDR (WIFI_SERVER_IP_ADDR + WIFI_SERVER_IP_SIZE)	/*!< �������˿� �洢��ַ */ 
#define WIFI_SERVER_PORT_SIZE (4UL) 										/*!< �������˿� �洢��С*/ 
#define WIFI_CONNET_TYPE_ADDR (WIFI_SERVER_PORT_ADDR + WIFI_SERVER_PORT_SIZE)	/*!< ���ӷ�ʽ �洢��ַ */ 
#define WIFI_CONNET_TYPE_SIZE (4UL) 											/*!< ���ӷ�ʽ �洢��С*/ 

#define WIFI_LOCAL_IP_ADDR (WIFI_CONNET_TYPE_ADDR + WIFI_CONNET_TYPE_SIZE)	/*!< IP �洢��ַ */ 
#define WIFI_LOCAL_IP_SIZE (4UL) 											/*!< IP �洢��С*/ 
#define WIFI_LOCAL_GW_ADDR (WIFI_LOCAL_IP_ADDR + WIFI_LOCAL_IP_SIZE)	/*!< ���� �洢��ַ */ 
#define WIFI_LOCAL_GW_SIZE (4UL) 										/*!< ���� �洢��С*/ 
#define WIFI_LOCAL_MASK_ADDR (WIFI_LOCAL_GW_ADDR + WIFI_LOCAL_GW_SIZE)	/*!< ���� �洢��ַ */ 
#define WIFI_LOCAL_MASK_SIZE (4UL) 											/*!< ���� �洢��С*/ 
#define WIFI_LOCAL_MAC_ADDR (WIFI_LOCAL_MASK_ADDR + WIFI_LOCAL_MASK_SIZE)	/*!< MAC��ַ �洢��ַ */ 
#define WIFI_LOCAL_MAC_SIZE (6UL) 											/*!< MAC��ַ �洢��С*/ 
#endif //OSAL_STM_W_CFG_H
