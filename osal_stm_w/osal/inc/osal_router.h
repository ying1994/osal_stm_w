/**
 * @file    osal_router.h
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   系统路由转发功能模块
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 * COPYRIGHT NOTICE  
 * Copyright 2016, wsf 
 * All rights res
 *
 */


#ifndef OSAL_ROUTER_H
#define OSAL_ROUTER_H

#include "types.h"
#include "comm.h"


#define MAX_BLOCK_NAME_LEN 128	/*!< 功能模块名称最大长度，包括"\n" */
#define MAX_BLOCK_SIZE 32 		/*!< 模块最大功能单元个数 */

/**
 * @brief 串口端口号
 */
typedef enum 
{
	OSAL_ROUTE_PORT0 = 0,	/*!< Port0 */
	OSAL_ROUTE_PORT1,		/*!< Port1 */
	OSAL_ROUTE_PORT2,		/*!< Port2 */
	OSAL_ROUTE_PORT3,		/*!< Port3 */
	OSAL_ROUTE_PORT4,		/*!< Port4 */
	OSAL_ROUTE_PORT5,		/*!< Port5 */
	OSAL_ROUTE_PORT_SIZE	/*!< 端口总数 */
}OsalRoutePortNumer;

/**
 * @brief 路由通道结点控制基本信息
 */
typedef struct _OSALRouterNodeDef
{
	UINT8 u8ChannelMode;	/*!< 通道通讯模式 */
	BOOL bIsConneted;		/*!< 通道是否在线 */
	UINT16 u16TimeCounter;	/*!< 心跳在线计数器，以秒为单位 */
	UINT16 u16TimerPeriod;	/*!< 结点在线记数周期，以秒为单位 */
}OSALRouterNodeDef;

/**
 * @brief 注册的功能模块信息注册表
 */
typedef struct _OSALRouterReg_t
{
	UINT8 blockAddr;						/*!< 功能模块结点地址 */
	UCHAR blockName[MAX_BLOCK_NAME_LEN];	/*!< 功能模块名称 */
	UCHAR functionCount;					/*!< 模块功能单元的个数 */
	UCHAR functionIDs[MAX_BLOCK_SIZE];		/*!< 模块功能单元ID集合 */
}OSALRouterReg_t;

/**
 * @brief 路由功能单元实现回调函数集合
 */
typedef struct _OSALRouterCBack_t
{
	/**
	 * @brief 通讯结点在线事件回调函数
	 * @param uChannel 产生事件通道结点地址
	 * @param bIsConneted 指定通道是否在线，TRUE 指定通道上线， FALSE 指定通道掉线
	 * @return void
	 */
	void (*OnConnetEnent)(UINT16 uPortAddr, BOOL bIsConneted);
	
	/**
	 * @brief 发送路由消息
	 * @param pMsg 待发送的消息句柄
	 * @return void
	 */
	void (*SendMsg)(MsgTypeDef* pMsg);
}OSALRouterCBack_t;

/**
 * @brief 路由功能单元初始化，注册路由基本处理回调函数
 * @param hRouterBase 路由消息处理回调函数集
 * @return void
 */
void osal_router_init(OSALRouterCBack_t* hRouterBase);

/**
 * @brief 接收消息路由转发
 * @param pMsg 接收到的消息句柄
 * @return 是否是本地消息
 * @retval TRUE 本地消息， FALSE 非本地消息
 */
BOOL osal_router_OnCommMsg(MsgTypeDef* pMsg);

/**
 * @brief 添加一个路由协议转发端口
 * @param hCommHandle 路由协议转发器句柄
 * @param uPortAddr 协议转发的端口地址
 * @return BOOL 路由添加是否成功
 * @retval TRUE 路由添加成功， FALSE 路由添加失败
 */
BOOL osal_router_setCommPort(CommTypeDef* hCommHandle, UINT16 uPortAddr);

/**
 * @brief 获得一个可用的消息存储结构句柄
 * @param None
 * @return 返回一个可用的消息句柄
 */
MsgTypeDef* osal_router_getFreeMsgHandle(void);

/**
 * @brief 路由消息发送
 * @param uAddr 发送消息地址
 * @param pMsg 预发送的消息存储句柄
 * @return BOOL 消息发送是否成功
 * @retval TRUE 消息发送成功，FALSE 消息发送失败
 */
BOOL osal_router_sendMsg(UINT8 uPortAddr, MsgTypeDef* pMsg);

/**
 * @brief 设置结点地址
 * @param uAddr 本地结点地址
 * @return 返回本地结点地址
 */
UINT16 osal_router_setAddress(UINT16 uAddr);
/**
 * @brief 获取结点地址
 * @param None
 * @return 返回本地结点地址
 */
UINT16 osal_router_getAddress(void);
/**
 * @brief 检查结点地址是否是本机地址
 * @param uAddr 结点地址
 * @return 地址合法返回TRUE, 否则返回FALSE
 */
BOOL osal_router_checkAddress(UINT16 uAddr);

#endif
