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
 * All rights Reserved
 *
 */
#ifndef _HAL_FLASH_H
#define _HAL_FLASH_H

#include "hal_cfg.h"
#include "hal_types.h"

#ifdef CFG_HAL_FLASH

/**
 * STM32F10x оƬ�洢�����֣�
 *
 * 0x8,000,000									Bootloader ����ַ
 *     |
 *     |		50K (bootloader program)		Bootloader Flash ��С
 *     |
 * 0x8,00C,800									Bootloader EEPROM ����ַ
 *     |
 *     |		2K (bootloader flash eeprom)	Bootloader EEPROM ��С
 *     |
 * 0x8,00C,FFF
 * 0x8,00D,000									App ����ַ
 *     |
 *     |		200K (app program)				App Flash ��С
 *     |
 * 0x8,03F,000									App EEPROM ����ַ
 *     |
 *     |		4K (app flash eeprom)			App EEPROM ��С
 *     |
 * 0x8,03F,FFF
 */
#define HAL_BOOTLOADER_BASE_ADDR 0x08000000UL												/*!< BootLoader ����ַ. */
#define HAL_BOOTLOADER_FLASH_SIZE (50*1024UL)												/*!< BootLoader ����洢����С. */
#define HAL_BOOTLOADER_EEPROM_ADDR (HAL_BOOTLOADER_BASE_ADDR + HAL_BOOTLOADER_FLASH_SIZE)	/*!< BootLoader EEPROM ����ַ. */
#define HAL_BOOTLOADER_EEPROM_SIZE (2*1024UL)												/*!< BootLoader EEPROM ��С. */

#define HAL_APP_BASE_ADDR (HAL_BOOTLOADER_EEPROM_ADDR + HAL_BOOTLOADER_EEPROM_SIZE)		/*!< APP ����ַ. */
#define HAL_APP_FLASH_SIZE (200*1024UL)													/*!< APP ����洢����С. */
#define HAL_APP_EEPROM_ADDR (HAL_APP_BASE_ADDR + HAL_APP_FLASH_SIZE)						/*!< APP EEPROM ����ַ. */
#define HAL_APP_EEPROM_SIZE (4*1024UL)														/*!< APP EEPROM ��С. */

#define HAL_IAP_EEPROM_ADDR	HAL_BOOTLOADER_EEPROM_ADDR							/*!< IAP Flash EEPROM ����ַ. */
#define HAL_IAP_EEPROM_SIZE	512													/*!< IAP Flash EEPROM ��С. */

#define HAL_FLASH_EEPROM_ADDR	HAL_APP_EEPROM_ADDR										/*!< Flash EEPROM ����ַ. */
#define HAL_FLASH_EEPROM_SIZE	(4*1024UL)														/*!< Flash EEPROM ��С. */

#define HAL_FLASH_PAGE_SIZE 		(2*1024UL)													/*!< Flash ҳ���С. */


#define APP_STATUS_ADDR HAL_IAP_EEPROM_ADDR			/*!< APP�����ִ��״̬�洢��ַ. */
#define APP_STATUS_ADDR_SIZE 2UL					/*!< APP�����ִ��״̬�洢��С. */



/** 
  * @brief  HAL FLASH Status  
  */

typedef enum
{ 
  HAL_FLASH_NOERROR = 0,
  HAL_FLASH_BUSY = 1,
  HAL_FLASH_ERROR_PG,
  HAL_FLASH_ERROR_WRP,
  HAL_FLASH_COMPLETE,
  HAL_FLASH_TIMEOUT,
  HAL_FLASH_ERROR_ADDR,
  HAL_FLASH_ERROR_SIZE
}HAL_FLASH_Status;

/**
 * @brief ���Flashд����
 * @param none
 * @return void
 */
void HalFlashUnlock(void);

/**
 * @brief ����ָ��Flashҳ��
 * @param uAddress: ����ҳ���ַ
 * @param size ̽����ҳ���С
 * @retval д��ɹ�����TRUE��ʧ�ܷ���FALSE
 */
BOOL HalFlashErase(UINT32 uAddress, UINT32 size);

/**
 * @brief дFlash
 * @param uAddress: д�����ݵ�ַ
 * @param pBuff: ���ݴ洢ָ��
 * @param size: д�����ݴ�С
 * @retval д��ɹ�����TRUE��ʧ�ܷ���FALSE
 */
BOOL HalFlashWrite(UINT32 uAddress, const UCHAR* pBuff, UINT32 size);

/**
 * @brief ��Flash
 * @param uAddress: ���ݴ洢��ַ
 * @param pBuff: ���ݴ洢ָ��
 * @param size: �������ݴ�С
 * @retval д��ɹ�����TRUE��ʧ�ܷ���FALSE
 */
BOOL HalFlashRead(UINT32 uAddress, UCHAR* pBuff, UINT32 size);

/**
 * @brief дFlash EEPROM
 * @param uAddress: д�����ݵ�ַ
 * @param pBuff: ���ݴ洢ָ��
 * @param size: д�����ݴ�С
 * @retval д��ɹ�����TRUE��ʧ�ܷ���FALSE
 */
BOOL HalFlashReadWriteE2PROW(UINT32 uAddress, UCHAR* pBuff, UINT32 size);

/**
 * @brief ȡ���һ�β���״̬��Ϣ
 * @retval �������һ�β���״̬��Ϣ
 */
UINT16 HalFlashLastStatus(void);

#endif //CFG_HAL_FLASH
#endif

/**************************************************************************************************
*/
