/**
 * @file    hal_enc.c
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   编码器接口
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 * COPYRIGHT NOTICE  
 * Copyright 2016, wsf 
 * All rights Reserved
 *
 */
#include "hal_enc.h"

#if defined(CFG_HAL_ENC) && defined(CFG_HAL_TIMER)

static HALEncTypeDef m_Instance[INDE_TIMER_MAX];
static HALEncTypeDef *m_pthis[INDE_TIMER_MAX] = {NULL};

static UINT16 m_uPeriod[INDE_TIMER_MAX] = {0};//周期
static UINT16 m_uOverflowCnt[INDE_TIMER_MAX] = {0};//计数器溢出次数
static BOOL m_bIsMeasured[INDE_TIMER_MAX] = {0};//是否已经启动测试
static UINT32 m_uPreviousCnt[INDE_TIMER_MAX] = {0};//上一次测试的数据

//static UINT16 m_uChannel[INDE_TIMER_MAX] = {0};//通道
static const UINT8 m_uIRQChannel[INDE_TIMER_MAX] = {TIM1_UP_IRQn, TIM2_IRQn, TIM3_IRQn, TIM4_IRQn, TIM5_IRQn, TIM6_IRQn, TIM8_UP_IRQn};//中断号

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

/* 定时器 ENC 模式 初始化 */
static void timer_enc_init(HalIndeTimerIDsTypeDef id, UINT32 uPeriod, UINT16 OCMode, UINT16 uOCPolarity, UINT16 uChannel, HalTimerCBack_t hfunc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef        TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//m_uChannel[id] = uChannel;
	
	TIM_TypeDef* TIMx = GetTimHandle(id);
	
	TIM_DeInit(TIMx);
	//定时器TIM2初始化
	m_uPeriod[id] = uPeriod; //定时周期: uPeriod+1
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = m_uPeriod[id]; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler = 0; //设置用来作为TIMx时钟频率除数的预分频值 输出脉冲频率:72MHz/(Prescaler+1)/(Period+1)
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	/* 定时器输出通道1模式配置 */
	/* 初始化TIM输入捕获参数 */
	if ((uChannel & HAL_ENC_CH1) && (uChannel & HAL_ENC_CH2))
	{
		TIM_EncoderInterfaceConfig(TIMx, 
								TIM_EncoderMode_TI12,
								TIM_ICPolarity_Rising,
								TIM_ICPolarity_Rising);
		TIM_ICStructInit(&TIM_ICInitStructure);
	}
	else
	{
		if (uChannel == HAL_ENC_CH1)
			uChannel = TIM_Channel_1;
		else if (uChannel == HAL_ENC_CH2)
			uChannel = TIM_Channel_2;
		else if (uChannel == HAL_ENC_CH3)
			uChannel = TIM_Channel_3;
		else if (uChannel == HAL_ENC_CH4)
			uChannel = TIM_Channel_4;
		TIM_ICStructInit(&TIM_ICInitStructure);
		/* CC1S=01 	选择输入端 IC1映射到TI1上 */
		TIM_ICInitStructure.TIM_Channel     = uChannel;
		/* 上升沿捕获 */
		TIM_ICInitStructure.TIM_ICPolarity  = uOCPolarity;	
		/* 映射到TI1上 */
		TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; 
		/* 配置输入分频,不分频  */
		TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	}	           
	/* IC1F=0000 配置输入滤波器 不滤波 */
	TIM_ICInitStructure.TIM_ICFilter    = 0x00;    
    TIM_ICInit(TIMx, &TIM_ICInitStructure);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = m_uIRQChannel[id];  //TIM 中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器
	
    TIM_ClearFlag(TIMx, TIM_FLAG_Update);
    TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
	
	HalUpdIndeTimerHandler(id, hfunc);

	//reset counter
	TIMx->CNT = 0x0000;
	/* 使能定时器重载寄存器ARR */
	//TIM_ARRPreloadConfig(TIMx, ENABLE);
	TIM_Cmd(TIMx, ENABLE);  //使能TIMx	
}

/* 定时器2清零 */
static void timer_enc_deInit(HalIndeTimerIDsTypeDef id)
{
	TIM_TypeDef* TIMx = GetTimHandle(id);
	HalClearTimerWithID(id);
	//TIM_ARRPreloadConfig(TIMx, DISABLE);
	TIM_Cmd(TIMx, DISABLE);  //禁止TIMx	
	TIM_DeInit(TIMx);
}

