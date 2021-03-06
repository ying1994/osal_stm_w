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

/** 共享定时器最大个数 */
#define SHARE_TIMER_MAX 32

/** 共享定时器基准时间 */
#define SHARE_TIMER_BASE_TIME 1

/** 共享定时器起始编号 */
//#define INDE_TIMER_BASE_NUM 0
/** 共享定时器起始编号 */
#define SHARE_TIMER_BASE_NUM INDE_TIMER_MAX


/* 独立定时器初始化操作句柄原型 */
typedef void (*HalTimeInit_t)(BOOL bIsOn, UINT32 uus);
/* 独立定时器清零操作句柄原型 */
typedef void (*HalTimeClear_t)(void);

/** 共享定时器存储结构 */
typedef struct
{
	HalTimerCBack_t hTimerFunc;	/*!< 定时器回调函数句柄 */
	UINT32 uCounter;				/*!< 定时器执行时间(ms) */
	UINT32 uTime;					/*!< 定时器执行同期(ms) */
}HAL_TIMER_TYPE;

/* 独立定时器句柄集合 */
static HalTimerCBack_t m_hIndeTimer[INDE_TIMER_MAX] = {NULL};
static HalTimeClear_t m_hClearTimer[INDE_TIMER_MAX] = {NULL};
/* 共享定时器句柄集合 */
static HAL_TIMER_TYPE m_hShareTimer[SHARE_TIMER_MAX] = {{NULL, 0}};
static UINT16 m_uShareTimerCount = 0;

/* 独立定时器初始化操作句柄 */
static HalTimeInit_t m_hTimerInitFunc[INDE_TIMER_MAX] = {NULL};

/* 定时器1初始化 */
static void timer1_init(BOOL bIsOn, UINT32 uus)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	static BOOL bInit = FALSE;

	if (bIsOn && (uus > 0))
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //时钟使能
		
		TIM_DeInit(TIM1);
		//定时器TIM2初始化
		if (uus < 1000)//1000us = 1ms
		{
			TIM_TimeBaseStructure.TIM_Period = uus; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
			TIM_TimeBaseStructure.TIM_Prescaler =71; //4-1 设置用来作为TIMx时钟频率除数的预分频值
		}
		else //ms级别
		{
			uus /= 1000; 
			TIM_TimeBaseStructure.TIM_Period = (uus>>1); //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
			TIM_TimeBaseStructure.TIM_Prescaler =36000-1; //4-1 设置用来作为TIMx时钟频率除数的预分频值 72MHz/36000 = 72000000/36000=2000=2KHz
		}
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
		TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
	 
		if (!bInit)//中断优先级NVIC设置
		{
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
			NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  //TIM3中断
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
			NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
			bInit = TRUE;
		}
		
		/* 清除计数器中断标志位 */
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		TIM_ClearFlag(TIM1, TIM_FLAG_Update);//清除中断标志位
		TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE ); //使能指定的TIM2中断,允许更新中断
		TIM_Cmd(TIM1, ENABLE);  //使能TIMx	
	}
	else
	{
		TIM_ClearFlag(TIM1, TIM_FLAG_Update);//清除中断标志位
		TIM_ITConfig(TIM1,TIM_IT_Update,DISABLE ); //禁止指定的TIM3中断,允许更新中断
		TIM_Cmd(TIM1, DISABLE);  //禁止TIMx	
		TIM_DeInit(TIM1);
	}
}

/* 定时器1清零 */
static void timer1_clear(void)
{
	TIM_SetCounter(TIM1, 0);
}

