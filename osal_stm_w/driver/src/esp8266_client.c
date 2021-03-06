/**
 * @file    esp8266_client.c
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
#include "esp8266_client.h"
#include  "hal_defs.h"
#include  "hal_types.h"
#include  "global.h"


#if (defined(CFG_USE_WIFI) && defined(CFG_ESP8266_CLIENT))

/* 数据透传是否启动 */
static BOOL m_bTransEnable = FALSE;
/* 连接方式 TRUE: TCP连接, FALSE: UDB连接 */
static BOOL m_bTcpTrans = TRUE;

/* 数据接收缓冲区 */
static UCHAR m_rxDataBuffer[ESP8266_MAX_MSG_SIZE] = {0};
/* 数据缓冲区读指针 */
static UINT16 m_uReadPriter = 0;
static UINT16 m_uReadPriter1 = 0;
/* 数据缓冲区写指针 */
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
		for (j = 0; j < 10000; j++)
			__NOP();
		HalIwdgFred();
	}
}

void esp8266_client_delay(UINT32 t)
{
	delay(t);
}

/**
 * @brief Uart端口数据接收回调函数
 * @param data 接收到的数据
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
 * @brief 等待接收
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
 * @brief 等待接收
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

#define ESP8266_SENDCMD_TRYCNT 10
#define ESP8266_SENDCMD_TIME_OUT 300
/**
 * @brief 发送命令, 并校验返回数据是否正确
 * @param cmd 发送命令
 * @param res 检验命令
 * @retval 校验正确返回TRUE, 否则返回FALSE
 */
static BOOL SendCmd(const char *cmd, const char *res)
{
	BOOL ret = FALSE;
	//UINT16 timeOut = 0;
	m_bTransEnable = FALSE;
	
	if ((NULL == m_hUart) || (NULL == cmd) || (NULL == res))
		return FALSE;

	m_uReadPriter = 0;
	m_hUart->write((UCHAR*)cmd, strlen(cmd));	//写命令到网络设备
	DBG(TRACE("[SendCmd] send: %s\r\n", cmd));
	
	//for (timeOut = 0; timeOut < ESP8266_SENDCMD_TIME_OUT; timeOut++)	//等待
	{
		delay(4);//挂起等待
		//if(WaitForRecive())	//数据接收完成
		if(WaitForReciveEx(ESP8266_SENDCMD_TIME_OUT << 1))	//数据接收完成
		{
			DBG(TRACE("[SendCmd] recv: %s\r\n", m_rxDataBuffer));
			if(strstr((const char *)m_rxDataBuffer, (const char *)res) != NULL)	//如果检索到关键词
			{
				esp8266_client_ClrData();	//清空缓存
				ret = TRUE;
				//break;
			}
		}
	}
	
	return ret;
}

/**
 * @brief ESP8266初始化
 * @param hUart:  Uart实例句柄
 * @param bConetServer:  是否连接服务器
 * @retval None
 */
void esp8266_client_Init(HALUartTypeDef* hUart, BOOL bConetServer)
{
	BOOL result = FALSE;
	if (NULL != hUart)
	{
		if (m_hUart != NULL)
		{
			m_hUart->deInit();//默认 115200, N, 8, 1
		}
		m_hUart = hUart;
	}

	if (m_hUart != NULL)
	{
		HalGpioInit(ESP8266_RST_GPIO_TYPE, ESP8266_RST_GPIO_PIN, HAL_GPIOMode_Out_PP);
		HalGpioWriteBit(ESP8266_RST_GPIO_TYPE, ESP8266_RST_GPIO_PIN, TRUE);
		m_hUart->init();//默认 115200, N, 8, 1
		m_hUart->add_rx_obser(OnComm);
		
		esp8266_client_reset();
		if (bConetServer)
		{
			result = esp8266_client_setIp(g_aLocalIP, g_aLocalGateway, g_aLocalMask);//设置IP
			if (result)
			{
				esp8266_client_delay(500);
				result = esp8266_client_setMac(g_aLocalMac);//设置MAC地址
			}
			
			if (result)
			{
				esp8266_client_delay(500);
				result = esp8266_client_connet_wifi(g_aSsid, g_aPwd);//连接Wifi
			}
			
			if (result)
			{
				esp8266_client_delay(500);
				result = esp8266_client_connet(g_aServerIp, g_uServerPort, g_bTcpConnet);//连接服务器
			}
			
			if (result)
			{
				esp8266_client_delay(500);
				esp8266_client_StartTransparent();//ESP8266进入透传模式
			}
		}
	}
}

