/**
 * @file    hal_cpu.c
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

#include "hal_cpu.h"

#define CPU_ID_LEN 12			/* CPU编号长度 */
#define CPU_ID_ADDR 0x1ffff7e8	/* CPU编号存放地址 */

static u8  fac_us=0;//us延时倍乘数
static u16 fac_ms=0;//ms延时倍乘数
static UINT32 m_uWdgTime = 0;//看门狗时间

/**
 * @brief: CPU资源初始化
 * @param: void
 * @retval: void
 */
void HalCpuInit(void)
{
	ErrorStatus HSEStartUpStatus;
	
	/* RCC system reset(for debug purpose) */
	RCC_DeInit();

	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);

	/* Wait till HSE is ready */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();

	if(HSEStartUpStatus == SUCCESS)
	{
		/* Flash 2 wait state */
		FLASH_SetLatency(FLASH_Latency_2);
		/* Enable Prefetch Buffer */
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		/* HCLK = SYSCLK */
		RCC_HCLKConfig(RCC_SYSCLK_Div1); 
		/* PCLK2 = HCLK */
		RCC_PCLK2Config(RCC_HCLK_Div1); 
		/* PCLK1 = HCLK/2 */
		RCC_PCLK1Config(RCC_HCLK_Div2);
		/* PLLCLK = 8MHz * 9 = 72 MHz */
#if !defined (STM32F10X_LD_VL) && !defined (STM32F10X_MD_VL) && !defined (STM32F10X_HD_VL) && !defined (STM32F10X_CL)
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
#else
		RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
#endif
		/* Enable PLL */ 
		RCC_PLLCmd(ENABLE);

		/* Wait till PLL is ready */
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
		}

		/* Select PLL as system clock source */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		/* Wait till PLL is used as system clock source */
		while(RCC_GetSYSCLKSource() != 0x08)
		{
		}
	}

	/* Enable GPIOX and AFIO clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | 
						   RCC_APB2Periph_GPIOB | 
						   RCC_APB2Periph_GPIOC | 
						   RCC_APB2Periph_GPIOD | 
						   RCC_APB2Periph_GPIOE | 
						   RCC_APB2Periph_GPIOF | 
						   RCC_APB2Periph_GPIOG | 
						   RCC_APB2Periph_AFIO, 
						   ENABLE);
	/* Disable JTAG & SWJ Pin, use with GPIO */
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	/* Enable LSI CLK, for IWDG */
	RCC_LSICmd(ENABLE);
	
	/* SysTick_CLK = HCLK/8 */
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); 
	fac_us = SystemCoreClock / 8000000;	//为系统时钟的1/8 
	fac_ms = SystemCoreClock / 8000;	//为系统时钟的1/8  
}


/**
 * @brief: 获取CPU唯一ID
 * @param: idBuf CPU唯一ID
 * @retval: ID的长度
 */
UCHAR HalCpuGetID(UCHAR* idBuf)
{
	UINT32 cpu_id0, cpu_id1, cpu_id2;
	if (NULL == idBuf)
		return 0;
	
	cpu_id0 = *(UINT32*)(CPU_ID_ADDR);
	idBuf[0] = (UCHAR)(cpu_id0);
	idBuf[1] = (UCHAR)(cpu_id0 >> 8);
	idBuf[2] = (UCHAR)(cpu_id0 >> 16);
	idBuf[3] = (UCHAR)(cpu_id0 >> 24);
	
	cpu_id1 = *(UINT32*)(CPU_ID_ADDR + 4);
	idBuf[4] = (UCHAR)(cpu_id1);
	idBuf[5] = (UCHAR)(cpu_id1 >> 8);
	idBuf[6] = (UCHAR)(cpu_id1 >> 16);
	idBuf[7] = (UCHAR)(cpu_id1 >> 24);
	
	cpu_id2 = *(UINT32*)(CPU_ID_ADDR + 8);
	idBuf[8] = (UCHAR)(cpu_id2);
	idBuf[9] = (UCHAR)(cpu_id2 >> 8);
	idBuf[10] = (UCHAR)(cpu_id2 >> 16);
	idBuf[11] = (UCHAR)(cpu_id2 >> 24);
	
	return CPU_ID_LEN;
}

/* 看门狗喂狗线程 */
void FreeIWDG_task(void)
{
	HalIwdgFred();
}

/**
 * @brief 独立看门狗初始化
 * @param utime 看门狗定时时间(s)
 * @retval void
 */
void HalIwdgInit(UINT32 utime)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //使能对寄存器IWDG_PR和IWDG_RLR的写操作
	
	IWDG_SetPrescaler(IWDG_Prescaler_128);  //设置IWDG预分频值:设置IWDG预分频值为64
	
	/* 
	  LSICLK = 40K
	  IWDGCLK = LSICLK / Prescaler
	  time(ms) = 1000 / IWDGCLK = 1000 * Prescaler / LSICLK = Prescaler / 40 (ms)
	  freeTime(ms) = (Prescaler / 40) * RLR
      RLR = freetime / (Prescaler / 40)
	
	  1s = 1000ms = (Prescaler / 40) * RLR
	  RLR = 1000 / (Prescaler / 40) = 1000 / ((128/4)*0.1) = 312
	*/
	m_uWdgTime = utime;
	IWDG_SetReload(312*utime);  //设置IWDG重装载值
	
	IWDG_ReloadCounter();  //重装载IWDG计数器
	
	IWDG_Enable();  //使能IWDG
}

/**
 * @brief: 独立看门狗喂狗
 * @param: void
 * @retval: void
 */
void HalIwdgFred(void)
{   
 	IWDG_ReloadCounter();		//重装载IWDG计数器								   
}

/**
 * @brief 微秒级延时
 * @param nus: 延时微秒数
 * @retval void
 */
void usleep(UINT32 nus)
{		
	UINT32 temp;	    	 
	SysTick->LOAD = nus * fac_us; //时间加载	  		 
	SysTick->VAL = 0x00;        //清空计数器
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;          //开始倒数	 
	do
	{
		if (m_uWdgTime > 0)
			HalIwdgFred();//看门狗喂狗
		temp = SysTick->CTRL;
	}
	while((temp&0x01) && !(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
	SysTick->VAL = 0X00;       //清空计数器	 
}

/**
 * @brief 毫秒级延时
 *        注意nms的范围: SysTick->LOAD为24位寄存器,所以,最大延时为:
 *        nms <= 0xffffff*8*1000/SYSCLK
 *        SYSCLK单位为Hz,nms单位为ms
 *        对72M条件下,nms<=1864 
 * @param nms: 延时毫秒数
 * @retval void
*/
void msleep(UINT16 nms)
{	 		  	  
	UINT32 temp;		   
	SysTick->LOAD = (UINT32)nms * fac_ms;//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL = 0x00;           //清空计数器
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;          //开始倒数  
	do
	{
		if (m_uWdgTime > 0)
			HalIwdgFred();//看门狗喂狗
		temp = SysTick->CTRL;
	}
	while((temp&0x01) && !(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;       //关闭计数器
	SysTick->VAL = 0X00;       //清空计数器	  	    
} 

