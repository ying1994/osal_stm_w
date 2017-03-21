/**
 * @file    function.h
 * @author  WSF
 * @version V1.0.0
 * @date    2016.03.15
 * @brief   实现数据功能操作类型定义
 ******************************************************************************
 * @attention
 *
 ******************************************************************************
 * COPYRIGHT NOTICE
 * Copyright 2016, wsf
 * All rights reserved.
 *
 */
#ifndef FUNCTION_H
#define FUNCTION_H

/**
 * @defgroup FunctionID
 * @brief 数据功能操作类型定义
 * @{
 */

/** 
 * @defgroup UPDATE_UNIT 
 * @brief 系统升级单元
 * @{
 */
#define STOP		0x0000	/*!< 开始升级 */
#define START		0x0001	/*!< 开始升级 */
#define READY		0x0002	/*!< 升级准备完成 */
#define UPDATING	0x0003	/*!< 升级中 */
#define RESULT		0x0004	/*!< 单个升级结果反馈 */
#define LAST_RESULT	0x0005	/*!< 反馈最终升级结果 */
#define END			0x0006	/*!< 升级完成 */

/**
 * @}
 */

/**
 * @defgroup DEBUG_UNIT
 * @brief 系统调试单元
 * @{
 */
#define GET_APP_VERSION		0x0011 /*!< 获取应用程序版本号 */
#define GET_BIOS_VERSION	0x0012 /*!< 获取BootLoader版本号 */
#define GET_CHIP_ID		0x0013 /*!< 获取芯片ID */
#define NET_LOCAL_IP		0x0014 /*!< 本地IP、网关、掩码、MAC地址 */
#define NET_SERVER_IP		0x0015 /*!< 服务器IP、端口、连接方式 */
#define NET_WIFI_PARAM		0x0016 /*!< Wifi参数（SSID、PWD） */
#define LOCAL_TIME		0x0017 /*!< 本地时间 */
#define NET_WIFI_CONNET		0x0018 /*!< Wifi连接 */
#define NET_MSG_TEST		0x0019 /*!< 消息测试 */

/**
 * @}
 */

/**
 * @defgroup ROUTER_UNIT
 * @brief 路由单元
 * @{
 */
#define COMM_HANDLE			0x0021 /*!< 通讯握手心跳包 */
#define DEV_ADDR			0x0022 /*!< 设备地址 */

/**
 * @}
 */


/**
 * @}
 */
#endif /* FUNCTION_H */
