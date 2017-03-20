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
#include "stdafx.h"
#include "esp8266.h"
#include  "hal_defs.h"
#include  "hal_types.h"


#ifdef CFG_ESP8266


/* ����͸���Ƿ����� */
static BOOL m_bTransEnable = FALSE;
/* ���ӷ�ʽ TRUE: TCP����, FALSE: UDB���� */
static BOOL m_bTcpTrans = TRUE;

/* ���ݽ��ջ����� */
static UCHAR m_rxDataBuffer[ESP8266_MAX_MSG_SIZE] = {0};
/* ���ݻ�������ָ�� */
static UINT16 m_uReadPriter = 0;
static UINT16 m_uReadPriter1 = 0;
/* ���ݻ�����дָ�� */
//static UINT16 m_uWritePriter = 0;

static HalUartCBack_t m_hUartObser = NULL;

static HALUartTypeDef *m_hUart = NULL;

static HALUartTypeDef m_hInstance;
static HALUartTypeDef* pthis = NULL;

static UINT16 m_uWifiState = 3;


static void delay(UINT32 t)
{
	UINT32 i = 0;
	UINT32 j = 0;
	for (i = 0; i < t; i++)
	{
		for (j = 0; j < 9000; j++)
			__NOP();
		HalIwdgFred();
	}
}

/**
 * @brief Uart�˿����ݽ��ջص�����
 * @param data ���յ�������
 * @return void
 */
static void OnComm(UCHAR data)
{
	if (m_bTransEnable)
	{
		if (m_hUartObser != NULL)
				m_hUartObser(data);
	}
	else
	{
		m_rxDataBuffer[m_uReadPriter++] = data;
		
		if (m_uReadPriter >= ESP8266_MAX_MSG_SIZE)
		{
			m_uReadPriter = 0;
		}
	}
}

/**
 * @brief �ȴ�����
 * @param None
 * @retval 
 */
static BOOL WaitForRecive(void)
{
	if (0 == m_uReadPriter)
		return FALSE;
	if (m_uReadPriter == m_uReadPriter1)
	{
		m_rxDataBuffer[m_uReadPriter] = '\0';
		m_uReadPriter = 0;
		return TRUE;
	}
	m_uReadPriter1 = m_uReadPriter;
	return FALSE;
}

/**
 * @brief �ȴ�����
 * @param None
 * @retval 
 */
static BOOL WaitForReciveEx(UINT32 t)
{
	delay(t);
	if (0 == m_uReadPriter)
		return FALSE;
	m_rxDataBuffer[m_uReadPriter] = '\0';
	return TRUE;
}

#define ESP8266_SENDCMD_TIME_OUT 150
/**
 * @brief ��������, ��У�鷵�������Ƿ���ȷ
 * @param cmd ��������
 * @param res ��������
 * @retval У����ȷ����TRUE, ���򷵻�FALSE
 */
static BOOL SendCmd(const char *cmd, const char *res)
{
	BOOL ret = FALSE;
	//UINT16 timeOut = 0;
	m_bTransEnable = FALSE;
	
	if ((NULL == m_hUart) || (NULL == cmd) || (NULL == res))
		return FALSE;

	m_uReadPriter = 0;
	m_hUart->write((UCHAR*)cmd, strlen(cmd));	//д��������豸
	DBG(TRACE("send: %s\r\n", cmd));
	
	//for (timeOut = 0; timeOut < ESP8266_SENDCMD_TIME_OUT; timeOut++)	//�ȴ�
	{
		delay(4);//����ȴ�
		//if(WaitForRecive())	//���ݽ������
		if(WaitForReciveEx(ESP8266_SENDCMD_TIME_OUT << 1))	//���ݽ������
		{
			DBG(TRACE("recv: %s\r\n", m_rxDataBuffer));
			if(strstr((const char *)m_rxDataBuffer, (const char *)res) != NULL)	//����������ؼ���
			{
				esp8266_ClrData();	//��ջ���
				ret = TRUE;
				//break;
			}
		}
	}
	
	return ret;

}

