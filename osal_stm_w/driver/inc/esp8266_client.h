/**
 * @file    esp8266.h
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
#ifndef ESP8266_H
#define ESP8266_H


#include "hal_types.h"
#include "hal_board.h"
#include "hal_gpio.h"
#include "hal_uart.h"

#ifdef CFG_ESP8266


#define ESP8266_RST_GPIO_TYPE	GPIOD		//GPIOA
#define ESP8266_RST_GPIO_PIN	GPIO_Pin_1	//GPIO_Pin_2

#define ESP8266_MAX_MSG_SIZE	256

typedef enum _ESP8266_CONNST_STATUS
{
	ESP8266_CONNETED = 0,		/*!< �������� ��wifi�ͷ���������*/
	ESP8266_CONNET_LOST,		/*!< ʧȥ���� �������Ͽ���wifi����*/
	ESP8266_CONNET_GETIP,		/*!< WIFI���ߣ�������wifi*/
	ESP8266_CONNET_LOST_WIFI	/*!< ������� ��wifi����*/
}Esp8266ConnetStatus;

/**
 * @brief ESP8266��ʼ��
 * @param hUart:  Uartʵ�����
 * @retval None
 */
void esp8266_Init(HALUartTypeDef* hUart);

/**
 * @brief ESP8266��Դ�ͷ�
 * @param hUart:  Uartʵ�����
 * @retval None
 */
void esp8266_DeInit(void);

/**
 * @brief: ESP8266 ���ӵ�·����
 * @param ssid: Wifi
 * @param passwd: �˿ں�
 * @retval: ���ӳɹ�����TRUE, ʧ�ܷ���FALSE
 */
BOOL esp8266_connet_wifi(const UCHAR *ssid, const UCHAR *pwd);

/**
 * @brief: ESP8266 �Ͽ�·��������
 * @param None
 * @retval: ���ӳɹ�����TRUE, ʧ�ܷ���FALSE
 */
BOOL esp8266_desconnet_wifi(void);

/**
 * @brief ���·�����Ƿ�����
 * @param None
 * @retval �������ӷ���TRUE, δ���ӷ���FALSE
 */
BOOL esp8266_isWifiConnet(void);

/**
 * @brief: ESP8266 ���ӵ�������
 * @param ip: ip��ַ
 * @param port: �˿ں�
 * @param bTcp: ���ӷ�ʽ, TRUE: TCP����, FALSE: UDB����
 * @retval: ���ӳɹ�����TRUE, ʧ�ܷ���FALSE
 */
BOOL esp8266_connet(const UCHAR *ip, UINT16 port, BOOL bTcp);

/**
 * @brief: ESP8266 �Ͽ�����
 * @param None
 * @retval: ���ӳɹ�����TRUE, ʧ�ܷ���FALSE
 */
BOOL esp8266_desconnet(void);

/**
 * @brief ��������Ƿ�����
 * @param None
 * @retval �������ӷ���TRUE, δ���ӷ���FALSE
 */
BOOL esp8266_isConnet(void);

/**
 * @brief: ����ESP8266��IP��ַ
 * @param ip: ip��ַ, ������Ϊ��
 * @param gateway: ���ص�ַ
 * @param netmask: �����ַ
 * @retval: ���óɹ�����TRUE, ʧ�ܷ���FALSE
 */
BOOL esp8266_setIp(const UCHAR *ip, const UCHAR *gateway, const UCHAR *netmask);

/**
 * @brief: ����ESP8266��MAC��ַ
 * @param mac: mac��ַ
 * @retval: ���óɹ�����TRUE, ʧ�ܷ���FALSE
 */
BOOL esp8266_setMac(const UCHAR *mac);

/**
 * @brief: ע�ᴮ�ڽ��չ۲���
 * @param hrxobser: ���ڽ��չ۲��߾��
 * @retval: void
 */
void esp8266_add_rx_obser(HalUartCBack_t hrxobser);
/**
 * @brief: �򴮿�д��һ������
 * @param *pdata: д�����ݴ洢��ַָ��
 * @param len: д�����ݳ���
 * @retval: ���ط��͵����ݸ���
 */
UINT16 esp8266_write(UCHAR *pdata, UINT16 len);

/**
 * @brief ESP8266����͸��ģʽ
 * @param None
 * @retval None
 */
void esp8266_StartTransparent(void);

/**
 * @brief ESP8266�˳�͸��ģʽ
 *        ��������������+����Ȼ��ر�͸��ģʽ
 * @param None
 * @retval None
 */
void esp8266_StopTransparent(void);

/**
 * @brief ESP8266��ս��ջ���
 * @param None
 * @retval None
 */
void esp8266_ClrData(void);

/**
 * @brief ESP8266��λ
 * @param None
 * @retval None
 */
void esp8266_reset(void);

/**
 * @brief ESP8266״̬���
 * @param None
 * @retval ����ESP8266״̬
 */
UINT16 esp8266_check(void);

void esp8266_delay(UINT32 t);

/**
 * @brief: ��ȡ�˿ڴ���ͨѶ���
 * @param None
 * @retval: ָ���˿ڴ���ͨѶ���
 */
HALUartTypeDef* esp8266_getinstance(void);


#endif //CFG_ESP8266
#endif
