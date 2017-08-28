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

static UINT16 m_uMask[INDE_TIMER_MAX] = {0};
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

/* 定时器 PWM 模式 初始化 */
static void timer_pwm_init(HalIndeTimerIDsTypeDef id, UINT32 uus, UINT16 OCMode, UINT16 uPolarity, UINT16 uMask)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_TypeDef* TIMx = GetTimHandle(id);
	
	TIM_DeInit(TIMx);
	//定时器TIM2初始化
	m_uPeriod[id] = (uus>1) ? uus-1 : 0;
	TIM_TimeBaseStructure.TIM_Period = m_uPeriod[id]; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =71; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	/* 定时器输出通道1模式配置 */
	/* 模式配置：PWM模式1 */
	if (!IS_TIM_OC_MODE(OCMode))
		OCMode = TIM_OCMode_PWM1;
	m_TIM_OCInitStructure[id].TIM_OCMode = OCMode;
	/* 输出状态设置：使能输出 */
	m_TIM_OCInitStructure[id].TIM_OutputState = TIM_OutputState_Enable;	
	/* 互补通道输出状态设置：使能输出 */
	if ((TIMx == TIM1) || (TIMx == TIM8))
		m_TIM_OCInitStructure[id].TIM_OutputNState = TIM_OutputNState_Enable;
	/* 设置跳变值，当计数器计数到这个值时，电平发生跳变 */
	m_TIM_OCInitStructure[id].TIM_Pulse = 0;
	/* 当定时器计数值小于CCR1_Val时为高电平 */
	if (uPolarity != HAL_PWM_Polarity_High)
		uPolarity = HAL_PWM_Polarity_Low;
	m_TIM_OCInitStructure[id].TIM_OCPolarity = uPolarity;
	if ((TIMx == TIM1) || (TIMx == TIM8))
	{
		m_TIM_OCInitStructure[id].TIM_OCIdleState = TIM_OCIdleState_Set;
		m_TIM_OCInitStructure[id].TIM_OCNIdleState = TIM_OCIdleState_Reset;		
	}

	
	/* 初始化定时器通道1输出PWM */
	if (m_uMask[id] & HAL_PWM_MASK_CH1)
	{
		TIM_OC1Init(TIMx, &m_TIM_OCInitStructure[id]);
		/* 定时器比较输出通道1预装载配置：使能预装载 */
		TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
	}
	
	/* 初始化定时器通道2输出PWM */
	if (m_uMask[id] & HAL_PWM_MASK_CH2)
	{
		TIM_OC2Init(TIMx, &m_TIM_OCInitStructure[id]);
		/* 定时器比较输出通道2预装载配置：使能预装载 */
		TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);
	}
	
	/* 初始化定时器通道3输出PWM */
	if (m_uMask[id] & HAL_PWM_MASK_CH3)
	{
		TIM_OC3Init(TIMx, &m_TIM_OCInitStructure[id]);
		/* 定时器比较输出通道3预装载配置：使能预装载 */
		TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);
	}
	
	/* 初始化定时器通道4输出PWM */
	if (m_uMask[id] & HAL_PWM_MASK_CH4)
	{
		TIM_OC4Init(TIMx, &m_TIM_OCInitStructure[id]);
		/* 定时器比较输出通道4预装载配置：使能预装载 */
		TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);
	}

	/* 使能定时器重载寄存器ARR */
	TIM_ARRPreloadConfig(TIMx, ENABLE);
	TIM_Cmd(TIMx, ENABLE);  //使能TIMx	
	
	/* TIM主输出使能 */
	if ((TIMx == TIM1) || (TIMx == TIM8))
		TIM_CtrlPWMOutputs(TIMx, ENABLE);
}

/* 定时器2清零 */
static void timer_pwm_deInit(HalIndeTimerIDsTypeDef id)
{
	TIM_TypeDef* TIMx = GetTimHandle(id);
	TIM_ARRPreloadConfig(TIMx, DISABLE);
	TIM_Cmd(TIMx, DISABLE);  //禁止TIMx	
	TIM_DeInit(TIMx);
}

