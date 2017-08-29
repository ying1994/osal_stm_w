/**
 * @file    bd_timer.c
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

#include "hal_timer.h"

#ifdef CFG_HAL_TIMER

/** ����ʱ�������� */
#define SHARE_TIMER_MAX 32

/** ����ʱ����׼ʱ�� */
#define SHARE_TIMER_BASE_TIME 1

/** ����ʱ����ʼ��� */
//#define INDE_TIMER_BASE_NUM 0
/** ����ʱ����ʼ��� */
#define SHARE_TIMER_BASE_NUM INDE_TIMER_MAX


/* ������ʱ����ʼ���������ԭ�� */
typedef void (*HalTimeInit_t)(BOOL bIsOn, UINT32 uus);
/* ������ʱ������������ԭ�� */
typedef void (*HalTimeClear_t)(void);

/** ����ʱ���洢�ṹ */
typedef struct
{
	HalTimerCBack_t hTimerFunc;	/*!< ��ʱ���ص�������� */
	UINT32 uCounter;				/*!< ��ʱ��ִ��ʱ��(ms) */
	UINT32 uTime;					/*!< ��ʱ��ִ��ͬ��(ms) */
}HAL_TIMER_TYPE;

/* ������ʱ��������� */
static HalTimerCBack_t m_hIndeTimer[INDE_TIMER_MAX] = {NULL};
static HalTimeClear_t m_hClearTimer[INDE_TIMER_MAX] = {NULL};
/* ����ʱ��������� */
static HAL_TIMER_TYPE m_hShareTimer[SHARE_TIMER_MAX] = {{NULL, 0}};
static UINT16 m_uShareTimerCount = 0;

/* ������ʱ����ʼ��������� */
static HalTimeInit_t m_hTimerInitFunc[INDE_TIMER_MAX] = {NULL};

/* ��ʱ��2��ʼ�� */
static void timer2_init(BOOL bIsOn, UINT32 uus)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	static BOOL bInit = FALSE;

	if (bIsOn && (uus > 0))
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��
		
		TIM_DeInit(TIM2);
		//��ʱ��TIM2��ʼ��
		TIM_TimeBaseStructure.TIM_Period = 2*uus-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
		TIM_TimeBaseStructure.TIM_Prescaler =35; //4-1 ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
		TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	 
		if (!bInit)//�ж����ȼ�NVIC����
		{
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
			NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3�ж�
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
			NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
			bInit = TRUE;
		}
		
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);//����жϱ�־λ
		TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM2�ж�,��������ж�
		TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx	
	}
	else
	{
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);//����жϱ�־λ
		TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE ); //��ָֹ����TIM3�ж�,��������ж�
		TIM_Cmd(TIM2, DISABLE);  //��ֹTIMx	
		TIM_DeInit(TIM2);
	}
}

/* ��ʱ��3���� */
static void timer2_clear(void)
{
	TIM_SetCounter(TIM2, 0);
}

/* ��ʱ��3��ʼ�� */
static void timer3_init(BOOL bIsOn, UINT32 uus)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	static BOOL bInit = FALSE;
	
	if (bIsOn && (uus > 0))
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
		
		TIM_DeInit(TIM3);
		//��ʱ��TIM3��ʼ��
		TIM_TimeBaseStructure.TIM_Period = 2*uus-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
		TIM_TimeBaseStructure.TIM_Prescaler =35; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
		TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	 
		if (!bInit)//�ж����ȼ�NVIC����
		{
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
			NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
			NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
			bInit = TRUE;
		}
		
		TIM_ClearFlag(TIM3, TIM_FLAG_Update);//����жϱ�־λ
		TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�
		TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx	
	}
	else
	{
		TIM_ClearFlag(TIM3, TIM_FLAG_Update);//����жϱ�־λ
		TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE ); //��ָֹ����TIM3�ж�,��������ж�
		TIM_Cmd(TIM3, DISABLE);  //��ֹTIMx	
		TIM_DeInit(TIM3);
	}
}

/* ��ʱ��3���� */
static void timer3_clear(void)
{
	TIM_SetCounter(TIM3, 0);
}

