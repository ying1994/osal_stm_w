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
#include  "hal_adc.h"
#include  "hal_defs.h"
#include  "hal_types.h"

#ifdef CFG_HAL_ADC
/**
 * @brief: ADC��Դ��ʼ��
 * @param: void
 * @retval: void
 */
void HalAdcInit (void)
{
#if (HAL_ADC == TRUE)
  
#endif
}

/**
 * @brief: ��ָ��ADCͨ�����ֱ��ʶ�ȡֵ
 * @param: channel ָ��ADCͨ��
 * @param: resolution ָ��ADC�ֱ��ʣ�8λ��12λ��16λ��
 * @retval: ADCת�����
 */
uint16 HalAdcRead (uint8 channel, uint8 resolution)
{
  int16  reading = 0;

#if (HAL_ADC == TRUE)

#else
  // unused arguments

#endif

  return ((uint16)reading);
}

/**
 * @brief: ����ADC�ο���ѹ
 * @param: reference ADC�ο���ѹ
 * @retval: void
 */
void HalAdcSetReference ( uint8 reference )
{
#if (HAL_ADC == TRUE)

#endif
}

/**
 * @brief: Check for minimum Vdd specified.
 * @param: vdd - The board-specific Vdd reading to check for.
 * @retval: TRUE if the Vdd measured is greater than the 'vdd' minimum parameter;
 *          FALSE if not.
 */
bool HalAdcCheckVdd(uint8 vdd)
{
	return 0;
}

#endif //CFG_HAL_ADC
