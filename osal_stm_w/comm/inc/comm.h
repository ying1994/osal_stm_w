/**
 * @file    comm.h
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   通讯协议处理模块，实现数据物理帧的解析及消息发送功能
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 * COPYRIGHT NOTICE  
 * Copyright 2016, wsf 
 * All rights Reserved
 */
#ifndef COMM_H
#define COMM_H
#include "osal.h"

#ifdef CFG_OSAL_COMM

#include "hal_board.h"

#include "unit.h"
#include "optype.h"
#include "function.h"

/** 数据物理帧起始标志 */
#define PHY_DATA_FLAG1 0x55
#define PHY_DATA_FLAG2 0xee

/** 使用发送数据缓冲区开关 */
#define USE_TX_MSG_FIFO 0

/** 数据区的最大数据长度 */
#define MAX_MSG_DATA_SIZE 1024
/** 物理层数据区的最大数据长度 */
#define MAX_MSG_PHY_DATA_SIZE MAX_MSG_DATA_SIZE + 20

/** 数据发送FIFO大小 */
#define MAX_MSG_FIFO_SIZE 10
/** 消息帧大小 */
#define COMM_MSG_SISE MAX_MSG_DATA_SIZE + 8

/**
 * @brief COMM通道枚举
 */
typedef enum
{
	COMM_CHANNEL0 = 0,	/*!< COMM通道0 */
	COMM_CHANNEL1,		/*!< COMM通道1 */
	COMM_CHANNEL2,		/*!< COMM通道2 */
	COMM_CHANNEL3,		/*!< COMM通道3 */
	COMM_CHANNEL4,		/*!< COMM通道4 */
	COMM_CHANNEL_SIZE,	/*!< COMM通道总通道数 */
}CommChannelDef;

/**
 * @brief BD数据协议的数据帧结构
 */
typedef struct _MsgTypeDef
{
	UCHAR uSerPort;					/*!< 数据端口 */
	UINT16 uAddr;					/*!< 设备地址 */
	UINT16 blockID;				/*!< 消息功能单元 */
	UINT16 functionID;				/*!< 消息操作功能ID */
	UCHAR opType;					/*!< 操作状态类型码 */
	UINT16 len;					/*!< 数据长度 */
	UCHAR data[MAX_MSG_DATA_SIZE];	/*!< 用户数据 */
}MsgTypeDef;


/**
 * @brief COMM 回调函数
 * @param uIDE: 帧类型标识符（标准帧或扩展帧）
 * @param uID: 数据帧标识符
 * @param pMsg: 数据帧内容
 * @param size: 数据帧大小
 * @retval None
 */
typedef void (*CommRxCBack_t)(MsgTypeDef* pMsg);


/**
 * @brief COMM操作结构定义
 */
typedef struct _CommTypeDef
{
	/**
	 * @brief COMM初始化
	 * @param hUart:  Uart实例句柄
	 * @retval None
	 */
	void (*init)(HALUartTypeDef* hUart);
	
	/**
	 * @brief COMM资源注销
	 * @param None
	 * @retval None
	 */
	void (*deInit)(void);
	
	/**
	 * @brief: 注册报文接收观察者
	 * @param hrxobser: 报文接收观察者句柄
	 * @retval: void
	 */
	void (*add_rx_obser)(CommRxCBack_t hrxobser);
	
	/**
	 * @brief COMM消息发送
	 * @param pMsg: 数据帧内容
	 * @param len: 数据帧大小
	 * @retval  发送成功返回TRUE，失败返回FALSE
	 */
	BOOL (*transmit)(MsgTypeDef* pMsg);
	
	/**
	 * @brief 通过FIFO的方式发送COMM消息
	 * @param pMsg: 数据帧内容
	 * @param len: 数据帧大小
	 * @retval  发送成功返回TRUE，失败返回FALSE
	 */
#if USE_TX_MSG_FIFO
	BOOL (*transmitByFIFO)(MsgTypeDef* pMsg);
#endif
	
}CommTypeDef;



/**
 * @brief 获取Comm操作结构句柄
 * @param eChennal: COMM通道枚举 @ref CommChannelDef
 * @retval COMM操作结构句柄
 */
CommTypeDef* comm_getInstance(CommChannelDef eChannel);

/**
 * @brief 注册一个Comm操作结构
 * @param hCommHandle Comm操作结构句柄
 * @param eChennal COMM通道枚举 @ref CommChannelDef
 * @return 返回操作结果
 * @retval 注册成功返回TRUE，失败返回FALSE
 */
BOOL comm_registe(CommTypeDef* hCommHandle, CommChannelDef eChannel);

#endif // CFG_OSAL_COMM
#endif //__COMM_H_