/* 定时器 设置PWM占空比 */
static void timer_pwm_SetDutyCycle(HalIndeTimerIDsTypeDef id, UINT8 eCh, UINT8 dutycycle)
{
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_TypeDef* TIMx = GetTimHandle(id);
	
	dutycycle = (dutycycle > 100) ? 100 : dutycycle;
	/* 模式配置：PWM模式1 */
	TIM_OCInitStructure.TIM_OCMode = m_TIM_OCInitStructure[id].TIM_OCMode;
	/* 输出状态设置：使能输出 */
	TIM_OCInitStructure.TIM_OutputState = m_TIM_OCInitStructure[id].TIM_OutputState;	
	/* 设置跳变值，当计数器计数到这个值时，电平发生跳变 */
	TIM_OCInitStructure.TIM_Pulse = m_uPeriod[id] * (dutycycle / 100.0);
	/* 当定时器计数值小于CCR1_Val时为高电平 */
	TIM_OCInitStructure.TIM_OCPolarity = m_TIM_OCInitStructure[id].TIM_OCPolarity;
	
	switch (eCh)
	{
	case HAL_PWM_CH1: //定时器通道1
		/* 初始化定时器通道1输出PWM */
		if (m_uMask[id] & HAL_PWM_MASK_CH1)
		{
			TIM_OC1Init(TIMx, &TIM_OCInitStructure);
			/* 定时器比较输出通道1预装载配置：使能预装载 */
			TIM_OC1PreloadConfig(TIMx, TIM_OCPreload_Enable);
		}
		break;
	case HAL_PWM_CH2: //定时器通道2
		/* 初始化定时器通道2输出PWM */
		if (m_uMask[id] & HAL_PWM_MASK_CH2)
		{
			TIM_OC2Init(TIMx, &TIM_OCInitStructure);
			/* 定时器比较输出通道2预装载配置：使能预装载 */
			TIM_OC2PreloadConfig(TIMx, TIM_OCPreload_Enable);
		}
		break;
	case HAL_PWM_CH3: //定时器通道3
		/* 初始化定时器通道3输出PWM */
		if (m_uMask[id] & HAL_PWM_MASK_CH3)
		{
			TIM_OC3Init(TIMx, &TIM_OCInitStructure);
			/* 定时器比较输出通道3预装载配置：使能预装载 */
			TIM_OC3PreloadConfig(TIMx, TIM_OCPreload_Enable);
		}
		break;
	case HAL_PWM_CH4: //定时器通道4
		/* 初始化定时器通道4输出PWM */
		if (m_uMask[id] & HAL_PWM_MASK_CH4)
		{
			TIM_OC4Init(TIMx, &TIM_OCInitStructure);
			/* 定时器比较输出通道4预装载配置：使能预装载 */
			TIM_OC4PreloadConfig(TIMx, TIM_OCPreload_Enable);
		}
		break;
	default:
		break;
	}
}

/* 定时器1 PWM 模式 初始化 */
static void timer1_pwm_init(UINT32 uus, UINT16 uPolarity, UINT16 uMask)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	m_uMask[INDE_TIMER_TIM1] = uMask;
  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //时钟使能
	
	/* 配置定时器通道1输出引脚模式：复用推挽输出模式 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (m_uMask[INDE_TIMER_TIM1] & HAL_PWM_MASK_CH1)
	{
		GPIO_InitStructure.GPIO_Pin =  TIM1_PWM_CH1_PIN;
		GPIO_Init(TIM1_PWM_CH1_PORT, &GPIO_InitStructure);
		/* 配置定时器互补通道1输出引脚模式：复用推挽输出模式 */
		GPIO_InitStructure.GPIO_Pin =  TIM1_PWM_CH1N_PIN;
		GPIO_Init(TIM1_PWM_CH1_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道2输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM1] & HAL_PWM_MASK_CH2)
	{
		GPIO_InitStructure.GPIO_Pin = TIM1_PWM_CH2_PIN;
		GPIO_Init(TIM1_PWM_CH2_PORT, &GPIO_InitStructure);
		/* 配置定时器互补通道2输出引脚模式 */
		GPIO_InitStructure.GPIO_Pin = TIM1_PWM_CH2N_PIN;
		GPIO_Init(TIM1_PWM_CH2_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道3输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM1] & HAL_PWM_MASK_CH3)
	{
		GPIO_InitStructure.GPIO_Pin = TIM1_PWM_CH3_PIN;
		GPIO_Init(TIM1_PWM_CH3_PORT, &GPIO_InitStructure);
		/* 配置定时器互补通道3输出引脚模式 */
		GPIO_InitStructure.GPIO_Pin = TIM1_PWM_CH3N_PIN;
		GPIO_Init(TIM1_PWM_CH3_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道4输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM1] & HAL_PWM_MASK_CH4)
	{
		GPIO_InitStructure.GPIO_Pin = TIM1_PWM_CH4_PIN;
		GPIO_Init(TIM1_PWM_CH4_PORT, &GPIO_InitStructure);
	}



	timer_pwm_init(INDE_TIMER_TIM1, uus, TIM_OCMode_PWM1, uPolarity, uMask);
}

