//*****************************************************************************
//
//! \file wizchip_conf.h
//! \brief WIZCHIP Config Header File.
//! \version 1.0.0
//! \date 2013/10/21
//! \par  Revision history
//!       <2013/10/21> 1st Release
//! \author MidnightCow
//! \copyright
//!
//! Copyright (c)  2013, WIZnet Co., LTD.

#ifndef  _WIZCHIP_NET_H_
#define  _WIZCHIP_NET_H_

#include "stm32f10x.h"
#include "stdafx.h"
#include "hal_types.h"
#include "hal_board.h"

#if (defined(CFG_USE_NET) && defined(CFG_WIZCHIP))

#include "hal_spi.h"
#include "hal_uart.h"

#include <stdint.h>
#include "Ethernet/wizchip_conf.h"

//! CS
#define WIZCHIP_CS_GPIO_TYPE	SPI1_GPIO_TYPE 
#define WIZCHIP_CS_GPIO_PIN		SPI1_NSS 

#define WIZCHIP_RST_GPIO_TYPE	GPIOB 
#define WIZCHIP_RST_GPIO_PIN	GPIO_Pin_1

#define WIZCHIP_INT_GPIO_TYPE	GPIOB 
#define WIZCHIP_INT_GPIO_PIN	GPIO_Pin_0 

#define SOCK_TCP_SERVER        0
#define SOCK_UDP_SERVER        1
#define SOCK_TCP_CLIENT        2
#define SOCK_UDP_CLIENT        3


#define SOCKET_DATA_BUF_SIZE   1024

/**
 * @brief W5500 初始化
 * @param hspi SPI接口操作句柄
 * @param netinfo 网络相关信息，如IP、Gateway、MAC ......
 * @param bSetNet 是否重新设置网络参数
 * @retval 初始化成功返回0, 否则返回-1
 */
int wizchip_net_Init(HALSpiTypeDef *hspi, wiz_NetInfo *netinfo, BOOL bSetNet);

/**
 * @brief W5500 资源释放
 * @param None
 * @retval None
 */
void wizchip_net_DeInit(void);

/**
 * @brief W5500 启动服务
 * @param sn Socket端口号, 取值 <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>
 * @param mode 工作模式
 * @param server 服务器IP地址, 只对于客户端器模式(TCP Client, UDP Client)有效
 * @param port 服务端口号, 对于服务器模式(TCP Server, UDP Server)为监听端口, 对于客户端器模式(TCP Client, UDP Client)为远程服务器端口, 端口号需大于0, 否则按默认端口号
 * @retval 初始化成功返回0, 否则返回-1
 */
int vizchip_net_start(UINT8 sn, UINT8 mode, UCHAR* server, UINT16 port);

/**
 * @brief W5500 停止服务
 * @param 
 * @retval 初始化成功返回0, 否则返回-1
 */
int vizchip_net_stop(void);

/**
 * @brief: 获取指定端口串口通讯句柄
 * @param 
 * @retval: 指定端口串口通讯句柄
 */
HALUartTypeDef* vizchip_net_getinstance(void);

#endif //CFG_USE_NET
#endif   // _WIZCHIP_NET_H_
