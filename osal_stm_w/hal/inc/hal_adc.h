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
 * All rights Reserved
 *
 */
#ifndef _HAL_ADC_H
#define _HAL_ADC_H

#include "hal_cfg.h"
#include "hal_types.h"
#include "hal_board.h"


#ifdef CFG_HAL_ADC

/* Resolution */
#define HAL_ADC_RESOLUTION_8       0x01
#define HAL_ADC_RESOLUTION_10      0x02
#define HAL_ADC_RESOLUTION_12      0x03
#define HAL_ADC_RESOLUTION_14      0x04


/**
 * @brief ADC�˿ں�
 */
typedef enum 
{
	HAL_ADC0 = 0x00,	/*!< ADC0 */
	HAL_ADC1,           /*!< ADC1 */
	HAL_ADC2,           /*!< ADC2 */
	HAL_ADC3,           /*!< ADC3 */
	HAL_ADC_SIZE		/*!< ADC �˿����� */
}HALAdcNumer;

/**
 * @brief ADCͨ���ź�
 */
typedef enum 
{
	HAL_ADC_CHN_AIN0 = ADC_Channel_0,
	HAL_ADC_CHN_AIN1 = ADC_Channel_1,
	HAL_ADC_CHN_AIN2 = ADC_Channel_2,
	HAL_ADC_CHN_AIN3 = ADC_Channel_3,
	HAL_ADC_CHN_AIN4 = ADC_Channel_4,
	HAL_ADC_CHN_AIN5 = ADC_Channel_5,
	HAL_ADC_CHN_AIN6 = ADC_Channel_6,
	HAL_ADC_CHN_AIN7 = ADC_Channel_7,
	HAL_ADC_CHN_AIN8 = ADC_Channel_8,
	HAL_ADC_CHN_AIN9 = ADC_Channel_9,
	HAL_ADC_CHN_AIN10 = ADC_Channel_10,
	HAL_ADC_CHN_AIN11 = ADC_Channel_11,
	HAL_ADC_CHN_AIN12 = ADC_Channel_12,
	HAL_ADC_CHN_AIN13 = ADC_Channel_13,
	HAL_ADC_CHN_AIN14 = ADC_Channel_14,
	HAL_ADC_CHN_AIN15 = ADC_Channel_15,
	HAL_ADC_CHN_AIN16 = ADC_Channel_16,
	HAL_ADC_CHN_AIN17 = ADC_Channel_17,
	HAL_ADC_CHN_AIN_TempSensor = ADC_Channel_TempSensor,
	HAL_ADC_CHN_AIN_Vrefint = ADC_Channel_Vrefint
}HALAdcChannel;

#define IS_HAL_ADC_CHANNEL(CHANNEL) (((CHANNEL) == HAL_ADC_CHN_AIN0) || ((CHANNEL) == HAL_ADC_CHN_AIN1) || \
									 ((CHANNEL) == HAL_ADC_CHN_AIN2) || ((CHANNEL) == HAL_ADC_CHN_AIN3) || \
									 ((CHANNEL) == HAL_ADC_CHN_AIN4) || ((CHANNEL) == HAL_ADC_CHN_AIN5) || \
									 ((CHANNEL) == HAL_ADC_CHN_AIN6) || ((CHANNEL) == HAL_ADC_CHN_AIN7) || \
									 ((CHANNEL) == HAL_ADC_CHN_AIN8) || ((CHANNEL) == HAL_ADC_CHN_AIN9) || \
									 ((CHANNEL) == HAL_ADC_CHN_AIN10) || ((CHANNEL) == HAL_ADC_CHN_AIN11) || \
									 ((CHANNEL) == HAL_ADC_CHN_AIN12) || ((CHANNEL) == HAL_ADC_CHN_AIN13) || \
									 ((CHANNEL) == HAL_ADC_CHN_AIN14) || ((CHANNEL) == HAL_ADC_CHN_AIN15) || \
									 ((CHANNEL) == HAL_ADC_CHN_AIN16) || ((CHANNEL) == HAL_ADC_CHN_AIN17))



/**
 * @brief ���ڲ����ṹ����
 */
typedef struct _HALAdcTypeDef
{
	/**
	 * @brief: ADC��Դ��ʼ��
	 * @param: void
	 * @retval: void
	 */
	void (*init)(void);

	/**
	 * @brief: ע��ADC��Դ
	 * @param: void
	 * @retval: void
	 */
	void (*deInit)(void);
	
	/**
	 * @brief: ��ָ��ADCͨ�����ֱ��ʶ�ȡֵ
	 * @param: resolution ָ��ADC�ֱ��ʣ�8λ��12λ��16λ��
	 * @retval: ADCת�����
	 */
	uint16 (*read)(uint8 resolution);

	/**
	 * @brief: ����ADC�ο���ѹ
	 * @param: reference ADC�ο���ѹ
	 * @retval: void
	 */
	void (*setReference)(uint8 reference);

	/**
	 * @brief: Check for minimum Vdd specified.
	 * @param: vdd - The board-specific Vdd reading to check for.
	 * @retval: TRUE if the Vdd measured is greater than the 'vdd' minimum parameter;
	 *          FALSE if not.
	 */
	bool (*checkVdd)(uint8 vdd);
}HALAdcTypeDef;

/**
 * @brief: ��ȡָ���˿�ͨѶ���
 * @param numer: �˿ں�
 * @retval: ָ���˿�ͨѶ���
 */
HALAdcTypeDef* hal_adc_getinstance(HALAdcNumer numer);

#endif //CFG_HAL_ADC
#endif
