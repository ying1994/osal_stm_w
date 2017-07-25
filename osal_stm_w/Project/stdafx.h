/**
 * @file    stdafx.h
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
#ifndef __STDAFX_H_
#define __STDAFX_H_

#include "stm32f10x.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

/* hal driver resource */
#include "hal_board.h"

/* osal resource */
#include "osal.h"

#define TRACE printf


#define ENABLE_BOOTLOADER_CODE 0

#define BOOTLOADER_VERSION "BootLoader V1.0.0"
#define APPLICATION_VERSION "Appliction V1.0.0"

#ifdef _DEBUG
	#if ENABLE_BOOTLOADER_CODE
		#define FRIMWARE_VERSION BOOTLOADER_VERSION
	#else
		#define FRIMWARE_VERSION APPLICATION_VERSION" Dbg"
	#endif
#else
	#if ENABLE_BOOTLOADER_CODE
		#define FRIMWARE_VERSION BOOTLOADER_VERSION
	#else
		#define FRIMWARE_VERSION APPLICATION_VERSION
	#endif
#endif

/* driver resource define */
//#define CFG_USE_NET

/* wifi configure */
#ifdef CFG_USE_NET
#define CFG_USE_WIFI
#endif //CFG_USE_NET

/* WIZCHIP driver configure, for W5100, W5500 .etc */
#ifdef CFG_USE_NET
#define CFG_WIZCHIP
#endif //CFG_USE_NET

/* ESP8266 driver configure */
#ifdef CFG_USE_WIFI
#define CFG_ESP8266_CLIENT
#endif //CFG_USE_WIFI

/* comm on ESP8266 configure */
#ifdef CFG_ESP8266_CLIENT
#define CFG_USE_COMM_ESP8266
#endif //CFG_USE_WIFI

/* HT1621 driver configure */
//#define CFG_HT1621B
/* SHT2x driver configure, for HST20, HST21 .etc */
//#define CFG_SHT2X


/* hal driver resource */
#include "hal_board.h"

/* osal resource */
#include "osal.h"

/* proctrol resource */
#include "comm.h"

/* DebugUnit */
#include "debugunit.h"


/* bootloader */
#include "bootloader.h"

/* application */
#include "application.h"

#endif
