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

#define _DEBUG

#define TRACE printf

#ifdef _DEBUG
#define DBG(code) code 
#else
#define DBG(code)
#endif


#define ENABLE_BOOTLOADER_CODE 0

#define BOOTLOADER_VERSION "BootLoader V1.0.0"

#ifdef _DEBUG
	#if ENABLE_BOOTLOADER_CODE
		#define FRIMWARE_VERSION BOOTLOADER_VERSION
	#else
		#define FRIMWARE_VERSION "ApplicationVersion_Dbg"
	#endif
#else
	#if ENABLE_BOOTLOADER_CODE
		#define FRIMWARE_VERSION BOOTLOADER_VERSION
	#else
		#define FRIMWARE_VERSION "ApplicationVersion"
	#endif
#endif

/* hal driver resource */
#include "hal_cpu.h"
#include "hal_exti.h"
#include "hal_gpio.h"
#include "hal_i2c.h"
#include "hal_spi.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "hal_flash.h"
#include "hal_iap.h"
#include "hal_crc.h"
#include "hal_rtc.h"

/* osal resource */
#include "osal.h"
#include "osal_sock.h"
#include "osal_task.h"
#include "osal_router.h"



/* proctrol resource */
#include "unit.h"
#include "function.h"
#include "optype.h"


/* UpdateUnit */
#include "updateunit.h"

/* DebugUnit */
#include "debugunit.h"


/* bootloader */
#include "bootloader.h"

/* application */
#include "application.h"

#endif