/**
 * @brief ESP8266资源释放
 * @param hUart:  Uart实例句柄
 * @retval None
 */
void esp8266_client_DeInit(void)
{
	if (NULL == m_hUart)
		return;
	
	esp8266_client_StopTransparent();
	esp8266_client_desconnet();
	esp8266_client_desconnet_wifi();
	
	HalGpioInit(ESP8266_RST_GPIO_TYPE, ESP8266_RST_GPIO_PIN, HAL_GPIOMode_IN_FLOATING);
	
	m_hUart->deInit();//默认 115200, N, 8, 1
	m_hUart = NULL;
}

/**
 * @brief: ESP8266 连接到路由器
 * @param ssid: Wifi
 * @param passwd: 端口号
 * @retval: 连接成功返回TRUE, 失败返回FALSE
 */
BOOL esp8266_client_connet_wifi(const UCHAR *ssid, const UCHAR *pwd)
{
	char cmd[256] = {0};
	UINT16 uCnt = 0;
	if ((NULL == m_hUart) || (NULL == ssid) || (NULL == pwd) || (strlen((const char*)ssid) == 0)  || (strlen((const char*)pwd) == 0))
		return FALSE;
	
	esp8266_client_StopTransparent();
	//设置WiFi模式 1:station 2:softAP 3:softAP + station
	uCnt = 0;
	while ((!SendCmd("AT+CWMODE=1\r\n", "OK")) && (uCnt < ESP8266_SENDCMD_TRYCNT))
	{
		delay(1000);
		uCnt++;
	}
	//if (!SendCmd("AT+CWMODE=1\r\n", "OK"))
	if ((uCnt >= ESP8266_SENDCMD_TRYCNT))
		return FALSE; 
	delay(300);
	
	//关闭透传
	uCnt = 0;
	while ((!SendCmd("AT+CIPMODE=0\r\n", "OK")) && (uCnt < ESP8266_SENDCMD_TRYCNT))
	{
		delay(1000);
		uCnt++;
	}
	//if (!SendCmd("AT+CIPMODE=0\r\n", "OK"))
	if ((uCnt >= ESP8266_SENDCMD_TRYCNT))
		return FALSE;  
	delay(300);
	
	//连接路由
	m_uReadPriter = 0;
	sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);	//<ssid>,<pwd>[,<bssid>]
	m_hUart->write((UCHAR*)cmd, strlen(cmd));
	//SendCmd(cmd, "OK");
	DBG(TRACE("[esp8266_client_connet_wifi] send:%s\r\n", cmd));
	delay(3500);
	TRACE("[esp8266_client_connet_wifi] recv:%s\r\n", m_rxDataBuffer);
	//if(strstr((const char *)m_rxDataBuffer, "OK"))			//获得IP
	if(strstr((const char *)m_rxDataBuffer, "WIFI CONNECTED") == NULL)	//连接WIFI
	{
		return FALSE;
	}
	//if (esp8266_client_check() != ESP8266_CONNET_LOST_WIFI)//物理掉线
	//	return FALSE;  
	
	//上电自动连接
	uCnt = 0;
	while ((!SendCmd("AT+CWAUTOCONN=0\r\n", "OK")) && (uCnt < ESP8266_SENDCMD_TRYCNT))
	{
		delay(1000);
		uCnt++;
	}
	//if (!SendCmd("AT+CWAUTOCONN=0\r\n", "OK"))
	//if ((uCnt >= ESP8266_SENDCMD_TRYCNT))
	//	return FALSE; 
	
	return TRUE;
}

/**
 * @brief: ESP8266 断开路由器连接
 * @param None
 * @retval: 连接成功返回TRUE, 失败返回FALSE
 */
BOOL esp8266_client_desconnet_wifi(void)
{
	esp8266_client_StopTransparent();
	return SendCmd("AT+CWQAP\r\n", "OK"); 
}

/**
 * @brief 检查路由器是否连接
 * @param None
 * @retval 网络连接返回TRUE, 未连接返回FALSE
 */
BOOL esp8266_client_isWifiConnet(void)
{
	return (esp8266_client_check() == ESP8266_CONNET_LOST_WIFI) ? FALSE : TRUE;
}

