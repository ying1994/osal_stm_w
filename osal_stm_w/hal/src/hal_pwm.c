/**
 * @file    hal_pwm.c
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
#include "hal_pwm.h"

#if defined(CFG_HAL_PWM) && defined(CFG_HAL_TIMER)

static HALPwmTypeDef m_Instance[INDE_TIMER_MAX];
static HALPwmTypeDef *m_pthis[INDE_TIMER_MAX] = {NULL};

static UINT16 m_uPeriod[INDE_TIMER_MAX] = {0};
static TIM_OCInitTypeDef  m_TIM_OCInitStructure[INDE_TIMER_MAX];

static TIM_TypeDef* GetTimHandle(HalIndeTimerIDsTypeDef id)
{
	TIM_TypeDef* TIMx = NULL;
	switch (id)
	{
	case INDE_TIMER_TIM1:
		TIMx = TIM1;
		break;
	case INDE_TIMER_TIM2:
		TIMx = TIM2;
		break;
	case INDE_TIMER_TIM3:
		TIMx = TIM3;
		break;
	case INDE_TIMER_TIM4:
		TIMx = TIM4;
		break;
	case INDE_TIMER_TIM5:
		TIMx = TIM5;
		break;
	case INDE_TIMER_TIM6:
		TIMx = TIM6;
		break;
	case INDE_TIMER_TIM8:
		TIMx = TIM8;
		break;
	default:
		break;
	}
	return TIMx;
}

/* ��ʱ�� PWM ģʽ ��ʼ�� */
static void timer_pwm_init(HalIndeTimerIDsTypeDef id, UINT32 uus, UINT16 OCMode, UINT16 uPolarity)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TypeDef* TIMx = GetTimHandle(id);
	
	TIM_DeInit(TIMx);
	//��ʱ��TIM2��ʼ��
	m_uPeriod[id] = (uus>1) ? uus-1 : 0;
	TIM_TimeBaseStructure.TIM_Period = m_uPeriod[id]; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =71; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	/* ��ʱ�����ͨ��1ģʽ���� */
	/* ģʽ���ã�PWMģʽ1 */
	if (!IS_TIM_OC_MODE(OCMode))
		OCMode = TIM_OCMode_PWM1;
	m_TIM_OCInitStructure[id].TIM_OCMode = OCMode;
	/* ���״̬���ã�ʹ����� */
	m_TIM_OCInitStructure[id].TIM_OutputState = TIM_OutputState_Enable;	
	/* ����ͨ�����״̬���ã�ʹ����� */
	if ((TIMx == TIM1) || (TIMx == TIM8))
		m_TIM_OCInitStructure[id].TIM_OutputNState = TIM_OutputNState_Enable;
	/* ��������ֵ�������������������ֵʱ����ƽ�������� */
	m_TIM_OCInitStructure[id].TIM_Pulse = 0;
	/* ����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ */
	if (uPolarity != HAL_PWM_Polarity_High)
		uPolarity = HAL_PWM_Polarity_Low;
	m_TIM_OCInitStructure[id].TIM_OCPolarity = uPolarity;
	if ((TIMx == TIM1) || (TIMx == TIM8))
	{
		m_TIM_OCInitStructure[id].TIM_OCIdleState = TIM_OCIdleState_Set;
		m_TIM_OCInitStructure[id].TIM_OCNIdleState = TIM_OCIdleState_Reset;		
	}

	
	/* ��ʼ����ʱ��ͨ��1���PWM */
	TIM_OC1Init(TIMx, &m_TIM_OCInitStructure[id]);
	/* ��ʱ���Ƚ����ͨ��1Ԥװ�����ã�ʹ��Ԥװ�� */
	TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
	
	/* ��ʼ����ʱ��ͨ��2���PWM */
	TIM_OC2Init(TIMx, &m_TIM_OCInitStructure[id]);
	/* ��ʱ���Ƚ����ͨ��2Ԥװ�����ã�ʹ��Ԥװ�� */
	TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);
	
	/* ��ʼ����ʱ��ͨ��3���PWM */
	TIM_OC3Init(TIMx, &m_TIM_OCInitStructure[id]);
	/* ��ʱ���Ƚ����ͨ��3Ԥװ�����ã�ʹ��Ԥװ�� */
	TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);
	
	/* ��ʼ����ʱ��ͨ��4���PWM */
	TIM_OC4Init(TIMx, &m_TIM_OCInitStructure[id]);
	/* ��ʱ���Ƚ����ͨ��4Ԥװ�����ã�ʹ��Ԥװ�� */
	TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);

	/* ʹ�ܶ�ʱ�����ؼĴ���ARR */
	TIM_ARRPreloadConfig(TIMx, ENABLE);
	TIM_Cmd(TIMx, ENABLE);  //ʹ��TIMx	
	
	/* TIM�����ʹ�� */
	if ((TIMx == TIM1) || (TIMx == TIM8))
		TIM_CtrlPWMOutputs(TIMx, ENABLE);
}

