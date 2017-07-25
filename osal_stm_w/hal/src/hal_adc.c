/**
 * @file    hal_adc.c
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
#include  "hal_adc.h"
#include  "hal_defs.h"
#include  "hal_types.h"

#ifdef CFG_HAL_ADC


static HALAdcTypeDef m_hAdcInstance[HAL_ADC_SIZE];
static HALAdcTypeDef* pthis[HAL_ADC_SIZE] = {NULL};

/**
 * @brief: ADC��Դ��ʼ��
 * @param: ADCx ADCͨ��
 * @retval: void
 */
static void HalAdcInit (ADC_TypeDef* ADCx)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	__IO uint16_t ADCConvertedValue;

	if (ADC1 == ADCx)
	{
		/* ʹ�� ADC1 and GPIOC clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);
		/* ����PC4Ϊģ������(ADC Channel14) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}
	else if (ADC2 == ADCx)
	{
		/* ʹ�� ADC1 and GPIOC clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2 | RCC_APB2Periph_GPIOC, ENABLE);
		/* ����PC4Ϊģ������(ADC Channel14) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}
	else //if (ADC3 == ADCx)
	{
		/* ʹ�� ADC1 and GPIOC clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3 | RCC_APB2Periph_GPIOC, ENABLE);
		/* ����PC4Ϊģ������(ADC Channel14) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}

	/* ����ADC1, ����DMA */
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADCx, &ADC_InitStructure);

	/* ADCx regular channel14 configuration */
	ADC_RegularChannelConfig(ADCx, ADC_Channel_14, 1, ADC_SampleTime_55Cycles5);

	/* Enable ADCx DMA */
	ADC_DMACmd(ADCx, ENABLE);

	/* Enable ADCx */
	ADC_Cmd(ADCx, ENABLE);

	/* Enable ADCx reset calibaration register */
	ADC_ResetCalibration(ADCx);
	/* Check the end of ADCx reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADCx));

	/* Start ADCx calibaration */
	ADC_StartCalibration(ADCx);
	/* Check the end of ADCx calibration */
	while(ADC_GetCalibrationStatus(ADCx));

	/* Start ADCx Software Conversion */
	ADC_SoftwareStartConvCmd(ADCx, ENABLE);
}

/**
 * @brief: ע��ADC��Դ
 * @param: ADCx ADCͨ��
 * @retval: void
 */
static void HalAdcDeInit (ADC_TypeDef* ADCx)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	if (ADC1 == ADCx)
	{
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}
	else if (ADC2 == ADCx)
	{
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}
	else //if (ADC3 == ADCx)
	{
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}
	
	ADC_DeInit(ADCx);
}


/**
 * @brief: ADC��Դ��ʼ��
 * @param: void
 * @retval: void
 */
static void HalAdc0Init (void)
{
	HalAdcInit(ADC1);
}

/**
 * @brief: ע��ADC��Դ
 * @param: void
 * @retval: void
 */
static void HalAdc0DeInit (void)
{
	HalAdcDeInit(ADC1);
}

/**
 * @brief: ��ָ��ADCͨ�����ֱ��ʶ�ȡֵ
 * @param: resolution ָ��ADC�ֱ��ʣ�8λ��12λ��16λ��
 * @retval: ADCת�����
 */
static uint16 HalAdc0Read (uint8 resolution)
{
  int16  reading = 0;

  return ((uint16)reading);
}

/**
 * @brief: ����ADC�ο���ѹ
 * @param: reference ADC�ο���ѹ
 * @retval: void
 */
static void HalAdc0SetReference ( uint8 reference )
{

}

/**
 * @brief: Check for minimum Vdd specified.
 * @param: vdd - The board-specific Vdd reading to check for.
 * @retval: TRUE if the Vdd measured is greater than the 'vdd' minimum parameter;
 *          FALSE if not.
 */
static bool HalAdc0CheckVdd(uint8 vdd)
{
	return 0;
}

/**
 * @brief: ADC��Դ��ʼ��
 * @param: void
 * @retval: void
 */
static void HalAdc1Init (void)
{

}

/**
 * @brief: ע��ADC��Դ
 * @param: void
 * @retval: void
 */
static void HalAdc1DeInit (void)
{

}

/**
 * @brief: ��ָ��ADCͨ�����ֱ��ʶ�ȡֵ
 * @param: resolution ָ��ADC�ֱ��ʣ�8λ��12λ��16λ��
 * @retval: ADCת�����
 */
static uint16 HalAdc1Read (uint8 resolution)
{
  int16  reading = 0;

  return ((uint16)reading);
}

/**
 * @brief: ����ADC�ο���ѹ
 * @param: reference ADC�ο���ѹ
 * @retval: void
 */
static void HalAdc1SetReference ( uint8 reference )
{

}

/**
 * @brief: Check for minimum Vdd specified.
 * @param: vdd - The board-specific Vdd reading to check for.
 * @retval: TRUE if the Vdd measured is greater than the 'vdd' minimum parameter;
 *          FALSE if not.
 */
static bool HalAdc1CheckVdd(uint8 vdd)
{
	return 0;
}