/* 定时器2初始化 */
static void timer2_init(BOOL bIsOn, UINT32 uus)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	static BOOL bInit = FALSE;

	if (bIsOn && (uus > 0))
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能
		
		TIM_DeInit(TIM2);
		//定时器TIM2初始化
		if (uus < 1000)//1000us = 1ms
		{
			TIM_TimeBaseStructure.TIM_Period = uus; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
			TIM_TimeBaseStructure.TIM_Prescaler =71; //4-1 设置用来作为TIMx时钟频率除数的预分频值
		}
		else //ms级别
		{
			uus /= 1000; 
			TIM_TimeBaseStructure.TIM_Period = (uus>>1); //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
			TIM_TimeBaseStructure.TIM_Prescaler =36000-1; //4-1 设置用来作为TIMx时钟频率除数的预分频值 72MHz/36000 = 72000000/36000=2000=2KHz
		}
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
	 
		if (!bInit)//中断优先级NVIC设置
		{
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
			NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3中断
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
			NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
			bInit = TRUE;
		}
		
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);//清除中断标志位
		TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //使能指定的TIM2中断,允许更新中断
		TIM_Cmd(TIM2, ENABLE);  //使能TIMx	
	}
	else
	{
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);//清除中断标志位
		TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE ); //禁止指定的TIM3中断,允许更新中断
		TIM_Cmd(TIM2, DISABLE);  //禁止TIMx	
		TIM_DeInit(TIM2);
	}
}

/* 定时器3清零 */
static void timer2_clear(void)
{
	TIM_SetCounter(TIM2, 0);
}

/* 定时器3初始化 */
static void timer3_init(BOOL bIsOn, UINT32 uus)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	static BOOL bInit = FALSE;
	
	if (bIsOn && (uus > 0))
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
		
		TIM_DeInit(TIM3);
		//定时器TIM3初始化
		if (uus < 1000)//1000us = 1ms
		{
			TIM_TimeBaseStructure.TIM_Period = uus; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
			TIM_TimeBaseStructure.TIM_Prescaler =71; //4-1 设置用来作为TIMx时钟频率除数的预分频值
		}
		else //ms级别
		{
			uus /= 1000; 
			TIM_TimeBaseStructure.TIM_Period = (uus>>1); //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
			TIM_TimeBaseStructure.TIM_Prescaler =36000-1; //4-1 设置用来作为TIMx时钟频率除数的预分频值 72MHz/36000 = 72000000/36000=2000=2KHz
		}
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
	 
		if (!bInit)//中断优先级NVIC设置
		{
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
			NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
			NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
			bInit = TRUE;
		}
		
		TIM_ClearFlag(TIM3, TIM_FLAG_Update);//清除中断标志位
		TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断
		TIM_Cmd(TIM3, ENABLE);  //使能TIMx	
	}
	else
	{
		TIM_ClearFlag(TIM3, TIM_FLAG_Update);//清除中断标志位
		TIM_ITConfig(TIM3,TIM_IT_Update,DISABLE ); //禁止指定的TIM3中断,允许更新中断
		TIM_Cmd(TIM3, DISABLE);  //禁止TIMx	
		TIM_DeInit(TIM3);
	}
}

/* 定时器3清零 */
static void timer3_clear(void)
{
	TIM_SetCounter(TIM3, 0);
}

/* 定时器4初始化 */
static void timer4_init(BOOL bIsOn, UINT32 uus)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	static BOOL bInit = FALSE;
	
	if (bIsOn && (uus > 0))
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能
		
		TIM_DeInit(TIM4);
		//定时器TIM4初始化
		if (uus < 1000)//1000us = 1ms
		{
			TIM_TimeBaseStructure.TIM_Period = uus; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
			TIM_TimeBaseStructure.TIM_Prescaler =71; //4-1 设置用来作为TIMx时钟频率除数的预分频值
		}
		else //ms级别
		{
			uus /= 1000; 
			TIM_TimeBaseStructure.TIM_Period = (uus>>1); //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
			TIM_TimeBaseStructure.TIM_Prescaler =36000-1; //4-1 设置用来作为TIMx时钟频率除数的预分频值 72MHz/36000 = 72000000/36000=2000=2KHz
		}
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
		TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
	 

		if (!bInit)//中断优先级NVIC设置
		{
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
			NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4中断
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
			NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
			bInit = TRUE;
		}
		
		TIM_ClearFlag(TIM4, TIM_FLAG_Update);//清除中断标志位
		TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE ); //使能指定的TIM4中断,允许更新中断
		TIM_Cmd(TIM4, ENABLE);  //使能TIMx	
	}
	else
	{
		TIM_ClearFlag(TIM4, TIM_FLAG_Update);//清除中断标志位
		TIM_ITConfig(TIM4,TIM_IT_Update,DISABLE ); //禁止指定的TIM4中断,允许更新中断
		TIM_Cmd(TIM4, DISABLE);  //禁止TIMx	
		TIM_DeInit(TIM4);
	}
}

