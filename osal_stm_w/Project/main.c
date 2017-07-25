/**
 * @file    main.c
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
#include "stdafx.h"
#include "stdio.h"

//bootloader: 	[0x8000000, size:0x40000]
//application:	[0x800D000, size:0x33000]

#if ENABLE_BOOTLOADER_CODE
	#define VECT_TAB_OFFSET 0x0000
#else
	#define VECT_TAB_OFFSET 0xD000
#endif


static void showFrimwareVersion(void)
{
	CHAR* pVersion = NULL;
	UINT len = 0;
	UINT i;
	
	printf("%s", "FrimwareVersion: ");
	
	pVersion = FRIMWARE_VERSION;
	len = sizeof(FRIMWARE_VERSION)-1;
	for (i=0; i<len; ++i)
	{
		printf("%c", pVersion[i]);
	}
	printf("\r\n");
}


int main(void)
{
	__enable_irq(); 
	SCB->VTOR = NVIC_VectTab_FLASH | VECT_TAB_OFFSET;
	
	HalCpuInit();
	//hal_rtc_init();
	hal_uart_getinstance(HAL_UART1)->init();
	
	showFrimwareVersion();

#if ENABLE_BOOTLOADER_CODE
	/*TODO: Add Bootloader code here */
	bootloader_init();
#else
	/*TODO: Add application code here */
	HalIwdgInit(30);
	osal_task_create(FreeIWDG_task, 2000);//看门狗喂狗任务
	application_init();
#endif
	/* run message task */
	while(1)
	{
		osal_task_run();
	}
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number*/
	printf("Wrong parameters value: file %s on line %d\r\n", file, line) ;

	/* Infinite loop */
	while (1)
	{}
}

#endif