/* 定时器 设置ENC占空比 */
static INT32 timer_enc_readEncoderCnt(HalIndeTimerIDsTypeDef id)
{
	TIM_TypeDef* TIMx = GetTimHandle(id);
    INT32 iDelta;
    UINT16 uOverflowCnt;
    UINT16 uCurrentCnt;
    INT32 temp;
    INT16 haux;

    if (m_bIsMeasured[id])//电机B以清除速度缓存数组
    {
        uOverflowCnt = m_uOverflowCnt[id];  //得到采样时间内的编码数   
        uCurrentCnt = TIMx->CNT;
        haux = uCurrentCnt;
        m_uOverflowCnt[id] = 0;//清除脉冲数累加

        if ( (TIMx->CR1 & TIM_CounterMode_Down) == TIM_CounterMode_Down)  
        {
            // encoder timer down-counting 反转的速度计算
            iDelta = (INT32)((uOverflowCnt) * m_uPeriod[id] -(uCurrentCnt - m_uPreviousCnt[id]));  
        }
        else  
        {
            //encoder timer up-counting 正转的速度计算
            iDelta = (INT32)(uCurrentCnt - m_uPreviousCnt[id] + (uOverflowCnt) * m_uPeriod[id]);
        }
        temp=iDelta;
    }
    else
    {
        m_bIsMeasured[id] = TRUE;//电机B以清除速度缓存数组标志位
        temp = 0;
        m_uOverflowCnt[id] = 0;
        haux = TIMx->CNT;       
    }
    m_uPreviousCnt[id] = haux;  
	return temp; 
}

/* 定时器1 ENC 回调函数 */
static void timer1_enc_IrqHandler(void)
{
    if (m_uOverflowCnt[INDE_TIMER_TIM1] < 0xffff)//不超范围  
    {
        m_uOverflowCnt[INDE_TIMER_TIM1]++; //脉冲数累加
    }
}

/* 定时器1 ENC 模式 初始化 */
static void timer1_enc_init(UINT32 uPeriod, UINT16 OCMode, UINT16 uOCPolarity, UINT16 uChannel)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //时钟使能
	
	/* 配置定时器通道1输出引脚模式：复用推挽输出模式 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (uChannel == HAL_ENC_CH1)
	{
		GPIO_InitStructure.GPIO_Pin =  TIM1_ENC_CH1_PIN;
		GPIO_Init(TIM1_ENC_CH1_PORT, &GPIO_InitStructure);
	}
	/* 配置定时器通道2输出引脚模式 */
	else if (uChannel == HAL_ENC_CH2)
	{
		GPIO_InitStructure.GPIO_Pin = TIM1_ENC_CH2_PIN;
		GPIO_Init(TIM1_ENC_CH2_PORT, &GPIO_InitStructure);
	}
	/* 配置定时器通道3输出引脚模式 */
	else if (uChannel == HAL_ENC_CH3)
	{
		GPIO_InitStructure.GPIO_Pin = TIM1_ENC_CH3_PIN;
		GPIO_Init(TIM1_ENC_CH3_PORT, &GPIO_InitStructure);
	}
	/* 配置定时器通道4输出引脚模式 */
	else if (uChannel == HAL_ENC_CH4)
	{
		GPIO_InitStructure.GPIO_Pin = TIM1_ENC_CH4_PIN;
		GPIO_Init(TIM1_ENC_CH4_PORT, &GPIO_InitStructure);
	}
	else if ((uChannel & HAL_ENC_CH1) && (uChannel & HAL_ENC_CH2))
	{
		GPIO_InitStructure.GPIO_Pin = TIM1_ENC_CH1_PIN | TIM1_ENC_CH2_PIN;;
		GPIO_Init(TIM1_ENC_CH1_PORT, &GPIO_InitStructure);
	}

	timer_enc_init(INDE_TIMER_TIM1, uPeriod, OCMode, uOCPolarity, uChannel, timer1_enc_IrqHandler);
}

/* 定时器1清零 */
static void timer1_enc_deInit(void)
{
	timer_enc_deInit(INDE_TIMER_TIM1);
}

