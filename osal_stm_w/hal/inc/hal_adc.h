/**
 * @file    hal_adc.h
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
#ifndef _HAL_ADC_H
#define _HAL_ADC_H

#include "hal_cfg.h"
#include "hal_types.h"

#if defined(CFG_HAL_ADC) && defined(CFG_HAL_GPIO)

/**
 * @brief ���ڶ˿ں�
 */
typedef enum 
{
	HAL_ADC1 = 0,	/*!< ADC1 */
	HAL_ADC2,		/*!< ADC2 */
	HAL_ADC3,		/*!< ADC3 */
	HAL_ADC_SIZE	/*!< ADC �˿����� */
}HALAdcNumer;

/** @defgroup HAL_ADC_channels 
  * @{
  */

#define HAL_ADC_CHANNEL_0  ADC_Channel_0 
#define HAL_ADC_CHANNEL_1  ADC_Channel_1 
#define HAL_ADC_CHANNEL_2  ADC_Channel_2 
#define HAL_ADC_CHANNEL_3  ADC_Channel_3 
#define HAL_ADC_CHANNEL_4  ADC_Channel_4 
#define HAL_ADC_CHANNEL_5  ADC_Channel_5 
#define HAL_ADC_CHANNEL_6  ADC_Channel_6 
#define HAL_ADC_CHANNEL_7  ADC_Channel_7 
#define HAL_ADC_CHANNEL_8  ADC_Channel_8 
#define HAL_ADC_CHANNEL_9  ADC_Channel_9 
#define HAL_ADC_CHANNEL_10 ADC_Channel_10
#define HAL_ADC_CHANNEL_11 ADC_Channel_11
#define HAL_ADC_CHANNEL_12 ADC_Channel_12
#define HAL_ADC_CHANNEL_13 ADC_Channel_13
#define HAL_ADC_CHANNEL_14 ADC_Channel_14
#define HAL_ADC_CHANNEL_15 ADC_Channel_15
#define HAL_ADC_CHANNEL_16 ADC_Channel_16
#define HAL_ADC_CHANNEL_17 ADC_Channel_17

/**
  * @}
  */
/** @defgroup HAL_ADC_mode 
  * @{
  */

#define HAL_ADC_Mode_Independent             ADC_Mode_Independent
#define HAL_ADC_Mode_RegInjecSimult          ADC_Mode_RegInjecSimult
#define HAL_ADC_Mode_RegSimult_AlterTrig     ADC_Mode_RegSimult_AlterTrig
#define HAL_ADC_Mode_InjecSimult_FastInterl  ADC_Mode_InjecSimult_FastInterl
#define HAL_ADC_Mode_InjecSimult_SlowInterl  ADC_Mode_InjecSimult_SlowInterl
#define HAL_ADC_Mode_InjecSimult             ADC_Mode_InjecSimult
#define HAL_ADC_Mode_RegSimult               ADC_Mode_RegSimult
#define HAL_ADC_Mode_FastInterl              ADC_Mode_FastInterl
#define HAL_ADC_Mode_SlowInterl              ADC_Mode_SlowInterl
#define HAL_ADC_Mode_AlterTrig               ADC_Mode_AlterTrig
/**
  * @}
  */
  
/** @defgroup HAL_ADC_sampling_time 
  * @{
  */
  
#define HAL_ADC_SampleTime_1Cycles5   ADC_SampleTime_1Cycles5  
#define HAL_ADC_SampleTime_7Cycles5   ADC_SampleTime_7Cycles5   
#define HAL_ADC_SampleTime_13Cycles5  ADC_SampleTime_13Cycles5  
#define HAL_ADC_SampleTime_28Cycles5  ADC_SampleTime_28Cycles5  
#define HAL_ADC_SampleTime_41Cycles5  ADC_SampleTime_41Cycles5  
#define HAL_ADC_SampleTime_55Cycles5  ADC_SampleTime_55Cycles5  
#define HAL_ADC_SampleTime_71Cycles5  ADC_SampleTime_71Cycles5  
#define HAL_ADC_SampleTime_239Cycles5 ADC_SampleTime_239Cycles5 