/* 定时器1清零 */
static void timer1_pwm_deInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	/* 配置定时器通道1输出引脚模式：浮空输入模式 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (m_uMask[INDE_TIMER_TIM1] & HAL_PWM_MASK_CH1)
	{
		GPIO_InitStructure.GPIO_Pin =  TIM1_PWM_CH1_PIN;
		GPIO_Init(TIM1_PWM_CH1_PORT, &GPIO_InitStructure);
		/* 配置定时器互补通道1输出引脚模式：浮空输入模式 */
		GPIO_InitStructure.GPIO_Pin =  TIM1_PWM_CH1N_PIN;
		GPIO_Init(TIM1_PWM_CH1_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道2输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM1] & HAL_PWM_MASK_CH2)
	{
		GPIO_InitStructure.GPIO_Pin = TIM1_PWM_CH2_PIN;
		GPIO_Init(TIM1_PWM_CH2_PORT, &GPIO_InitStructure);
		/* 配置定时器互补通道2输出引脚模式 */
		GPIO_InitStructure.GPIO_Pin = TIM1_PWM_CH2N_PIN;
		GPIO_Init(TIM1_PWM_CH2_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道3输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM1] & HAL_PWM_MASK_CH3)
	{
		GPIO_InitStructure.GPIO_Pin = TIM1_PWM_CH3_PIN;
		GPIO_Init(TIM1_PWM_CH3_PORT, &GPIO_InitStructure);
		/* 配置定时器互补通道3输出引脚模式 */
		GPIO_InitStructure.GPIO_Pin = TIM1_PWM_CH3N_PIN;
		GPIO_Init(TIM1_PWM_CH3_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道4输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM1] & HAL_PWM_MASK_CH4)
	{
		GPIO_InitStructure.GPIO_Pin = TIM1_PWM_CH4_PIN;
		GPIO_Init(TIM1_PWM_CH4_PORT, &GPIO_InitStructure);
	}
	



	timer_pwm_deInit(INDE_TIMER_TIM1);
}

/* 定时器1 设置PWM占空比 */
static void timer1_pwm_SetDutyCycle(UINT8 eCh, UINT8 dutycycle)
{
	//if (eCh > HAL_PWM_CH_SIZE)
	//	eCh = HAL_PWM_CH_SIZE;
	
	timer_pwm_SetDutyCycle(INDE_TIMER_TIM1, eCh, dutycycle);
}