/* 定时器1 设置ENC占空比 */
static INT32 timer1_enc_readEncoderCnt(void)
{
	return timer_enc_readEncoderCnt(INDE_TIMER_TIM1);
}

/* 定时器2 ENC 回调函数 */
static void timer2_enc_IrqHandler(void)
{
    if (m_uOverflowCnt[INDE_TIMER_TIM2] < 0xffff)//不超范围  
    {
        m_uOverflowCnt[INDE_TIMER_TIM2]++; //脉冲数累加
    }
}

/* 定时器2 ENC 模式 初始化 */
static void timer2_enc_init(UINT32 uPeriod, UINT16 OCMode, UINT16 uOCPolarity, UINT16 uChannel)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能
	
	/* 配置定时器通道1输出引脚模式：复用推挽输出模式 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (uChannel == HAL_ENC_CH1)
	{
		GPIO_InitStructure.GPIO_Pin =  TIM2_ENC_CH1_PIN;
		GPIO_Init(TIM2_ENC_CH1_PORT, &GPIO_InitStructure);
	}
	/* 配置定时器通道2输出引脚模式 */
	else if (uChannel == HAL_ENC_CH2)
	{
		GPIO_InitStructure.GPIO_Pin = TIM2_ENC_CH2_PIN;
		GPIO_Init(TIM2_ENC_CH2_PORT, &GPIO_InitStructure);
	}
	/* 配置定时器通道3输出引脚模式 */
	else if (uChannel == HAL_ENC_CH3)
	{
		GPIO_InitStructure.GPIO_Pin = TIM2_ENC_CH3_PIN;
		GPIO_Init(TIM2_ENC_CH3_PORT, &GPIO_InitStructure);
	}
	/* 配置定时器通道4输出引脚模式 */
	else if (uChannel == HAL_ENC_CH4)
	{
		GPIO_InitStructure.GPIO_Pin = TIM2_ENC_CH4_PIN;
		GPIO_Init(TIM2_ENC_CH4_PORT, &GPIO_InitStructure);
	}
	else if ((uChannel & HAL_ENC_CH1) && (uChannel & HAL_ENC_CH2))
	{
		GPIO_InitStructure.GPIO_Pin = TIM2_ENC_CH1_PIN | TIM2_ENC_CH2_PIN;;
		GPIO_Init(TIM2_ENC_CH1_PORT, &GPIO_InitStructure);
	}

	timer_enc_init(INDE_TIMER_TIM2, uPeriod, OCMode, uOCPolarity, uChannel, timer2_enc_IrqHandler);
}

/* 定时器2清零 */
static void timer2_enc_deInit(void)
{
	timer_enc_deInit(INDE_TIMER_TIM2);
}

/* 定时器2 设置ENC占空比 */
static INT32 timer2_enc_readEncoderCnt(void)
{
	return timer_enc_readEncoderCnt(INDE_TIMER_TIM2);
}

/* 定时器3 ENC 回调函数 */
static void timer3_enc_IrqHandler(void)
{
    if (m_uOverflowCnt[INDE_TIMER_TIM3] < 0xffff)//不超范围  
    {
        m_uOverflowCnt[INDE_TIMER_TIM3]++; //脉冲数累加
    }
}

/* 定时器3 ENC 模式 初始化 */
static void timer3_enc_init(UINT32 uPeriod, UINT16 OCMode, UINT16 uOCPolarity, UINT16 uChannel)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
	
	/* 配置定时器通道1输出引脚模式：复用推挽输出模式 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (uChannel == HAL_ENC_CH1)
	{
		GPIO_InitStructure.GPIO_Pin =  TIM3_ENC_CH1_PIN;
		GPIO_Init(TIM3_ENC_CH1_PORT, &GPIO_InitStructure);
	}
	/* 配置定时器通道2输出引脚模式 */
	else if (uChannel == HAL_ENC_CH2)
	{
		GPIO_InitStructure.GPIO_Pin = TIM3_ENC_CH2_PIN;
		GPIO_Init(TIM3_ENC_CH2_PORT, &GPIO_InitStructure);
	}
	/* 配置定时器通道3输出引脚模式 */
	else if (uChannel == HAL_ENC_CH3)
	{
		GPIO_InitStructure.GPIO_Pin = TIM3_ENC_CH3_PIN;
		GPIO_Init(TIM3_ENC_CH3_PORT, &GPIO_InitStructure);
	}
	/* 配置定时器通道4输出引脚模式 */
	else if (uChannel == HAL_ENC_CH4)
	{
		GPIO_InitStructure.GPIO_Pin = TIM3_ENC_CH4_PIN;
		GPIO_Init(TIM3_ENC_CH4_PORT, &GPIO_InitStructure);
	}
	else if ((uChannel & HAL_ENC_CH1) && (uChannel & HAL_ENC_CH2))
	{
		GPIO_InitStructure.GPIO_Pin = TIM3_ENC_CH1_PIN | TIM3_ENC_CH2_PIN;;
		GPIO_Init(TIM3_ENC_CH1_PORT, &GPIO_InitStructure);
	}

	timer_enc_init(INDE_TIMER_TIM3, uPeriod, OCMode, uOCPolarity, uChannel, timer3_enc_IrqHandler);
}