/**
 * @brief ESP8266��ʼ��
 * @param hUart:  Uartʵ�����
 * @retval None
 */
void esp8266_Init(HALUartTypeDef* hUart)
{
	if (NULL != hUart)
	{
		if (m_hUart != NULL)
		{
			m_hUart->deInit();//Ĭ�� 115200, N, 8, 1
		}
		m_hUart = hUart;
	}

	if (m_hUart != NULL)
	{
		HalGpioInit(ESP8266_RST_GPIO_TYPE, ESP8266_RST_GPIO_PIN, HAL_GPIOMode_Out_PP);
		HalGpioWrite(ESP8266_RST_GPIO_TYPE, ESP8266_RST_GPIO_PIN, TRUE);
		m_hUart->init();//Ĭ�� 115200, N, 8, 1
		m_hUart->add_rx_obser(OnComm);
	}
}

/**
 * @brief ESP8266��Դ�ͷ�
 * @param hUart:  Uartʵ�����
 * @retval None
 */
void esp8266_DeInit(void)
{
	if (NULL == m_hUart)
		return;
	
	esp8266_StopTransparent();
	esp8266_desconnet();
	esp8266_desconnet_wifi();
	
	HalGpioInit(ESP8266_RST_GPIO_TYPE, ESP8266_RST_GPIO_PIN, HAL_GPIOMode_IN_FLOATING);
	
	m_hUart->deInit();//Ĭ�� 115200, N, 8, 1
	m_hUart = NULL;
}

/**
 * @brief: ESP8266 ���ӵ�·����
 * @param ssid: Wifi
 * @param passwd: �˿ں�
 * @retval: ���ӳɹ�����TRUE, ʧ�ܷ���FALSE
 */
BOOL esp8266_connet_wifi(const UCHAR *ssid, const UCHAR *pwd)
{
	char cmd[256] = {0};
	if (NULL == m_hUart)
		return FALSE;
	
	esp8266_StopTransparent();
	//����WiFiģʽ 1:station 2:softAP 3:softAP + station
	if (!SendCmd("AT+CWMODE=1\r\n", "OK"))
		return FALSE; 
	delay(10);
	
	//�ر�͸��
	if (!SendCmd("AT+CIPMODE=0\r\n", "OK"))
		return FALSE;  
	delay(10);
	
	//����·��
	m_uReadPriter = 0;
	sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);	//<ssid>,<pwd>[,<bssid>]
	m_hUart->write((UCHAR*)cmd, strlen(cmd));
	//SendCmd(cmd, "OK");
	DBG(TRACE("send:%s\r\n", cmd));
	delay(3000);
	TRACE("recv:%s\r\n", m_rxDataBuffer);
	if(strstr((const char *)m_rxDataBuffer, "OK"))			//���IP
	{
	}
	//if (esp8266_check() != ESP8266_CONNET_LOST_WIFI)//�������
	//	return FALSE;  
	
	//�ϵ��Զ�����
	if (!SendCmd("AT+CWAUTOCONN=0", "OK"))
		return FALSE; 
	
	return TRUE;
}

/**
 * @brief: ESP8266 �Ͽ�·��������
 * @param None
 * @retval: ���ӳɹ�����TRUE, ʧ�ܷ���FALSE
 */
BOOL esp8266_desconnet_wifi(void)
{
	esp8266_StopTransparent();
	return SendCmd("AT+CWQAP\r\n", "OK"); 
}

/**
 * @brief ���·�����Ƿ�����
 * @param None
 * @retval �������ӷ���TRUE, δ���ӷ���FALSE
 */
BOOL esp8266_isWifiConnet(void)
{
	return (esp8266_check() == ESP8266_CONNET_LOST_WIFI) ? FALSE : TRUE;
}