/* 定时器2 PWM 模式 初始化 */
static void timer2_pwm_init(UINT32 uus, UINT16 uPolarity, UINT16 uMask)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能
	
	/* 配置定时器通道1输出引脚模式：复用推挽输出模式 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (m_uMask[INDE_TIMER_TIM2] & HAL_PWM_MASK_CH1)
	{
		GPIO_InitStructure.GPIO_Pin =  TIM2_PWM_CH1_PIN;
		GPIO_Init(TIM2_PWM_CH1_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道2输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM2] & HAL_PWM_MASK_CH2)
	{
		GPIO_InitStructure.GPIO_Pin = TIM2_PWM_CH2_PIN;
		GPIO_Init(TIM2_PWM_CH2_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道3输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM2] & HAL_PWM_MASK_CH3)
	{
		GPIO_InitStructure.GPIO_Pin = TIM2_PWM_CH3_PIN;
		GPIO_Init(TIM2_PWM_CH3_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道4输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM2] & HAL_PWM_MASK_CH4)
	{
		GPIO_InitStructure.GPIO_Pin = TIM2_PWM_CH4_PIN;
		GPIO_Init(TIM2_PWM_CH4_PORT, &GPIO_InitStructure);
	}

	timer_pwm_init(INDE_TIMER_TIM2, uus, TIM_OCMode_PWM1, uPolarity, uMask);
}

/* 定时器2清零 */
static void timer2_pwm_deInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	/* 配置定时器通道1输出引脚模式：浮空输入模式 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (m_uMask[INDE_TIMER_TIM2] & HAL_PWM_MASK_CH1)
	{
		GPIO_InitStructure.GPIO_Pin =  TIM2_PWM_CH1_PIN;
		GPIO_Init(TIM2_PWM_CH1_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道2输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM2] & HAL_PWM_MASK_CH2)
	{
		GPIO_InitStructure.GPIO_Pin = TIM2_PWM_CH2_PIN;
		GPIO_Init(TIM2_PWM_CH2_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道3输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM2] & HAL_PWM_MASK_CH3)
	{
		GPIO_InitStructure.GPIO_Pin = TIM2_PWM_CH3_PIN;
		GPIO_Init(TIM2_PWM_CH3_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道4输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM2] & HAL_PWM_MASK_CH4)
	{
		GPIO_InitStructure.GPIO_Pin = TIM2_PWM_CH4_PIN;
		GPIO_Init(TIM2_PWM_CH4_PORT, &GPIO_InitStructure);
	}
	
	timer_pwm_deInit(INDE_TIMER_TIM2);
}

/* 定时器2 设置PWM占空比 */
static void timer2_pwm_SetDutyCycle(UINT8 eCh, UINT8 dutycycle)
{
	if (eCh > HAL_PWM_CH4)
		eCh = HAL_PWM_CH_SIZE;
	timer_pwm_SetDutyCycle(INDE_TIMER_TIM2, eCh, dutycycle);
}

/* 定时器3 PWM 模式 初始化 */
static void timer3_pwm_init(UINT32 uus, UINT16 uPolarity, UINT16 uMask)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
	
	/* 配置定时器通道1输出引脚模式：复用推挽输出模式 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (m_uMask[INDE_TIMER_TIM3] & HAL_PWM_MASK_CH1)
	{
		GPIO_InitStructure.GPIO_Pin =  TIM3_PWM_CH1_PIN;
		GPIO_Init(TIM3_PWM_CH1_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道2输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM3] & HAL_PWM_MASK_CH2)
	{
		GPIO_InitStructure.GPIO_Pin = TIM3_PWM_CH2_PIN;
		GPIO_Init(TIM3_PWM_CH2_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道3输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM3] & HAL_PWM_MASK_CH3)
	{
		GPIO_InitStructure.GPIO_Pin = TIM3_PWM_CH3_PIN;
		GPIO_Init(TIM3_PWM_CH3_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道4输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM3] & HAL_PWM_MASK_CH4)
	{
		GPIO_InitStructure.GPIO_Pin = TIM3_PWM_CH4_PIN;
		GPIO_Init(TIM3_PWM_CH4_PORT, &GPIO_InitStructure);
	}

	timer_pwm_init(INDE_TIMER_TIM3, uus, TIM_OCMode_PWM1, uPolarity, uMask);
}

/* 定时器3清零 */
static void timer3_pwm_deInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	/* 配置定时器通道1输出引脚模式：浮空输入模式 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (m_uMask[INDE_TIMER_TIM3] & HAL_PWM_MASK_CH1)
	{
		GPIO_InitStructure.GPIO_Pin =  TIM3_PWM_CH1_PIN;
		GPIO_Init(TIM3_PWM_CH1_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道2输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM3] & HAL_PWM_MASK_CH2)
	{
		GPIO_InitStructure.GPIO_Pin = TIM3_PWM_CH2_PIN;
		GPIO_Init(TIM3_PWM_CH2_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道3输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM3] & HAL_PWM_MASK_CH3)
	{
		GPIO_InitStructure.GPIO_Pin = TIM3_PWM_CH3_PIN;
		GPIO_Init(TIM3_PWM_CH3_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道4输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM3] & HAL_PWM_MASK_CH4)
	{
		GPIO_InitStructure.GPIO_Pin = TIM3_PWM_CH4_PIN;
		GPIO_Init(TIM3_PWM_CH4_PORT, &GPIO_InitStructure);
	}
	
	timer_pwm_deInit(INDE_TIMER_TIM3);
}

/* 定时器3 设置PWM占空比 */
static void timer3_pwm_SetDutyCycle(UINT8 eCh, UINT8 dutycycle)
{
	if (eCh > HAL_PWM_CH4)
		eCh = HAL_PWM_CH_SIZE;
	
	timer_pwm_SetDutyCycle(INDE_TIMER_TIM3, eCh, dutycycle);
}

