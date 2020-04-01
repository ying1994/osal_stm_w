/**
 * @file    hal_enc.c
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   �������ӿ�
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

static UINT16 m_uPeriod[INDE_TIMER_MAX] = {0};//����
static UINT16 m_uOverflowCnt[INDE_TIMER_MAX] = {0};//�������������
static BOOL m_bIsMeasured[INDE_TIMER_MAX] = {0};//�Ƿ��Ѿ���������
static UINT32 m_uPreviousCnt[INDE_TIMER_MAX] = {0};//��һ�β��Ե�����

//static UINT16 m_uChannel[INDE_TIMER_MAX] = {0};//ͨ��
static const UINT8 m_uIRQChannel[INDE_TIMER_MAX] = {TIM1_UP_IRQn, TIM2_IRQn, TIM3_IRQn, TIM4_IRQn, TIM5_IRQn, TIM6_IRQn, TIM8_UP_IRQn};//�жϺ�

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

/* ��ʱ�� ENC ģʽ ��ʼ�� */
static void timer_enc_init(HalIndeTimerIDsTypeDef id, UINT32 uPeriod, UINT16 OCMode, UINT16 uOCPolarity, UINT16 uChannel, HalTimerCBack_t hfunc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef        TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//m_uChannel[id] = uChannel;
	
	TIM_TypeDef* TIMx = GetTimHandle(id);
	
	TIM_DeInit(TIMx);
	//��ʱ��TIM2��ʼ��
	m_uPeriod[id] = uPeriod; //��ʱ����: uPeriod+1
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Period = m_uPeriod[id]; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler = 0; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ �������Ƶ��:72MHz/(Prescaler+1)/(Period+1)
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIMx, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	/* ��ʱ�����ͨ��1ģʽ���� */
	/* ��ʼ��TIM���벶����� */
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
		/* CC1S=01 	ѡ������� IC1ӳ�䵽TI1�� */
		TIM_ICInitStructure.TIM_Channel     = uChannel;
		/* �����ز��� */
		TIM_ICInitStructure.TIM_ICPolarity  = uOCPolarity;	
		/* ӳ�䵽TI1�� */
		TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; 
		/* ���������Ƶ,����Ƶ  */
		TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	}	           
	/* IC1F=0000 ���������˲��� ���˲� */
	TIM_ICInitStructure.TIM_ICFilter    = 0x00;    
    TIM_ICInit(TIMx, &TIM_ICInitStructure);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = m_uIRQChannel[id];  //TIM �ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
	
    TIM_ClearFlag(TIMx, TIM_FLAG_Update);
    TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);
	
	HalUpdIndeTimerHandler(id, hfunc);

	//reset counter
	TIMx->CNT = 0x0000;
	/* ʹ�ܶ�ʱ�����ؼĴ���ARR */
	//TIM_ARRPreloadConfig(TIMx, ENABLE);
	TIM_Cmd(TIMx, ENABLE);  //ʹ��TIMx	
}

/* ��ʱ��2���� */
static void timer_enc_deInit(HalIndeTimerIDsTypeDef id)
{
	TIM_TypeDef* TIMx = GetTimHandle(id);
	HalClearTimerWithID(id);
	//TIM_ARRPreloadConfig(TIMx, DISABLE);
	TIM_Cmd(TIMx, DISABLE);  //��ֹTIMx	
	TIM_DeInit(TIMx);
}

/* ��ʱ�� ����ENCռ�ձ� */
static INT32 timer_enc_readEncoderCnt(HalIndeTimerIDsTypeDef id)
{
	TIM_TypeDef* TIMx = GetTimHandle(id);
    INT32 iDelta;
    UINT16 uOverflowCnt;
    UINT16 uCurrentCnt;
    INT32 temp;
    INT16 haux;

    if (m_bIsMeasured[id])//���B������ٶȻ�������
    {
        uOverflowCnt = m_uOverflowCnt[id];  //�õ�����ʱ���ڵı�����   
        uCurrentCnt = TIMx->CNT;
        haux = uCurrentCnt;
        m_uOverflowCnt[id] = 0;//����������ۼ�

        if ( (TIMx->CR1 & TIM_CounterMode_Down) == TIM_CounterMode_Down)  
        {
            // encoder timer down-counting ��ת���ٶȼ���
            iDelta = (INT32)((uOverflowCnt) * m_uPeriod[id] -(uCurrentCnt - m_uPreviousCnt[id]));  
        }
        else  
        {
            //encoder timer up-counting ��ת���ٶȼ���
            iDelta = (INT32)(uCurrentCnt - m_uPreviousCnt[id] + (uOverflowCnt) * m_uPeriod[id]);
        }
        temp=iDelta;
    }
    else
    {
        m_bIsMeasured[id] = TRUE;//���B������ٶȻ��������־λ
        temp = 0;
        m_uOverflowCnt[id] = 0;
        haux = TIMx->CNT;       
    }
    m_uPreviousCnt[id] = haux;  
	return temp; 
}

