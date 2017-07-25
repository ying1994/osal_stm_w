/**
 * @file    hal_flash.c
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

#include "hal_flash.h"

#ifdef CFG_HAL_FLASH

static UCHAR byFlashData[HAL_FLASH_PAGE_SIZE] = {0};
static UINT16 m_uLastStatus = 0;
/**
 * @brief 计算Falsh页
 * @param Size:文件长度
 * @retval 页的数量
 */
static UINT32 HalFlashMask(UINT32 size)
{
    UINT32 pagenumber = 0;
	UINT32 index = 0;

	while (1)
	{
		pagenumber++;
		index += HAL_FLASH_PAGE_SIZE;
		if (index >= size)
			break;
	}
    return pagenumber;

}

/**
 * @brief 解除Flash写保护
 * @param none
 * @return void
 */
void HalFlashUnlock(void)
{
	uint32_t UserMemoryMask = 0;
    uint32_t useroptionbyte = 0, WRPR = 0;
    uint16_t var1 = OB_IWDG_SW, var2 = OB_STOP_NoRST, var3 = OB_STDBY_NoRST;
    FLASH_Status status = FLASH_BUSY;

    WRPR = FLASH_GetWriteProtectionOptionByte();

    //测试是否写保护
    if ((WRPR & UserMemoryMask) != UserMemoryMask)
    {
        useroptionbyte = FLASH_GetUserOptionByte();

        UserMemoryMask |= WRPR;

        status = FLASH_EraseOptionBytes();

        if (UserMemoryMask != 0xFFFFFFFF)
        {
            status = FLASH_EnableWriteProtection((uint32_t)~UserMemoryMask);
        }
        //用处选项字是否有编程
        if ((useroptionbyte & 0x07) != 0x07)
        {
            //重新保存选项字
            if ((useroptionbyte & 0x01) == 0x0)
            {
                var1 = OB_IWDG_HW;
            }
            if ((useroptionbyte & 0x02) == 0x0)
            {
                var2 = OB_STOP_RST;
            }
            if ((useroptionbyte & 0x04) == 0x0)
            {
                var3 = OB_STDBY_RST;
            }

            FLASH_UserOptionByteConfig(var1, var2, var3);
        }

        if (status == FLASH_COMPLETE)
        {
            NVIC_SystemReset();
        }
    }
}

/**
 * @brief 擦除指定Flash页面
 * @param uAddress: 擦除页面地址
 * @param size 探擦除页面大小
 * @retval 写入成功返回TRUE，失败返回FALSE
 */
BOOL HalFlashErase(UINT32 uAddress, UINT32 size)
{
	BOOL bRetVal = FALSE;
	UINT32 nbrOfPage = 0;
	UINT32 uEraseCounter = 0;
	FLASH_Status status;
	
	//计算需要擦除Flash的页
	nbrOfPage = HalFlashMask(size);

	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	
	//擦除Flash
	status = FLASH_COMPLETE;
	m_uLastStatus = status;
	for (uEraseCounter = 0; (uEraseCounter < nbrOfPage) && (status == FLASH_COMPLETE); uEraseCounter++)
	{
		status = FLASH_ErasePage(uAddress + (HAL_FLASH_PAGE_SIZE * uEraseCounter));
		m_uLastStatus = status;
	}
	//status = FLASH_ErasePage(uAddress);
	if (FLASH_COMPLETE == status)
	{
		bRetVal = TRUE;
	}
	FLASH_Lock();
	return bRetVal;
}

/**
 * @brief 写Flash
 * @param uAddress: 写入数据地址
 * @param pBuff: 数据存储指针
 * @param size: 写入数据大小
 * @retval 写入成功返回TRUE，失败返回FALSE
 */
BOOL HalFlashWrite(UINT32 uAddress, const UCHAR* pBuff, UINT32 size)
{
	UINT32 i = 0;
	FLASH_Status status;
	BOOL bMemoryProgramStatus = TRUE;
	//UINT16 uWriteData = 0;
	UINT16 *pWriteData = NULL;
	UINT32 uWriteAddress = 0;
	
	pWriteData = (UINT16*)pBuff;
	uWriteAddress = uAddress;
	
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	
	status = FLASH_COMPLETE;
	for (i = 0; (i < size) && (FLASH_COMPLETE == status); i += 2)
	{
		//uWriteData = pBuff[i+1];
		//uWriteData = (uWriteData << 8) | pBuff[i];
		status = FLASH_ProgramHalfWord(uWriteAddress, *pWriteData);
		m_uLastStatus = status;
		if (*(UINT16*)uWriteAddress != *pWriteData)
		{
			bMemoryProgramStatus = FALSE;
			break;
		}
		pWriteData++;
		uWriteAddress += 2;
	}
	
	if (FLASH_COMPLETE != status)
	{
		bMemoryProgramStatus = FALSE;
	}
	
	FLASH_Lock();
	
	return bMemoryProgramStatus;
}

/**
 * @brief 读Flash
 * @param uAddress: 数据存储地址
 * @param pBuff: 数据存储指针
 * @param size: 读出数据大小
 * @retval 写入成功返回TRUE，失败返回FALSE
 */
BOOL HalFlashRead(UINT32 uAddress, UCHAR* pBuff, UINT32 size)
{
	UINT32 i;
	if (NULL == pBuff)
		return FALSE;
	
	for(i=0; i < size; ++i)
	{
		pBuff[i] = (UCHAR)(*(volatile UINT32*)(uAddress + i));
		//pBuff[i] = (UCHAR)(*(volatile UINT8*)(uAddress + i));
	}
	return TRUE;
}

/**
 * @brief 写Flash EEPROM
 * @param uAddress: 写入数据地址
 * @param pBuff: 数据存储指针
 * @param size: 写入数据大小
 * @retval 写入成功返回TRUE，失败返回FALSE
 */
BOOL HalFlashReadWriteE2PROW(UINT32 uAddress, UCHAR* pBuff, UINT32 size)
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
 * @brief 取最后一次操作状态信息
 * @retval 返回最后一次操作状态信息
 */
UINT16 HalFlashLastStatus(void)
{
	return m_uLastStatus;
}

#endif //CFG_HAL_FLASH
