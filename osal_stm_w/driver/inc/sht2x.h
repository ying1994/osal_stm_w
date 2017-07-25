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

/*SHT2X 设备操作相关宏定义，详见手册*/
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
 * @brief SHT2X 检查数据正确性
 * @param data 读取到的数据
 * @param nb 需要校验的数量
 * @retval 返回校验值
 */
UINT8 sht2x_CalCRC(UCHAR *data, INT16 nb);

/**
 * @brief SHT2X 复位
 */
void sht2x_reset(void);

/**
 * @brief SHT2X 读取温度值
 * @param[out] tem 温度值
 * @retval 操作成功返回0, 失败返回-1
 */
int sht2x_GetTemperture(float *tem);

/**
 * @brief SHT2X 读取湿度值
 * @param[out] hum 湿度值
 * @retval 操作成功返回0, 失败返回-1
 */
int sht2x_GetHumidity(float *hum);

/**
 * @brief SHT2X 读取用户寄存器
 */
UINT8 sht2x_read_user_reg(void);

/**
 * @brief SHT2X 初始化
 * @param hiic I2C操作句柄
 * @param uID 设备ID
 * @retval 
 */
void sht2x_Init(HALI2CTypeDef* hiic, UINT8 uID);

/**
 * @brief SHT2X 释放资源
 * @retval 
 */
void sht2x_deInit(void);

#endif