/* 定时器4 PWM 模式 初始化 */
static void timer4_pwm_init(UINT32 uus, UINT16 uPolarity, UINT16 uMask)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能
	
	/* 配置定时器通道1输出引脚模式：复用推挽输出模式 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (m_uMask[INDE_TIMER_TIM4] & HAL_PWM_MASK_CH1)
	{
		GPIO_InitStructure.GPIO_Pin =  TIM4_PWM_CH1_PIN;
		GPIO_Init(TIM4_PWM_CH1_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道2输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM4] & HAL_PWM_MASK_CH2)
	{
		GPIO_InitStructure.GPIO_Pin = TIM4_PWM_CH2_PIN;
		GPIO_Init(TIM4_PWM_CH2_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道3输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM4] & HAL_PWM_MASK_CH3)
	{
		GPIO_InitStructure.GPIO_Pin = TIM4_PWM_CH3_PIN;
		GPIO_Init(TIM4_PWM_CH3_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道4输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM4] & HAL_PWM_MASK_CH4)
	{
		GPIO_InitStructure.GPIO_Pin = TIM4_PWM_CH4_PIN;
		GPIO_Init(TIM4_PWM_CH4_PORT, &GPIO_InitStructure);
	}

	timer_pwm_init(INDE_TIMER_TIM4, uus, TIM_OCMode_PWM1, uPolarity, uMask);
}

/* 定时器4清零 */
static void timer4_pwm_deInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	/* 配置定时器通道1输出引脚模式：浮空输入模式 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (m_uMask[INDE_TIMER_TIM4] & HAL_PWM_MASK_CH1)
	{
		GPIO_InitStructure.GPIO_Pin =  TIM4_PWM_CH1_PIN;
		GPIO_Init(TIM4_PWM_CH1_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道2输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM4] & HAL_PWM_MASK_CH2)
	{
		GPIO_InitStructure.GPIO_Pin = TIM4_PWM_CH2_PIN;
		GPIO_Init(TIM4_PWM_CH2_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道3输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM4] & HAL_PWM_MASK_CH3)
	{
		GPIO_InitStructure.GPIO_Pin = TIM4_PWM_CH3_PIN;
		GPIO_Init(TIM4_PWM_CH3_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道4输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM4] & HAL_PWM_MASK_CH4)
	{
		GPIO_InitStructure.GPIO_Pin = TIM4_PWM_CH4_PIN;
		GPIO_Init(TIM4_PWM_CH4_PORT, &GPIO_InitStructure);
	}
	
	timer_pwm_deInit(INDE_TIMER_TIM4);
}

/* 定时器4 设置PWM占空比 */
static void timer4_pwm_SetDutyCycle(UINT8 eCh, UINT8 dutycycle)
{
	if (eCh > HAL_PWM_CH4)
		eCh = HAL_PWM_CH_SIZE;
	
	timer_pwm_SetDutyCycle(INDE_TIMER_TIM4, eCh, dutycycle);
}

