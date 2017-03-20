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

#endif //OSAL_STM_W_CFG_H