/* 定时器4清零 */
static void timer4_clear(void)
{
	TIM_SetCounter(TIM4, 0);
}

/* 定时器4初始化 */
static void timer5_init(BOOL bIsOn, UINT32 uus)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	static BOOL bInit = FALSE;
	
	if (bIsOn && (uus > 0))
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //时钟使能
		
		TIM_DeInit(TIM5);
		//定时器TIM5初始化
		if (uus < 1000)//1000us = 1ms
		{
			TIM_TimeBaseStructure.TIM_Period = uus; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
			TIM_TimeBaseStructure.TIM_Prescaler =71; //4-1 设置用来作为TIMx时钟频率除数的预分频值
		}
		else //ms级别
		{
			uus /= 1000; 
			TIM_TimeBaseStructure.TIM_Period = (uus>>1); //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
			TIM_TimeBaseStructure.TIM_Prescaler =36000-1; //4-1 设置用来作为TIMx时钟频率除数的预分频值 72MHz/36000 = 72000000/36000=2000=2KHz
		}
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
		TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
	 

		if (!bInit)//中断优先级NVIC设置
		{
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
			NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4中断
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
			NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
			bInit = TRUE;
		}
		
		TIM_ClearFlag(TIM5, TIM_FLAG_Update);//清除中断标志位
		TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE ); //使能指定的TIM4中断,允许更新中断
		TIM_Cmd(TIM5, ENABLE);  //使能TIMx	
	}
	else
	{
		TIM_ClearFlag(TIM5, TIM_FLAG_Update);//清除中断标志位
		TIM_ITConfig(TIM5,TIM_IT_Update,DISABLE ); //禁止指定的TIM4中断,允许更新中断
		TIM_Cmd(TIM5, DISABLE);  //禁止TIMx	
		TIM_DeInit(TIM5);
	}
}

/* 定时器4清零 */
static void timer5_clear(void)
{
	TIM_SetCounter(TIM5, 0);
}

/* 定时器6初始化 */
static void timer6_init(BOOL bIsOn, UINT32 uus)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	static BOOL bInit = FALSE;
	if (bIsOn && (uus > 0))
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //时钟使能
		
		TIM_DeInit(TIM6);
		//定时器TIM6初始化
		if (uus < 1000)//1000us = 1ms
		{
			TIM_TimeBaseStructure.TIM_Period = uus; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
			TIM_TimeBaseStructure.TIM_Prescaler =71; //4-1 设置用来作为TIMx时钟频率除数的预分频值
		}
		else //ms级别
		{
			uus /= 1000; 
			TIM_TimeBaseStructure.TIM_Period = (uus>>1); //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
			TIM_TimeBaseStructure.TIM_Prescaler =36000-1; //4-1 设置用来作为TIMx时钟频率除数的预分频值 72MHz/36000 = 72000000/36000=2000=2KHz
		}
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
		TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
	 

		if (!bInit)//中断优先级NVIC设置
		{
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
			NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;  //TIM3中断
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
			NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
			bInit = TRUE;
		}
		
		TIM_ClearFlag(TIM6, TIM_FLAG_Update);//清除中断标志位
		TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE ); //使能指定的TIM6中断,允许更新中断
		TIM_Cmd(TIM6, ENABLE);  //使能TIMx	
	}
	else
	{
		TIM_ClearFlag(TIM6, TIM_FLAG_Update);//清除中断标志位
		TIM_ITConfig(TIM6,TIM_IT_Update,DISABLE ); //禁止指定的TIM6中断,允许更新中断
		TIM_Cmd(TIM6, DISABLE);  //禁止TIMx	
		TIM_DeInit(TIM6);
	}
}