/* ��ʱ��2���� */
static void timer_pwm_deInit(HalIndeTimerIDsTypeDef id)
{
	TIM_TypeDef* TIMx = GetTimHandle(id);
	TIM_ARRPreloadConfig(TIMx, DISABLE);
	TIM_Cmd(TIMx, DISABLE);  //��ֹTIMx	
	TIM_DeInit(TIMx);
}

/* ��ʱ�� ����PWMռ�ձ� */
static void timer_pwm_SetDutyCycle(HalIndeTimerIDsTypeDef id, UINT8 eCh, UINT8 dutycycle)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_TypeDef* TIMx = GetTimHandle(id);
	
	dutycycle = (dutycycle > 100) ? 100 : dutycycle;
	/* ģʽ���ã�PWMģʽ1 */
	TIM_OCInitStructure.TIM_OCMode = m_TIM_OCInitStructure[id].TIM_OCMode;
	/* ���״̬���ã�ʹ����� */
	TIM_OCInitStructure.TIM_OutputState = m_TIM_OCInitStructure[id].TIM_OutputState;	
	/* ��������ֵ�������������������ֵʱ����ƽ�������� */
	TIM_OCInitStructure.TIM_Pulse = m_uPeriod[id] * (dutycycle / 100.0);
	/* ����ʱ������ֵС��CCR1_ValʱΪ�ߵ�ƽ */
	TIM_OCInitStructure.TIM_OCPolarity = m_TIM_OCInitStructure[id].TIM_OCPolarity;
	
	switch (eCh)
	{
	case HAL_PWM_CH1: //��ʱ��ͨ��1
		/* ��ʼ����ʱ��ͨ��1���PWM */
		TIM_OC1Init(TIMx, &TIM_OCInitStructure);
		/* ��ʱ���Ƚ����ͨ��1Ԥװ�����ã�ʹ��Ԥװ�� */
		TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
		break;
	case HAL_PWM_CH2: //��ʱ��ͨ��2
		/* ��ʼ����ʱ��ͨ��2���PWM */
		TIM_OC2Init(TIMx, &TIM_OCInitStructure);
		/* ��ʱ���Ƚ����ͨ��2Ԥװ�����ã�ʹ��Ԥװ�� */
		TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);
		break;
	case HAL_PWM_CH3: //��ʱ��ͨ��3
		/* ��ʼ����ʱ��ͨ��3���PWM */
		TIM_OC3Init(TIMx, &TIM_OCInitStructure);
		/* ��ʱ���Ƚ����ͨ��3Ԥװ�����ã�ʹ��Ԥװ�� */
		TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);
		break;
	case HAL_PWM_CH4: //��ʱ��ͨ��4
		/* ��ʼ����ʱ��ͨ��4���PWM */
		TIM_OC4Init(TIMx, &TIM_OCInitStructure);
		/* ��ʱ���Ƚ����ͨ��4Ԥװ�����ã�ʹ��Ԥװ�� */
		TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);
		break;
	default:
		break;
	}
}