/**
 * @brief: ESP8266 连接到服务器
 * @param ip: ip地址
 * @param port: 端口号
 * @param bTcp: 连接方式, TRUE: TCP连接, FALSE: UDB连接
 * @retval: 连接成功返回TRUE, 失败返回FALSE
 */
BOOL esp8266_client_connet(const UCHAR *ip, UINT16 port, BOOL bTcp)
{
	BOOL bTrans = m_bTransEnable;
	char cmd[256] = {0};
	UINT16 uCnt = 0;
	
	if ((NULL == m_hUart) || (NULL == ip) || (strlen((const char*)ip) == 0))
		return FALSE;
	esp8266_client_StopTransparent();
	
	esp8266_client_desconnet();//断开原来的连接
	
	m_bTcpTrans = bTcp;
	
	SendCmd("AT+CIPMUX=0\r\n", "OK");	//单链接模式
	
	if (m_bTcpTrans)
		sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%d,1\r\n", ip, port); //<type>,<remote IP>,<remote port> [,<TCP keep alive>] 
	else
		sprintf(cmd, "AT+CIPSTART=\"UDP\",\"%s\",%d\r\n", ip, port); //<type>,<remote IP>,<remote port> [,<TCP keep alive>] 
	
	uCnt = 0;
	while ((!SendCmd(cmd, "CONNECT")) && (uCnt < ESP8266_SENDCMD_TRYCNT))
	{
		delay(1000);
		uCnt++;
	}
	if ((uCnt < ESP8266_SENDCMD_TRYCNT))
		return TRUE; 
	//for (timeOut = 0; timeOut < 10; timeOut++)
	//{
	//	if (SendCmd(cmd, "CONNECT"))
	//		return TRUE;
	//	delay(300);
	//	//TRACE("%d: %s", timeOut, cmd);
	//}
	
	if (bTrans)
		esp8266_client_StartTransparent();
	
	return FALSE;
}

/**
 * @brief: ESP8266 断开连接
 * @param None
 * @retval: 连接成功返回TRUE, 失败返回FALSE
 */
BOOL esp8266_client_desconnet(void)
{
	esp8266_client_StopTransparent();
	return SendCmd("AT+CIPCLOSE\r\n", "OK");
}

/**
 * @brief 检查网络是否连接
 * @param None
 * @retval 网络连接返回TRUE, 未连接返回FALSE
 */
BOOL esp8266_client_isConnet(void)
{
	return (esp8266_client_check() == ESP8266_CONNETED) ? TRUE : FALSE;
}

/**
 * @brief: 设置ESP8266的IP地址
 * @param ip: ip地址, 不允许为空
 * @param gateway: 网关地址
 * @param netmask: 掩码地址
 * @retval: 设置成功返回TRUE, 失败返回FALSE
 */
BOOL esp8266_client_setIp(const UCHAR *ip, const UCHAR *gateway, const UCHAR *netmask)
{
	BOOL bTrans = m_bTransEnable;
	BOOL bRet = FALSE;
	char cmd[128] = {0};
	
	if ((NULL == m_hUart) || (NULL == ip))
		return FALSE;
	
	esp8266_client_StopTransparent();
	if ((NULL != gateway) && (NULL != netmask))
		sprintf(cmd, "AT+CIPSTA=\"%d.%d.%d.%d\",\"%d.%d.%d.%d\",\"%d.%d.%d.%d\"\r\n", ip[0], ip[1], ip[2], ip[3], gateway[0], gateway[1], gateway[2], gateway[3], netmask[0], netmask[1], netmask[2], netmask[3]);
	else
		sprintf(cmd, "AT+CIPSTA=\"%d.%d.%d.%d\"\r\n", ip[0], ip[1], ip[2], ip[3]);
	bRet = SendCmd(cmd, "OK");
	
	if (bTrans)
		esp8266_client_StartTransparent();
	
	return bRet;
}

/**
 * @brief: 设置ESP8266的MAC地址
 * @param mac: mac地址
 * @retval: 设置成功返回TRUE, 失败返回FALSE
 */