/* 定时器6清零 */
static void timer6_clear(void)
{
	TIM_SetCounter(TIM6, 0);
}

/* 定时器7初始化 */
static void timer7_init(BOOL bIsOn, UINT32 ums)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	static BOOL bInit = FALSE;

	if (bIsOn && (ums > 0))
	{
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE); //时钟使能
		
		TIM_DeInit(TIM7);
		//定时器TIM7初始化
		TIM_TimeBaseStructure.TIM_Period = 2*ums; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
		TIM_TimeBaseStructure.TIM_Prescaler =35999; //设置用来作为TIMx时钟频率除数的预分频值
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
		TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
	 

		if (!bInit)//中断优先级NVIC设置
		{
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
			NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;  //TIM3中断
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
			NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
			bInit = TRUE;
		}

		TIM_ClearFlag(TIM7, TIM_FLAG_Update);//清除中断标志位
		TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE ); //使能指定的TIM7中断,允许更新中断
		TIM_Cmd(TIM7, ENABLE);  //使能TIMx	
	}
	else
	{
		TIM_ClearFlag(TIM7, TIM_FLAG_Update);//清除中断标志位
		TIM_ITConfig(TIM7,TIM_IT_Update,DISABLE ); //禁止指定的TIM7中断,允许更新中断
		TIM_Cmd(TIM7, DISABLE);  //禁止TIMx	
		TIM_DeInit(TIM7);
	}
}

/* 定时器7清零 */
//static void timer7_clear(void)
//{
//	TIM_SetCounter(TIM7, 0);
//}

#ifdef STM32F10X_HD
/* 定时器8初始化 */
static void timer8_init(BOOL bIsOn, UINT32 uus)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	static BOOL bInit = FALSE;

	if (bIsOn && (uus > 0))
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE); //时钟使能
		
		TIM_DeInit(TIM8);
		//定时器TIM2初始化
		if (uus < 1000)//1000us = 1ms
		{
			TIM_TimeBaseStructure.TIM_Period = uus; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
			TIM_TimeBaseStructure.TIM_Prescaler =71; //4-1 设置用来作为TIMx时钟频率除数的预分频值
		}
		else //ms级别
		{
			uus /= 1000; 
			TIM_TimeBaseStructure.TIM_Period = (uus>>1); //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
			TIM_TimeBaseStructure.TIM_Prescaler =36000-1; //4-1 设置用来作为TIMx时钟频率除数的预分频值 72MHz/36000 = 72000000/36000=2000=2KHz
		}
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
		TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
	 
		if (!bInit)//中断优先级NVIC设置
		{
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
			NVIC_InitStructure.NVIC_IRQChannel = TIM8_UP_IRQn;  //TIM3中断
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
			NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
			bInit = TRUE;
		}
		
		/* 清除计数器中断标志位 */
		TIM_ClearITPendingBit(TIM8, TIM_IT_Update);
		TIM_ClearFlag(TIM8, TIM_FLAG_Update);//清除中断标志位
		TIM_ITConfig(TIM8,TIM_IT_Update,ENABLE ); //使能指定的TIM2中断,允许更新中断
		TIM_Cmd(TIM8, ENABLE);  //使能TIMx	
	}
	else
	{
		TIM_ClearFlag(TIM8, TIM_FLAG_Update);//清除中断标志位
		TIM_ITConfig(TIM8,TIM_IT_Update,DISABLE ); //禁止指定的TIM3中断,允许更新中断
		TIM_Cmd(TIM8, DISABLE);  //禁止TIMx	
		TIM_DeInit(TIM8);
	}
}

/* 定时器8清零 */
static void timer8_clear(void)
{
	TIM_SetCounter(TIM8, 0);
}
#endif // STM32F10X_HD