/* ��ʱ��1 ENC �ص����� */
static void timer1_enc_IrqHandler(void)
{
    if (m_uOverflowCnt[INDE_TIMER_TIM1] < 0xffff)//������Χ  
    {
        m_uOverflowCnt[INDE_TIMER_TIM1]++; //�������ۼ�
    }
}

/* ��ʱ��1 ENC ģʽ ��ʼ�� */
static void timer1_enc_init(UINT32 uPeriod, UINT16 OCMode, UINT16 uOCPolarity, UINT16 uChannel)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //ʱ��ʹ��
	
	/* ���ö�ʱ��ͨ��1�������ģʽ�������������ģʽ */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (uChannel == HAL_ENC_CH1)
	{
		GPIO_InitStructure.GPIO_Pin =  TIM1_ENC_CH1_PIN;
		GPIO_Init(TIM1_ENC_CH1_PORT, &GPIO_InitStructure);
	}
	/* ���ö�ʱ��ͨ��2�������ģʽ */
	else if (uChannel == HAL_ENC_CH2)
	{
		GPIO_InitStructure.GPIO_Pin = TIM1_ENC_CH2_PIN;
		GPIO_Init(TIM1_ENC_CH2_PORT, &GPIO_InitStructure);
	}
	/* ���ö�ʱ��ͨ��3�������ģʽ */
	else if (uChannel == HAL_ENC_CH3)
	{
		GPIO_InitStructure.GPIO_Pin = TIM1_ENC_CH3_PIN;
		GPIO_Init(TIM1_ENC_CH3_PORT, &GPIO_InitStructure);
	}
	/* ���ö�ʱ��ͨ��4�������ģʽ */
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

/* ��ʱ��1���� */
static void timer1_enc_deInit(void)
{
	timer_enc_deInit(INDE_TIMER_TIM1);
}

/* ��ʱ��1 ����ENCռ�ձ� */
static INT32 timer1_enc_readEncoderCnt(void)
{
	return timer_enc_readEncoderCnt(INDE_TIMER_TIM1);
}

/* ��ʱ��2 ENC �ص����� */
static void timer2_enc_IrqHandler(void)
{
    if (m_uOverflowCnt[INDE_TIMER_TIM2] < 0xffff)//������Χ  
    {
        m_uOverflowCnt[INDE_TIMER_TIM2]++; //�������ۼ�
    }
}

/* ��ʱ��2 ENC ģʽ ��ʼ�� */
static void timer2_enc_init(UINT32 uPeriod, UINT16 OCMode, UINT16 uOCPolarity, UINT16 uChannel)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��
	
	/* ���ö�ʱ��ͨ��1�������ģʽ�������������ģʽ */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (uChannel == HAL_ENC_CH1)
	{
		GPIO_InitStructure.GPIO_Pin =  TIM2_ENC_CH1_PIN;
		GPIO_Init(TIM2_ENC_CH1_PORT, &GPIO_InitStructure);
	}
	/* ���ö�ʱ��ͨ��2�������ģʽ */
	else if (uChannel == HAL_ENC_CH2)
	{
		GPIO_InitStructure.GPIO_Pin = TIM2_ENC_CH2_PIN;
		GPIO_Init(TIM2_ENC_CH2_PORT, &GPIO_InitStructure);
	}
	/* ���ö�ʱ��ͨ��3�������ģʽ */
	else if (uChannel == HAL_ENC_CH3)
	{
		GPIO_InitStructure.GPIO_Pin = TIM2_ENC_CH3_PIN;
		GPIO_Init(TIM2_ENC_CH3_PORT, &GPIO_InitStructure);
	}
	/* ���ö�ʱ��ͨ��4�������ģʽ */
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

