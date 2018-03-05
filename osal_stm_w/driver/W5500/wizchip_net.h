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
#define WIZCHIP_CS_GPIO_TYPE	SPI2_GPIO_TYPE 
#define WIZCHIP_CS_GPIO_PIN		SPI2_NSS 

#define WIZCHIP_RST_GPIO_TYPE	GPIOD 
#define WIZCHIP_RST_GPIO_PIN	GPIO_Pin_15

#define WIZCHIP_INT_GPIO_TYPE	GPIOD 
#define WIZCHIP_INT_GPIO_PIN	GPIO_Pin_14 

#define SOCK_TCP_SERVER        0
#define SOCK_UDP_SERVER        1
#define SOCK_TCP_CLIENT        2
#define SOCK_UDP_CLIENT        3


#define SOCKET_DATA_BUF_SIZE   512

typedef enum {
	WIZCHIP_NET_CH0 = 0,
	WIZCHIP_NET_CH1,
	WIZCHIP_NET_CH2,
	WIZCHIP_NET_CH3,
#if _WIZCHIP_ > 5100
	WIZCHIP_NET_CH4,
	WIZCHIP_NET_CH5,
	WIZCHIP_NET_CH6,
	WIZCHIP_NET_CH7,
#endif  
	WIZCHIP_NET_SIZE,
}EnWizchipNetChannel;

/**
 * @brief W5500 ��ʼ��
 * @param hspi SPI�ӿڲ������
 * @param netinfo ���������Ϣ����IP��Gateway��MAC ......
 * @param bSetNet �Ƿ����������������
 * @retval ��ʼ���ɹ�����0, ���򷵻�-1
 */
int wizchip_net_Init(HALSpiTypeDef *hspi, wiz_NetInfo *netinfo, BOOL bSetNet);

/**
 * @brief W5500 ��Դ�ͷ�
 * @param None
 * @retval None
 */
void wizchip_net_DeInit(void);

/**
 * @brief W5500 ��������
 * @param sn Socket�˿ں�, ȡֵ <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>
 * @param mode ����ģʽ
 * @param server ������IP��ַ, ֻ���ڿͻ�����ģʽ(TCP Client, UDP Client)��Ч
 * @param port ����˿ں�, ���ڷ�����ģʽ(TCP Server, UDP Server)Ϊ�����˿�, ���ڿͻ�����ģʽ(TCP Client, UDP Client)ΪԶ�̷������˿�, �˿ں������0, ����Ĭ�϶˿ں�
 * @retval ��ʼ���ɹ�����0, ���򷵻�-1
 */
int vizchip_net_start(UINT8 sn, UINT8 mode, UCHAR* server, UINT16 port);

/**
 * @brief W5500 ֹͣ����
 * @param 
 * @retval ��ʼ���ɹ�����0, ���򷵻�-1
 */
int vizchip_net_stop(UINT8 sn);

/**
 * @brief: ��ȡָ���˿ڴ���ͨѶ���
 * @param 
 * @retval: ָ���˿ڴ���ͨѶ���
 */
HALUartTypeDef* vizchip_net_getinstance(UINT8 sn);

#endif //CFG_USE_NET
#endif   // _WIZCHIP_NET_H_
