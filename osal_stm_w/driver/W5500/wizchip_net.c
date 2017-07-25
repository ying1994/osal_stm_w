
#include <stddef.h>
#include "wizchip_net.h"

#include "hal_board.h"
#include "osal.h"
#include "Ethernet/socket.h"

#if (defined(CFG_USE_NET) && defined(CFG_WIZCHIP))
//! CS
#define WIZCHIP_CS_ON	HalGpioWrite(WIZCHIP_CS_GPIO_TYPE, WIZCHIP_CS_GPIO_PIN, 1)
#define WIZCHIP_CS_OFF	HalGpioWrite(WIZCHIP_CS_GPIO_TYPE, WIZCHIP_CS_GPIO_PIN, 0)

static HALSpiTypeDef *m_hspi = NULL;
//static wiz_NetInfo m_WIZNETINFO;

static HALUartTypeDef m_hInstance;
static HALUartTypeDef* pthis = NULL;
static HalUartCBack_t m_hrxobser = NULL;

static UINT8 m_uMode = SOCK_TCP_SERVER;
static UINT8 m_uSocketNumber = 0;
static UINT8 m_aDataBuffer[SOCKET_DATA_BUF_SIZE] = {0};
static UINT16 m_uRecvSize = 0;
static UCHAR *m_szServer = NULL;
static UINT16 m_uPort = 502;

// For UDP Server
static UINT8 *m_szDestIp[4];
static UINT16 m_uDestport;

/**
 * @brief Default function to enable interrupt.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
static void _wizchip_cris_enter(void)
{
}

/**
 * @brief Default function to disable interrupt.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
static void _wizchip_cris_exit(void)
{
	
}

/**
 * @brief Default function to select chip.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
static void _wizchip_cs_select(void)
{
	if (m_hspi != NULL)
	{
		WIZCHIP_CS_OFF;
	}
}

/**
 * @brief Default function to deselect chip.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
static void _wizchip_cs_deselect(void)
{
	if (m_hspi != NULL)
	{
		WIZCHIP_CS_ON;
	}
}

/**
 * @brief Default function to read in direct or indirect interface.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */

/**
 * @brief Default function to read in SPI interface.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
uint8_t _wizchip_spi_readbyte(void)
{
	if (m_hspi != NULL)
	{
		return (uint8_t)m_hspi->access(0xFFFF);
	}
	return 0;
}
/**
 * @brief Default function to write in SPI interface.
 * @note This function help not to access wrong address. If you do not describe this function or register any functions,
 * null function is called.
 */
void _wizchip_spi_writebyte(uint8_t wb)
{
	if (m_hspi != NULL)
	{
		m_hspi->access(wb);
	}
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
}


/**
 * @brief: 注册串口接收观察者
 * @param hrxobser: 串口接收观察者句柄
 * @retval: void
 */
