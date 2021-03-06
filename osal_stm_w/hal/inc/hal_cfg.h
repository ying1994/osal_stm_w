/**
 * @file    hal_board_cfg.h
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
#ifndef _HAL_BORAD_CFG_H
#define _HAL_BORAD_CFG_H

#include "stm32f10x.h"

/** 使用UART1作为调试口 */
#define DEBUG_UART1


#define CFG_HAL_CPU
#define CFG_HAL_ADC
#define CFG_HAL_CRC
#define CFG_HAL_EXTI
#define CFG_HAL_FLASH
#define CFG_HAL_GPIO
#define CFG_HAL_I2C
#define CFG_HAL_SPI
#define CFG_HAL_TIMER
#define CFG_HAL_UART
#define CFG_HAL_RTC
#define CFG_HAL_CAN
#define CFG_HAL_PWM
#define CFG_HAL_ENC

#define CFG_HAL_IAP


#endif
