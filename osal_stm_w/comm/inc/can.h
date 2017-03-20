/**
 * @file    can.h
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
#ifndef CAN_H
#define CAN_H
#include "types.h"

/** CAN1端口定义 */
#define CAN1_GPIO_TYPE	GPIOA
#define CAN1_TX		GPIO_Pin_12
#define CAN1_RX		GPIO_Pin_11

/** CAN1端口定义 */
#define CAN2_GPIO_TYPE	GPIOB
#define CAN2_TX		GPIO_Pin_13
#define CAN2_RX		GPIO_Pin_12


#define CAN_FRAME_SIZE	17		/*!< CAN数据帧大小 */

#define CAN_MSG_BUFFER_SIZE	20	/*!< CAN缓冲区大小(数据帧总数) */

/** CAN 工作模式 */
#define HAL_CAN_Mode_Normal          CAN_Mode_Normal           /*!< normal mode */
#define HAL_CAN_Mode_LoopBack        CAN_Mode_LoopBack         /*!< loopback mode */
#define HAL_CAN_Mode_Silent          CAN_Mode_Silent           /*!< silent mode */
#define HAL_CAN_Mode_Silent_LoopBack CAN_Mode_Silent_LoopBack  /*!< loopback combined with silent mode */

/**
 * @brief CAN通道枚举
 */
typedef enum
{
	CAN_CHANNEL1 = 0,/*!< CAN通道1 */
#ifdef STM32F10X_CL
	CAN_CHANNEL2,	/*!< CAN通道2 */
#endif
	CAN_CHANNEL_SIZE,/*!< CAN通道总通道数 */
}CAN_CHANNEL;

/**
 * @brief CAN 波特率
 */
typedef enum
{
	CAN_BAUDRATE_1M = 1000,	/*!< 1MHz */
	CAN_BAUDRATE_800K = 800,	/*!< 800KHz */
	CAN_BAUDRATE_500K = 500,	/*!< 500KHz */
	CAN_BAUDRATE_250K = 250,	/*!< 250KHz */
	CAN_BAUDRATE_200K = 200,	/*!< 200KHz */
	CAN_BAUDRATE_125K = 125,	/*!< 125KHz */
	CAN_BAUDRATE_100K = 100,	/*!< 100KHz */
}CAN_BAUDRATE;

/**
 * @brief CAN 回调函数
 * @param uIDE: 帧类型标识符（标准帧或扩展帧）
 * @param uID: 数据帧标识符
 * @param pMsg: 数据帧内容
 * @param size: 数据帧大小
 * @retval None
 */
typedef void (*CAN_RX_BASE_FUNC)(UINT32 uIDE, UINT32 uID, UCHAR* pMsg, UINT8 size);

/**
 * @brief CAN操作结构定义
 */
typedef struct _CanTypeDef
{
	/**
	 * @brief CAN初始化
	 * @param eMode: 工作模式
	 * @param uBaudrate: 波特率
	 * @param hRxFunc: CAN接收回调函数句柄
	 * @retval None
	 */
	void (*init)(UINT8 eMode, UINT uBaudrate, CAN_RX_BASE_FUNC hRxFunc);
	
	/**
	 * @brief CAN资源注销
	 * @param None
	 * @retval None
	 */
	void (*deInit)(void);
	
	/**
	 * @brief CAN 波特率设置
	 * @param uBaudrate: 波特率 @ref CAN_BAUDRATE
	 * @retval None
	 */
	void (*setBaudrate)(UINT32 uBaudrate);
	
	/**
	 * @brief 设置CAN过滤ID
	 * @param pIDs: 过滤ID组
	 * @param size: 过滤ID数量
	 * @retval None
	 */
	void (*setFilterIDs)(UINT32* pIDs, UINT32 size);
	
	/**
	 * @brief 允许CAN消息自动接收
	 * @param bIsOn: 允许(TRUE)或禁止(FALSE)CAN消息自动接收
	 * @retval None
	 */
	void (*enableReceive)(BOOL bIsOn);
	
	/**
	 * @brief CAN消息发送
	 * @param uIDE: 帧类型标识符（标准帧或扩展帧）
	 * @param uID: 数据帧标识符
	 * @param pMsg: 数据帧内容
	 * @param len: 数据帧大小
	 * @retval  发送成功返回TRUE，失败返回FALSE
	 */
	BOOL (*transmit)(UINT32 uIDE, UINT32 uID, UCHAR* pMsg, UINT8 len);
	
	/**
	 * @brief CAN消息接收
	 * @param uIDE: 帧类型标识符（标准帧或扩展帧）
	 * @param uID: 数据帧标识符
	 * @param pMsg: 数据帧内容
	 * @retval 数据帧大小
	 */
	UINT8 (*receive)(UINT32 *pIDE, UINT32 *pID, UCHAR* pMsg);
	
}CanTypeDef;

/**
 * @brief 获取CAN操作结构句柄
 * @param eChennal: CAN通道枚举 @ref CAN_CHANNEL
 * @retval CAN操作结构句柄
 */
CanTypeDef* can_getInstance(CAN_CHANNEL eChannel);

#endif