/* 定时器3清零 */
static void timer3_enc_deInit(void)
{
	timer_enc_deInit(INDE_TIMER_TIM3);
}

/* 定时器3 设置ENC占空比 */
static INT32 timer3_enc_readEncoderCnt(void)
{
	return timer_enc_readEncoderCnt(INDE_TIMER_TIM3);
}

/* 定时器4 ENC 回调函数 */
static void timer4_enc_IrqHandler(void)
{
    if (m_uOverflowCnt[INDE_TIMER_TIM4] < 0xffff)//不超范围  
    {
        m_uOverflowCnt[INDE_TIMER_TIM4]++; //脉冲数累加
    }
}

/* 定时器4 ENC 模式 初始化 */
static void timer4_enc_init(UINT32 uPeriod, UINT16 OCMode, UINT16 uOCPolarity, UINT16 uChannel)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能
	
	/* 配置定时器通道1输出引脚模式：复用推挽输出模式 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (uChannel == HAL_ENC_CH1)
	{
		GPIO_InitStructure.GPIO_Pin =  TIM4_ENC_CH1_PIN;
		GPIO_Init(TIM4_ENC_CH1_PORT, &GPIO_InitStructure);
	}
	/* 配置定时器通道2输出引脚模式 */
	else if (uChannel == HAL_ENC_CH2)
	{
		GPIO_InitStructure.GPIO_Pin = TIM4_ENC_CH2_PIN;
		GPIO_Init(TIM4_ENC_CH2_PORT, &GPIO_InitStructure);
	}
	/* 配置定时器通道3输出引脚模式 */
	else if (uChannel == HAL_ENC_CH3)
	{
		GPIO_InitStructure.GPIO_Pin = TIM4_ENC_CH3_PIN;
		GPIO_Init(TIM4_ENC_CH3_PORT, &GPIO_InitStructure);
	}
	/* 配置定时器通道4输出引脚模式 */
	else if (uChannel == HAL_ENC_CH4)
	{
		GPIO_InitStructure.GPIO_Pin = TIM4_ENC_CH4_PIN;
		GPIO_Init(TIM4_ENC_CH4_PORT, &GPIO_InitStructure);
	}
	else if ((uChannel & HAL_ENC_CH1) && (uChannel & HAL_ENC_CH2))
	{
		GPIO_InitStructure.GPIO_Pin = TIM4_ENC_CH1_PIN | TIM4_ENC_CH2_PIN;;
		GPIO_Init(TIM4_ENC_CH1_PORT, &GPIO_InitStructure);
	}

	timer_enc_init(INDE_TIMER_TIM4, uPeriod, OCMode, uOCPolarity, uChannel, timer4_enc_IrqHandler);
}

/* 定时器4清零 */
static void timer4_enc_deInit(void)
{
	timer_enc_deInit(INDE_TIMER_TIM4);
}

/* 定时器4 设置ENC占空比 */
static INT32 timer4_enc_readEncoderCnt(void)
{
	return timer_enc_readEncoderCnt(INDE_TIMER_TIM4);
}

/* 定时器5 ENC 回调函数 */
static void timer5_enc_IrqHandler(void)
{
    if (m_uOverflowCnt[INDE_TIMER_TIM5] < 0xffff)//不超范围  
    {
        m_uOverflowCnt[INDE_TIMER_TIM5]++; //脉冲数累加
    }
}

