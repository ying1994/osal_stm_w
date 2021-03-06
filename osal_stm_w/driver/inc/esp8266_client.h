/**
 * @file    esp8266_client.h
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
#ifndef ESP8266_CLIENT_H
#define ESP8266_CLIENT_H


#include "hal_types.h"
#include "hal_board.h"
#include "hal_gpio.h"
#include "hal_uart.h"

#ifdef CFG_ESP8266_CLIENT


#define ESP8266_RST_GPIO_TYPE	GPIOD		//GPIOA
#define ESP8266_RST_GPIO_PIN	GPIO_Pin_1	//GPIO_Pin_2

#define ESP8266_MAX_MSG_SIZE	256

typedef enum _ESP8266_CONNST_STATUS
{
	ESP8266_CONNETED = 0,		/*!< 建立连接 ，wifi和服务器连上*/
	ESP8266_CONNET_LOST,		/*!< 失去连接 服务器断开，wifi正常*/
	ESP8266_CONNET_GETIP,		/*!< WIFI在线，连接上wifi*/
	ESP8266_CONNET_LOST_WIFI	/*!< 物理掉线 ，wifi掉线*/
}Esp8266ConnetStatus;

/**
 * @brief ESP8266初始化
 * @param hUart:  Uart实例句柄
 * @param bConetServer:  是否连接服务器
 * @retval None
 */
void esp8266_client_Init(HALUartTypeDef* hUart, BOOL bConetServer);

/**
 * @brief ESP8266资源释放
 * @param hUart:  Uart实例句柄
 * @retval None
 */
void esp8266_client_DeInit(void);

/**
 * @brief: ESP8266 连接到路由器
 * @param ssid: Wifi
 * @param passwd: 端口号
 * @retval: 连接成功返回TRUE, 失败返回FALSE
 */
BOOL esp8266_client_connet_wifi(const UCHAR *ssid, const UCHAR *pwd);

/**
 * @brief: ESP8266 断开路由器连接
 * @param None
 * @retval: 连接成功返回TRUE, 失败返回FALSE
 */
BOOL esp8266_client_desconnet_wifi(void);

/**
 * @brief 检查路由器是否连接
 * @param None
 * @retval 网络连接返回TRUE, 未连接返回FALSE
 */
BOOL esp8266_client_isWifiConnet(void);

/**
 * @brief: ESP8266 连接到服务器
 * @param ip: ip地址
 * @param port: 端口号
 * @param bTcp: 连接方式, TRUE: TCP连接, FALSE: UDB连接
 * @retval: 连接成功返回TRUE, 失败返回FALSE
 */
BOOL esp8266_client_connet(const UCHAR *ip, UINT16 port, BOOL bTcp);

/**
 * @brief: ESP8266 断开连接
 * @param None
 * @retval: 连接成功返回TRUE, 失败返回FALSE
 */
BOOL esp8266_client_desconnet(void);

/**
 * @brief 检查网络是否连接
 * @param None
 * @retval 网络连接返回TRUE, 未连接返回FALSE
 */
BOOL esp8266_client_isConnet(void);

/**
 * @brief: 设置ESP8266的IP地址
 * @param ip: ip地址, 不允许为空
 * @param gateway: 网关地址
 * @param netmask: 掩码地址
 * @retval: 设置成功返回TRUE, 失败返回FALSE
 */
BOOL esp8266_client_setIp(const UCHAR *ip, const UCHAR *gateway, const UCHAR *netmask);

/**
 * @brief: 设置ESP8266的MAC地址
 * @param mac: mac地址
 * @retval: 设置成功返回TRUE, 失败返回FALSE
 */
BOOL esp8266_client_setMac(const UCHAR *mac);

/**
 * @brief: 注册串口接收观察者
 * @param hrxobser: 串口接收观察者句柄
 * @retval: void
 */
void esp8266_client_add_rx_obser(HalUartCBack_t hrxobser);
/**
 * @brief: 向串口写入一组数据
 * @param *pdata: 写入数据存储地址指针
 * @param len: 写入数据长度
 * @retval: 返回发送的数据个数
 */
UINT16 esp8266_client_write(UCHAR *pdata, UINT16 len);

/**
 * @brief ESP8266进入透传模式
 * @param None
 * @retval None
 */
void esp8266_client_StartTransparent(void);

/**
 * @brief ESP8266退出透传模式
 *        连续发送三个‘+’，然后关闭透传模式
 * @param None
 * @retval None
 */
void esp8266_client_StopTransparent(void);

/**
 * @brief ESP8266清空接收缓存
 * @param None
 * @retval None
 */
void esp8266_client_ClrData(void);

/**
 * @brief ESP8266复位
 * @param None
 * @retval None
 */
void esp8266_client_reset(void);

/**
 * @brief ESP8266状态检查
 * @param None
 * @retval 返回ESP8266状态
 */
UINT16 esp8266_client_check(void);

void esp8266_client_delay(UINT32 t);

/**
 * @brief: 获取端口串口通讯句柄
 * @param None
 * @retval: 指定端口串口通讯句柄
 */
HALUartTypeDef* esp8266_client_getinstance(void);


#endif //CFG_ESP8266
#endif