BOOL esp8266_client_setMac(const UCHAR *mac)
{
	BOOL bTrans = m_bTransEnable;
	BOOL bRet = FALSE;
	char cmd[64] = {0};
	
	if ((NULL == m_hUart) || (NULL == mac))
		return FALSE;
	
	esp8266_client_StopTransparent();
	sprintf(cmd, "AT+CIPSTAMAC=\"%02X:%02X:%02X:%02X:%02X:%02X\"\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	bRet = SendCmd(cmd, "OK");
	
	if (bTrans)
		esp8266_client_StartTransparent();
	
	return bRet;
}

/**
 * @brief: 注册串口接收观察者
 * @param hrxobser: 串口接收观察者句柄
 * @retval: void
 */
void esp8266_client_add_rx_obser(HalUartCBack_t hrxobser)
{
	m_hUartObser = hrxobser;
}

/**
 * @brief: 向串口写入一组数据
 * @param *pdata: 写入数据存储地址指针
 * @param len: 写入数据长度
 * @retval: 返回发送的数据个数
 */
UINT16 esp8266_client_write(UCHAR *pdata, UINT16 len)
{
	//char cmd[32] = {0};
	if (NULL == m_hUart)
		return 0;
	
	if (ESP8266_CONNETED != m_uWifiState)
		return FALSE;
	if (m_bTransEnable)
	{
		//m_bTransEnable = FALSE;
		//sprintf(cmd, "AT+CIPSEND\r\n");		//发送命令
		//if(!SendCmd(cmd, ">"))			//收到‘>’时可以发送数据
		//SendCmd(cmd, ">");
		m_hUart->write(pdata, len);
		//m_bTransEnable = TRUE;
		return len;
	}
	return 0;
}

/**
 * @brief ESP8266进入透传模式
 * @param None
 * @retval None
 */
void esp8266_client_StartTransparent(void)
{
	//BOOL bCipMode = FALSE;
	//BOOL bCIPSend = FALSE;
	UINT16 uCnt1 = 0;
	UINT16 uCnt2 = 0;
	if (NULL == m_hUart)
		return;
	if (!m_bTransEnable)
	{
		//SendCmd("AT+CIPMUX=0\r\n", "OK");	//单链接模式
		while ((!SendCmd("AT+CIPMODE=1\r\n", "OK")) && (uCnt1 < ESP8266_SENDCMD_TRYCNT))
		{
			delay(1000);
			uCnt1++;
		}
		while ((!SendCmd("AT+CIPSEND\r\n", ">")) && (uCnt2 < ESP8266_SENDCMD_TRYCNT))	//使能透传
		{
			delay(1000);
			uCnt2++;
		}
		//bCipMode = SendCmd("AT+CIPMODE=1\r\n", "OK");	//单链接模式
		//bCIPSend = SendCmd("AT+CIPSEND\r\n", ">");//使能透传
		if ((uCnt1 < ESP8266_SENDCMD_TRYCNT) && (uCnt2 < ESP8266_SENDCMD_TRYCNT))
			m_bTransEnable = TRUE;
		//DBG(TRACE("[esp8266_client_StartTransparent] send: TransEnable = %s\r\n", m_bTransEnable ? "TRUE" : "FALSE"));
		esp8266_client_ClrData();
	}
}

/**
 * @brief ESP8266退出透传模式
 *        连续发送三个‘+’，然后关闭透传模式
 * @param None
 * @retval None
 */
void esp8266_client_StopTransparent(void)
{
	//static BOOL bFirst = TRUE;
	UINT16 uCnt1 = 0;
	if (NULL == m_hUart)
		return;
	//if ((bFirst) || (m_bTransEnable))
	{
		//bFirst = FALSE;
		m_bTransEnable = FALSE;
		do
		{
		m_hUart->write("+++", 3);
		delay(3000);
		uCnt1++;
		}while ((!SendCmd("AT+CIPMODE=0\r\n", "OK")) && (uCnt1 < ESP8266_SENDCMD_TRYCNT));//关闭透传模式
	}
	esp8266_client_ClrData();
}

/**
 * @brief ESP8266清空接收缓存
 * @param None
 * @retval None
 */
void esp8266_client_ClrData(void)
{
	memset(m_rxDataBuffer, 0, sizeof(m_rxDataBuffer));	//清空缓存
	m_uReadPriter = 0;
}

/**
 * @brief ESP8266复位
 * @param None
 * @retval None
 */
void esp8266_client_reset(void)
{
	esp8266_client_StopTransparent();	//退出透传模式
	HalGpioWriteBit(ESP8266_RST_GPIO_TYPE, ESP8266_RST_GPIO_PIN, FALSE);	//复位
	delay(2000);
	
	HalGpioWriteBit(ESP8266_RST_GPIO_TYPE, ESP8266_RST_GPIO_PIN, TRUE);	//结束复位
	delay(1000);
}


#define ESP8266_CHECK_TIME_OUT 150
/**
 * @brief ESP8266状态检查
 * @param None
 * @retval 返回ESP8266状态
 */
UINT16 esp8266_client_check(void)
{
	BOOL bTrans = m_bTransEnable;
	UINT16 timeOut = 0;
	UINT16 status = ESP8266_CONNET_LOST_WIFI;
	
	if (NULL == m_hUart)
		return ESP8266_CONNET_LOST_WIFI;
	
	esp8266_client_StopTransparent();
	
	m_uReadPriter = 0;
	m_hUart->write("AT+CIPSTATUS\r\n", 14);
	for (timeOut = 0; timeOut < ESP8266_CHECK_TIME_OUT; timeOut++)
	{
		if(WaitForRecive())
		{
			if(strstr((const char *)m_rxDataBuffer, "STATUS:2"))			//获得IP
			{
				status = ESP8266_CONNET_GETIP;
			}
			else if(strstr((const char *)m_rxDataBuffer, "STATUS:3"))	//建立连接
			{
				status = ESP8266_CONNETED;
			}
			else if(strstr((const char *)m_rxDataBuffer, "STATUS:4"))	//失去连接
			{
				status = ESP8266_CONNET_LOST;
			}
			else if(strstr((const char *)m_rxDataBuffer, "STATUS:5"))	//物理掉线
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
		esp8266_client_StartTransparent();
	
	m_uWifiState = status;
	return status;
}

/**
 * @brief: 初始化串口通讯端口
 * @param: void
 * @retval: void
 */
static void init(void)
{
}


/**
 * @brief 资源注销
 * @param None
 * @retval None
 */
static void deInit(void)
{
	esp8266_client_DeInit();
}


/**
 * @brief: 注册串口接收观察者
 * @param hrxobser: 串口接收观察者句柄
 * @retval: void
 */
static void add_rx_obser(HalUartCBack_t hrxobser)
{
	esp8266_client_add_rx_obser(hrxobser);
}


/**
 * @brief: 设置串口通讯波特率
 * @param baudrate: 串口通讯波特率
 * @retval: void
 */
static void set_baudrate(UINT32 baudrate)
{
}


/**
 * @brief: 设置串口通讯字长
 * @param wordlength: 串口通讯字长
 * @retval: void
 */
static void set_wordlength(UINT16 wordlength)
{
}


/**
 * @brief: 设置串口通讯停止位
 * @param stopbit: 串口通讯停止位
 * @retval: void
 */
static void set_stopbit(UINT16 stopbit)
{
}


/**
 * @brief: 设置串口通讯校验位
 * @param parity: 串口通讯校验位
 * @retval: void
 */
static void set_parity(UINT16 parity)
{
}

/**
 * @brief: 向串口写入一组数据
 * @param *pdata: 写入数据存储地址指针
 * @param len: 写入数据长度
 * @retval: void
 */
static void write(UCHAR *pdata, UINT16 len)
{
	esp8266_client_write(pdata, len);
}

/**
 * @brief: 创建端口串口通讯对象
 * @param numer: 串口端口号
 * @retval None
 */
static void New(void)
{
	m_hInstance.add_rx_obser = add_rx_obser;
	m_hInstance.init = init;
	m_hInstance.deInit = deInit;
	m_hInstance.set_baudrate = set_baudrate;
	m_hInstance.set_databits = set_wordlength;
	m_hInstance.set_stopbit = set_stopbit;
	m_hInstance.set_parity = set_parity;
	m_hInstance.write = write;
	pthis = &m_hInstance;
	pthis->init();
}

/**
 * @brief: 获取端口串口通讯句柄
 * @param None
 * @retval: 指定端口串口通讯句柄
 */
HALUartTypeDef* esp8266_client_getinstance(void)
{
	if (NULL == m_hUart)
		return NULL;
	
	if (NULL == pthis)
	{
		New();
	}
	
	return pthis;
}
#endif //CFG_ESP8266_CLIENT