/* 定时器1中断服务程序 */
void TIM1_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)  //检查TIM2更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);  //清除TIMx更新中断标志 
		
		if (m_hIndeTimer[INDE_TIMER_TIM1] != NULL)
			m_hIndeTimer[INDE_TIMER_TIM1]();
	}
}

/* 定时器2中断服务程序 */
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM2更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx更新中断标志 
		
		if (m_hIndeTimer[INDE_TIMER_TIM2] != NULL)
			m_hIndeTimer[INDE_TIMER_TIM2]();
	}
}

/* 定时器3中断服务程序 */
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIMx更新中断标志 
		
		if (m_hIndeTimer[INDE_TIMER_TIM3] != NULL)
			m_hIndeTimer[INDE_TIMER_TIM3]();
	}
}

/* 定时器4中断服务程序 */
void TIM4_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)  //检查TIM4更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update  );  //清除TIMx更新中断标志 
		
		if (m_hIndeTimer[INDE_TIMER_TIM4] != NULL)
			m_hIndeTimer[INDE_TIMER_TIM4]();
	}
}

/* 定时器4中断服务程序 */
void TIM5_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)  //检查TIM4更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update  );  //清除TIMx更新中断标志 
		
		if (m_hIndeTimer[INDE_TIMER_TIM5] != NULL)
			m_hIndeTimer[INDE_TIMER_TIM5]();
	}
}

/* 定时器6中断服务程序 */
void TIM6_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)  //检查TIM6更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM6, TIM_IT_Update  );  //清除TIMx更新中断标志 
		
		if (m_hIndeTimer[INDE_TIMER_TIM6] != NULL)
			m_hIndeTimer[INDE_TIMER_TIM6]();
	}
}

/* 定时器7中断服务程序 */
void TIM7_IRQHandler(void)
{
	UINT16 i;
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)  //检查TIM7更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update  );  //清除TIMx更新中断标志 
		
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

#ifdef STM32F10X_HD
/* 定时器1中断服务程序 */
void TIM8_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM8, TIM_IT_Update) != RESET)  //检查TIM2更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM8, TIM_IT_Update);  //清除TIMx更新中断标志 
		
		if (m_hIndeTimer[INDE_TIMER_TIM8] != NULL)
			m_hIndeTimer[INDE_TIMER_TIM8]();
	}
}
#endif

static void InitIndtTimers(void)
{
	m_hTimerInitFunc[INDE_TIMER_TIM1] = timer1_init;
	m_hTimerInitFunc[INDE_TIMER_TIM2] = timer2_init;
	m_hTimerInitFunc[INDE_TIMER_TIM3] = timer3_init;
	m_hTimerInitFunc[INDE_TIMER_TIM4] = timer4_init;
	m_hTimerInitFunc[INDE_TIMER_TIM5] = timer5_init;
	m_hTimerInitFunc[INDE_TIMER_TIM6] = timer6_init;
#ifdef STM32F10X_HD
	m_hTimerInitFunc[INDE_TIMER_TIM8] = timer8_init;
#endif
	
	m_hClearTimer[INDE_TIMER_TIM1] = timer1_clear;
	m_hClearTimer[INDE_TIMER_TIM2] = timer2_clear;
	m_hClearTimer[INDE_TIMER_TIM3] = timer3_clear;
	m_hClearTimer[INDE_TIMER_TIM4] = timer4_clear;
	m_hClearTimer[INDE_TIMER_TIM5] = timer5_clear;
	m_hClearTimer[INDE_TIMER_TIM6] = timer6_clear;
#ifdef STM32F10X_HD
	m_hClearTimer[INDE_TIMER_TIM8] = timer8_clear;
#endif
}

/**
 * @brief 注册一个独立定时器
 * @param tID 定时器ID @ref IndeTimerIDsTypeDef
 * @param hFunc 共享定时器回调函数句柄
 * @param time  定时时间长度（单位：us）
 * @retval 注册成功返回当前定时器编号，失败返回-1
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
		if (m_hIndeTimer[tID] != hTimerFunc)//定时器已注册
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
 * @brief 更新独立定时器回调函数
 * @param tID 定时器ID @ref IndeTimerIDsTypeDef
 * @param hFunc 共享定时器回调函数句柄
 * @retval 注册成功返回当前定时器编号，失败返回-1
 */