/**
 * @brief: ADC��Դ��ʼ��
 * @param: void
 * @retval: void
 */
static void HalAdc2Init (void)
{

}

/**
 * @brief: ע��ADC��Դ
 * @param: void
 * @retval: void
 */
static void HalAdc2DeInit (void)
{

}

/**
 * @brief: ��ָ��ADCͨ�����ֱ��ʶ�ȡֵ
 * @param: resolution ָ��ADC�ֱ��ʣ�8λ��12λ��16λ��
 * @retval: ADCת�����
 */
static uint16 HalAdc2Read (uint8 resolution)
{
  int16  reading = 0;

  return ((uint16)reading);
}

/**
 * @brief: ����ADC�ο���ѹ
 * @param: reference ADC�ο���ѹ
 * @retval: void
 */
static void HalAdc2SetReference ( uint8 reference )
{

}

/**
 * @brief: Check for minimum Vdd specified.
 * @param: vdd - The board-specific Vdd reading to check for.
 * @retval: TRUE if the Vdd measured is greater than the 'vdd' minimum parameter;
 *          FALSE if not.
 */
static bool HalAdc2CheckVdd(uint8 vdd)
{
	return 0;
}

/**
 * @brief: ADC��Դ��ʼ��
 * @param: void
 * @retval: void
 */
static void HalAdc3Init (void)
{

}

/**
 * @brief: ע��ADC��Դ
 * @param: void
 * @retval: void
 */
static void HalAdc3DeInit (void)
{

}

/**
 * @brief: ��ָ��ADCͨ�����ֱ��ʶ�ȡֵ
 * @param: resolution ָ��ADC�ֱ��ʣ�8λ��12λ��16λ��
 * @retval: ADCת�����
 */
static uint16 HalAdc3Read (uint8 resolution)
{
  int16  reading = 0;

  return ((uint16)reading);
}

/**
 * @brief: ����ADC�ο���ѹ
 * @param: reference ADC�ο���ѹ
 * @retval: void
 */
static void HalAdc3SetReference ( uint8 reference )
{

}

/**
 * @brief: Check for minimum Vdd specified.
 * @param: vdd - The board-specific Vdd reading to check for.
 * @retval: TRUE if the Vdd measured is greater than the 'vdd' minimum parameter;
 *          FALSE if not.
 */
static bool HalAdc3CheckVdd(uint8 vdd)
{
	return 0;
}

/**
 * @brief: ADC��Դ��ʼ��
 * @param: void
 * @retval: void
 */
static void HalAdc4Init (void)
{

}

/**
 * @brief: ע��ADC��Դ
 * @param: void
 * @retval: void
 */
static void HalAdc4DeInit (void)
{

}

/**
 * @brief: ��ָ��ADCͨ�����ֱ��ʶ�ȡֵ
 * @param: resolution ָ��ADC�ֱ��ʣ�8λ��12λ��16λ��
 * @retval: ADCת�����
 */
static uint16 HalAdc4Read (uint8 resolution)
{
  int16  reading = 0;

  return ((uint16)reading);
}

/**
 * @brief: ����ADC�ο���ѹ
 * @param: reference ADC�ο���ѹ
 * @retval: void
 */
static void HalAdc4SetReference ( uint8 reference )
{

}

/**
 * @brief: Check for minimum Vdd specified.
 * @param: vdd - The board-specific Vdd reading to check for.
 * @retval: TRUE if the Vdd measured is greater than the 'vdd' minimum parameter;
 *          FALSE if not.
 */
static bool HalAdc4CheckVdd(uint8 vdd)
{
	return 0;
}

/**
 * @brief: ADC��Դ��ʼ��
 * @param: void
 * @retval: void
 */
static void HalAdc5Init (void)
{

}

/**
 * @brief: ע��ADC��Դ
 * @param: void
 * @retval: void
 */
static void HalAdc5DeInit (void)
{

}

/**
 * @brief: ��ָ��ADCͨ�����ֱ��ʶ�ȡֵ
 * @param: resolution ָ��ADC�ֱ��ʣ�8λ��12λ��16λ��
 * @retval: ADCת�����
 */
static uint16 HalAdc5Read (uint8 resolution)
{
  int16  reading = 0;

  return ((uint16)reading);
}

/**
 * @brief: ����ADC�ο���ѹ
 * @param: reference ADC�ο���ѹ
 * @retval: void
 */
static void HalAdc5SetReference ( uint8 reference )
{

}

/**
 * @brief: Check for minimum Vdd specified.
 * @param: vdd - The board-specific Vdd reading to check for.
 * @retval: TRUE if the Vdd measured is greater than the 'vdd' minimum parameter;
 *          FALSE if not.
 */
static bool HalAdc5CheckVdd(uint8 vdd)
{
	return 0;
}

/**
 * @brief: ADC��Դ��ʼ��
 * @param: void
 * @retval: void
 */
static void HalAdc6Init (void)
{

}

/**
 * @brief: ע��ADC��Դ
 * @param: void
 * @retval: void
 */
static void HalAdc6DeInit (void)
{

}

