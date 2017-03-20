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
 * All rights res
 *
 */
#include "stdafx.h"
#include "hal_timer.h"

#ifdef CFG_HAL_TIMER

/** ������ʱ�������� */
#define INDE_TIMER_MAX 4
/** ����ʱ�������� */
#define SHARE_TIMER_MAX 32

/** ����ʱ����׼ʱ�� */
#define SHARE_TIMER_BASE_TIME 50

/** ����ʱ����ʼ��� */
#define INDE_TIMER_BASE_NUM 0
/** ����ʱ����ʼ��� */
#define SHARE_TIMER_BASE_NUM INDE_TIMER_MAX


/* ������ʱ����ʼ���������ԭ�� */
typedef void (*HalTimeInit_t)(BOOL bIsOn, UINT32 ums);

/** ����ʱ���洢�ṹ */
typedef struct
{
	HalTimerCBack_t hTimerFunc;	/*!< ��ʱ���ص�������� */
	UINT32 uCounter;				/*!< ��ʱ��ִ��ʱ��(ms) */
	UINT32 uTime;					/*!< ��ʱ��ִ��ͬ��(ms) */
}HAL_TIMER_TYPE;

/* ������ʱ��������� */
static HalTimerCBack_t m_hIndeTimer[INDE_TIMER_MAX] = {NULL};
/* ����ʱ��������� */
static HAL_TIMER_TYPE m_hShareTimer[SHARE_TIMER_MAX] = {{NULL, 0}};
static UINT16 m_uShareTimerCount = 0;

/* ������ʱ����ʼ��������� */
static HalTimeInit_t m_hTimerInitFunc[INDE_TIMER_MAX] = {NULL};

/* ��ʱ��2��ʼ�� */
static void timer2_init(BOOL bIsOn, UINT32 ums)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	static BOOL bInit = FALSE;

	if (bIsOn)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��
		
		TIM_DeInit(TIM2);
		//��ʱ��TIM2��ʼ��
		TIM_TimeBaseStructure.TIM_Period = 2*ums-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
		TIM_TimeBaseStructure.TIM_Prescaler =35999; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
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

/* ��ʱ��3��ʼ�� */
static void timer3_init(BOOL bIsOn, UINT32 ums)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	static BOOL bInit = FALSE;
	
	if (bIsOn)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
		
		TIM_DeInit(TIM3);
		//��ʱ��TIM3��ʼ��
		TIM_TimeBaseStructure.TIM_Period = 2*ums-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
		TIM_TimeBaseStructure.TIM_Prescaler =35999; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
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

/* ��ʱ��4��ʼ�� */
static void timer4_init(BOOL bIsOn, UINT32 ums)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	static BOOL bInit = FALSE;
	
	if (bIsOn)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��
		
		TIM_DeInit(TIM4);
		//��ʱ��TIM4��ʼ��
		TIM_TimeBaseStructure.TIM_Period = 2*ums-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
		TIM_TimeBaseStructure.TIM_Prescaler =35999; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
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

/* ��ʱ��6��ʼ�� */
static void timer6_init(BOOL bIsOn, UINT32 ums)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	static BOOL bInit = FALSE;
	if (bIsOn)
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //ʱ��ʹ��
		
		TIM_DeInit(TIM6);
		//��ʱ��TIM6��ʼ��
		TIM_TimeBaseStructure.TIM_Period = 2*ums-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
		TIM_TimeBaseStructure.TIM_Prescaler =35999; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
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

/* ��ʱ��7��ʼ�� */
static void timer7_init(BOOL bIsOn, UINT32 ums)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	static BOOL bInit = FALSE;

	if (bIsOn)
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

/* ��ʱ��2�жϷ������ */
void TIM2_IRQHandler(void)
{
	UINT16 i;
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //���TIM2�����жϷ������
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		
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

/* ��ʱ��3�жϷ������ */
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		
		if (m_hIndeTimer[0] != NULL)
			m_hIndeTimer[0]();
	}
}

/* ��ʱ��4�жϷ������ */
void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //���TIM4�����жϷ������
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		
		if (m_hIndeTimer[1] != NULL)
			m_hIndeTimer[1]();
	}
}

/* ��ʱ��6�жϷ������ */
void TIM6_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)  //���TIM6�����жϷ������
	{
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		
		if (m_hIndeTimer[2] != NULL)
			m_hIndeTimer[2]();
	}
}

/* ��ʱ��7�жϷ������ */
void TIM7_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)  //���TIM7�����жϷ������
	{
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		
		if (m_hIndeTimer[3] != NULL)
			m_hIndeTimer[3]();
	}
}

/**
 * @brief ע��һ��������ʱ��
 * @param hFunc ����ʱ���ص��������
 * @param time  ��ʱʱ�䳤�ȣ���λ��ms��
 * @retval ע��ɹ����ص�ǰ��ʱ����ţ�ʧ�ܷ���-1
 */
INT32 HalSetIndeTimer(HalTimerCBack_t hTimerFunc, UINT32 ums)
{
	UINT16 i = 0;
	INT32 uID = -1;
	if (NULL == hTimerFunc)
		return -1;
	
	if (NULL == m_hTimerInitFunc[0])
	{
		m_hTimerInitFunc[0] = timer3_init;
		m_hTimerInitFunc[1] = timer4_init;
		m_hTimerInitFunc[2] = timer6_init;
		m_hTimerInitFunc[3] = timer7_init;
	}
	
	for (i = 0; i < INDE_TIMER_MAX; ++i)
	{
		if (m_hIndeTimer[i] == hTimerFunc)//��ʱ����ע��
		{
			m_hTimerInitFunc[i](TRUE, ums);
			uID = i + INDE_TIMER_BASE_NUM;
			break;
		}
	}
	if (uID < 0)//��ʱ��δע��
	{
		for (i = 0; i < INDE_TIMER_MAX; ++i)
		{
			if (NULL == m_hIndeTimer[i])
			{
				m_hIndeTimer[i] = hTimerFunc;
				m_hTimerInitFunc[i](TRUE, ums);
				uID = i + INDE_TIMER_BASE_NUM;
				break;
			}
		}
		if (uID < 0)//������ʱ����Դ���㣬ע�Ṳ��ʱ��
		{
			uID = HalSetShareTimer(hTimerFunc, ums);
		}
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
		
	timer2_init(TRUE, SHARE_TIMER_BASE_TIME);
	
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
		m_hTimerInitFunc[0] = timer3_init;
		m_hTimerInitFunc[1] = timer4_init;
		m_hTimerInitFunc[2] = timer6_init;
		m_hTimerInitFunc[3] = timer7_init;
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
void HalKillTimerWithID(UINT32 uID)
{
	UINT16 i;
	
	if (NULL == m_hTimerInitFunc[0])
	{
		m_hTimerInitFunc[0] = timer3_init;
		m_hTimerInitFunc[1] = timer4_init;
		m_hTimerInitFunc[2] = timer6_init;
		m_hTimerInitFunc[3] = timer7_init;
	}
	
	if (uID - INDE_TIMER_BASE_NUM < INDE_TIMER_MAX)//������ʱ��
	{
		i = uID - INDE_TIMER_BASE_NUM;
		m_hIndeTimer[i] = NULL;
		m_hTimerInitFunc[i](FALSE, 0);
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

#endif //CFG_HAL_TIMER
