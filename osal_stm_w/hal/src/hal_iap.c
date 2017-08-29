/**
 * @file    hal_iap.c
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
#include "hal_types.h"
#include "hal_cpu.h"
#include "hal_iap.h"
#include "hal_flash.h"
#include "cortexm3_macro.h"

#include "stdio.h"

#ifdef CFG_HAL_IAP
static UCHAR byFlashData[HAL_FLASH_PAGE_SIZE] = {0};

/**
 * @brief App������
 * @param uAddress: ��̵�ַ�����Ե�ַ(��0x800D000)
 * @param pBuff: ���ݴ洢ָ��
 * @param size: ������ݴ�С
 * @retval ��̳ɹ�����TRUE��ʧ�ܷ���FALSE
 */
BOOL HalIapProgramApp(UINT32 uAddress, const UCHAR *pBuff, UINT32 size)
{
	if(uAddress < HAL_APP_BASE_ADDR || ((uAddress + size) >= (HAL_APP_BASE_ADDR + HAL_APP_FLASH_SIZE)))//�Ƿ���ַ
		return FALSE;
	if (NULL == pBuff)
		return FALSE;
	
	
	return HalFlashWrite(uAddress, pBuff, size);
}

/**
 * @brief IAP������
 * @param uAddress: ��̵�ַ�����Ե�ַ(��0x800D000)
 * @param pBuff: ���ݴ洢ָ��
 * @param size: ������ݴ�С
 * @retval ��ȡ�ɹ�����TRUE��ʧ�ܷ���FALSE
 */
BOOL HalIapRead(UINT32 uAddress, UCHAR *pBuff, UINT32 size)
{
	if(uAddress < HAL_APP_BASE_ADDR)//�Ƿ���ַ
		return FALSE;
	
	return HalFlashRead(uAddress, pBuff, size);
}

/**
 * @brief IAPдFlash EEPROM
 * @param uAddress: д�����ݵ�ַ�����Ե�ַ(��0x8000000)
 * @param pBuff: ���ݴ洢ָ��
 * @param size: д�����ݴ�С
 * @retval д��ɹ�����TRUE��ʧ�ܷ���FALSE
 */
BOOL HalIapRW_EEPROM(UINT32 uAddress, UCHAR* pBuff, UINT32 size)
{
	BOOL bRetVal = FALSE;
	UINT32 uWriteAddr = 0;
	UINT32 uBaseAddr = 0;
	BOOL bValue = FALSE;
	UINT32 uBufIdx = 0;
	UINT32 uFlashIdx = 0;
	
	while (uBufIdx < size)
	{
		uWriteAddr = uAddress - (uAddress % HAL_FLASH_PAGE_SIZE);
		
		HalFlashRead(uWriteAddr, byFlashData, HAL_FLASH_PAGE_SIZE);
		
		uBaseAddr = uAddress % HAL_FLASH_PAGE_SIZE;
		uFlashIdx = uBaseAddr;
		for (; (uBufIdx < size) && (uFlashIdx < HAL_FLASH_PAGE_SIZE); )
		{
			if (byFlashData[uFlashIdx] != pBuff[uBufIdx])
			{
				byFlashData[uFlashIdx] = pBuff[uBufIdx];
				bValue = TRUE;
			}
			uFlashIdx++;
			uBufIdx++;
		}
		
		if (bValue)//�����б仯������EEPROM
		{
			if (HalFlashErase(uWriteAddr, HAL_FLASH_PAGE_SIZE))
			{
				if (HalFlashWrite(uWriteAddr, byFlashData, HAL_FLASH_PAGE_SIZE))
				{
					bRetVal = TRUE;
				}
			}
		}
		uWriteAddr += HAL_FLASH_PAGE_SIZE;
		uAddress += uBufIdx;
	}
	
	return bRetVal;
}

/**
 * @brief ����APP����Ŀ�ִ��״̬
 * @param bIsAppAvailable APP�����Ƿ�ִ��
 * @return void
 */
void HalIapSetIsAppActive(BOOL bIsAppAvailable)
{
	UCHAR writeData = bIsAppAvailable;
	HalIapRW_EEPROM(APP_STATUS_ADDR, &writeData, 1);
}

/**
 * @brief ��ȡAPP����Ŀ�ִ��״̬
 * @param None 
 * @return APP�����Ƿ�ִ��
 * @retval TRUEΪ�����ִ�У�FALSEΪ���򲻿�ִ��
 */
BOOL HalIapIsAppActive(void)
{
	UCHAR IsAvailable = 0;
	HalFlashRead(APP_STATUS_ADDR, &IsAvailable, 1);
	
	return (0 == !IsAvailable) ? TRUE : FALSE;
}

/**
 * @brief IAP �ص��ж��Լ�Ӳ������
 * @param None 
 * @return void
 */
static void CloseIRQHard(void)
{
	//NVIC_SETFAULTMASK();  //�ر����ж� 
	//��IO
	GPIO_DeInit(GPIOA);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, DISABLE);	 //�� ʹ��PA�˿�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,DISABLE);     //�� ����ʱ��ʱ��
	__disable_irq();   // �ر����ж�
	//���ж�
}

/**
 * @brief ��ת��ָ���ĵ�ִַ�г����������ת��APP������Ҫ��ȷ��APP��
 *        ��ִ�еĲŽ�����ת�����������ʹ������벻ȷ����״̬
 * @param uAddr: ָ����ת��Ŀ�ĵ�ַ
 * @retval None
 */
void HalIapJmp2addr(UINT32 uAddr)
{
	HAL_BASE_FUNC jump2addr = NULL;
	if (HAL_APP_BASE_ADDR == uAddr)
	{
        //�ж��û��Ƿ��Ѿ����س�����Ϊ��������´˵�ַ��ջ��ַ��
        if ((((*(volatile UINT32*)HAL_APP_BASE_ADDR) & 0x2FFE0000 ) == 0x20000000) && HalIapIsAppActive())//APP�����ִ��
		{
			TRACE("Jump to Application");
			TRACE("\r\n");
			usleep(5000);
			CloseIRQHard();
			jump2addr = (HAL_BASE_FUNC)(*(volatile UINT32*)(uAddr + 4));
			__set_MSP(*(volatile UINT32*) uAddr);
			jump2addr();
		}
		else
		{
			TRACE("Application is not active!\r\n");
		}
	}
	else
	{
		TRACE("Jump to Addr: 0X%08lX", uAddr);
		TRACE("\r\n");
		usleep(5000);
		CloseIRQHard();
		jump2addr = (HAL_BASE_FUNC)(*(volatile UINT32*)(uAddr + 4));
		__set_MSP(*(volatile UINT32*) uAddr);
		jump2addr();	
	}
}

/**
 * @brief ����ӳ���ж�������
 * @param uAddr: ӳ���ж��������ַ
 * @retval None
 */
void HalIapRemap(UINT32 uAddr)
{
	//NVIC_RESETFAULTMASK();
	//__disable_irq();   // �ر����ж�
	SCB->VTOR = uAddr;
	//__enable_irq();   // �����ж�
}

#endif //CFG_HAL_IAP
