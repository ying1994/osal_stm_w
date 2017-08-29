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
 * @brief App程序编程
 * @param uAddress: 编程地址，绝对地址(如0x800D000)
 * @param pBuff: 数据存储指针
 * @param size: 编程数据大小
 * @retval 编程成功返回TRUE，失败返回FALSE
 */
BOOL HalIapProgramApp(UINT32 uAddress, const UCHAR *pBuff, UINT32 size)
{
	if(uAddress < HAL_APP_BASE_ADDR || ((uAddress + size) >= (HAL_APP_BASE_ADDR + HAL_APP_FLASH_SIZE)))//非法地址
		return FALSE;
	if (NULL == pBuff)
		return FALSE;
	
	
	return HalFlashWrite(uAddress, pBuff, size);
}

/**
 * @brief IAP读数据
 * @param uAddress: 编程地址，绝对地址(如0x800D000)
 * @param pBuff: 数据存储指针
 * @param size: 编程数据大小
 * @retval 读取成功返回TRUE，失败返回FALSE
 */
BOOL HalIapRead(UINT32 uAddress, UCHAR *pBuff, UINT32 size)
{
	if(uAddress < HAL_APP_BASE_ADDR)//非法地址
		return FALSE;
	
	return HalFlashRead(uAddress, pBuff, size);
}

/**
 * @brief IAP写Flash EEPROM
 * @param uAddress: 写入数据地址，绝对地址(如0x8000000)
 * @param pBuff: 数据存储指针
 * @param size: 写入数据大小
 * @retval 写入成功返回TRUE，失败返回FALSE
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
		
		if (bValue)//数据有变化，更新EEPROM
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
 * @brief 设置APP程序的可执行状态
 * @param bIsAppAvailable APP程序是否执行
 * @return void
 */
void HalIapSetIsAppActive(BOOL bIsAppAvailable)
{
	UCHAR writeData = bIsAppAvailable;
	HalIapRW_EEPROM(APP_STATUS_ADDR, &writeData, 1);
}

/**
 * @brief 获取APP程序的可执行状态
 * @param None 
 * @return APP程序是否执行
 * @retval TRUE为程序可执行，FALSE为程序不可执行
 */
BOOL HalIapIsAppActive(void)
{
	UCHAR IsAvailable = 0;
	HalFlashRead(APP_STATUS_ADDR, &IsAvailable, 1);
	
	return (0 == !IsAvailable) ? TRUE : FALSE;
}

/**
 * @brief IAP 关掉中断以及硬件外设
 * @param None 
 * @return void
 */
static void CloseIRQHard(void)
{
	//NVIC_SETFAULTMASK();  //关闭总中断 
	//关IO
	GPIO_DeInit(GPIOA);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, DISABLE);	 //关 使能PA端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,DISABLE);     //关 辅助时能时钟
	__disable_irq();   // 关闭总中断
	//关中断
}

/**
 * @brief 跳转到指定的地址执行程序，如果是跳转到APP程序则要先确定APP是
 *        可执行的才进行跳转操作，否则会使程序进入不确定的状态
 * @param uAddr: 指定跳转的目的地址
 * @retval None
 */
void HalIapJmp2addr(UINT32 uAddr)
{
	HAL_BASE_FUNC jump2addr = NULL;
	if (HAL_APP_BASE_ADDR == uAddr)
	{
        //判断用户是否已经下载程序，因为正常情况下此地址是栈地址。
        if ((((*(volatile UINT32*)HAL_APP_BASE_ADDR) & 0x2FFE0000 ) == 0x20000000) && HalIapIsAppActive())//APP程序可执行
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
 * @brief 重新映射中断向量表
 * @param uAddr: 映射中断向量表地址
 * @retval None
 */
void HalIapRemap(UINT32 uAddr)
{
	//NVIC_RESETFAULTMASK();
	//__disable_irq();   // 关闭总中断
	SCB->VTOR = uAddr;
	//__enable_irq();   // 打开总中断
}

#endif //CFG_HAL_IAP