/**
 * @brief: ESP8266 ���ӵ�������
 * @param ip: ip��ַ
 * @param port: �˿ں�
 * @param bTcp: ���ӷ�ʽ, TRUE: TCP����, FALSE: UDB����
 * @retval: ���ӳɹ�����TRUE, ʧ�ܷ���FALSE
 */
BOOL esp8266_connet(const UCHAR *ip, UINT16 port, BOOL bTcp)
{
	char cmd[256] = {0};
	//UINT16 timeOut = 0;
	
	esp8266_StopTransparent();
	
	esp8266_desconnet();//�Ͽ�ԭ��������
	
	m_bTcpTrans = bTcp;
	
	SendCmd("AT+CIPMUX=0\r\n", "OK");	//������ģʽ
	
	if (m_bTcpTrans)
		sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%d,60\r\n", ip, port); //<type>,<remote IP>,<remote port> [,<TCP keep alive>] 
	else
		sprintf(cmd, "AT+CIPSTART=\"UDP\",\"%s\",%d\r\n", ip, port); //<type>,<remote IP>,<remote port> [,<TCP keep alive>] 
	
	//for (timeOut = 0; timeOut < 10; timeOut++)
	{
		if (SendCmd(cmd, "CONNECT"))
			return TRUE;
		delay(10);
		//TRACE("%d: %s", timeOut, cmd);
	}
	
	//esp8266_StartTransparent();
	
	return FALSE;
}

/**
 * @brief: ESP8266 �Ͽ�����
 * @param None
 * @retval: ���ӳɹ�����TRUE, ʧ�ܷ���FALSE
 */
BOOL esp8266_desconnet(void)
{
	esp8266_StopTransparent();
	return SendCmd("AT+CIPCLOSE", "OK");
}

/**
 * @brief ��������Ƿ�����
 * @param None
 * @retval �������ӷ���TRUE, δ���ӷ���FALSE
 */
BOOL esp8266_isConnet(void)
{
	return (esp8266_check() == ESP8266_CONNETED) ? TRUE : FALSE;
}

/**
 * @brief: ����ESP8266��IP��ַ
 * @param ip: ip��ַ, ������Ϊ��
 * @param gateway: ���ص�ַ
 * @param netmask: �����ַ
 * @retval: ���óɹ�����TRUE, ʧ�ܷ���FALSE
 */
BOOL esp8266_setIp(const UCHAR *ip, const UCHAR *gateway, const UCHAR *netmask)
{
	BOOL bTrans = m_bTransEnable;
	BOOL bRet = FALSE;
	char cmd[128] = {0};
	
	if (NULL == ip)
		return FALSE;
	
	if (bTrans)
		esp8266_StopTransparent();
	if ((NULL == gateway) && (NULL == netmask))
		sprintf(cmd, "AT+CIPSTA=\"%d.%d.%d.%d\",\"%d.%d.%d.%d\",\"%d.%d.%d.%d\"", ip[0], ip[1], ip[2], ip[3], gateway[0], gateway[1], gateway[2], gateway[3], netmask[0], netmask[1], netmask[2], netmask[3]);
	else
		sprintf(cmd, "AT+CIPSTA=\"%d.%d.%d.%d\"", ip[0], ip[1], ip[2], ip[3]);
	bRet = SendCmd(cmd, "OK");
	
	if (bTrans)
		esp8266_StartTransparent();
	
	return bRet;
}

/**
 * @brief: ����ESP8266��MAC��ַ
 * @param mac: mac��ַ
 * @retval: ���óɹ�����TRUE, ʧ�ܷ���FALSE
 */
