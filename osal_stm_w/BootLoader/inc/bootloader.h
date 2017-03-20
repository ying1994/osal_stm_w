/**
 * @file    bootloader.h
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
#ifndef __BD_BOOTLOADER_H_
#define __BD_BOOTLOADER_H_
#include "stdafx.h"

#if ENABLE_BOOTLOADER_CODE
/**
 * @brief BootLoader≥ı ºªØ
 * @param None
 * @retval None
 */
void bootloader_init(void);

#endif //ENABLE_BOOTLOADER_CODE
#endif
