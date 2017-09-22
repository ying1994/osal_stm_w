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
 * @brief ADCͨ������GPIO��Դ, ָ����GPIO���Զ�����Ϊģ������
 * @param numer ADC�˿ں� @ref HALAdcNumer
 * @param channel ADCͨ���� @ref HAL_ADC_channels
 * @param gpiox GPIO���� @ref HAL_ADC_Channel_Pins
 * @param pin GPIO�˿����ź� @ref HAL_ADC_Channel_Pins
 * @retval void
 */
void HalAdcGpioRegist(HALAdcNumer numer, UINT8 channel, HANDLE gpiox, UINT32 pin)
{
	/* ����Ϊģ������ */
	HalGpioInit(gpiox, pin, HAL_GPIOMode_AIN);
}

/**
 * @brief ADC��Դ��ʼ��
 * @param numer ADC�˿ں� @ref HALAdcNumer
 * @param mode ����ģʽ @ref HAL_ADC_mode
 * @param trigger ������ʽ @ref HAL_ADC_external_trigger_sources_for_regular_channels_conversion
 * @param dataAlign ���뷽ʽ @ref HAL_ADC_data_align
 * @retval void
 * @attention ��ʼ��֮ǰ�����ͨ�� HalAdcGpioRegist() ��������GPIO��Դ 
 */
void HalAdcInit (HALAdcNumer numer, UINT32 mode, UINT32 trigger, UINT32 dataAlign)
{
	ADC_TypeDef* ADCx = NULL;
	ADC_InitTypeDef ADC_InitStructure;
    /* ʹ�� ADC1 and GPIOC clock */
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
	
	/* ����ADC1, ����DMA, ������Լ����� */
	ADC_InitStructure.ADC_Mode = mode; //����ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//��ֹɨ��ģʽ����ͨ����Ҫ����ͨ������Ҫ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; //����ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = trigger; //������ʽ
	ADC_InitStructure.ADC_DataAlign = dataAlign; //���ݶ��뷽ʽ
	ADC_InitStructure.ADC_NbrOfChannel = 1; //ͨ����
	ADC_Init(ADCx, &ADC_InitStructure);

	/* ����ADCʱ��ΪPCLK2��8��Ƶ����9MHz */
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	
	/* ADC1 regular channel14 configuration */
	//ADC_RegularChannelConfig(ADCx, ADC_Channel_14, 1, ADC_SampleTime_55Cycles5);

	/* Enable ADC DMA */
	//ADC_DMACmd(ADCx, DMAState);

	/* Enable ADC */
	ADC_Cmd(ADCx, ENABLE);

	/* Enable ADC reset calibaration register */
	ADC_ResetCalibration(ADCx);
	/* Check the end of ADC reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADCx));

	/* Start ADC calibaration */
	ADC_StartCalibration(ADCx);
	/* Check the end of ADC calibration */
	while(ADC_GetCalibrationStatus(ADCx));

	/* Start ADC Software Conversion */
	//ADC_SoftwareStartConvCmd(ADCx, ENABLE);
}

/**
 * @brief ADC��Դ��ʼ��
 * @param numer ADC�˿ں� @ref HALAdcNumer
 * @param NewState ADCʹ��״̬
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
 * @brief ��ָ��ADCͨ�����ֱ��ʶ�ȡֵ
 * @param numer ADC�˿ں� @ref HALAdcNumer
 * @param channel ADCͨ���� @ref HAL_ADC_channels
 * @retval ADCת�����
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

	ADC_SoftwareStartConvCmd(ADCx, ENABLE);	/* ��������´�ADCת�� */
	while (!ADC_GetFlagStatus(ADCx, ADC_FLAG_EOC));
	uVal = ADC_GetConversionValue(ADCx);
	
	return uVal;
}

/**
 * @brief ��ָ��ADCͨ�����ֱ��ʶ�ȡֵ
 * @param numer ADC�˿ں� @ref HALAdcNumer
 * @param channel ADCͨ���� @ref HAL_ADC_channels
 * @param count ת�����������ȡƽ��ֵ��
 * @retval ADCת�����
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
		ADC_SoftwareStartConvCmd(ADCx, ENABLE);	/* ��������´�ADCת�� */
		HalIwdgFred();//ι����ֹת��������������¿��Ź�����
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
