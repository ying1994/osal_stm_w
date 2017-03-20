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
 * @brief: ADC资源初始化
 * @param: void
 * @retval: void
 */
void HalAdcInit (void)
{
#if (HAL_ADC == TRUE)
  
#endif
}

/**
 * @brief: 从指定ADC通道及分辨率读取值
 * @param: channel 指定ADC通道
 * @param: resolution 指定ADC分辨率（8位、12位、16位）
 * @retval: ADC转换结果
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
 * @brief: 设置ADC参考电压
 * @param: reference ADC参考电压
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