/* 定时器5 ENC 模式 初始化 */
static void timer5_enc_init(UINT32 uPeriod, UINT16 OCMode, UINT16 uOCPolarity, UINT16 uChannel)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //时钟使能
	
	/* 配置定时器通道1输出引脚模式：复用推挽输出模式 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (uChannel == HAL_ENC_CH1)
	{
		GPIO_InitStructure.GPIO_Pin =  TIM5_ENC_CH1_PIN;
		GPIO_Init(TIM5_ENC_CH1_PORT, &GPIO_InitStructure);
	}
	/* 配置定时器通道2输出引脚模式 */
	else if (uChannel == HAL_ENC_CH2)
	{
		GPIO_InitStructure.GPIO_Pin = TIM5_ENC_CH2_PIN;
		GPIO_Init(TIM5_ENC_CH2_PORT, &GPIO_InitStructure);
	}
	/* 配置定时器通道3输出引脚模式 */
	else if (uChannel == HAL_ENC_CH3)
	{
		GPIO_InitStructure.GPIO_Pin = TIM5_ENC_CH3_PIN;
		GPIO_Init(TIM5_ENC_CH3_PORT, &GPIO_InitStructure);
	}
	/* 配置定时器通道4输出引脚模式 */
	else if (uChannel == HAL_ENC_CH4)
	{
		GPIO_InitStructure.GPIO_Pin = TIM5_ENC_CH4_PIN;
		GPIO_Init(TIM5_ENC_CH4_PORT, &GPIO_InitStructure);
	}
	else if ((uChannel & HAL_ENC_CH1) && (uChannel & HAL_ENC_CH2))
	{
		GPIO_InitStructure.GPIO_Pin = TIM5_ENC_CH1_PIN | TIM5_ENC_CH2_PIN;;
		GPIO_Init(TIM5_ENC_CH1_PORT, &GPIO_InitStructure);
	}

	timer_enc_init(INDE_TIMER_TIM5, uPeriod, OCMode, uOCPolarity, uChannel, timer5_enc_IrqHandler);
}

/* 定时器5清零 */
static void timer5_enc_deInit(void)
{
	timer_enc_deInit(INDE_TIMER_TIM5);
}

/* 定时器5 设置ENC占空比 */
static INT32 timer5_enc_readEncoderCnt(void)
{
	return timer_enc_readEncoderCnt(INDE_TIMER_TIM5);
}

/* 定时器8 ENC 回调函数 */
static void timer8_enc_IrqHandler(void)
{
    if (m_uOverflowCnt[INDE_TIMER_TIM8] < 0xffff)//不超范围  
    {
        m_uOverflowCnt[INDE_TIMER_TIM8]++; //脉冲数累加
    }
}

/* 定时器8 ENC 模式 初始化 */
static void timer8_enc_init(UINT32 uPeriod, UINT16 OCMode, UINT16 uOCPolarity, UINT16 uChannel)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE); //时钟使能
	
	/* 配置定时器通道1输出引脚模式：复用推挽输出模式 */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (uChannel == HAL_ENC_CH1)
	{
		GPIO_InitStructure.GPIO_Pin =  TIM8_ENC_CH1_PIN;
		GPIO_Init(TIM8_ENC_CH1_PORT, &GPIO_InitStructure);
	}
	/* 配置定时器通道2输出引脚模式 */
	else if (uChannel == HAL_ENC_CH2)
	{
		GPIO_InitStructure.GPIO_Pin = TIM8_ENC_CH2_PIN;
		GPIO_Init(TIM8_ENC_CH2_PORT, &GPIO_InitStructure);
	}
	/* 配置定时器通道3输出引脚模式 */
	else if (uChannel == HAL_ENC_CH3)
	{
		GPIO_InitStructure.GPIO_Pin = TIM8_ENC_CH3_PIN;
		GPIO_Init(TIM8_ENC_CH3_PORT, &GPIO_InitStructure);
	}
	/* 配置定时器通道4输出引脚模式 */
	else if (uChannel == HAL_ENC_CH4)
	{
		GPIO_InitStructure.GPIO_Pin = TIM8_ENC_CH4_PIN;
		GPIO_Init(TIM8_ENC_CH4_PORT, &GPIO_InitStructure);
	}
	else if ((uChannel & HAL_ENC_CH1) && (uChannel & HAL_ENC_CH2))
	{
		GPIO_InitStructure.GPIO_Pin = TIM8_ENC_CH1_PIN | TIM8_ENC_CH2_PIN;;
		GPIO_Init(TIM8_ENC_CH1_PORT, &GPIO_InitStructure);
	}

	timer_enc_init(INDE_TIMER_TIM8, uPeriod, OCMode, uOCPolarity, uChannel, timer8_enc_IrqHandler);
}