/**
 * @brief: ��ָ��ADCͨ�����ֱ��ʶ�ȡֵ
 * @param: resolution ָ��ADC�ֱ��ʣ�8λ��12λ��16λ��
 * @retval: ADCת�����
 */
static uint16 HalAdc6Read (uint8 resolution)
{
  int16  reading = 0;

  return ((uint16)reading);
}

/**
 * @brief: ����ADC�ο���ѹ
 * @param: reference ADC�ο���ѹ
 * @retval: void
 */
static void HalAdc6SetReference ( uint8 reference )
{

}

/**
 * @brief: Check for minimum Vdd specified.
 * @param: vdd - The board-specific Vdd reading to check for.
 * @retval: TRUE if the Vdd measured is greater than the 'vdd' minimum parameter;
 *          FALSE if not.
 */
static bool HalAdc6CheckVdd(uint8 vdd)
{
	return 0;
}

/**
 * @brief: ADC��Դ��ʼ��
 * @param: void
 * @retval: void
 */
static void HalAdc7Init (void)
{

}

/**
 * @brief: ע��ADC��Դ
 * @param: void
 * @retval: void
 */
static void HalAdc7DeInit (void)
{

}

/**
 * @brief: ��ָ��ADCͨ�����ֱ��ʶ�ȡֵ
 * @param: resolution ָ��ADC�ֱ��ʣ�8λ��12λ��16λ��
 * @retval: ADCת�����
 */
static uint16 HalAdc7Read (uint8 resolution)
{
  int16  reading = 0;

  return ((uint16)reading);
}

/**
 * @brief: ����ADC�ο���ѹ
 * @param: reference ADC�ο���ѹ
 * @retval: void
 */
static void HalAdc7SetReference ( uint8 reference )
{

}

/**
 * @brief: Check for minimum Vdd specified.
 * @param: vdd - The board-specific Vdd reading to check for.
 * @retval: TRUE if the Vdd measured is greater than the 'vdd' minimum parameter;
 *          FALSE if not.
 */
static bool HalAdc7CheckVdd(uint8 vdd)
{
	return 0;
}

/**
 * @brief: �����˿ڴ���ͨѶ����
 * @param numer: ���ڶ˿ں�
 * @retval None
 */
static void New(HALAdcNumer numer)
{
	switch (numer)
	{
	case HAL_ADC0:
		m_hAdcInstance[HAL_ADC0].init = HalAdc0Init;
		m_hAdcInstance[HAL_ADC0].deInit = HalAdc0DeInit;
		m_hAdcInstance[HAL_ADC0].read = HalAdc0Read;
		m_hAdcInstance[HAL_ADC0].setReference = HalAdc0SetReference;
		m_hAdcInstance[HAL_ADC0].checkVdd = HalAdc0CheckVdd;
		pthis[HAL_ADC0] = &m_hAdcInstance[HAL_ADC0];
		pthis[HAL_ADC0]->init();
		break;
	case HAL_ADC1:
		m_hAdcInstance[HAL_ADC1].init = HalAdc1Init;
		m_hAdcInstance[HAL_ADC1].deInit = HalAdc1DeInit;
		m_hAdcInstance[HAL_ADC1].read = HalAdc1Read;
		m_hAdcInstance[HAL_ADC1].setReference = HalAdc1SetReference;
		m_hAdcInstance[HAL_ADC1].checkVdd = HalAdc1CheckVdd;
		pthis[HAL_ADC1] = &m_hAdcInstance[HAL_ADC1];
		pthis[HAL_ADC1]->init();
		break;
	case HAL_ADC2:
		m_hAdcInstance[HAL_ADC2].init = HalAdc2Init;
		m_hAdcInstance[HAL_ADC2].deInit = HalAdc2DeInit;
		m_hAdcInstance[HAL_ADC2].read = HalAdc2Read;
		m_hAdcInstance[HAL_ADC2].setReference = HalAdc2SetReference;
		m_hAdcInstance[HAL_ADC2].checkVdd = HalAdc2CheckVdd;
		pthis[HAL_ADC2] = &m_hAdcInstance[HAL_ADC2];
		pthis[HAL_ADC2]->init();
		break;
	case HAL_ADC3:
		m_hAdcInstance[HAL_ADC3].init = HalAdc3Init;
		m_hAdcInstance[HAL_ADC3].deInit = HalAdc3DeInit;
		m_hAdcInstance[HAL_ADC3].read = HalAdc3Read;
		m_hAdcInstance[HAL_ADC3].setReference = HalAdc3SetReference;
		m_hAdcInstance[HAL_ADC3].checkVdd = HalAdc3CheckVdd;
		pthis[HAL_ADC3] = &m_hAdcInstance[HAL_ADC3];
		pthis[HAL_ADC3]->init();
		break;
	default:
		break;
	}
}
/**
 * @brief: ��ȡָ���˿�ͨѶ���
 * @param numer: �˿ں�
 * @retval: ָ���˿�ͨѶ���
 */
HALAdcTypeDef* hal_adc_getinstance(HALAdcNumer numer)
{
	if (NULL == pthis[numer])
	{
		New(numer);
	}
	
	return pthis[numer];
}
#endif //CFG_HAL_ADC