/* 定时器5 PWM 模式 初始化 */
static void timer5_pwm_init(UINT32 uus, UINT16 uPolarity, UINT16 uMask)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //时钟使能
	
	/* 配置定时器通道1输出引脚模式：复用推挽输出模式 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (m_uMask[INDE_TIMER_TIM5] & HAL_PWM_MASK_CH1)
	{
		GPIO_InitStructure.GPIO_Pin =  TIM5_PWM_CH1_PIN;
		GPIO_Init(TIM5_PWM_CH1_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道2输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM5] & HAL_PWM_MASK_CH2)
	{
		GPIO_InitStructure.GPIO_Pin = TIM5_PWM_CH2_PIN;
		GPIO_Init(TIM5_PWM_CH2_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道3输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM5] & HAL_PWM_MASK_CH3)
	{
		GPIO_InitStructure.GPIO_Pin = TIM5_PWM_CH3_PIN;
		GPIO_Init(TIM5_PWM_CH3_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道4输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM5] & HAL_PWM_MASK_CH4)
	{
		GPIO_InitStructure.GPIO_Pin = TIM5_PWM_CH4_PIN;
		GPIO_Init(TIM5_PWM_CH4_PORT, &GPIO_InitStructure);
	}

	timer_pwm_init(INDE_TIMER_TIM5, uus, TIM_OCMode_PWM1, uPolarity, uMask);
}

/* 定时器5清零 */
static void timer5_pwm_deInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	/* 配置定时器通道1输出引脚模式：浮空输入模式 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (m_uMask[INDE_TIMER_TIM5] & HAL_PWM_MASK_CH1)
	{
		GPIO_InitStructure.GPIO_Pin =  TIM5_PWM_CH1_PIN;
		GPIO_Init(TIM5_PWM_CH1_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道2输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM5] & HAL_PWM_MASK_CH2)
	{
		GPIO_InitStructure.GPIO_Pin = TIM5_PWM_CH2_PIN;
		GPIO_Init(TIM5_PWM_CH2_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道3输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM5] & HAL_PWM_MASK_CH3)
	{
		GPIO_InitStructure.GPIO_Pin = TIM5_PWM_CH3_PIN;
		GPIO_Init(TIM5_PWM_CH3_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道4输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM5] & HAL_PWM_MASK_CH4)
	{
		GPIO_InitStructure.GPIO_Pin = TIM5_PWM_CH4_PIN;
		GPIO_Init(TIM5_PWM_CH4_PORT, &GPIO_InitStructure);
	}
	
	timer_pwm_deInit(INDE_TIMER_TIM5);
}

/* 定时器5 设置PWM占空比 */
static void timer5_pwm_SetDutyCycle(UINT8 eCh, UINT8 dutycycle)
{
	if (eCh > HAL_PWM_CH4)
		eCh = HAL_PWM_CH_SIZE;
	
	timer_pwm_SetDutyCycle(INDE_TIMER_TIM5, eCh, dutycycle);
}