/* 定时器8清零 */
static void timer8_enc_deInit(void)
{
	timer_enc_deInit(INDE_TIMER_TIM8);
}

/* 定时器8 设置ENC占空比 */
static INT32 timer8_enc_readEncoderCnt(void)
{
	return timer_enc_readEncoderCnt(INDE_TIMER_TIM8);
}


/**
 * @brief 申请ENC操作对象
 * @param eID ENC定时器 @ref HalIndeTimerIDsTypeDef
 * @retval None
 */
static void New(HalIndeTimerIDsTypeDef eId)
{
	switch (eId)
	{
	case INDE_TIMER_TIM1:
		m_Instance[INDE_TIMER_TIM1].init = timer1_enc_init;
		m_Instance[INDE_TIMER_TIM1].deInit = timer1_enc_deInit;
		m_Instance[INDE_TIMER_TIM1].readEncoderCnt = timer1_enc_readEncoderCnt;
		m_pthis[INDE_TIMER_TIM1] = &m_Instance[INDE_TIMER_TIM1];
		break;
	case INDE_TIMER_TIM2:
		m_Instance[INDE_TIMER_TIM2].init = timer2_enc_init;
		m_Instance[INDE_TIMER_TIM2].deInit = timer2_enc_deInit;
		m_Instance[INDE_TIMER_TIM2].readEncoderCnt = timer2_enc_readEncoderCnt;
		m_pthis[INDE_TIMER_TIM2] = &m_Instance[INDE_TIMER_TIM2];
		break;
	case INDE_TIMER_TIM3:
		m_Instance[INDE_TIMER_TIM3].init = timer3_enc_init;
		m_Instance[INDE_TIMER_TIM3].deInit = timer3_enc_deInit;
		m_Instance[INDE_TIMER_TIM3].readEncoderCnt = timer3_enc_readEncoderCnt;
		m_pthis[INDE_TIMER_TIM3] = &m_Instance[INDE_TIMER_TIM3];
		break;
	case INDE_TIMER_TIM4:
		m_Instance[INDE_TIMER_TIM4].init = timer4_enc_init;
		m_Instance[INDE_TIMER_TIM4].deInit = timer4_enc_deInit;
		m_Instance[INDE_TIMER_TIM4].readEncoderCnt = timer4_enc_readEncoderCnt;
		m_pthis[INDE_TIMER_TIM4] = &m_Instance[INDE_TIMER_TIM4];
		break;
	case INDE_TIMER_TIM5:
		m_Instance[INDE_TIMER_TIM5].init = timer5_enc_init;
		m_Instance[INDE_TIMER_TIM5].deInit = timer5_enc_deInit;
		m_Instance[INDE_TIMER_TIM5].readEncoderCnt = timer5_enc_readEncoderCnt;
		m_pthis[INDE_TIMER_TIM5] = &m_Instance[INDE_TIMER_TIM5];
		break;
	case INDE_TIMER_TIM6:
		break;
	case INDE_TIMER_TIM8:
		m_Instance[INDE_TIMER_TIM8].init = timer8_enc_init;
		m_Instance[INDE_TIMER_TIM8].deInit = timer8_enc_deInit;
		m_Instance[INDE_TIMER_TIM8].readEncoderCnt = timer8_enc_readEncoderCnt;
		m_pthis[INDE_TIMER_TIM8] = &m_Instance[INDE_TIMER_TIM8];
		break;
	default:
		break;
	}
}

/**
 * @brief 获取ENC操作结构句柄
 * @param eChannel ENC 通道 @ref HALENCNumer
 * @retval ENC操作结构句柄
 */
HALEncTypeDef* HalEncGetInstance(HalIndeTimerIDsTypeDef eId)
{
	if (NULL == m_pthis[eId])
	{
		New(eId);
	}
	return m_pthis[eId];
}

#endif //defined(CFG_HAL_ENC) && defined(CFG_HAL_TIMER)