/* ��ʱ��4��ʼ�� */
static void timer4_init(BOOL bIsOn, UINT32 uus)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	static BOOL bInit = FALSE;
	
	if (bIsOn && (uus > 0))
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��
		
		TIM_DeInit(TIM4);
		//��ʱ��TIM4��ʼ��
		TIM_TimeBaseStructure.TIM_Period = 2*uus-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
		TIM_TimeBaseStructure.TIM_Prescaler =35; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
		TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
		TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	 

		if (!bInit)//�ж����ȼ�NVIC����
		{
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
			NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4�ж�
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
			NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
			bInit = TRUE;
		}
		
		TIM_ClearFlag(TIM4, TIM_FLAG_Update);//����жϱ�־λ
		TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM4�ж�,��������ж�
		TIM_Cmd(TIM4, ENABLE);  //ʹ��TIMx	
	}
	else
	{
		TIM_ClearFlag(TIM4, TIM_FLAG_Update);//����жϱ�־λ
		TIM_ITConfig(TIM4,TIM_IT_Update,DISABLE ); //��ָֹ����TIM4�ж�,��������ж�
		TIM_Cmd(TIM4, DISABLE);  //��ֹTIMx	
		TIM_DeInit(TIM4);
	}
}

/* ��ʱ��4���� */
static void timer4_clear(void)
{
	TIM_SetCounter(TIM4, 0);
}

/* ��ʱ��4��ʼ�� */
static void timer5_init(BOOL bIsOn, UINT32 uus)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	static BOOL bInit = FALSE;
	
	if (bIsOn && (uus > 0))
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //ʱ��ʹ��
		
		TIM_DeInit(TIM5);
		//��ʱ��TIM5��ʼ��
		TIM_TimeBaseStructure.TIM_Period = 2*uus-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
		TIM_TimeBaseStructure.TIM_Prescaler =35; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
		TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
		TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	 

		if (!bInit)//�ж����ȼ�NVIC����
		{
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
			NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4�ж�
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
			NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
			bInit = TRUE;
		}
		
		TIM_ClearFlag(TIM5, TIM_FLAG_Update);//����жϱ�־λ
		TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM4�ж�,��������ж�
		TIM_Cmd(TIM5, ENABLE);  //ʹ��TIMx	
	}
	else
	{
		TIM_ClearFlag(TIM5, TIM_FLAG_Update);//����жϱ�־λ
		TIM_ITConfig(TIM5,TIM_IT_Update,DISABLE ); //��ָֹ����TIM4�ж�,��������ж�
		TIM_Cmd(TIM5, DISABLE);  //��ֹTIMx	
		TIM_DeInit(TIM5);
	}
}

/* ��ʱ��4���� */
static void timer5_clear(void)
{
	TIM_SetCounter(TIM5, 0);
}

/* ��ʱ��6��ʼ�� */
static void timer6_init(BOOL bIsOn, UINT32 uus)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	static BOOL bInit = FALSE;
	if (bIsOn && (uus > 0))
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //ʱ��ʹ��
		
		TIM_DeInit(TIM6);
		//��ʱ��TIM6��ʼ��
		TIM_TimeBaseStructure.TIM_Period = 2*uus-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
		TIM_TimeBaseStructure.TIM_Prescaler =35; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
		TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
		TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	 

		if (!bInit)//�ж����ȼ�NVIC����
		{
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
			NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;  //TIM3�ж�
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
			NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
			bInit = TRUE;
		}
		
		TIM_ClearFlag(TIM6, TIM_FLAG_Update);//����жϱ�־λ
		TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM6�ж�,��������ж�
		TIM_Cmd(TIM6, ENABLE);  //ʹ��TIMx	
	}
	else
	{
		TIM_ClearFlag(TIM6, TIM_FLAG_Update);//����жϱ�־λ
		TIM_ITConfig(TIM6,TIM_IT_Update,DISABLE ); //��ָֹ����TIM6�ж�,��������ж�
		TIM_Cmd(TIM6, DISABLE);  //��ֹTIMx	
		TIM_DeInit(TIM6);
	}
}

