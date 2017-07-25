/**
 * @file    ht1621b.h
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   HT1621B����
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 * COPYRIGHT NOTICE  
 * Copyright 2016, wsf 
 * All rights res
 *
 */
#ifndef HT1621B_H
#define HT1621B_H
#include "stdafx.h"
#include "stm32f10x.h"
#include "hal_types.h"
#include "hal_board.h"

#include "hal_gpio.h"

#ifdef CFG_HT1621B

//! CS
#define HT1621B_CS_GPIO_TYPE	GPIOB 
#define HT1621B_CS_GPIO_PIN	GPIO_Pin_5 

//! RD
#define HT1621B_RD_GPIO_TYPE	GPIOB 
#define HT1621B_RD_GPIO_PIN	GPIO_Pin_3 

//! WR
#define HT1621B_WR_GPIO_TYPE	GPIOB
#define HT1621B_WR_GPIO_PIN	GPIO_Pin_8

//! DATA
#define HT1621B_DATA_GPIO_TYPE	GPIOB 
#define HT1621B_DATA_GPIO_PIN	GPIO_Pin_9 

/**
 * @brief HT1621B ����
 */
typedef enum
{
	HT1621B_CMD_BIAS_40 = 0x40,	//LCD 1/2ƫѹ, ��ѡ2��������
	HT1621B_CMD_BIAS_42 = 0x42,	//LCD 1/3ƫѹ, ��ѡ2��������
	HT1621B_CMD_BIAS_48 = 0x48,	//LCD 1/2ƫѹ, ��ѡ3��������
	HT1621B_CMD_BIAS_4a = 0x4a,	//LCD 1/3ƫѹ, ��ѡ3��������
	HT1621B_CMD_BIAS_50 = 0x50,	//LCD 1/2ƫѹ, ��ѡ4��������
	HT1621B_CMD_BIAS_52 = 0x52,	//LCD 1/3ƫѹ, ��ѡ4��������
	
	HT1621B_CMD_SYSEN 	= 0x02,	//��ϵͳ����
	HT1621B_CMD_SYSDIS 	= 0x00,	//�ر�ϵͳ����
	
	HT1621B_CMD_LCDON 	= 0x06,	//��LCDƫѹ������
	HT1621B_CMD_LCDOFF 	= 0x04,	//�ر�LCDƫѹ������
	
	HT1621B_CMD_RC256  	= 0X30,	//ϵͳʱ��ѡ��Ƭ��ʱ��
	HT1621B_CMD_EXT256  = 0X38,	//ϵͳʱ��ѡ���ⲿʱ��
}HT1621B_CMD;

/**
 * @brief HT1621B ��ʼ��
 * @param 
 * @retval ��ʼ���ɹ�����0, ���򷵻�-1
 */
int ht1621b_Init(void);

/**
 * @brief HT1621B ��Դ�ͷ�
 * @param None
 * @retval None
 */
void ht1621b_DeInit(void);

/**
 * @brief д HT1621B ����
 * @param cmd ����
 * @retval ����ʵ�ʶ�ȡ�����ݴ�С
 */
int ht1621b_Cmd(UINT8 cmd);

/**
 * @brief ��ȡ HT1621B оƬ
 * @param offset ���ݴ洢��ַ
 * @param buf ���ݴ洢ָ��
 * @param size �������ݴ�С
 * @retval ����ʵ�ʶ�ȡ�����ݴ�С
 */
int ht1621b_Read(UINT8 offset, UINT8* buf, int size);

/**
 * @brief д HT1621B оƬ
 * @param offset ���ݴ洢��ַ
 * @param buf ���ݴ洢ָ��(1BYTE: size=2)
 * @param size д�����ݴ�С
 * @retval ����ʵ��д������ݴ�С
 */
int ht1621b_Write(UINT8 offset, const UINT8* buf, int size);

#endif //CFG_HT1621B

#endif
