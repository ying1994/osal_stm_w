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

/* �ⲿ�ж��¼��ص����� */
static HalExtiCBlack_t m_hExtiEventCB[HAL_EXTI_SOURCE_MAX] = {NULL};

/**
 * @brief ע��һ���ⲿ�¼��ж�
 * @param hPortSource: �ⲿ�¼��жϴ����˿� @ref HAL_EXTI_PORT
 * @param eSource: �ж�Դ @ref HAL_EXTI_SOURCE
 * @param eMode: �жϹ�����ʽ @ref HAL_EXTI_MODE
 * @param eTrigger: �жϴ�����ʽ @ref HAL_EXTI_TRIGGER
 * @param hExtiEventCB: �ж��¼��ص��������
 * @retval None
 */
void HalExtiRegist(UINT8 hPortSource, HAL_EXTI_SOURCE eSource, HAL_EXTI_MODE eMode, HAL_EXTI_TRIGGER eTrigger, HalExtiCBlack_t hExtiEventCB)
{
 	EXTI_InitTypeDef EXTI_InitStructure;
 	NVIC_InitTypeDef NVIC_InitStructure;

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��
	
	//�ж����Լ��жϳ�ʼ������ 
  	GPIO_EXTILineConfig(hPortSource, GPIO_PinSource0 + eSource);
	
  	EXTI_InitStructure.EXTI_Line = (UINT32)(1 << eSource);
  	EXTI_InitStructure.EXTI_Mode = (EXTIMode_TypeDef)eMode;	
  	EXTI_InitStructure.EXTI_Trigger = (EXTITrigger_TypeDef)eTrigger;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	if (eSource < HAL_EXTI_LINE5)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn + eSource;	//ʹ�ܶ�Ӧ���ⲿ�ж�ͨ��
	}
	else if (eSource < HAL_EXTI_LINE10)
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//ʹ�ܶ�Ӧ���ⲿ�ж�ͨ��
	}
	else
	{
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;		//ʹ�ܶ�Ӧ���ⲿ�ж�ͨ��
	}
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//��ռ���ȼ�2�� 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);

	m_hExtiEventCB[eSource] = hExtiEventCB;
}


//�ⲿ�ж�0������� 
void EXTI0_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line0) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line0); //���LINE0�ϵ��жϱ�־λ 
		if (m_hExtiEventCB[0] != NULL)
		{
			m_hExtiEventCB[0]();
		}
	}
}
 
//�ⲿ�ж�1������� 
void EXTI1_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line1) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line1); //���LINE1�ϵ��жϱ�־λ  
		if (m_hExtiEventCB[1] != NULL)
		{
			m_hExtiEventCB[1]();
		}
	}
}
//�ⲿ�ж�2�������
void EXTI2_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line2) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line2); //���LINE2�ϵ��жϱ�־λ  
		if (m_hExtiEventCB[2] != NULL)
		{
			m_hExtiEventCB[2]();
		}
	}
}

//�ⲿ�ж�3�������
void EXTI3_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line3) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line3); //���LINE3�ϵ��жϱ�־λ  
		if (m_hExtiEventCB[3] != NULL)
		{
			m_hExtiEventCB[3]();
		}
	} 
}

//�ⲿ�ж�4�������
void EXTI4_IRQHandler(void)
{ 
	if (EXTI_GetITStatus(EXTI_Line4) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line4); //���LINE4�ϵ��жϱ�־λ  
		if (m_hExtiEventCB[4] != NULL)
		{
			m_hExtiEventCB[4]();
		}
	} 
}
 
//�ⲿ�ж� 9-5 �������
void EXTI9_5_IRQHandler(void)
{ 
	if (EXTI_GetITStatus(EXTI_Line5) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line5); //���LINE5�ϵ��жϱ�־λ  
		if (m_hExtiEventCB[5] != NULL)
		{
			m_hExtiEventCB[5]();
		}
	} 
	
	if (EXTI_GetITStatus(EXTI_Line6) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line6); //���LINE6�ϵ��жϱ�־λ  
		if (m_hExtiEventCB[6] != NULL)
		{
			m_hExtiEventCB[6]();
		}
	}
	
	if (EXTI_GetITStatus(EXTI_Line7) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line7); //���LINE7�ϵ��жϱ�־λ  
		if (m_hExtiEventCB[7] != NULL)
		{
			m_hExtiEventCB[7]();
		}
	}
	
	if (EXTI_GetITStatus(EXTI_Line8) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line8); //���LINE8�ϵ��жϱ�־λ  
		if (m_hExtiEventCB[8] != NULL)
		{
			m_hExtiEventCB[8]();
		}
	}
	
	if (EXTI_GetITStatus(EXTI_Line9) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line9); //���LINE9�ϵ��жϱ�־λ  
		if (m_hExtiEventCB[9] != NULL)
		{
			m_hExtiEventCB[9]();
		}
	}
}

//�ⲿ�ж� 15-10 �������
void EXTI15_10_IRQHandler(void)
{ 
	if (EXTI_GetITStatus(EXTI_Line10) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line10); //���LINE10�ϵ��жϱ�־λ  
		if (m_hExtiEventCB[10] != NULL)
		{
			m_hExtiEventCB[10]();
		}
	}
	
	if (EXTI_GetITStatus(EXTI_Line11) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line11); //���LINE11�ϵ��жϱ�־λ  
		if (m_hExtiEventCB[11] != NULL)
		{
			m_hExtiEventCB[11]();
		}
	}
	
	if (EXTI_GetITStatus(EXTI_Line12) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line12); //���LINE12�ϵ��жϱ�־λ  
		if (m_hExtiEventCB[12] != NULL)
		{
			m_hExtiEventCB[12]();
		}
	}
	
	if (EXTI_GetITStatus(EXTI_Line13) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line13); //���LINE13�ϵ��жϱ�־λ  
		if (m_hExtiEventCB[13] != NULL)
		{
			m_hExtiEventCB[13]();
		}
	}
	
	if (EXTI_GetITStatus(EXTI_Line14) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line14); //���LINE14�ϵ��жϱ�־λ  
		if (m_hExtiEventCB[14] != NULL)
		{
			m_hExtiEventCB[14]();
		}
	}
	
	if (EXTI_GetITStatus(EXTI_Line15) == SET)
	{
		EXTI_ClearITPendingBit(EXTI_Line15); //���LINE15�ϵ��жϱ�־λ  
		if (m_hExtiEventCB[15] != NULL)
		{
			m_hExtiEventCB[15]();
		}
	}
}

#endif //CFG_HAL_EXTI