/* ��ʱ��6���� */
static void timer6_clear(void)
{
	TIM_SetCounter(TIM6, 0);
}

/* ��ʱ��7��ʼ�� */
static void timer7_init(BOOL bIsOn, UINT32 ums)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	static BOOL bInit = FALSE;

	if (bIsOn && (ums > 0))
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE); //ʱ��ʹ��
		
		TIM_DeInit(TIM7);
		//��ʱ��TIM7��ʼ��
		TIM_TimeBaseStructure.TIM_Period = 2*ums-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
		TIM_TimeBaseStructure.TIM_Prescaler =35999; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
		TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
		TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	 

		if (!bInit)//�ж����ȼ�NVIC����
		{
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
			NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;  //TIM3�ж�
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
			NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
			bInit = TRUE;
		}

		TIM_ClearFlag(TIM7, TIM_FLAG_Update);//����жϱ�־λ
		TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM7�ж�,��������ж�
		TIM_Cmd(TIM7, ENABLE);  //ʹ��TIMx	
	}
	else
	{
		TIM_ClearFlag(TIM7, TIM_FLAG_Update);//����жϱ�־λ
		TIM_ITConfig(TIM7,TIM_IT_Update,DISABLE ); //��ָֹ����TIM7�ж�,��������ж�
		TIM_Cmd(TIM7, DISABLE);  //��ֹTIMx	
		TIM_DeInit(TIM7);
	}
}

/* ��ʱ��7���� */
//static void timer7_clear(void)
//{
//	TIM_SetCounter(TIM7, 0);
//}

/* ��ʱ��2�жϷ������ */
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //���TIM2�����жϷ������
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		
		if (m_hIndeTimer[INDE_TIMER_TIM2] != NULL)
			m_hIndeTimer[INDE_TIMER_TIM2]();
	}
}

/* ��ʱ��3�жϷ������ */
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		
		if (m_hIndeTimer[INDE_TIMER_TIM3] != NULL)
			m_hIndeTimer[INDE_TIMER_TIM3]();
	}
}

/* ��ʱ��4�жϷ������ */
void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //���TIM4�����жϷ������
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		
		if (m_hIndeTimer[INDE_TIMER_TIM4] != NULL)
			m_hIndeTimer[INDE_TIMER_TIM4]();
	}
}

/* ��ʱ��4�жϷ������ */
void TIM5_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)  //���TIM4�����жϷ������
	{
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		
		if (m_hIndeTimer[INDE_TIMER_TIM5] != NULL)
			m_hIndeTimer[INDE_TIMER_TIM5]();
	}
}

/* ��ʱ��6�жϷ������ */
void TIM6_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)  //���TIM6�����жϷ������
	{
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		
		if (m_hIndeTimer[INDE_TIMER_TIM6] != NULL)
			m_hIndeTimer[INDE_TIMER_TIM6]();
	}
}

/* ��ʱ��7�жϷ������ */
void TIM7_IRQHandler(void)
{
	UINT16 i;
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)  //���TIM7�����жϷ������
	{
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		
		for (i = 0; i < SHARE_TIMER_MAX; ++i)
		{
			if (m_hShareTimer[i].hTimerFunc != NULL)
			{
				m_hShareTimer[i].uCounter++;
				if (m_hShareTimer[i].uCounter >= m_hShareTimer[i].uTime)
				{
					m_hShareTimer[i].uCounter = 0;
					m_hShareTimer[i].hTimerFunc();
				}
			}
		}
	}
}