/**
  * @}
  */
/** @defgroup HAL_ADC_external_trigger_sources_for_regular_channels_conversion 
  * @{
  */

#define HAL_ADC_ExtTrigConv_T1_CC1                ADC_ExternalTrigConv_T1_CC1              /*!< For ADC1 and ADC2 */
#define HAL_ADC_ExtTrigConv_T1_CC2                ADC_ExternalTrigConv_T1_CC2              /*!< For ADC1 and ADC2 */
#define HAL_ADC_ExtTrigConv_T2_CC2                ADC_ExternalTrigConv_T2_CC2              /*!< For ADC1 and ADC2 */
#define HAL_ADC_ExtTrigConv_T3_TRGO               ADC_ExternalTrigConv_T3_TRGO             /*!< For ADC1 and ADC2 */
#define HAL_ADC_ExtTrigConv_T4_CC4                ADC_ExternalTrigConv_T4_CC4              /*!< For ADC1 and ADC2 */
#define HAL_ADC_ExtTrigConv_Ext_IT11_TIM8_TRGO    ADC_ExternalTrigConv_Ext_IT11_TIM8_TRGO  /*!< For ADC1 and ADC2 */
#define HAL_ADC_ExtTrigConv_T1_CC3                ADC_ExternalTrigConv_T1_CC3              /*!< For ADC1, ADC2 and ADC3 */
#define HAL_ADC_ExtTrigConv_None                  ADC_ExternalTrigConv_None                /*!< For ADC1, ADC2 and ADC3 */
#define HAL_ADC_ExtTrigConv_T3_CC1                ADC_ExternalTrigConv_T3_CC1              /*!< For ADC3 only */
#define HAL_ADC_ExtTrigConv_T2_CC3                ADC_ExternalTrigConv_T2_CC3              /*!< For ADC3 only */
#define HAL_ADC_ExtTrigConv_T8_CC1                ADC_ExternalTrigConv_T8_CC1              /*!< For ADC3 only */
#define HAL_ADC_ExtTrigConv_T8_TRGO               ADC_ExternalTrigConv_T8_TRGO             /*!< For ADC3 only */
#define HAL_ADC_ExtTrigConv_T5_CC1                ADC_ExternalTrigConv_T5_CC1              /*!< For ADC3 only */
#define HAL_ADC_ExtTrigConv_T5_CC3                ADC_ExternalTrigConv_T5_CC3              /*!< For ADC3 only */
/**
  * @}
  */

/** @defgroup HAL_ADC_external_trigger_sources_for_injected_channels_conversion 
  * @{
  */

