/**
 * @file    hal_flash.h
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
#ifndef _HAL_FLASH_H
#define _HAL_FLASH_H

/* ------------------------------------------------------------------------------------------------
 *                                          Includes
 * ------------------------------------------------------------------------------------------------
 */

#include "hal_board.h"
#include "hal_types.h"

#ifdef CFG_HAL_FLASH

/**
 * STM32F10x 芯片存储区划分：
 *
 * 0x8,000,000									Bootloader 基地址
 *     |
 *     |		50K (bootloader program)		Bootloader Flash 大小
 *     |
 * 0x8,00C,800									Bootloader EEPROM 基地址
 *     |
 *     |		2K (bootloader flash eeprom)	Bootloader EEPROM 大小
 *     |
 * 0x8,00C,FFF
 * 0x8,00D,000									App 基地址
 *     |
 *     |		200K (app program)				App Flash 大小
 *     |
 * 0x8,03F,000									App EEPROM 基地址
 *     |
 *     |		4K (app flash eeprom)			App EEPROM 大小
 *     |
 * 0x8,03F,FFF
 */
#define HAL_BOOTLOADER_BASE_ADDR 0x08000000UL												/*!< BootLoader 基地址. */
#define HAL_BOOTLOADER_FLASH_SIZE (50*1024UL)												/*!< BootLoader 程序存储区大小. */
#define HAL_BOOTLOADER_EEPROM_ADDR (HAL_BOOTLOADER_BASE_ADDR + HAL_BOOTLOADER_FLASH_SIZE)	/*!< BootLoader EEPROM 基地址. */
#define HAL_BOOTLOADER_EEPROM_SIZE (2*1024UL)												/*!< BootLoader EEPROM 大小. */

#define HAL_APP_BASE_ADDR (HAL_BOOTLOADER_EEPROM_ADDR + HAL_BOOTLOADER_EEPROM_SIZE)		/*!< APP 基地址. */
#define HAL_APP_FLASH_SIZE (200*1024UL)													/*!< APP 程序存储区大小. */
#define HAL_APP_EEPROM_ADDR (HAL_APP_BASE_ADDR + HAL_APP_FLASH_SIZE)						/*!< APP EEPROM 基地址. */
#define HAL_APP_EEPROM_SIZE (4*1024UL)														/*!< APP EEPROM 大小. */

#define HAL_IAP_EEPROM_ADDR	HAL_BOOTLOADER_EEPROM_ADDR							/*!< IAP Flash EEPROM 基地址. */
#define HAL_IAP_EEPROM_SIZE	512													/*!< IAP Flash EEPROM 大小. */

#define HAL_FLASH_EEPROM_ADDR	HAL_APP_EEPROM_ADDR										/*!< Flash EEPROM 基地址. */
#define HAL_FLASH_EEPROM_SIZE	512														/*!< Flash EEPROM 大小. */

#define HAL_FLASH_PAGE_SIZE 		(2*1024UL)													/*!< Flash 页面大小. */


#define APP_STATUS_ADDR HAL_IAP_EEPROM_ADDR			/*!< APP程序可执行状态存储地址. */
#define APP_STATUS_ADDR_SIZE 2UL					/*!< APP程序可执行状态存储大小. */

/**
 * @brief 解除Flash写保护
 * @param none
 * @return void
 */
void HalFlashUnlock(void);

/**
 * @brief 擦除指定Flash页面
 * @param uAddress: 擦除页面地址
 * @param size 探擦除页面大小
 * @retval 写入成功返回TRUE，失败返回FALSE
 */
BOOL HalFlashErase(UINT32 uAddress, UINT32 size);

/**
 * @brief 写Flash
 * @param uAddress: 写入数据地址
 * @param pBuff: 数据存储指针
 * @param size: 写入数据大小
 * @retval 写入成功返回TRUE，失败返回FALSE
 */
BOOL HalFlashWrite(UINT32 uAddress, const UCHAR* pBuff, UINT32 size);

/**
 * @brief 读Flash
 * @param uAddress: 数据存储地址
 * @param pBuff: 数据存储指针
 * @param size: 读出数据大小
 * @retval 写入成功返回TRUE，失败返回FALSE
 */
BOOL HalFlashRead(UINT32 uAddress, UCHAR* pBuff, UINT32 size);

/**
 * @brief 写Flash EEPROM
 * @param uAddress: 写入数据地址
 * @param pBuff: 数据存储指针
 * @param size: 写入数据大小
 * @retval 写入成功返回TRUE，失败返回FALSE
 */
BOOL HalFlashReadWriteE2PROW(UINT32 uAddress, UCHAR* pBuff, UINT32 size);

#endif //CFG_HAL_FLASH
#endif

/**************************************************************************************************
*/