/* ��ʱ��2���� */
static void timer2_enc_deInit(void)
{
	timer_enc_deInit(INDE_TIMER_TIM2);
}

/* ��ʱ��2 ����ENCռ�ձ� */
static INT32 timer2_enc_readEncoderCnt(void)
{
	return timer_enc_readEncoderCnt(INDE_TIMER_TIM2);
}

/* ��ʱ��3 ENC �ص����� */
static void timer3_enc_IrqHandler(void)
{
    if (m_uOverflowCnt[INDE_TIMER_TIM3] < 0xffff)//������Χ  
    {
        m_uOverflowCnt[INDE_TIMER_TIM3]++; //�������ۼ�
    }
}

/* ��ʱ��3 ENC ģʽ ��ʼ�� */
static void timer3_enc_init(UINT32 uPeriod, UINT16 OCMode, UINT16 uOCPolarity, UINT16 uChannel)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
	
	/* ���ö�ʱ��ͨ��1�������ģʽ�������������ģʽ */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (uChannel == HAL_ENC_CH1)
	{
		GPIO_InitStructure.GPIO_Pin =  TIM3_ENC_CH1_PIN;
		GPIO_Init(TIM3_ENC_CH1_PORT, &GPIO_InitStructure);
	}
	/* ���ö�ʱ��ͨ��2�������ģʽ */
	else if (uChannel == HAL_ENC_CH2)
	{
		GPIO_InitStructure.GPIO_Pin = TIM3_ENC_CH2_PIN;
		GPIO_Init(TIM3_ENC_CH2_PORT, &GPIO_InitStructure);
	}
	/* ���ö�ʱ��ͨ��3�������ģʽ */
	else if (uChannel == HAL_ENC_CH3)
	{
		GPIO_InitStructure.GPIO_Pin = TIM3_ENC_CH3_PIN;
		GPIO_Init(TIM3_ENC_CH3_PORT, &GPIO_InitStructure);
	}
	/* ���ö�ʱ��ͨ��4�������ģʽ */
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

/* ��ʱ��3���� */
static void timer3_enc_deInit(void)
{
	timer_enc_deInit(INDE_TIMER_TIM3);
}

/* ��ʱ��3 ����ENCռ�ձ� */
static INT32 timer3_enc_readEncoderCnt(void)
{
	return timer_enc_readEncoderCnt(INDE_TIMER_TIM3);
}

/* ��ʱ��4 ENC �ص����� */
static void timer4_enc_IrqHandler(void)
{
    if (m_uOverflowCnt[INDE_TIMER_TIM4] < 0xffff)//������Χ  
    {
        m_uOverflowCnt[INDE_TIMER_TIM4]++; //�������ۼ�
    }
}

/* ��ʱ��4 ENC ģʽ ��ʼ�� */
static void timer4_enc_init(UINT32 uPeriod, UINT16 OCMode, UINT16 uOCPolarity, UINT16 uChannel)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��
	
	/* ���ö�ʱ��ͨ��1�������ģʽ�������������ģʽ */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (uChannel == HAL_ENC_CH1)
	{
		GPIO_InitStructure.GPIO_Pin =  TIM4_ENC_CH1_PIN;
		GPIO_Init(TIM4_ENC_CH1_PORT, &GPIO_InitStructure);
	}
	/* ���ö�ʱ��ͨ��2�������ģʽ */
	else if (uChannel == HAL_ENC_CH2)
	{
		GPIO_InitStructure.GPIO_Pin = TIM4_ENC_CH2_PIN;
		GPIO_Init(TIM4_ENC_CH2_PORT, &GPIO_InitStructure);
	}
	/* ���ö�ʱ��ͨ��3�������ģʽ */
	else if (uChannel == HAL_ENC_CH3)
	{
		GPIO_InitStructure.GPIO_Pin = TIM4_ENC_CH3_PIN;
		GPIO_Init(TIM4_ENC_CH3_PORT, &GPIO_InitStructure);
	}
	/* ���ö�ʱ��ͨ��4�������ģʽ */
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

/* ��ʱ��4���� */
static void timer4_enc_deInit(void)
{
	timer_enc_deInit(INDE_TIMER_TIM4);
}

/* ��ʱ��4 ����ENCռ�ձ� */
static INT32 timer4_enc_readEncoderCnt(void)
{
	return timer_enc_readEncoderCnt(INDE_TIMER_TIM4);
}