#define HAL_ADC_ExtTrigInjecConv_T2_TRGO           ADC_ExternalTrigInjecConv_T2_TRGO           /*!< For ADC1 and ADC2 */
#define HAL_ADC_ExtTrigInjecConv_T2_CC1            ADC_ExternalTrigInjecConv_T2_CC1            /*!< For ADC1 and ADC2 */
#define HAL_ADC_ExtTrigInjecConv_T3_CC4            ADC_ExternalTrigInjecConv_T3_CC4            /*!< For ADC1 and ADC2 */
#define HAL_ADC_ExtTrigInjecConv_T4_TRGO           ADC_ExternalTrigInjecConv_T4_TRGO           /*!< For ADC1 and ADC2 */
#define HAL_ADC_ExtTrigInjecConv_Ext_IT15_TIM8_CC4 ADC_ExternalTrigInjecConv_Ext_IT15_TIM8_CC4 /*!< For ADC1 and ADC2 */
#define HAL_ADC_ExtTrigInjecConv_T1_TRGO           ADC_ExternalTrigInjecConv_T1_TRGO           /*!< For ADC1, ADC2 and ADC3 */
#define HAL_ADC_ExtTrigInjecConv_T1_CC4            ADC_ExternalTrigInjecConv_T1_CC4            /*!< For ADC1, ADC2 and ADC3 */
#define HAL_ADC_ExtTrigInjecConv_None              ADC_ExternalTrigInjecConv_None              /*!< For ADC1, ADC2 and ADC3 */
#define HAL_ADC_ExtTrigInjecConv_T4_CC3            ADC_ExternalTrigInjecConv_T4_CC3            /*!< For ADC3 only */
#define HAL_ADC_ExtTrigInjecConv_T8_CC2            ADC_ExternalTrigInjecConv_T8_CC2            /*!< For ADC3 only */
#define HAL_ADC_ExtTrigInjecConv_T8_CC4            ADC_ExternalTrigInjecConv_T8_CC4            /*!< For ADC3 only */
#define HAL_ADC_ExtTrigInjecConv_T5_TRGO           ADC_ExternalTrigInjecConv_T5_TRGO           /*!< For ADC3 only */
#define HAL_ADC_ExtTrigInjecConv_T5_CC4            ADC_ExternalTrigInjecConv_T5_CC4            /*!< For ADC3 only */
/**
  * @}
  */

/** @defgroup HAL_ADC_data_align 
  * @{
  */

#define HAL_ADC_DataAlign_Right ADC_DataAlign_Right
#define HAL_ADC_DataAlign_Left  ADC_DataAlign_Left 
/**
  * @}
  */

/**
 * @brief ADCͨ������GPIO��Դ
 * @param numer ADC�˿ں� @ref HALAdcNumer
 * @param channel ADCͨ���� @ref HAL_ADC_channels
 * @param gpiox GPIO����
 * @param pin GPIO�˿����ź�
 * @retval void
 */
void HalAdcGpioRegist(HALAdcNumer numer, UINT8 channel, HANDLE gpiox, UINT32 pin);

/**
 * @brief ADC��Դ��ʼ��
 * @param numer ADC�˿ں� @ref HALAdcNumer
 * @param mode ����ģʽ @ref HAL_ADC_mode
 * @param trigger ������ʽ @ref HAL_ADC_external_trigger_sources_for_regular_channels_conversion
 * @param dataAlign ���뷽ʽ @ref HAL_ADC_data_align
 * @param DMAState DMAʹ��״̬
 * @retval void
 * @attention ��ʼ��֮ǰ�����ͨ�� HalAdcGpioRegist() ��������GPIO��Դ 
 */
void HalAdcInit (HALAdcNumer numer, UINT32 mode, UINT32 trigger, UINT32 dataAlign, FunctionalState DMAState);

/**
 * @brief ADC��Դ��ʼ��
 * @param numer ADC�˿ں� @ref HALAdcNumer
 * @param NewState ADCʹ��״̬
 * @retval void
 */
void HalAdcCmd(HALAdcNumer numer, FunctionalState NewState);

/**
 * @brief ��ָ��ADCͨ�����ֱ��ʶ�ȡֵ
 * @param numer ADC�˿ں� @ref HALAdcNumer
 * @param channel ADCͨ���� @ref HAL_ADC_channels
 * @retval ADCת�����
 */
UINT16 HalAdcRead (HALAdcNumer numer, UINT8 channel);

/**
 * @brief ��ָ��ADCͨ�����ֱ��ʶ�ȡֵ
 * @param numer ADC�˿ں� @ref HALAdcNumer
 * @param channel ADCͨ���� @ref HAL_ADC_channels
 * @param count ת�����������ȡƽ��ֵ��
 * @retval ADCת�����
 */
UINT16 HalAdcReadAvg (HALAdcNumer numer, UINT8 channel, UINT16 count );

#endif //CFG_HAL_ADC
#endif