/* 定时器8 PWM 模式 初始化 */
static void timer8_pwm_init(UINT32 uus, UINT16 uPolarity, UINT16 uMask)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE); //时钟使能
	
	/* 配置定时器通道1输出引脚模式：复用推挽输出模式 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (m_uMask[INDE_TIMER_TIM8] & HAL_PWM_MASK_CH1)
	{
		GPIO_InitStructure.GPIO_Pin =  TIM8_PWM_CH1_PIN;
		GPIO_Init(TIM8_PWM_CH1_PORT, &GPIO_InitStructure);
		/* 配置定时器互补通道1输出引脚模式 */
		GPIO_InitStructure.GPIO_Pin =  TIM8_PWM_CH1N_PIN;
		GPIO_Init(TIM8_PWM_CH1_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道2输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM8] & HAL_PWM_MASK_CH2)
	{
		GPIO_InitStructure.GPIO_Pin = TIM8_PWM_CH2_PIN;
		GPIO_Init(TIM8_PWM_CH2_PORT, &GPIO_InitStructure);
		/* 配置定时器互补通道2输出引脚模式 */
		GPIO_InitStructure.GPIO_Pin = TIM8_PWM_CH2N_PIN;
		GPIO_Init(TIM8_PWM_CH2_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道3输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM8] & HAL_PWM_MASK_CH3)
	{
		GPIO_InitStructure.GPIO_Pin = TIM8_PWM_CH3_PIN;
		GPIO_Init(TIM8_PWM_CH3_PORT, &GPIO_InitStructure);
		/* 配置定时器互补通道3输出引脚模式 */
		GPIO_InitStructure.GPIO_Pin = TIM8_PWM_CH3N_PIN;
		GPIO_Init(TIM8_PWM_CH3_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道4输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM8] & HAL_PWM_MASK_CH4)
	{
		GPIO_InitStructure.GPIO_Pin = TIM8_PWM_CH4_PIN;
		GPIO_Init(TIM8_PWM_CH4_PORT, &GPIO_InitStructure);
	}



	timer_pwm_init(INDE_TIMER_TIM8, uus, TIM_OCMode_PWM1, uPolarity, uMask);
}

/* 定时器8清零 */
static void timer8_pwm_deInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	/* 配置定时器通道1输出引脚模式：浮空输入模式 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (m_uMask[INDE_TIMER_TIM8] & HAL_PWM_MASK_CH1)
	{
		GPIO_InitStructure.GPIO_Pin =  TIM8_PWM_CH1_PIN;
		GPIO_Init(TIM8_PWM_CH1_PORT, &GPIO_InitStructure);
		/* 配置定时器互补通道1输出引脚模式：浮空输入模式 */
		GPIO_InitStructure.GPIO_Pin =  TIM8_PWM_CH1N_PIN;
		GPIO_Init(TIM8_PWM_CH1_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道2输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM8] & HAL_PWM_MASK_CH2)
	{
		GPIO_InitStructure.GPIO_Pin = TIM8_PWM_CH2_PIN;
		GPIO_Init(TIM8_PWM_CH2_PORT, &GPIO_InitStructure);
		/* 配置定时器互补通道2输出引脚模式 */
		GPIO_InitStructure.GPIO_Pin = TIM8_PWM_CH2N_PIN;
		GPIO_Init(TIM8_PWM_CH2_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道3输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM8] & HAL_PWM_MASK_CH3)
	{
		GPIO_InitStructure.GPIO_Pin = TIM8_PWM_CH3_PIN;
		GPIO_Init(TIM8_PWM_CH3_PORT, &GPIO_InitStructure);
		/* 配置定时器互补通道3输出引脚模式 */
		GPIO_InitStructure.GPIO_Pin = TIM8_PWM_CH3N_PIN;
		GPIO_Init(TIM8_PWM_CH3_PORT, &GPIO_InitStructure);
	}

	/* 配置定时器通道4输出引脚模式 */
	if (m_uMask[INDE_TIMER_TIM8] & HAL_PWM_MASK_CH4)
	{
		GPIO_InitStructure.GPIO_Pin = TIM8_PWM_CH4_PIN;
		GPIO_Init(TIM8_PWM_CH4_PORT, &GPIO_InitStructure);
	}

	timer_pwm_deInit(INDE_TIMER_TIM8);
}

/* 定时器8 设置PWM占空比 */
static void timer8_pwm_SetDutyCycle(UINT8 eCh, UINT8 dutycycle)
{
	//if (eCh > HAL_PWM_CH_SIZE)
	//	eCh = HAL_PWM_CH_SIZE;
	
	timer_pwm_SetDutyCycle(INDE_TIMER_TIM8, eCh, dutycycle);
}


/**
 * @brief 申请PWM操作对象
 * @param eID PWM定时器 @ref HalIndeTimerIDsTypeDef
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
 * @brief 获取I2C操作结构句柄
 * @param eChannel I2C 通道 @ref HALI2CNumer
 * @retval I2C操作结构句柄
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