BOOL esp8266_setMac(const UCHAR *mac)
{
	BOOL bTrans = m_bTransEnable;
	BOOL bRet = FALSE;
	char cmd[64] = {0};
	
	if (NULL == mac)
		return FALSE;
	
	
	if (bTrans)
		esp8266_StopTransparent();
	sprintf(cmd, "AT+CIPSTAMAC=\"%02X:%02X:%02X:%02X:%02X:%02X\"", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	bRet = SendCmd(cmd, "OK");
	
	if (bTrans)
		esp8266_StartTransparent();
	
	return bRet;
}

/**
 * @brief: ע�ᴮ�ڽ��չ۲���
 * @param hrxobser: ���ڽ��չ۲��߾��
 * @retval: void
 */
void esp8266_add_rx_obser(HalUartCBack_t hrxobser)
{
	m_hUartObser = hrxobser;
}

/**
 * @brief: �򴮿�д��һ������
 * @param *pdata: д�����ݴ洢��ַָ��
 * @param len: д�����ݳ���
 * @retval: ���ط��͵����ݸ���
 */
UINT16 esp8266_write(UCHAR *pdata, UINT16 len)
{
	//char cmd[32] = {0};
	if (NULL == m_hUart)
		return 0;
	
	if (ESP8266_CONNETED != m_uWifiState)
		return FALSE;
	if (m_bTransEnable)
	{
		m_bTransEnable = FALSE;
		//sprintf(cmd, "AT+CIPSEND\r\n");		//��������
		//if(!SendCmd(cmd, ">"))			//�յ���>��ʱ���Է�������
		//SendCmd(cmd, ">");
		m_hUart->write(pdata, len);
		m_bTransEnable = TRUE;
		return len;
	}
	return 0;
}

/**
 * @brief ESP8266����͸��ģʽ
 * @param None
 * @retval None
 */
void esp8266_StartTransparent(void)
{
	BOOL bCipMode = FALSE;
	BOOL bCIPSend = FALSE;
	if (NULL == m_hUart)
		return;
	if (!m_bTransEnable)
	{
		//SendCmd("AT+CIPMUX=0\r\n", "OK");	//������ģʽ
		bCipMode = SendCmd("AT+CIPMODE=1\r\n", "OK");	//ʹ��͸��
		bCIPSend = SendCmd("AT+CIPSEND\r\n", ">");
		if (bCipMode && bCIPSend)
			m_bTransEnable = TRUE;
		esp8266_ClrData();
	}
}

/**
 * @brief ESP8266�˳�͸��ģʽ
 *        ��������������+����Ȼ��ر�͸��ģʽ
 * @param None
 * @retval None
 */
void esp8266_StopTransparent(void)
{
	static BOOL bFirst = TRUE;
	if (NULL == m_hUart)
		return;
	if ((bFirst) || (m_bTransEnable))
	{
		m_bTransEnable = FALSE;
		m_hUart->write("+++", 3);
		delay(500);
		SendCmd("AT+CIPMODE=0\r\n", "OK"); //�ر�͸��ģʽ
		bFirst = FALSE;
	}
	esp8266_ClrData();
}

/**
 * @brief ESP8266��ս��ջ���
 * @param None
 * @retval None
 */
void esp8266_ClrData(void)
{
	memset(m_rxDataBuffer, 0, sizeof(m_rxDataBuffer));	//��ջ���
	m_uReadPriter = 0;
}

/**
 * @brief ESP8266��λ
 * @param None
 * @retval None
 */
void esp8266_reset(void)
{
	esp8266_StopTransparent();	//�˳�͸��ģʽ
	HalGpioWrite(ESP8266_RST_GPIO_TYPE, ESP8266_RST_GPIO_PIN, FALSE);	//��λ
	delay(50);
	
	HalGpioWrite(ESP8266_RST_GPIO_TYPE, ESP8266_RST_GPIO_PIN, TRUE);	//������λ
	delay(200);
}


#define ESP8266_CHECK_TIME_OUT 100
/**
 * @brief ESP8266״̬���
 * @param None
 * @retval ����ESP8266״̬
 */
UINT16 esp8266_check(void)
{
	BOOL bTrans = m_bTransEnable;
	UINT16 timeOut = 0;
	UINT16 status = ESP8266_CONNET_LOST_WIFI;
	
	if (NULL == m_hUart)
		return ESP8266_CONNET_LOST_WIFI;
	
	if (bTrans)
		esp8266_StopTransparent();
	m_uReadPriter = 0;
	m_hUart->write("AT+CIPSTATUS\r\n", 14);
	for (timeOut = 0; timeOut < ESP8266_CHECK_TIME_OUT; timeOut++)
	{
		if(WaitForRecive())
		{
			if(strstr((const char *)m_rxDataBuffer, "STATUS:2"))			//���IP
			{
				status = ESP8266_CONNET_GETIP;
			}
			else if(strstr((const char *)m_rxDataBuffer, "STATUS:3"))	//��������
			{
				status = ESP8266_CONNETED;
			}
			else if(strstr((const char *)m_rxDataBuffer, "STATUS:4"))	//ʧȥ����
			{
				status = ESP8266_CONNET_LOST;
			}
			else if(strstr((const char *)m_rxDataBuffer, "STATUS:5"))	//�������
			{
				status = ESP8266_CONNET_LOST_WIFI;
			}
			
			break;
		}
		
		delay(2);
	}
	
	if(timeOut >= ESP8266_CHECK_TIME_OUT)
	{
		status = ESP8266_CONNET_LOST_WIFI;
	}
	
	if (bTrans)
		esp8266_StartTransparent();
	
	m_uWifiState = status;
	return status;
}

/**
 * @brief: ��ʼ������ͨѶ�˿�
 * @param: void
 * @retval: void
 */
static void init(void)
{
}


/**
 * @brief ��Դע��
 * @param None
 * @retval None
 */
static void deInit(void)
{
	esp8266_DeInit();
}


/**
 * @brief: ע�ᴮ�ڽ��չ۲���
 * @param hrxobser: ���ڽ��չ۲��߾��
 * @retval: void
 */
static void add_rx_obser(HalUartCBack_t hrxobser)
{
	esp8266_add_rx_obser(hrxobser);
}


/**
 * @brief: ���ô���ͨѶ������
 * @param baudrate: ����ͨѶ������
 * @retval: void
 */
static void set_baudrate(UINT32 baudrate)
{
}


/**
 * @brief: ���ô���ͨѶ�ֳ�
 * @param wordlength: ����ͨѶ�ֳ�
 * @retval: void
 */
static void set_wordlength(UINT16 wordlength)
{
}


/**
 * @brief: ���ô���ͨѶֹͣλ
 * @param stopbit: ����ͨѶֹͣλ
 * @retval: void
 */
static void set_stopbit(UINT16 stopbit)
{
}


/**
 * @brief: ���ô���ͨѶУ��λ
 * @param parity: ����ͨѶУ��λ
 * @retval: void
 */
static void set_parity(UINT16 parity)
{
}

/**
 * @brief: �򴮿�д��һ������
 * @param *pdata: д�����ݴ洢��ַָ��
 * @param len: д�����ݳ���
 * @retval: void
 */
static void write(UCHAR *pdata, UINT16 len)
{
	esp8266_write(pdata, len);
}

/**
 * @brief: �����˿ڴ���ͨѶ����
 * @param numer: ���ڶ˿ں�
 * @retval None
 */
static void New(void)
{
	m_hInstance.add_rx_obser = add_rx_obser;
	m_hInstance.init = init;
	m_hInstance.deInit = deInit;
	m_hInstance.set_baudrate = set_baudrate;
	m_hInstance.set_wordlength = set_wordlength;
	m_hInstance.set_stopbit = set_stopbit;
	m_hInstance.set_parity = set_parity;
	m_hInstance.write = write;
	pthis = &m_hInstance;
	pthis->init();
}

/**
 * @brief: ��ȡ�˿ڴ���ͨѶ���
 * @param None
 * @retval: ָ���˿ڴ���ͨѶ���
 */
HALUartTypeDef* esp8266_getinstance(void)
{
	if (NULL == m_hUart)
		return NULL;
	
	if (NULL == pthis)
	{
		New();
	}
	
	return pthis;
}
#endif //CFG_ESP8266