static void InitIndtTimers(void)
{
	//m_hTimerInitFunc[INDE_TIMER_TIM1] = timer1_init;
	m_hTimerInitFunc[INDE_TIMER_TIM2] = timer2_init;
	m_hTimerInitFunc[INDE_TIMER_TIM3] = timer3_init;
	m_hTimerInitFunc[INDE_TIMER_TIM4] = timer4_init;
	m_hTimerInitFunc[INDE_TIMER_TIM5] = timer5_init;
	m_hTimerInitFunc[INDE_TIMER_TIM6] = timer6_init;
	//m_hTimerInitFunc[INDE_TIMER_TIM8] = timer8_init;
	
	//m_hClearTimer[INDE_TIMER_TIM1] = timer1_clear;
	m_hClearTimer[INDE_TIMER_TIM2] = timer2_clear;
	m_hClearTimer[INDE_TIMER_TIM3] = timer3_clear;
	m_hClearTimer[INDE_TIMER_TIM4] = timer4_clear;
	m_hClearTimer[INDE_TIMER_TIM5] = timer5_clear;
	m_hClearTimer[INDE_TIMER_TIM6] = timer6_clear;
	//m_hClearTimer[INDE_TIMER_TIM8] = timer8_clear;
}

/**
 * @brief ע��һ��������ʱ��
 * @param tID ��ʱ��ID @ref IndeTimerIDsTypeDef
 * @param hFunc ����ʱ���ص��������
 * @param time  ��ʱʱ�䳤�ȣ���λ��us��
 * @retval ע��ɹ����ص�ǰ��ʱ����ţ�ʧ�ܷ���-1
 */
INT32 HalSetIndeTimer(UINT32 tID, HalTimerCBack_t hTimerFunc, UINT32 uus)
{
	INT32 uID = -1;
	if (NULL == hTimerFunc)
		return -1;
	
	if (NULL == m_hTimerInitFunc[0])
	{
		InitIndtTimers();		
	}
	
	if (tID < INDE_TIMER_MAX)
	{
		if (m_hIndeTimer[tID] != hTimerFunc)//��ʱ����ע��
		{
			m_hIndeTimer[tID] = hTimerFunc;
			m_hTimerInitFunc[tID](TRUE, uus);
			uID = tID;
		}
	}
	else
	{
		uID = HalSetShareTimer(hTimerFunc, uus);
	}
	
	return uID;
}

/**
 * @brief ע��һ������ʱ��
 * @param hFunc ����ʱ���ص��������
 * @param time  ��ʱʱ�䳤�ȣ���λ��ms��
 * @retval ע��ɹ����ص�ǰ��ʱ����ţ�ʧ�ܷ���-1
 */
INT32 HalSetShareTimer(HalTimerCBack_t hTimerFunc, UINT32 ums)
{
	UINT16 i;
	UINT32 utime = 1;
	if (NULL == hTimerFunc)
		return -1;
	if (m_uShareTimerCount >= SHARE_TIMER_MAX)//��ʱ����������
		return -1;
		
	timer7_init(TRUE, SHARE_TIMER_BASE_TIME);
	
	/* ���㹲��ʱ��ʱ��Ƭ */
	if (ums > SHARE_TIMER_BASE_TIME)
	{
		utime = ums / SHARE_TIMER_BASE_TIME;
	}
	else
	{
		utime = 1;
	}
	
	/* ����ʱ���Ѿ����ڣ�����¶�ʱ��ʱ�� */
	for (i = 0; i < m_uShareTimerCount; ++i)
	{
		if (m_hShareTimer[i].hTimerFunc == hTimerFunc)
		{
			m_hShareTimer[i].uCounter = 0;
			m_hShareTimer[i].uTime = utime;
			return i + SHARE_TIMER_BASE_NUM;
		}
	}
	
	/* ��ʱ���Ѿ������ڣ�ע��һ���µĶ�ʱ�� */
	if (i == m_uShareTimerCount)
	{
		m_hShareTimer[i].hTimerFunc = hTimerFunc;
		m_hShareTimer[i].uCounter = 0;
		m_hShareTimer[i].uTime = utime;
		m_uShareTimerCount++;
		return i + SHARE_TIMER_BASE_NUM;
	}
	
	return -1;
}


/**
 * @brief ͨ����ʱ�����ע��һ����ʱ��
 * @param hFunc ��ʱ���ص��������
 */