/* ��ʱ��1 PWM ģʽ ��ʼ�� */
static void timer1_pwm_init(UINT32 uus, UINT16 uPolarity)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //ʱ��ʹ��
	
	/* ���ö�ʱ��ͨ��1�������ģʽ�������������ģʽ */
	GPIO_InitStructure.GPIO_Pin =  TIM1_PWM_CH1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TIM1_PWM_CH1_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��2�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM1_PWM_CH2_PIN;
	GPIO_Init(TIM1_PWM_CH2_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��3�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM1_PWM_CH3_PIN;
	GPIO_Init(TIM1_PWM_CH3_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��4�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM1_PWM_CH4_PIN;
	GPIO_Init(TIM1_PWM_CH4_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ������ͨ��1�������ģʽ�������������ģʽ */
	GPIO_InitStructure.GPIO_Pin =  TIM1_PWM_CH1N_PIN;
	GPIO_Init(TIM1_PWM_CH1_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ������ͨ��2�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM1_PWM_CH2N_PIN;
	GPIO_Init(TIM1_PWM_CH2_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ������ͨ��3�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM1_PWM_CH3N_PIN;
	GPIO_Init(TIM1_PWM_CH3_PORT, &GPIO_InitStructure);

	timer_pwm_init(INDE_TIMER_TIM1, uus, TIM_OCMode_PWM1, uPolarity);
}

/* ��ʱ��1���� */
static void timer1_pwm_deInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	/* ���ö�ʱ��ͨ��1�������ģʽ����������ģʽ */
	GPIO_InitStructure.GPIO_Pin =  TIM1_PWM_CH1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TIM1_PWM_CH1_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��2�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM1_PWM_CH2_PIN;
	GPIO_Init(TIM1_PWM_CH2_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��3�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM1_PWM_CH3_PIN;
	GPIO_Init(TIM1_PWM_CH3_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��4�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM1_PWM_CH4_PIN;
	GPIO_Init(TIM1_PWM_CH4_PORT, &GPIO_InitStructure);
	
	/* ���ö�ʱ������ͨ��1�������ģʽ����������ģʽ */
	GPIO_InitStructure.GPIO_Pin =  TIM1_PWM_CH1N_PIN;
	GPIO_Init(TIM1_PWM_CH1_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ������ͨ��2�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM1_PWM_CH2N_PIN;
	GPIO_Init(TIM1_PWM_CH2_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ������ͨ��3�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM1_PWM_CH3N_PIN;
	GPIO_Init(TIM1_PWM_CH3_PORT, &GPIO_InitStructure);

	timer_pwm_deInit(INDE_TIMER_TIM1);
}

/* ��ʱ��1 ����PWMռ�ձ� */
static void timer1_pwm_SetDutyCycle(UINT8 eCh, UINT8 dutycycle)
{
	//if (eCh > HAL_PWM_CH_SIZE)
	//	eCh = HAL_PWM_CH_SIZE;
	
	timer_pwm_SetDutyCycle(INDE_TIMER_TIM1, eCh, dutycycle);
}

/* ��ʱ��2 PWM ģʽ ��ʼ�� */
static void timer2_pwm_init(UINT32 uus, UINT16 uPolarity)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��
	
	/* ���ö�ʱ��ͨ��1�������ģʽ�������������ģʽ */
	GPIO_InitStructure.GPIO_Pin =  TIM2_PWM_CH1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TIM2_PWM_CH1_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��2�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM2_PWM_CH2_PIN;
	GPIO_Init(TIM2_PWM_CH2_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��3�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM2_PWM_CH3_PIN;
	GPIO_Init(TIM2_PWM_CH3_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��4�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM2_PWM_CH4_PIN;
	GPIO_Init(TIM2_PWM_CH4_PORT, &GPIO_InitStructure);

	timer_pwm_init(INDE_TIMER_TIM2, uus, TIM_OCMode_PWM1, uPolarity);
}

/* ��ʱ��2���� */
static void timer2_pwm_deInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	/* ���ö�ʱ��ͨ��1�������ģʽ����������ģʽ */
	GPIO_InitStructure.GPIO_Pin =  TIM2_PWM_CH1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TIM2_PWM_CH1_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��2�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM2_PWM_CH2_PIN;
	GPIO_Init(TIM2_PWM_CH2_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��3�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM2_PWM_CH3_PIN;
	GPIO_Init(TIM2_PWM_CH3_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��4�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM2_PWM_CH4_PIN;
	GPIO_Init(TIM2_PWM_CH4_PORT, &GPIO_InitStructure);
	
	timer_pwm_deInit(INDE_TIMER_TIM2);
}

/* ��ʱ��2 ����PWMռ�ձ� */
static void timer2_pwm_SetDutyCycle(UINT8 eCh, UINT8 dutycycle)
{
	if (eCh > HAL_PWM_CH4)
		eCh = HAL_PWM_CH_SIZE;
	timer_pwm_SetDutyCycle(INDE_TIMER_TIM2, eCh, dutycycle);
}

/* ��ʱ��3 PWM ģʽ ��ʼ�� */
static void timer3_pwm_init(UINT32 uus, UINT16 uPolarity)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
	
	/* ���ö�ʱ��ͨ��1�������ģʽ�������������ģʽ */
	GPIO_InitStructure.GPIO_Pin =  TIM3_PWM_CH1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TIM3_PWM_CH1_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��2�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM3_PWM_CH2_PIN;
	GPIO_Init(TIM3_PWM_CH2_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��3�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM3_PWM_CH3_PIN;
	GPIO_Init(TIM3_PWM_CH3_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��4�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM3_PWM_CH4_PIN;
	GPIO_Init(TIM3_PWM_CH4_PORT, &GPIO_InitStructure);

	timer_pwm_init(INDE_TIMER_TIM3, uus, TIM_OCMode_PWM1, uPolarity);
}

/* ��ʱ��3���� */
static void timer3_pwm_deInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	/* ���ö�ʱ��ͨ��1�������ģʽ����������ģʽ */
	GPIO_InitStructure.GPIO_Pin =  TIM3_PWM_CH1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TIM3_PWM_CH1_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��2�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM3_PWM_CH2_PIN;
	GPIO_Init(TIM3_PWM_CH2_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��3�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM3_PWM_CH3_PIN;
	GPIO_Init(TIM3_PWM_CH3_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��4�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM3_PWM_CH4_PIN;
	GPIO_Init(TIM3_PWM_CH4_PORT, &GPIO_InitStructure);
	
	timer_pwm_deInit(INDE_TIMER_TIM3);
}

/* ��ʱ��3 ����PWMռ�ձ� */
static void timer3_pwm_SetDutyCycle(UINT8 eCh, UINT8 dutycycle)
{
	if (eCh > HAL_PWM_CH4)
		eCh = HAL_PWM_CH_SIZE;
	
	timer_pwm_SetDutyCycle(INDE_TIMER_TIM3, eCh, dutycycle);
}

/* ��ʱ��4 PWM ģʽ ��ʼ�� */
static void timer4_pwm_init(UINT32 uus, UINT16 uPolarity)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��
	
	/* ���ö�ʱ��ͨ��1�������ģʽ�������������ģʽ */
	GPIO_InitStructure.GPIO_Pin =  TIM4_PWM_CH1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TIM4_PWM_CH1_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��2�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM4_PWM_CH2_PIN;
	GPIO_Init(TIM4_PWM_CH2_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��3�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM4_PWM_CH3_PIN;
	GPIO_Init(TIM4_PWM_CH3_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��4�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM4_PWM_CH4_PIN;
	GPIO_Init(TIM4_PWM_CH4_PORT, &GPIO_InitStructure);

	timer_pwm_init(INDE_TIMER_TIM4, uus, TIM_OCMode_PWM1, uPolarity);
}

/* ��ʱ��4���� */
static void timer4_pwm_deInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	/* ���ö�ʱ��ͨ��1�������ģʽ����������ģʽ */
	GPIO_InitStructure.GPIO_Pin =  TIM4_PWM_CH1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TIM4_PWM_CH1_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��2�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM4_PWM_CH2_PIN;
	GPIO_Init(TIM4_PWM_CH2_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��3�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM4_PWM_CH3_PIN;
	GPIO_Init(TIM4_PWM_CH3_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��4�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM4_PWM_CH4_PIN;
	GPIO_Init(TIM4_PWM_CH4_PORT, &GPIO_InitStructure);
	
	timer_pwm_deInit(INDE_TIMER_TIM4);
}

/* ��ʱ��4 ����PWMռ�ձ� */
static void timer4_pwm_SetDutyCycle(UINT8 eCh, UINT8 dutycycle)
{
	if (eCh > HAL_PWM_CH4)
		eCh = HAL_PWM_CH_SIZE;
	
	timer_pwm_SetDutyCycle(INDE_TIMER_TIM4, eCh, dutycycle);
}

/* ��ʱ��5 PWM ģʽ ��ʼ�� */
static void timer5_pwm_init(UINT32 uus, UINT16 uPolarity)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //ʱ��ʹ��
	
	/* ���ö�ʱ��ͨ��1�������ģʽ�������������ģʽ */
	GPIO_InitStructure.GPIO_Pin =  TIM5_PWM_CH1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TIM5_PWM_CH1_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��2�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM5_PWM_CH2_PIN;
	GPIO_Init(TIM5_PWM_CH2_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��3�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM5_PWM_CH3_PIN;
	GPIO_Init(TIM5_PWM_CH3_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��4�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM5_PWM_CH4_PIN;
	GPIO_Init(TIM5_PWM_CH4_PORT, &GPIO_InitStructure);

	timer_pwm_init(INDE_TIMER_TIM5, uus, TIM_OCMode_PWM1, uPolarity);
}

/* ��ʱ��5���� */
static void timer5_pwm_deInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	/* ���ö�ʱ��ͨ��1�������ģʽ����������ģʽ */
	GPIO_InitStructure.GPIO_Pin =  TIM5_PWM_CH1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TIM5_PWM_CH1_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��2�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM5_PWM_CH2_PIN;
	GPIO_Init(TIM5_PWM_CH2_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��3�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM5_PWM_CH3_PIN;
	GPIO_Init(TIM5_PWM_CH3_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��4�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM5_PWM_CH4_PIN;
	GPIO_Init(TIM5_PWM_CH4_PORT, &GPIO_InitStructure);
	
	timer_pwm_deInit(INDE_TIMER_TIM5);
}

/* ��ʱ��5 ����PWMռ�ձ� */
static void timer5_pwm_SetDutyCycle(UINT8 eCh, UINT8 dutycycle)
{
	if (eCh > HAL_PWM_CH4)
		eCh = HAL_PWM_CH_SIZE;
	
	timer_pwm_SetDutyCycle(INDE_TIMER_TIM5, eCh, dutycycle);
}

/* ��ʱ��8 PWM ģʽ ��ʼ�� */
static void timer8_pwm_init(UINT32 uus, UINT16 uPolarity)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE); //ʱ��ʹ��
	
	/* ���ö�ʱ��ͨ��1�������ģʽ�������������ģʽ */
	GPIO_InitStructure.GPIO_Pin =  TIM8_PWM_CH1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TIM8_PWM_CH1_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��2�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM8_PWM_CH2_PIN;
	GPIO_Init(TIM8_PWM_CH2_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��3�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM8_PWM_CH3_PIN;
	GPIO_Init(TIM8_PWM_CH3_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��4�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM8_PWM_CH4_PIN;
	GPIO_Init(TIM8_PWM_CH4_PORT, &GPIO_InitStructure);
	
	/* ���ö�ʱ������ͨ��1�������ģʽ */
	GPIO_InitStructure.GPIO_Pin =  TIM8_PWM_CH1N_PIN;
	GPIO_Init(TIM8_PWM_CH1_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ������ͨ��2�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM8_PWM_CH2N_PIN;
	GPIO_Init(TIM8_PWM_CH2_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ������ͨ��3�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM8_PWM_CH3N_PIN;
	GPIO_Init(TIM8_PWM_CH3_PORT, &GPIO_InitStructure);

	timer_pwm_init(INDE_TIMER_TIM8, uus, TIM_OCMode_PWM1, uPolarity);
}

/* ��ʱ��8���� */
static void timer8_pwm_deInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	/* ���ö�ʱ��ͨ��1�������ģʽ����������ģʽ */
	GPIO_InitStructure.GPIO_Pin =  TIM8_PWM_CH1_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(TIM8_PWM_CH1_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��2�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM8_PWM_CH2_PIN;
	GPIO_Init(TIM8_PWM_CH2_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��3�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM8_PWM_CH3_PIN;
	GPIO_Init(TIM8_PWM_CH3_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ��ͨ��4�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM8_PWM_CH4_PIN;
	GPIO_Init(TIM8_PWM_CH4_PORT, &GPIO_InitStructure);
	
	/* ���ö�ʱ������ͨ��1�������ģʽ����������ģʽ */
	GPIO_InitStructure.GPIO_Pin =  TIM8_PWM_CH1N_PIN;
	GPIO_Init(TIM8_PWM_CH1_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ������ͨ��2�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM8_PWM_CH2N_PIN;
	GPIO_Init(TIM8_PWM_CH2_PORT, &GPIO_InitStructure);

	/* ���ö�ʱ������ͨ��3�������ģʽ */
	GPIO_InitStructure.GPIO_Pin = TIM8_PWM_CH3N_PIN;
	GPIO_Init(TIM8_PWM_CH3_PORT, &GPIO_InitStructure);

	timer_pwm_deInit(INDE_TIMER_TIM8);
}

/* ��ʱ��8 ����PWMռ�ձ� */
static void timer8_pwm_SetDutyCycle(UINT8 eCh, UINT8 dutycycle)
{
	//if (eCh > HAL_PWM_CH_SIZE)
	//	eCh = HAL_PWM_CH_SIZE;
	
	timer_pwm_SetDutyCycle(INDE_TIMER_TIM8, eCh, dutycycle);
}


/**
 * @brief ����PWM��������
 * @param eID PWM��ʱ�� @ref HalIndeTimerIDsTypeDef
 * @retval None
 */
static void New(HalIndeTimerIDsTypeDef eId)
{
	switch (eId)
	{
	case INDE_TIMER_TIM1:
		m_Instance[INDE_TIMER_TIM1].init = timer1_pwm_init;
		m_Instance[INDE_TIMER_TIM1].deInit = timer1_pwm_deInit;
		m_Instance[INDE_TIMER_TIM1].setDutyCycle = timer1_pwm_SetDutyCycle;
		m_pthis[INDE_TIMER_TIM1] = &m_Instance[INDE_TIMER_TIM1];
		break;
	case INDE_TIMER_TIM2:
		m_Instance[INDE_TIMER_TIM2].init = timer2_pwm_init;
		m_Instance[INDE_TIMER_TIM2].deInit = timer2_pwm_deInit;
		m_Instance[INDE_TIMER_TIM2].setDutyCycle = timer2_pwm_SetDutyCycle;
		m_pthis[INDE_TIMER_TIM2] = &m_Instance[INDE_TIMER_TIM2];
		break;
	case INDE_TIMER_TIM3:
		m_Instance[INDE_TIMER_TIM3].init = timer3_pwm_init;
		m_Instance[INDE_TIMER_TIM3].deInit = timer3_pwm_deInit;
		m_Instance[INDE_TIMER_TIM3].setDutyCycle = timer3_pwm_SetDutyCycle;
		m_pthis[INDE_TIMER_TIM3] = &m_Instance[INDE_TIMER_TIM3];
		break;
	case INDE_TIMER_TIM4:
		m_Instance[INDE_TIMER_TIM4].init = timer4_pwm_init;
		m_Instance[INDE_TIMER_TIM4].deInit = timer4_pwm_deInit;
		m_Instance[INDE_TIMER_TIM4].setDutyCycle = timer4_pwm_SetDutyCycle;
		m_pthis[INDE_TIMER_TIM4] = &m_Instance[INDE_TIMER_TIM4];
		break;
	case INDE_TIMER_TIM5:
		m_Instance[INDE_TIMER_TIM5].init = timer5_pwm_init;
		m_Instance[INDE_TIMER_TIM5].deInit = timer5_pwm_deInit;
		m_Instance[INDE_TIMER_TIM5].setDutyCycle = timer5_pwm_SetDutyCycle;
		m_pthis[INDE_TIMER_TIM5] = &m_Instance[INDE_TIMER_TIM5];
		break;
	case INDE_TIMER_TIM6:
		break;
	case INDE_TIMER_TIM8:
		m_Instance[INDE_TIMER_TIM8].init = timer8_pwm_init;
		m_Instance[INDE_TIMER_TIM8].deInit = timer8_pwm_deInit;
		m_Instance[INDE_TIMER_TIM8].setDutyCycle = timer8_pwm_SetDutyCycle;
		m_pthis[INDE_TIMER_TIM8] = &m_Instance[INDE_TIMER_TIM8];
		break;
	default:
		break;
	}
}

/**
 * @brief ��ȡI2C�����ṹ���
 * @param eChannel I2C ͨ�� @ref HALI2CNumer
 * @retval I2C�����ṹ���
 */
HALPwmTypeDef* HalPwmGetInstance(HalIndeTimerIDsTypeDef eId)
{
	if (NULL == m_pthis[eId])
	{
		New(eId);
	}
	return m_pthis[eId];
}

#endif //defined(CFG_HAL_PWM) && defined(CFG_HAL_TIMER)
