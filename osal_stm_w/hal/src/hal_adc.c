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
 * @brief: ADC资源初始化
 * @param: ADCx ADC通道
 * @retval: void
 */
static void HalAdcInit (ADC_TypeDef* ADCx)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	__IO uint16_t ADCConvertedValue;

	if (ADC1 == ADCx)
	{
		/* 使能 ADC1 and GPIOC clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);
		/* 配置PC4为模拟输入(ADC Channel14) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}
	else if (ADC2 == ADCx)
	{
		/* 使能 ADC1 and GPIOC clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2 | RCC_APB2Periph_GPIOC, ENABLE);
		/* 配置PC4为模拟输入(ADC Channel14) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}
	else //if (ADC3 == ADCx)
	{
		/* 使能 ADC1 and GPIOC clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3 | RCC_APB2Periph_GPIOC, ENABLE);
		/* 配置PC4为模拟输入(ADC Channel14) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
		GPIO_Init(GPIOC, &GPIO_InitStructure);
	}

	/* 配置ADC1, 不用DMA */
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
 * @brief: 注销ADC资源
 * @param: ADCx ADC通道
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
 * @brief: ADC资源初始化
 * @param: void
 * @retval: void
 */
static void HalAdc0Init (void)
{
	HalAdcInit(ADC1);
}

/**
 * @brief: 注销ADC资源
 * @param: void
 * @retval: void
 */
static void HalAdc0DeInit (void)
{
	HalAdcDeInit(ADC1);
}

/**
 * @brief: 从指定ADC通道及分辨率读取值
 * @param: resolution 指定ADC分辨率（8位、12位、16位）
 * @retval: ADC转换结果
 */
static uint16 HalAdc0Read (uint8 resolution)
{
  int16  reading = 0;

  return ((uint16)reading);
}

/**
 * @brief: 设置ADC参考电压
 * @param: reference ADC参考电压
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
 * @brief: ADC资源初始化
 * @param: void
 * @retval: void
 */
static void HalAdc1Init (void)
{

}

/**
 * @brief: 注销ADC资源
 * @param: void
 * @retval: void
 */
static void HalAdc1DeInit (void)
{

}

/**
 * @brief: 从指定ADC通道及分辨率读取值
 * @param: resolution 指定ADC分辨率（8位、12位、16位）
 * @retval: ADC转换结果
 */
static uint16 HalAdc1Read (uint8 resolution)
{
  int16  reading = 0;

  return ((uint16)reading);
}

/**
 * @brief: 设置ADC参考电压
 * @param: reference ADC参考电压
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
 * @brief: ADC资源初始化
 * @param: void
 * @retval: void
 */
static void HalAdc2Init (void)
{

}

/**
 * @brief: 注销ADC资源
 * @param: void
 * @retval: void
 */
static void HalAdc2DeInit (void)
{

}

/**
 * @brief: 从指定ADC通道及分辨率读取值
 * @param: resolution 指定ADC分辨率（8位、12位、16位）
 * @retval: ADC转换结果
 */
static uint16 HalAdc2Read (uint8 resolution)
{
  int16  reading = 0;

  return ((uint16)reading);
}

/**
 * @brief: 设置ADC参考电压
 * @param: reference ADC参考电压
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
 * @brief: ADC资源初始化
 * @param: void
 * @retval: void
 */
static void HalAdc3Init (void)
{

}

/**
 * @brief: 注销ADC资源
 * @param: void
 * @retval: void
 */
static void HalAdc3DeInit (void)
{

}

/**
 * @brief: 从指定ADC通道及分辨率读取值
 * @param: resolution 指定ADC分辨率（8位、12位、16位）
 * @retval: ADC转换结果
 */
static uint16 HalAdc3Read (uint8 resolution)
{
  int16  reading = 0;

  return ((uint16)reading);
}

/**
 * @brief: 设置ADC参考电压
 * @param: reference ADC参考电压
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
 * @brief: ADC资源初始化
 * @param: void
 * @retval: void
 */
static void HalAdc4Init (void)
{

}

/**
 * @brief: 注销ADC资源
 * @param: void
 * @retval: void
 */
static void HalAdc4DeInit (void)
{

}

/**
 * @brief: 从指定ADC通道及分辨率读取值
 * @param: resolution 指定ADC分辨率（8位、12位、16位）
 * @retval: ADC转换结果
 */
static uint16 HalAdc4Read (uint8 resolution)
{
  int16  reading = 0;

  return ((uint16)reading);
}

/**
 * @brief: 设置ADC参考电压
 * @param: reference ADC参考电压
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
 * @brief: ADC资源初始化
 * @param: void
 * @retval: void
 */
static void HalAdc5Init (void)
{

}

/**
 * @brief: 注销ADC资源
 * @param: void
 * @retval: void
 */
static void HalAdc5DeInit (void)
{

}

/**
 * @brief: 从指定ADC通道及分辨率读取值
 * @param: resolution 指定ADC分辨率（8位、12位、16位）
 * @retval: ADC转换结果
 */
static uint16 HalAdc5Read (uint8 resolution)
{
  int16  reading = 0;

  return ((uint16)reading);
}

/**
 * @brief: 设置ADC参考电压
 * @param: reference ADC参考电压
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
 * @brief: ADC资源初始化
 * @param: void
 * @retval: void
 */
static void HalAdc6Init (void)
{

}

/**
 * @brief: 注销ADC资源
 * @param: void
 * @retval: void
 */
static void HalAdc6DeInit (void)
{

}

/**
 * @brief: 从指定ADC通道及分辨率读取值
 * @param: resolution 指定ADC分辨率（8位、12位、16位）
 * @retval: ADC转换结果
 */
static uint16 HalAdc6Read (uint8 resolution)
{
  int16  reading = 0;

  return ((uint16)reading);
}

/**
 * @brief: 设置ADC参考电压
 * @param: reference ADC参考电压
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
 * @brief: ADC资源初始化
 * @param: void
 * @retval: void
 */
static void HalAdc7Init (void)
{

}

/**
 * @brief: 注销ADC资源
 * @param: void
 * @retval: void
 */
static void HalAdc7DeInit (void)
{

}

/**
 * @brief: 从指定ADC通道及分辨率读取值
 * @param: resolution 指定ADC分辨率（8位、12位、16位）
 * @retval: ADC转换结果
 */
static uint16 HalAdc7Read (uint8 resolution)
{
  int16  reading = 0;

  return ((uint16)reading);
}

/**
 * @brief: 设置ADC参考电压
 * @param: reference ADC参考电压
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
 * @brief: 创建端口串口通讯对象
 * @param numer: 串口端口号
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
 * @brief: 获取指定端口通讯句柄
 * @param numer: 端口号
 * @retval: 指定端口通讯句柄
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
