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
 * App EEPROM 存储区分配
 */
#define APP_DEVICE_ADDRESS_ADDR (HAL_FLASH_EEPROM_ADDR)	/*!< 通讯地址存储地址 */
#define APP_DEVICE_ADDRESS_SIZE 4UL						/*!< 通讯地址存储大小 */

#endif //OSAL_STM_W_CFG_H
