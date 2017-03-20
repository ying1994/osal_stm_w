/**
 * @file    hal_i2c.h
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
#ifndef _HAL_I2C_H
#define _HAL_I2C_H

#include "hal_board.h"
#include "hal_types.h"

#ifdef CFG_HAL_I2C

#define I2C1_GPIO_TYPE	GPIOB
#define I2C1_SMBA		GPIO_Pin_5
#define I2C1_SCL		GPIO_Pin_6
#define I2C1_SDA		GPIO_Pin_7

#define I2C2_GPIO_TYPE	GPIOE
#define I2C2_SMBA		GPIO_Pin_2
#define I2C2_SCL		GPIO_Pin_3
#define I2C2_SDA		GPIO_Pin_4

/**
 * @brief I2Cͨ��ö�� 
 */
typedef enum
{
	HALI2CNumer1 = 0,	/*!< I2Cͨ��1 */
	HALI2CNumer2,		/*!< I2Cͨ��2 */
	HALI2CNumer_SIZE		/*!< I2Cͨ������ */
}HALI2CNumer;

/** 
 * @brief I2C�����ṹ����
 */
typedef struct
{
	/**
	 * @brief I2C��ʼ��
	 * @param None
	 * @retval None
	 */
	void (*init)(void);
	
	/**
	 * @brief I2C��Դע��
	 * @param None
	 * @retval None
	 */
	void (*deInit)(void);
	
	/**
	 * @brief ����I2C�豸ID
	 * @param uID: I2C�豸ID
	 * @retval None
	 */
	void (*setDeviceID)(UINT16 uID);
	
	/**
	 * @brief ����I2C������
	 * @param baudrate: I2C������
	 * @retval None
	 */
	void (*setBaudrate)(UINT32 baudrate);
	
	/**
	 * @brief 8λ��ַ I2Cд����
	 * @param uAddress: д�����ݵ�ַ
	 * @param pBuff: ����ָ�뻺����ָ��
	 * @param size: д�����ݴ�С
	 * @retval None
	 */
	void (*write)(UINT8 uAddress, UCHAR* pBuff, UINT32 size);
	
	/**
	 * @brief 8λ��ַ I2C������
	 * @param uAddress: ���ݴ洢��ַ
	 * @param pBuff: ����ָ�뻺����ָ��
	 * @param size: �������ݴ�С
	 * @retval None
	 */
	UCHAR (*read)(UINT8 uAddress, UCHAR* pBuff, UINT32 size);
	
	/**
	 * @brief 10λ��ַ I2Cд����
	 * @param uAddress: д�����ݵ�ַ
	 * @param pBuff: ����ָ�뻺����ָ��
	 * @param size: д�����ݴ�С
	 * @retval None
	 */
	void (*writeEx)(UINT16 uAddress, UCHAR* pBuff, UINT32 size);
	
	/**
	 * @brief 10λ��ַ I2C������
	 * @param uAddress: ���ݴ洢��ַ
	 * @param pBuff: ����ָ�뻺����ָ��
	 * @param size: �������ݴ�С
	 * @retval None
	 */
	UCHAR (*readEx)(UINT16 uAddress, UCHAR* pBuff, UINT32 size);
}HALI2CTypeDef;


/**
 * @brief ��ȡI2C�����ṹ���
 * @param eChannel: I2C ͨ�� @ref HALI2CNumer
 * @retval I2C�����ṹ���
 */
HALI2CTypeDef* HalI2cGetInstance(HALI2CNumer eChannel);


#endif //CFG_HAL_I2C
#endif