/* ��ʱ��5 ENC �ص����� */
static void timer5_enc_IrqHandler(void)
{
    if (m_uOverflowCnt[INDE_TIMER_TIM5] < 0xffff)//������Χ  
    {
        m_uOverflowCnt[INDE_TIMER_TIM5]++; //�������ۼ�
    }
}

/* ��ʱ��5 ENC ģʽ ��ʼ�� */
static void timer5_enc_init(UINT32 uPeriod, UINT16 OCMode, UINT16 uOCPolarity, UINT16 uChannel)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //ʱ��ʹ��
	
	/* ���ö�ʱ��ͨ��1�������ģʽ�������������ģʽ */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (uChannel == HAL_ENC_CH1)
	{
		GPIO_InitStructure.GPIO_Pin =  TIM5_ENC_CH1_PIN;
		GPIO_Init(TIM5_ENC_CH1_PORT, &GPIO_InitStructure);
	}
	/* ���ö�ʱ��ͨ��2�������ģʽ */
	else if (uChannel == HAL_ENC_CH2)
	{
		GPIO_InitStructure.GPIO_Pin = TIM5_ENC_CH2_PIN;
		GPIO_Init(TIM5_ENC_CH2_PORT, &GPIO_InitStructure);
	}
	/* ���ö�ʱ��ͨ��3�������ģʽ */
	else if (uChannel == HAL_ENC_CH3)
	{
		GPIO_InitStructure.GPIO_Pin = TIM5_ENC_CH3_PIN;
		GPIO_Init(TIM5_ENC_CH3_PORT, &GPIO_InitStructure);
	}
	/* ���ö�ʱ��ͨ��4�������ģʽ */
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

/* ��ʱ��5���� */
static void timer5_enc_deInit(void)
{
	timer_enc_deInit(INDE_TIMER_TIM5);
}

/* ��ʱ��5 ����ENCռ�ձ� */
static INT32 timer5_enc_readEncoderCnt(void)
{
	return timer_enc_readEncoderCnt(INDE_TIMER_TIM5);
}

/* ��ʱ��8 ENC �ص����� */
static void timer8_enc_IrqHandler(void)
{
    if (m_uOverflowCnt[INDE_TIMER_TIM8] < 0xffff)//������Χ  
    {
        m_uOverflowCnt[INDE_TIMER_TIM8]++; //�������ۼ�
    }
}

/* ��ʱ��8 ENC ģʽ ��ʼ�� */
static void timer8_enc_init(UINT32 uPeriod, UINT16 OCMode, UINT16 uOCPolarity, UINT16 uChannel)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE); //ʱ��ʹ��
	
	/* ���ö�ʱ��ͨ��1�������ģʽ�������������ģʽ */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	if (uChannel == HAL_ENC_CH1)
	{
		GPIO_InitStructure.GPIO_Pin =  TIM8_ENC_CH1_PIN;
		GPIO_Init(TIM8_ENC_CH1_PORT, &GPIO_InitStructure);
	}
	/* ���ö�ʱ��ͨ��2�������ģʽ */
	else if (uChannel == HAL_ENC_CH2)
	{
		GPIO_InitStructure.GPIO_Pin = TIM8_ENC_CH2_PIN;
		GPIO_Init(TIM8_ENC_CH2_PORT, &GPIO_InitStructure);
	}
	/* ���ö�ʱ��ͨ��3�������ģʽ */
	else if (uChannel == HAL_ENC_CH3)
	{
		GPIO_InitStructure.GPIO_Pin = TIM8_ENC_CH3_PIN;
		GPIO_Init(TIM8_ENC_CH3_PORT, &GPIO_InitStructure);
	}
	/* ���ö�ʱ��ͨ��4�������ģʽ */
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

/* ��ʱ��8���� */
static void timer8_enc_deInit(void)
{
	timer_enc_deInit(INDE_TIMER_TIM8);
}

/* ��ʱ��8 ����ENCռ�ձ� */
static INT32 timer8_enc_readEncoderCnt(void)
{
	return timer_enc_readEncoderCnt(INDE_TIMER_TIM8);
}


/**
 * @brief ����ENC��������
 * @param eID ENC��ʱ�� @ref HalIndeTimerIDsTypeDef
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
 * @brief ��ȡENC�����ṹ���
 * @param eChannel ENC ͨ�� @ref HALENCNumer
 * @retval ENC�����ṹ���
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
