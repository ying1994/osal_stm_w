/**
 * @file    hal_exti.c
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

#include "hal_exti.h"

#ifdef CFG_HAL_EXTI

/* 外部中断事件回调函数 */
static HalExtiCBlack_t m_hExtiEventCB[HAL_EXTI_SOURCE_MAX] = {NULL};

/**
 * @brief 注册一个外部事件中断
 * @param hPortSource: 外部事件中断触发端口 @ref HAL_EXTI_PORT
 * @param eSource: 中断源 @ref HAL_EXTI_SOURCE
 * @param eMode: 中断工作方式 @ref HAL_EXTI_MODE
 * @param eTrigger: 中断触发方式 @ref HAL_EXTI_TRIGGER
 * @param hExtiEventCB: 中断事件回调函数句柄
 * @retval None
 */
void HalExtiRegist(UINT8 hPortSource, HAL_EXTI_SOURCE eSource, HAL_EXTI_MODE eMode, HAL_EXTI_TRIGGER eTrigger, HalExtiCBlack_t hExtiEventCB)
{
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟
	
	//中断线以及中断初始化配置 
  	GPIO_EXTILineConfig(hPortSource, GPIO_PinSource0 + eSource);
	
  	EXTI_InitStructure.EXTI_Line = (UINT32)(1 << eSource);
  	EXTI_InitStructure.EXTI_Mode = (EXTIMode_TypeDef)eMode;	
  	EXTI_InitStructure.EXTI_Trigger = (EXTITrigger_TypeDef)eTrigger;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	if (eSource < HAL_EXTI_LINE5)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn + eSource;	//使能对应的外部中断通道
	}
	else if (eSource < HAL_EXTI_LINE10)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能对应的外部中断通道
	}
	else
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;		//使能对应的外部中断通道
	}
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		//子优先级2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);

	m_hExtiEventCB[eSource] = hExtiEventCB;
}


//外部中断0服务程序 
void EXTI0_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line0) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位 
		if (m_hExtiEventCB[0] != NULL)
		{
			m_hExtiEventCB[0]();
		}
	}
}
 
//外部中断1服务程序 
void EXTI1_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line1) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line1); //清除LINE1上的中断标志位  
		if (m_hExtiEventCB[1] != NULL)
		{
			m_hExtiEventCB[1]();
		}
	}
}
//外部中断2服务程序
void EXTI2_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line2) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line2); //清除LINE2上的中断标志位  
		if (m_hExtiEventCB[2] != NULL)
		{
			m_hExtiEventCB[2]();
		}
	}
}

//外部中断3服务程序
void EXTI3_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line3) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line3); //清除LINE3上的中断标志位  
		if (m_hExtiEventCB[3] != NULL)
		{
			m_hExtiEventCB[3]();
		}
	} 
}

//外部中断4服务程序
void EXTI4_IRQHandler(void)
{ 
	if (EXTI_GetITStatus(EXTI_Line4) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line4); //清除LINE4上的中断标志位  
		if (m_hExtiEventCB[4] != NULL)
		{
			m_hExtiEventCB[4]();
		}
	} 
}
 
//外部中断 9-5 服务程序
void EXTI9_5_IRQHandler(void)
{ 
	if (EXTI_GetITStatus(EXTI_Line5) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line5); //清除LINE5上的中断标志位  
		if (m_hExtiEventCB[5] != NULL)
		{
			m_hExtiEventCB[5]();
		}
	} 
	
	if (EXTI_GetITStatus(EXTI_Line6) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line6); //清除LINE6上的中断标志位  
		if (m_hExtiEventCB[6] != NULL)
		{
			m_hExtiEventCB[6]();
		}
	}
	
	if (EXTI_GetITStatus(EXTI_Line7) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line7); //清除LINE7上的中断标志位  
		if (m_hExtiEventCB[7] != NULL)
		{
			m_hExtiEventCB[7]();
		}
	}
	
	if (EXTI_GetITStatus(EXTI_Line8) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line8); //清除LINE8上的中断标志位  
		if (m_hExtiEventCB[8] != NULL)
		{
			m_hExtiEventCB[8]();
		}
	}
	
	if (EXTI_GetITStatus(EXTI_Line9) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line9); //清除LINE9上的中断标志位  
		if (m_hExtiEventCB[9] != NULL)
		{
			m_hExtiEventCB[9]();
		}
	}
}

//外部中断 15-10 服务程序
void EXTI15_10_IRQHandler(void)
{ 
	if (EXTI_GetITStatus(EXTI_Line10) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line10); //清除LINE10上的中断标志位  
		if (m_hExtiEventCB[10] != NULL)
		{
			m_hExtiEventCB[10]();
		}
	}
	
	if (EXTI_GetITStatus(EXTI_Line11) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line11); //清除LINE11上的中断标志位  
		if (m_hExtiEventCB[11] != NULL)
		{
			m_hExtiEventCB[11]();
		}
	}
	
	if (EXTI_GetITStatus(EXTI_Line12) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line12); //清除LINE12上的中断标志位  
		if (m_hExtiEventCB[12] != NULL)
		{
			m_hExtiEventCB[12]();
		}
	}
	
	if (EXTI_GetITStatus(EXTI_Line13) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line13); //清除LINE13上的中断标志位  
		if (m_hExtiEventCB[13] != NULL)
		{
			m_hExtiEventCB[13]();
		}
	}
	
	if (EXTI_GetITStatus(EXTI_Line14) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line14); //清除LINE14上的中断标志位  
		if (m_hExtiEventCB[14] != NULL)
		{
			m_hExtiEventCB[14]();
		}
	}
	
	if (EXTI_GetITStatus(EXTI_Line15) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line15); //清除LINE15上的中断标志位  
		if (m_hExtiEventCB[15] != NULL)
		{
			m_hExtiEventCB[15]();
		}
	}
}

#endif //CFG_HAL_EXTI