void HalKillTimerWithFunc(HalTimerCBack_t hTimerFunc)
{
	UINT16 i;
	
	if (NULL == m_hTimerInitFunc[0])
	{
		InitIndtTimers();
	}
	
	for (i = 0; i < INDE_TIMER_MAX; ++i)
	{
		if (hTimerFunc == m_hIndeTimer[i])//�ö�ʱ��Ϊ������ʱ��
		{
			m_hIndeTimer[i] = NULL;
			m_hTimerInitFunc[i](FALSE, 0);
			return;
		}
	}
	
	for (i = 0; i < m_uShareTimerCount; ++i)
	{
		if (hTimerFunc == m_hShareTimer[i].hTimerFunc)//�ö�ʱ��Ϊ����ʱ��
		{
			break;
		}
	}
	if (i < m_uShareTimerCount)
	{
		for (i = 0; i < m_uShareTimerCount-1; ++i)
		{
			m_hShareTimer[i].hTimerFunc = m_hShareTimer[i+1].hTimerFunc;
			m_hShareTimer[i].uCounter = m_hShareTimer[i+1].uCounter;
			m_hShareTimer[i].uTime = m_hShareTimer[i+1].uTime;
		}
		m_uShareTimerCount--;
		if (0 == m_uShareTimerCount)
			timer2_init(FALSE, 0);
	}
}

/**
 * @brief ͨ����ʱ��IDע��һ����ʱ��
 * @param uID ��ʱ��ID
 */
void HalKillTimerWithID(INT32 uID)
{
	UINT16 i;
	if (uID < 0)
		return;
	
	if (NULL == m_hTimerInitFunc[0])
	{
		InitIndtTimers();
	}
	
	if (uID < INDE_TIMER_MAX)//������ʱ��
	{
		m_hIndeTimer[uID] = NULL;
		m_hTimerInitFunc[uID](FALSE, 0);
	}
	else if (uID - SHARE_TIMER_BASE_NUM < SHARE_TIMER_MAX)//����ʱ��
	{
		i = uID - SHARE_TIMER_BASE_NUM;
		m_hShareTimer[i].hTimerFunc = NULL;
		m_hShareTimer[i].uCounter = 0;
		m_hShareTimer[i].uTime = 0;
		m_uShareTimerCount--;
		if (m_uShareTimerCount == 0)
			timer2_init(FALSE, 0);
	}
}

/**
 * @brief ͨ����ʱ����������ʱ�����¼�ʱ
 * @param hFunc ��ʱ���ص��������
 */
void HalClearTimerWithFunc(HalTimerCBack_t hTimerFunc)
{
	UINT16 i;
	
	if (NULL == m_hTimerInitFunc[0])
	{
		InitIndtTimers();
	}
	
	for (i = 0; i < INDE_TIMER_MAX; ++i)
	{
		if (hTimerFunc == m_hIndeTimer[i])//�ö�ʱ��Ϊ������ʱ��
		{
			m_hClearTimer[i]();
			return;
		}
	}
	
	for (i = 0; i < m_uShareTimerCount; ++i)
	{
		if (hTimerFunc == m_hShareTimer[i].hTimerFunc)//�ö�ʱ��Ϊ����ʱ��
		{
			break;
		}
	}
	if (i < m_uShareTimerCount)
	{
		for (i = 0; i < m_uShareTimerCount-1; ++i)
		{
			m_hShareTimer[i].uCounter = 0;
		}
	}
}

/**
 * @brief ͨ����ʱ��IDע�������ʱ�����¼�ʱ
 * @param uID ��ʱ��ID
 */
void HalClearTimerWithID(INT32 uID)
{
	UINT16 i;
	
	if (uID < 0)
		return;
	
	if (NULL == m_hTimerInitFunc[0])
	{
		InitIndtTimers();
	}
	
	if (uID < INDE_TIMER_MAX)//������ʱ��
	{
		m_hClearTimer[uID]();
	}
	else if (uID - SHARE_TIMER_BASE_NUM < SHARE_TIMER_MAX)//����ʱ��
	{
		i = uID - SHARE_TIMER_BASE_NUM;
		m_hShareTimer[i].uCounter = 0;
	}
}

#endif //CFG_HAL_TIMER
