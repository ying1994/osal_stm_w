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
 * All rights res
 *
 */

#include "hal_adc.h"
#include "hal_cpu.h"
#include "hal_gpio.h"

#if defined(CFG_HAL_ADC) && defined(CFG_HAL_GPIO)

/**
 * @brief ADC通道关联GPIO资源, 指定的GPIO将自动配置为模拟输入
 * @param numer ADC端口号 @ref HALAdcNumer
 * @param channel ADC通道号 @ref HAL_ADC_channels
 * @param gpiox GPIO类型
 * @param pin GPIO端口引脚号
 * @retval void
 */
void HalAdcGpioRegist(HALAdcNumer numer, UINT8 channel, HANDLE gpiox, UINT32 pin)
{
	/* 配置为模拟输入 */
	HalGpioInit(gpiox, pin, HAL_GPIOMode_AIN);
}

/**
 * @brief ADC资源初始化
 * @param numer ADC端口号 @ref HALAdcNumer
 * @param mode 工作模式 @ref HAL_ADC_mode
 * @param trigger 触发方式 @ref HAL_ADC_external_trigger_sources_for_regular_channels_conversion
 * @param dataAlign 对齐方式 @ref HAL_ADC_data_align
 * @param DMAState DMA使能状态
 * @retval void
 * @attention 初始化之前勿必先通道 HalAdcGpioRegist() 函数关联GPIO资源 
 */
void HalAdcInit (HALAdcNumer numer, UINT32 mode, UINT32 trigger, UINT32 dataAlign, FunctionalState DMAState)
{
	ADC_TypeDef* ADCx = NULL;
	ADC_InitTypeDef ADC_InitStructure;
    /* 使能 ADC1 and GPIOC clock */
	if (HAL_ADC1 == numer)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
		ADCx = ADC1;
	}
	else if (HAL_ADC2 == numer)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
		ADCx = ADC2;
	}
	else if (HAL_ADC3 == numer)
	{
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
		ADCx = ADC3;
	}
	else
	{
		return;
	}
	
	/* 配置ADC1, 不用DMA, 用软件自己触发 */
	ADC_InitStructure.ADC_Mode = mode;
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = trigger;
	ADC_InitStructure.ADC_DataAlign = dataAlign;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADCx, &ADC_InitStructure);

	/* ADC1 regular channel14 configuration */
	//ADC_RegularChannelConfig(ADCx, ADC_Channel_14, 1, ADC_SampleTime_55Cycles5);

	/* Enable ADC DMA */
	ADC_DMACmd(ADCx, DMAState);

	/* Enable ADC */
	ADC_Cmd(ADCx, DISABLE);

	/* Enable ADC reset calibaration register */
	ADC_ResetCalibration(ADCx);
	/* Check the end of ADC reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADCx));

	/* Start ADC calibaration */
	ADC_StartCalibration(ADCx);
	/* Check the end of ADC calibration */
	while(ADC_GetCalibrationStatus(ADCx));

	/* Start ADC Software Conversion */
	ADC_SoftwareStartConvCmd(ADCx, ENABLE);
}

/**
 * @brief ADC资源初始化
 * @param numer ADC端口号 @ref HALAdcNumer
 * @param NewState ADC使能状态
 * @retval void
 */
void HalAdcCmd(HALAdcNumer numer, FunctionalState NewState)
{
	/* Enable ADC */
	if (HAL_ADC1 == numer)
		ADC_Cmd(ADC1, NewState);
	else if (HAL_ADC2 == numer)
		ADC_Cmd(ADC2, NewState);
	else if (HAL_ADC3 == numer)
		ADC_Cmd(ADC3, NewState);
}

/**
 * @brief 从指定ADC通道及分辨率读取值
 * @param numer ADC端口号 @ref HALAdcNumer
 * @param channel ADC通道号 @ref HAL_ADC_channels
 * @retval ADC转换结果
 */
UINT16 HalAdcRead (HALAdcNumer numer, UINT8 channel)
{
	UINT16 uVal;
	ADC_TypeDef* ADCx = NULL;
	if (HAL_ADC1 == numer)
		ADCx = ADC1;
	else if (HAL_ADC2 == numer)
		ADCx = ADC2;
	else if (HAL_ADC3 == numer)
		ADCx = ADC3;
	else
		return 0;
	/* ADC1 regular channe configuration */
	ADC_RegularChannelConfig(ADCx, channel, 1, HAL_ADC_SampleTime_55Cycles5);

	ADC_SoftwareStartConvCmd(ADCx, ENABLE);	/* 软件启动下次ADC转换 */
	while (!ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC));
	uVal = ADC_GetConversionValue(ADCx);
	
	return uVal;
}

/**
 * @brief 从指定ADC通道及分辨率读取值
 * @param numer ADC端口号 @ref HALAdcNumer
 * @param channel ADC通道号 @ref HAL_ADC_channels
 * @param count 转换次数（结果取平均值）
 * @retval ADC转换结果
 */
UINT16 HalAdcReadAvg (HALAdcNumer numer, UINT8 channel, UINT16 count )
{
	UINT32 uSum = 0;
	UINT16 uVal = 0;
	UINT16 uMax = 0;
	UINT16 uMin = 0xffff;
	UINT16 i = 0;
	ADC_TypeDef* ADCx = NULL;
	if (HAL_ADC1 == numer)
		ADCx = ADC1;
	else if (HAL_ADC2 == numer)
		ADCx = ADC2;
	else if (HAL_ADC3 == numer)
		ADCx = ADC3;
	else
		return 0;
	/* ADC1 regular channe configuration */
	ADC_RegularChannelConfig(ADCx, channel, 1, ADC_SampleTime_55Cycles5);

	for (i=0; i<count; i++)
	{
		ADC_SoftwareStartConvCmd(ADCx, ENABLE);	/* 软件启动下次ADC转换 */
		HalIwdgFred();
		while (!ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC));
		uVal = ADC_GetConversionValue(ADCx);
		uSum += uVal;
		uMax = (uMax < uVal) ? uVal : uMax;
		uMin = (uMin > uVal) ? uVal : uMin;
	}
	
	if (count >= 10)
	{
		uSum -= (uMax - uMin);
		count >>= 1;
	}
	
	return (UINT16)(uSum / count);
}
#endif //CFG_HAL_ADC
