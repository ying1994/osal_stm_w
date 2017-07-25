/**
 * @file    sht2x.h
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
#ifndef _SHT2X_H_
#define _SHT2X_H_

#include "hal_types.h"
#include "hal_i2c.h"

/*SHT2X �豸������غ궨�壬����ֲ�*/
#define SHT2X_Measurement_RH_HM		0XE5
#define SHT2X_Measurement_T_HM		0XE3
#define SHT2X_Measurement_RH_NHM	0XF5
#define SHT2X_Measurement_T_NHM		0XF3
#define SHT2X_READ_REG				0XE7
#define SHT2X_WRITE_REG				0XE6
#define SHT2X_SOFT_RESET			0XFE



typedef struct
{
	float fTem;
	float fHum;
} SHT2x_data;

/**
 * @brief SHT2X ���������ȷ��
 * @param data ��ȡ��������
 * @param nb ��ҪУ�������
 * @retval ����У��ֵ
 */
UINT8 sht2x_CalCRC(UCHAR *data, INT16 nb);

/**
 * @brief SHT2X ��λ
 */
void sht2x_reset(void);

/**
 * @brief SHT2X ��ȡ�¶�ֵ
 * @param[out] tem �¶�ֵ
 * @retval �����ɹ�����0, ʧ�ܷ���-1
 */
int sht2x_GetTemperture(float *tem);

/**
 * @brief SHT2X ��ȡʪ��ֵ
 * @param[out] hum ʪ��ֵ
 * @retval �����ɹ�����0, ʧ�ܷ���-1
 */
int sht2x_GetHumidity(float *hum);

/**
 * @brief SHT2X ��ȡ�û��Ĵ���
 */
UINT8 sht2x_read_user_reg(void);

/**
 * @brief SHT2X ��ʼ��
 * @param hiic I2C�������
 * @param uID �豸ID
 * @retval 
 */
void sht2x_Init(HALI2CTypeDef* hiic, UINT8 uID);

/**
 * @brief SHT2X �ͷ���Դ
 * @retval 
 */
void sht2x_deInit(void);

#endif