INT32 HalUpdIndeTimerHandler(UINT32 tID, HalTimerCBack_t hTimerFunc)
{
	INT32 uID = -1;
	if (tID < INDE_TIMER_MAX)
	{
		if (m_hIndeTimer[tID] != hTimerFunc)//定时器已注册
		{
			m_hIndeTimer[tID] = hTimerFunc;
			uID = tID;
		}
	}
	return uID;
}
	
/**
 * @brief 注册一个共享定时器
 * @param hFunc 共享定时器回调函数句柄
 * @param time  定时时间长度（单位：ms）
 * @retval 注册成功返回当前定时器编号，失败返回-1
 */
INT32 HalSetShareTimer(HalTimerCBack_t hTimerFunc, UINT32 ums)
{
	UINT16 i;
	UINT32 utime = 1;
	if (NULL == hTimerFunc)
		return -1;
	if (m_uShareTimerCount >= SHARE_TIMER_MAX)//定时器数量过多
		return -1;
		
	timer7_init(TRUE, SHARE_TIMER_BASE_TIME);
	
	/* 计算共享定时器时间片 */
	if (ums > SHARE_TIMER_BASE_TIME)
	{
		utime = ums / SHARE_TIMER_BASE_TIME;
	}
	else
	{
		utime = 1;
	}
	
	/* 若定时器已经存在，则更新定时器时间 */
	for (i = 0; i < m_uShareTimerCount; ++i)
	{
		if (m_hShareTimer[i].hTimerFunc == hTimerFunc)
		{
			m_hShareTimer[i].uCounter = 0;
			m_hShareTimer[i].uTime = utime;
			return i + SHARE_TIMER_BASE_NUM;
		}
	}
	
	/* 定时器已经不存在，注册一个新的定时器 */
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
 * @brief 通过定时器句柄注销一个定时器
 * @param hFunc 定时器回调函数句柄
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
		if (hTimerFunc == m_hIndeTimer[i])//该定时器为独立定时器
		{
			m_hIndeTimer[i] = NULL;
			m_hTimerInitFunc[i](FALSE, 0);
			return;
		}
	}
	
	for (i = 0; i < m_uShareTimerCount; ++i)
	{
		if (hTimerFunc == m_hShareTimer[i].hTimerFunc)//该定时器为共享定时器
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
 * @brief 通过定时器ID注销一个定时器
 * @param uID 定时器ID
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
	
	if (uID < INDE_TIMER_MAX)//独立定时器
	{
		m_hIndeTimer[uID] = NULL;
		m_hTimerInitFunc[uID](FALSE, 0);
	}
	else if (uID - SHARE_TIMER_BASE_NUM < SHARE_TIMER_MAX)//共享定时器
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
 * @brief 通过定时器句柄清除定时器重新计时
 * @param hFunc 定时器回调函数句柄
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
		if (hTimerFunc == m_hIndeTimer[i])//该定时器为独立定时器
		{
			m_hClearTimer[i]();
			return;
		}
	}
	
	for (i = 0; i < m_uShareTimerCount; ++i)
	{
		if (hTimerFunc == m_hShareTimer[i].hTimerFunc)//该定时器为共享定时器
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
 * @brief 通过定时器ID注销清除定时器重新计时
 * @param uID 定时器ID
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
	
	if (uID < INDE_TIMER_MAX)//独立定时器
	{
		m_hClearTimer[uID]();
	}
	else if (uID - SHARE_TIMER_BASE_NUM < SHARE_TIMER_MAX)//共享定时器
	{
		i = uID - SHARE_TIMER_BASE_NUM;
		m_hShareTimer[i].uCounter = 0;
	}
}

#endif //CFG_HAL_TIMER