static void add_rx_obser(HalUartCBack_t hrxobser)
{
	m_hrxobser = hrxobser;
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

static UINT16 read(UCHAR *pdata, UINT16 len)
{
	if (m_uRecvSize > 0)
	{
		if (len > m_uRecvSize)
			len = m_uRecvSize;
		memcpy(pdata, m_aDataBuffer, len);
		m_uRecvSize = 0;
		
		return len;
	}
	return 0;
}

/**
 * @brief: 向串口写入一组数据
 * @param *pdata: 写入数据存储地址指针
 * @param len: 写入数据长度
 * @retval: void
 */
static void write(UCHAR *pdata, UINT16 len)
{
	UINT8 *buf = pdata;
	UINT16 size = len;
	UINT16 sentsize=0;
	INT32 ret;
	switch (m_uMode)
	{
	case SOCK_TCP_SERVER:
		sentsize = 0;
		while(size != sentsize)
		{
		   ret = send(m_uSocketNumber, buf+sentsize, size-sentsize);
		   if(ret < 0)
		   {
			  close(m_uSocketNumber);
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_UDP_SERVER:
		sentsize = 0;
		while(sentsize != size)
		{
		   ret = sendto(m_uSocketNumber, buf+sentsize, size-sentsize, (uint8_t*)m_szDestIp, m_uDestport);
		   if(ret < 0)
		   {
			  //DBG(TRACE("%d: sendto error. %ld\r\n", m_uSocketNumber, ret));
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_TCP_CLIENT:
		sentsize = 0;
		while(size != sentsize)
		{
		   ret = send(m_uSocketNumber, buf+sentsize, size-sentsize);
		   if(ret < 0)
		   {
			  close(m_uSocketNumber);
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_UDP_CLIENT:
		sentsize = 0;
		while(sentsize != size)
		{
		   ret = sendto(m_uSocketNumber, buf+sentsize, size-sentsize, (uint8_t*)m_szDestIp, m_uDestport);
		   if(ret < 0)
		   {
			  //DBG(TRACE("%d: sendto error. %ld\r\n", m_uSocketNumber, ret));
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	default:
		break;
	}
}

/**
 * @brief: 创建端口串口通讯对象
 * @param numer: 串口端口号
 * @retval None
 */
static void New(void)
{
	if (NULL == pthis)
	{
		m_hInstance.add_rx_obser = add_rx_obser;
		m_hInstance.init = init;
		m_hInstance.deInit = deInit;
		m_hInstance.set_baudrate = set_baudrate;
		m_hInstance.set_databits = set_wordlength;
		m_hInstance.set_stopbit = set_stopbit;
		m_hInstance.set_parity = set_parity;
		m_hInstance.read = read;
		m_hInstance.write = write;
		pthis = &m_hInstance;
		//pthis->init();
	}
}

/**
 * @brief TCP Server
 */
static void taskForTcpServer(void)
{
	UINT8 *buf = m_aDataBuffer;
	UINT16 size = 0;
	//UINT16 sentsize=0;
	INT32 ret;
	UINT8 sn_sr = getSn_SR(m_uSocketNumber);
   switch(sn_sr)
   {
      case SOCK_ESTABLISHED :
         if(getSn_IR(m_uSocketNumber) & Sn_IR_CON)
         {
            DBG(TRACE("%d:Connected\r\n",m_uSocketNumber));
            setSn_IR(m_uSocketNumber,Sn_IR_CON);
         }
         if((size = getSn_RX_RSR(m_uSocketNumber)) > 0)
         {
            if(size > SOCKET_DATA_BUF_SIZE)
				size = SOCKET_DATA_BUF_SIZE;
            ret = recv(m_uSocketNumber,buf,size);
            if(ret <= 0)
				return;
			m_uRecvSize = ret;
			if (m_hrxobser != NULL)
				m_hrxobser(1);
			////////////////////// 回环测试 ///////////////////////
            //sentsize = 0;
            //while(size != sentsize)
            //{
            //   ret = send(m_uSocketNumber,buf+sentsize,size-sentsize);
            //   if(ret < 0)
            //   {
            //      close(m_uSocketNumber);
            //      return;
            //   }
            //   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            //}
         }
         break;
      case SOCK_CLOSE_WAIT :
         //DBG(TRACE("%d:CloseWait\r\n",m_uSocketNumber));
         if((ret=disconnect(m_uSocketNumber)) != SOCK_OK)
		 {
			DBG(TRACE("%d:CloseWait Field\r\n",m_uSocketNumber));
			 return;
		 }
         DBG(TRACE("%d:Closed\r\n",m_uSocketNumber));
         break;
      case SOCK_INIT :
    	  //DBG(TRACE("%d:Listen, port [%d]\r\n",m_uSocketNumber, m_uPort));
         if( (ret = listen(m_uSocketNumber)) != SOCK_OK)
		 {
			 DBG(TRACE("%d:Listen, port [%d] Field\r\n",m_uSocketNumber, m_uPort));
			 return;
		 }
         break;
      case SOCK_CLOSED:
         //DBG(TRACE("%d:LBTStart\r\n",m_uSocketNumber));
         if((ret=socket(m_uSocketNumber,Sn_MR_TCP,m_uPort,0x00)) != m_uSocketNumber)
		 {
			DBG(TRACE("%d:LBTStart Field\r\n",m_uSocketNumber));
            return;
		 }
         //DBG(TRACE("%d:Opened\r\n",m_uSocketNumber));
         break;
      default:
		  //DBG(TRACE("sn_sr:%d\r\n",sn_sr));
         break;
   }
}

/**
 * @brief UDP Server
 */
static void taskForUdpServer(void)
{
	UINT8 *buf = m_aDataBuffer;
	UINT16 size = 0;
	//UINT16 sentsize = 0;
	INT32  ret;
   //uint8_t  packinfo = 0;
	UINT8 sn_sr = getSn_SR(m_uSocketNumber);
   switch(sn_sr)
   {
      case SOCK_UDP :
         if((size = getSn_RX_RSR(m_uSocketNumber)) > 0)
         {
            if(size > SOCKET_DATA_BUF_SIZE)
				size = SOCKET_DATA_BUF_SIZE;
            ret = recvfrom(m_uSocketNumber,buf,size,(uint8_t*)m_szDestIp,(uint16_t*)&m_uDestport);
            if(ret <= 0)
            {
               DBG(TRACE("%d: recvfrom error. %ld\r\n",m_uSocketNumber,ret));
               return;
            }
			m_uRecvSize = ret;
			if (m_hrxobser != NULL)
				m_hrxobser(1);
			////////////////////// 回环测试 ///////////////////////
            //size = (uint16_t) ret;
            //sentsize = 0;
            //while(sentsize != size)
            //{
            //   ret = sendto(m_uSocketNumber,buf+sentsize,size-sentsize,(uint8_t*)m_szDestIp,m_uDestport);
            //   if(ret < 0)
            //   {
            //      DBG(TRACE("%d: sendto error. %ld\r\n",m_uSocketNumber,ret));
            //      return;
            //   }
            //   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            //}
         }
         break;
      case SOCK_CLOSED:
         //DBG(TRACE("%d:LBUStart\r\n",m_uSocketNumber));
         if((ret=socket(m_uSocketNumber,Sn_MR_UDP,m_uPort,0x00)) != m_uSocketNumber)
		 {
			DBG(TRACE("%d:Opened, port [%d] Field\r\n",m_uSocketNumber, m_uPort));
            return;
		 }
         //DBG(TRACE("%d:Opened, port [%d]\r\n",m_uSocketNumber, m_uPort));
         break;
      default :
		  //DBG(TRACE("sn_sr:%d\r\n",sn_sr));
         break;
   }
}

/**
 * @brief TCP Server
 */
static void taskForTcpClient(void)
{
	UINT16 anyport=20000;
	UINT8 *buf = m_aDataBuffer;
	UINT16 size = 0;
	//UINT16 sentsize=0;
	INT32 ret;
	UINT8 sn_sr = getSn_SR(m_uSocketNumber);
	
	if (m_szServer != NULL);
	{
	   switch(sn_sr)
	   {
		  case SOCK_ESTABLISHED :
			 if(getSn_IR(m_uSocketNumber) & Sn_IR_CON)
			 {
				DBG(TRACE("%d:Connected\r\n",m_uSocketNumber));
				setSn_IR(m_uSocketNumber,Sn_IR_CON);
			 }
			 if((size = getSn_RX_RSR(m_uSocketNumber)) > 0)
			 {
				if(size > SOCKET_DATA_BUF_SIZE)
					size = SOCKET_DATA_BUF_SIZE;
				ret = recv(m_uSocketNumber,buf,size);
				if(ret <= 0)
					return;
				m_uRecvSize = ret;
				if (m_hrxobser != NULL)
					m_hrxobser(1);
				////////////////////// 回环测试 ///////////////////////
				//sentsize = 0;
				//while(size != sentsize)
				//{
				//   ret = send(m_uSocketNumber,buf+sentsize,size-sentsize);
				//   if(ret < 0)
				//   {
				//      close(m_uSocketNumber);
				//      return;
				//   }
				//   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
				//}
			 }
			 break;
		  case SOCK_CLOSE_WAIT :
			 //DBG(TRACE("%d:CloseWait\r\n",m_uSocketNumber));
			 if((ret=disconnect(m_uSocketNumber)) != SOCK_OK)
			 {
				DBG(TRACE("%d:CloseWait Field\r\n",m_uSocketNumber));
				 return;
			 }
			 //DBG(TRACE("%d:Closed\r\n",m_uSocketNumber));
			 break;
		  case SOCK_INIT :
			 //DBG(TRACE("%d:connet[%d.%d.%d.%d], port [%d]\r\n",m_uSocketNumber, m_szServer[0], m_szServer[1], m_szServer[2], m_szServer[3], m_uPort));
			 if( (ret = connect(m_uSocketNumber, m_szServer, m_uPort)) != SOCK_OK)
			 {
				 DBG(TRACE("%d:connet[%d.%d.%d.%d], port [%d] Field\r\n",m_uSocketNumber, m_szServer[0], m_szServer[1], m_szServer[2], m_szServer[3],  m_uPort));
				 return;
			 }
			 break;
		  case SOCK_CLOSED:
			 //DBG(TRACE("%d:LBTStart\r\n",m_uSocketNumber));
			 if((ret=socket(m_uSocketNumber,Sn_MR_TCP,anyport++,Sn_MR_ND)) != m_uSocketNumber)
			 {
				DBG(TRACE("%d:LBTStart Field\r\n",m_uSocketNumber));
				return;
			 }
			 //DBG(TRACE("%d:Opened\r\n",m_uSocketNumber));
			 break;
		  default:
			  //DBG(TRACE("sn_sr:%d\r\n",sn_sr));
			 break;
	   }
   }
}

/**
 * @brief UDP Server
 */
static void taskForUdpClient(void)
{
}

/**
 * @brief W5500 初始化
 * @param hspi SPI接口操作句柄
 * @param netinfo 网络相关信息，如IP、Gateway、MAC ......
 * @retval 初始化成功返回0, 否则返回-1
 */
int wizchip_net_Init(HALSpiTypeDef *hspi, wiz_NetInfo *netinfo)
{
	UINT8 tmpstr[16];
	uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};	
		
	New();
	HalGpioInit(WIZCHIP_CS_GPIO_TYPE, WIZCHIP_CS_GPIO_PIN, HAL_GPIOMode_Out_PP);
	HalGpioInit(WIZCHIP_RST_GPIO_TYPE, WIZCHIP_RST_GPIO_PIN, HAL_GPIOMode_Out_PP);
	HalGpioWrite(WIZCHIP_RST_GPIO_TYPE, WIZCHIP_RST_GPIO_PIN, 0);
	delay_ms(100);
	HalGpioWrite(WIZCHIP_RST_GPIO_TYPE, WIZCHIP_RST_GPIO_PIN, 1);
	
	TRACE("hspi=%p netinfo=%p\r\n", hspi, netinfo);
	if ((hspi != NULL) && (netinfo != NULL))
	{
		m_hspi = hspi;
		m_hspi->init(HALSpiMode_Master, HALSpiDataSize_8b, HALSpiCPOL_Low, HALSpiCPHA_1Edge, HALSpiBaudRate_16, HALSpiFirstBit_MSB);

		reg_wizchip_cris_cbfunc(_wizchip_cris_enter, _wizchip_cris_exit);
#if   _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_VDM_
		reg_wizchip_cs_cbfunc(_wizchip_cs_select, _wizchip_cs_deselect);//注册SPI片选信号函数
#elif _WIZCHIP_IO_MODE_ == _WIZCHIP_IO_MODE_SPI_FDM_
		reg_wizchip_cs_cbfunc(_wizchip_cs_select, _wizchip_cs_deselect);// CS必须为低电平.
#else
   #if (_WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SIP_) != _WIZCHIP_IO_MODE_SIP_
      #error "Unknown _WIZCHIP_IO_MODE_"
   #else
      reg_wizchip_cs_cbfunc(wizchip_select, wizchip_deselect);
   #endif
#endif
		//reg_wizchip_bus_cbfunc(uint8_t (*bus_rb)(uint32_t addr), void (*bus_wb)(uint32_t addr, uint8_t wb));
		reg_wizchip_spi_cbfunc(_wizchip_spi_readbyte, _wizchip_spi_writebyte);
		
		/* WIZCHIP SOCKET Buffer initialize */
		if(ctlwizchip(CW_INIT_WIZCHIP,(void*)memsize) == -1){
			 printf("WIZCHIP Initialized fail.\r\n");
			 return -1;
		}
		
		ctlnetwork(CN_SET_NETINFO, (void*)netinfo);
		ctlnetwork(CN_GET_NETINFO, (void*)netinfo);
		ctlwizchip(CW_GET_ID,(void*)tmpstr);
		DBG(TRACE("\r\n=== %s NET CONF ===\r\n",(char*)tmpstr));
		DBG(TRACE("MAC: %02X:%02X:%02X:%02X:%02X:%02X\r\n",netinfo->mac[0],netinfo->mac[1],netinfo->mac[2],
			  netinfo->mac[3],netinfo->mac[4],netinfo->mac[5]));
		DBG(TRACE("SIP: %d.%d.%d.%d\r\n", netinfo->ip[0],netinfo->ip[1],netinfo->ip[2],netinfo->ip[3]));
		DBG(TRACE("GAR: %d.%d.%d.%d\r\n", netinfo->gw[0],netinfo->gw[1],netinfo->gw[2],netinfo->gw[3]));
		DBG(TRACE("SUB: %d.%d.%d.%d\r\n", netinfo->sn[0],netinfo->sn[1],netinfo->sn[2],netinfo->sn[3]));
		DBG(TRACE("DNS: %d.%d.%d.%d\r\n", netinfo->dns[0],netinfo->dns[1],netinfo->dns[2],netinfo->dns[3]));
		DBG(TRACE("======================\r\n"));

		return 0;
	}
	
	return -1;
}

/**
 * @brief W5500 资源释放
 * @param None
 * @retval None
 */
void wizchip_net_DeInit(void)
{
	if (m_hspi != NULL)
	{
		m_hspi->deInit();
		m_hspi = NULL;
	}
	if (pthis != NULL)
	{
		pthis->deInit();
		pthis = NULL;
	}
}

/**
 * @brief W5500 启动服务
 * @param sn Socket端口号, 取值 <b>0 ~ @ref \_WIZCHIP_SOCK_NUM_</b>
 * @param mode 工作模式
 * @param server 服务器IP地址, 只对于客户端器模式(TCP Client, UDP Client)有效
 * @param port 服务端口号, 对于服务器模式(TCP Server, UDP Server)为监听端口, 对于客户端器模式(TCP Client, UDP Client)为远程服务器端口, 端口号需大于0, 否则按默认端口号
 * @retval 初始化成功返回0, 否则返回-1
 */
int vizchip_net_start(UINT8 sn, UINT8 mode, UCHAR* server, UINT16 port)
{
	if (NULL == m_hspi)
		return -1;
	
	m_uSocketNumber = sn;
	m_uPort = (port > 0) ? port : 502;
	
#define TASK_NET_TIME 1
	
	switch (mode)
	{
	case SOCK_TCP_SERVER:
		m_uMode = mode;
		m_szServer = NULL;
		osal_task_create(taskForTcpServer, TASK_NET_TIME);
		DBG(TRACE("start tcp service\r\n"));
		break;
	case SOCK_UDP_SERVER:
		m_uMode = mode;
		m_szServer = NULL;
		osal_task_create(taskForUdpServer, TASK_NET_TIME);
		DBG(TRACE("start udp service\r\n"));
		break;
	case SOCK_TCP_CLIENT:
		m_uMode = mode;
		m_szServer = server;
		osal_task_create(taskForTcpClient, TASK_NET_TIME);
		DBG(TRACE("start tcp client\r\n"));
		break;
	case SOCK_UDP_CLIENT:
		m_uMode = mode;
		m_szServer = server;
		//osal_task_create(taskForUdpClient, TASK_NET_TIME);
		osal_task_create(taskForUdpServer, TASK_NET_TIME);
		DBG(TRACE("start udp client\r\n"));
		break;
	default:
		DBG(TRACE("unkown net service mode: %d\r\n", mode));
		return -1;
	}
	return 0;
}

/**
 * @brief W5500 停止服务
 * @param 
 * @retval 初始化成功返回0, 否则返回-1
 */
int vizchip_net_stop(void)
{
	switch (m_uMode)
	{
	case SOCK_TCP_SERVER:
		osal_task_kill(taskForTcpServer);
		break;
	case SOCK_UDP_SERVER:
		osal_task_kill(taskForUdpServer);
		break;
	case SOCK_TCP_CLIENT:
		osal_task_kill(taskForTcpClient);
		break;
	case SOCK_UDP_CLIENT:
		osal_task_kill(taskForUdpClient);
		break;
	default:
		break;
	}
	return 0;
}

/**
 * @brief: 获取指定端口串口通讯句柄
 * @param 
 * @retval: 指定端口串口通讯句柄
 */
HALUartTypeDef* vizchip_net_getinstance(void)
{
	if (NULL == pthis)
	{
		New();
	}
	
	return pthis;
}

#endif //CFG_USE_NET
