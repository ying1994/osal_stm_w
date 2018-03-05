
#include <stddef.h>
#include "wizchip_net.h"

#include "hal_board.h"
#include "osal.h"
#include "Ethernet/socket.h"

#if (defined(CFG_USE_NET) && defined(CFG_WIZCHIP))
//! CS
#define WIZCHIP_CS_ON	HalGpioWriteBit(WIZCHIP_CS_GPIO_TYPE, WIZCHIP_CS_GPIO_PIN, 1)
#define WIZCHIP_CS_OFF	HalGpioWriteBit(WIZCHIP_CS_GPIO_TYPE, WIZCHIP_CS_GPIO_PIN, 0)

static HALSpiTypeDef *m_hspi = NULL;
//static wiz_NetInfo m_WIZNETINFO;

static HALUartTypeDef m_hInstance[WIZCHIP_NET_SIZE];
static HALUartTypeDef* pthis[WIZCHIP_NET_SIZE] = {NULL};
static HalUartCBack_t m_hrxobser[WIZCHIP_NET_SIZE] = {NULL};

static UINT8 m_uMode[WIZCHIP_NET_SIZE] = {0};
static UINT8 m_aDataBuffer[WIZCHIP_NET_SIZE][SOCKET_DATA_BUF_SIZE] = {0};
static UINT16 m_uRecvSize[WIZCHIP_NET_SIZE] = {0};
static UCHAR *m_szServer[WIZCHIP_NET_SIZE] = {NULL};
static UINT16 m_uPort[WIZCHIP_NET_SIZE] = {0};

// For UDP Server
static UINT8 *m_szDestIp[WIZCHIP_NET_SIZE][4];
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
static void add_rx_obser0(HalUartCBack_t hrxobser)
{
	m_hrxobser[WIZCHIP_NET_CH0] = hrxobser;
}

/**
 * @brief: 注册串口接收观察者
 * @param hrxobser: 串口接收观察者句柄
 * @retval: void
 */
static void add_rx_obser1(HalUartCBack_t hrxobser)
{
	m_hrxobser[WIZCHIP_NET_CH1] = hrxobser;
}

/**
 * @brief: 注册串口接收观察者
 * @param hrxobser: 串口接收观察者句柄
 * @retval: void
 */
static void add_rx_obser2(HalUartCBack_t hrxobser)
{
	m_hrxobser[WIZCHIP_NET_CH2] = hrxobser;
}

/**
 * @brief: 注册串口接收观察者
 * @param hrxobser: 串口接收观察者句柄
 * @retval: void
 */
static void add_rx_obser3(HalUartCBack_t hrxobser)
{
	m_hrxobser[WIZCHIP_NET_CH3] = hrxobser;
}

#if _WIZCHIP_ > 5100
/**
 * @brief: 注册串口接收观察者
 * @param hrxobser: 串口接收观察者句柄
 * @retval: void
 */
static void add_rx_obser4(HalUartCBack_t hrxobser)
{
	m_hrxobser[WIZCHIP_NET_CH4] = hrxobser;
}

/**
 * @brief: 注册串口接收观察者
 * @param hrxobser: 串口接收观察者句柄
 * @retval: void
 */
static void add_rx_obser5(HalUartCBack_t hrxobser)
{
	m_hrxobser[WIZCHIP_NET_CH5] = hrxobser;
}

/**
 * @brief: 注册串口接收观察者
 * @param hrxobser: 串口接收观察者句柄
 * @retval: void
 */
static void add_rx_obser6(HalUartCBack_t hrxobser)
{
	m_hrxobser[WIZCHIP_NET_CH6] = hrxobser;
}

/**
 * @brief: 注册串口接收观察者
 * @param hrxobser: 串口接收观察者句柄
 * @retval: void
 */
static void add_rx_obser7(HalUartCBack_t hrxobser)
{
	m_hrxobser[WIZCHIP_NET_CH7] = hrxobser;
}
#endif //# _WIZCHIP_ > 5100

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

static UINT16 read0(UCHAR *pdata, UINT16 len)
{
	if (m_uRecvSize[WIZCHIP_NET_CH0] > 0)
	{
		if (len > m_uRecvSize[WIZCHIP_NET_CH0])
			len = m_uRecvSize[WIZCHIP_NET_CH0];
		memcpy(pdata, m_aDataBuffer[WIZCHIP_NET_CH0], len);
		m_uRecvSize[WIZCHIP_NET_CH0] = 0;
		
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
static void write0(UCHAR *pdata, UINT16 len)
{
	UINT8 *buf = pdata;
	UINT16 size = len;
	UINT16 sentsize=0;
	INT32 ret;
	switch (m_uMode[WIZCHIP_NET_CH0])
	{
	case SOCK_TCP_SERVER:
		sentsize = 0;
		while(size != sentsize)
		{
		   ret = send(WIZCHIP_NET_CH0, buf+sentsize, size-sentsize);
		   if(ret < 0)
		   {
			  close(0);
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_UDP_SERVER:
		sentsize = 0;
		while(sentsize != size)
		{
		   ret = sendto(WIZCHIP_NET_CH0, buf+sentsize, size-sentsize, (uint8_t*)m_szDestIp[WIZCHIP_NET_CH0], m_uDestport);
		   if(ret < 0)
		   {
			  //DBG(TRACE("%d: sendto error. %ld\r\n", 0, ret));
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_TCP_CLIENT:
		sentsize = 0;
		while(size != sentsize)
		{
		   ret = send(WIZCHIP_NET_CH0, buf+sentsize, size-sentsize);
		   if(ret < 0)
		   {
			  close(0);
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_UDP_CLIENT:
		sentsize = 0;
		while(sentsize != size)
		{
		   ret = sendto(WIZCHIP_NET_CH0, buf+sentsize, size-sentsize, (uint8_t*)m_szDestIp[WIZCHIP_NET_CH0], m_uDestport);
		   if(ret < 0)
		   {
			  //DBG(TRACE("%d: sendto error. %ld\r\n", 0, ret));
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	default:
		break;
	}
}

static UINT16 read1(UCHAR *pdata, UINT16 len)
{
	if (m_uRecvSize[WIZCHIP_NET_CH1] > 0)
	{
		if (len > m_uRecvSize[WIZCHIP_NET_CH1])
			len = m_uRecvSize[WIZCHIP_NET_CH1];
		memcpy(pdata, m_aDataBuffer[WIZCHIP_NET_CH1], len);
		m_uRecvSize[WIZCHIP_NET_CH1] = 0;
		
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
static void write1(UCHAR *pdata, UINT16 len)
{
	UINT8 *buf = pdata;
	UINT16 size = len;
	UINT16 sentsize=0;
	INT32 ret;
	switch (m_uMode[WIZCHIP_NET_CH1])
	{
	case SOCK_TCP_SERVER:
		sentsize = 0;
		while(size != sentsize)
		{
		   ret = send(WIZCHIP_NET_CH1, buf+sentsize, size-sentsize);
		   if(ret < 0)
		   {
			  close(0);
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_UDP_SERVER:
		sentsize = 0;
		while(sentsize != size)
		{
		   ret = sendto(WIZCHIP_NET_CH1, buf+sentsize, size-sentsize, (uint8_t*)m_szDestIp[WIZCHIP_NET_CH1], m_uDestport);
		   if(ret < 0)
		   {
			  //DBG(TRACE("%d: sendto error. %ld\r\n", 0, ret));
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_TCP_CLIENT:
		sentsize = 0;
		while(size != sentsize)
		{
		   ret = send(WIZCHIP_NET_CH1, buf+sentsize, size-sentsize);
		   if(ret < 0)
		   {
			  close(0);
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_UDP_CLIENT:
		sentsize = 0;
		while(sentsize != size)
		{
		   ret = sendto(WIZCHIP_NET_CH1, buf+sentsize, size-sentsize, (uint8_t*)m_szDestIp[WIZCHIP_NET_CH1], m_uDestport);
		   if(ret < 0)
		   {
			  //DBG(TRACE("%d: sendto error. %ld\r\n", 0, ret));
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	default:
		break;
	}
}

static UINT16 read2(UCHAR *pdata, UINT16 len)
{
	if (m_uRecvSize[WIZCHIP_NET_CH2] > 0)
	{
		if (len > m_uRecvSize[WIZCHIP_NET_CH2])
			len = m_uRecvSize[WIZCHIP_NET_CH2];
		memcpy(pdata, m_aDataBuffer[WIZCHIP_NET_CH2], len);
		m_uRecvSize[WIZCHIP_NET_CH2] = 0;
		
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
static void write2(UCHAR *pdata, UINT16 len)
{
	UINT8 *buf = pdata;
	UINT16 size = len;
	UINT16 sentsize=0;
	INT32 ret;
	switch (m_uMode[WIZCHIP_NET_CH2])
	{
	case SOCK_TCP_SERVER:
		sentsize = 0;
		while(size != sentsize)
		{
		   ret = send(WIZCHIP_NET_CH2, buf+sentsize, size-sentsize);
		   if(ret < 0)
		   {
			  close(0);
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_UDP_SERVER:
		sentsize = 0;
		while(sentsize != size)
		{
		   ret = sendto(WIZCHIP_NET_CH2, buf+sentsize, size-sentsize, (uint8_t*)m_szDestIp[WIZCHIP_NET_CH2], m_uDestport);
		   if(ret < 0)
		   {
			  //DBG(TRACE("%d: sendto error. %ld\r\n", 0, ret));
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_TCP_CLIENT:
		sentsize = 0;
		while(size != sentsize)
		{
		   ret = send(WIZCHIP_NET_CH2, buf+sentsize, size-sentsize);
		   if(ret < 0)
		   {
			  close(0);
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_UDP_CLIENT:
		sentsize = 0;
		while(sentsize != size)
		{
		   ret = sendto(WIZCHIP_NET_CH2, buf+sentsize, size-sentsize, (uint8_t*)m_szDestIp[WIZCHIP_NET_CH2], m_uDestport);
		   if(ret < 0)
		   {
			  //DBG(TRACE("%d: sendto error. %ld\r\n", 0, ret));
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	default:
		break;
	}
}

static UINT16 read3(UCHAR *pdata, UINT16 len)
{
	if (m_uRecvSize[WIZCHIP_NET_CH3] > 0)
	{
		if (len > m_uRecvSize[WIZCHIP_NET_CH3])
			len = m_uRecvSize[WIZCHIP_NET_CH3];
		memcpy(pdata, m_aDataBuffer[WIZCHIP_NET_CH3], len);
		m_uRecvSize[WIZCHIP_NET_CH3] = 0;
		
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
static void write3(UCHAR *pdata, UINT16 len)
{
	UINT8 *buf = pdata;
	UINT16 size = len;
	UINT16 sentsize=0;
	INT32 ret;
	switch (m_uMode[WIZCHIP_NET_CH3])
	{
	case SOCK_TCP_SERVER:
		sentsize = 0;
		while(size != sentsize)
		{
		   ret = send(WIZCHIP_NET_CH3, buf+sentsize, size-sentsize);
		   if(ret < 0)
		   {
			  close(0);
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_UDP_SERVER:
		sentsize = 0;
		while(sentsize != size)
		{
		   ret = sendto(WIZCHIP_NET_CH3, buf+sentsize, size-sentsize, (uint8_t*)m_szDestIp[WIZCHIP_NET_CH3], m_uDestport);
		   if(ret < 0)
		   {
			  //DBG(TRACE("%d: sendto error. %ld\r\n", 0, ret));
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_TCP_CLIENT:
		sentsize = 0;
		while(size != sentsize)
		{
		   ret = send(WIZCHIP_NET_CH3, buf+sentsize, size-sentsize);
		   if(ret < 0)
		   {
			  close(0);
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_UDP_CLIENT:
		sentsize = 0;
		while(sentsize != size)
		{
		   ret = sendto(WIZCHIP_NET_CH3, buf+sentsize, size-sentsize, (uint8_t*)m_szDestIp[WIZCHIP_NET_CH3], m_uDestport);
		   if(ret < 0)
		   {
			  //DBG(TRACE("%d: sendto error. %ld\r\n", 0, ret));
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	default:
		break;
	}
}

#if _WIZCHIP_ > 5100

static UINT16 read4(UCHAR *pdata, UINT16 len)
{
	if (m_uRecvSize[WIZCHIP_NET_CH4] > 0)
	{
		if (len > m_uRecvSize[WIZCHIP_NET_CH4])
			len = m_uRecvSize[WIZCHIP_NET_CH4];
		memcpy(pdata, m_aDataBuffer[WIZCHIP_NET_CH4], len);
		m_uRecvSize[WIZCHIP_NET_CH4] = 0;
		
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
static void write4(UCHAR *pdata, UINT16 len)
{
	UINT8 *buf = pdata;
	UINT16 size = len;
	UINT16 sentsize=0;
	INT32 ret;
	switch (m_uMode[WIZCHIP_NET_CH4])
	{
	case SOCK_TCP_SERVER:
		sentsize = 0;
		while(size != sentsize)
		{
		   ret = send(WIZCHIP_NET_CH4, buf+sentsize, size-sentsize);
		   if(ret < 0)
		   {
			  close(0);
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_UDP_SERVER:
		sentsize = 0;
		while(sentsize != size)
		{
		   ret = sendto(WIZCHIP_NET_CH4, buf+sentsize, size-sentsize, (uint8_t*)m_szDestIp[WIZCHIP_NET_CH4], m_uDestport);
		   if(ret < 0)
		   {
			  //DBG(TRACE("%d: sendto error. %ld\r\n", 0, ret));
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_TCP_CLIENT:
		sentsize = 0;
		while(size != sentsize)
		{
		   ret = send(WIZCHIP_NET_CH4, buf+sentsize, size-sentsize);
		   if(ret < 0)
		   {
			  close(0);
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_UDP_CLIENT:
		sentsize = 0;
		while(sentsize != size)
		{
		   ret = sendto(WIZCHIP_NET_CH4, buf+sentsize, size-sentsize, (uint8_t*)m_szDestIp[WIZCHIP_NET_CH4], m_uDestport);
		   if(ret < 0)
		   {
			  //DBG(TRACE("%d: sendto error. %ld\r\n", 0, ret));
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	default:
		break;
	}
}

static UINT16 read5(UCHAR *pdata, UINT16 len)
{
	if (m_uRecvSize[WIZCHIP_NET_CH5] > 0)
	{
		if (len > m_uRecvSize[WIZCHIP_NET_CH5])
			len = m_uRecvSize[WIZCHIP_NET_CH5];
		memcpy(pdata, m_aDataBuffer[WIZCHIP_NET_CH5], len);
		m_uRecvSize[WIZCHIP_NET_CH5] = 0;
		
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
static void write5(UCHAR *pdata, UINT16 len)
{
	UINT8 *buf = pdata;
	UINT16 size = len;
	UINT16 sentsize=0;
	INT32 ret;
	switch (m_uMode[WIZCHIP_NET_CH5])
	{
	case SOCK_TCP_SERVER:
		sentsize = 0;
		while(size != sentsize)
		{
		   ret = send(WIZCHIP_NET_CH5, buf+sentsize, size-sentsize);
		   if(ret < 0)
		   {
			  close(0);
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_UDP_SERVER:
		sentsize = 0;
		while(sentsize != size)
		{
		   ret = sendto(WIZCHIP_NET_CH5, buf+sentsize, size-sentsize, (uint8_t*)m_szDestIp[WIZCHIP_NET_CH5], m_uDestport);
		   if(ret < 0)
		   {
			  //DBG(TRACE("%d: sendto error. %ld\r\n", 0, ret));
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_TCP_CLIENT:
		sentsize = 0;
		while(size != sentsize)
		{
		   ret = send(WIZCHIP_NET_CH5, buf+sentsize, size-sentsize);
		   if(ret < 0)
		   {
			  close(0);
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_UDP_CLIENT:
		sentsize = 0;
		while(sentsize != size)
		{
		   ret = sendto(WIZCHIP_NET_CH5, buf+sentsize, size-sentsize, (uint8_t*)m_szDestIp[WIZCHIP_NET_CH5], m_uDestport);
		   if(ret < 0)
		   {
			  //DBG(TRACE("%d: sendto error. %ld\r\n", 0, ret));
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	default:
		break;
	}
}

static UINT16 read6(UCHAR *pdata, UINT16 len)
{
	if (m_uRecvSize[WIZCHIP_NET_CH6] > 0)
	{
		if (len > m_uRecvSize[WIZCHIP_NET_CH6])
			len = m_uRecvSize[WIZCHIP_NET_CH6];
		memcpy(pdata, m_aDataBuffer[WIZCHIP_NET_CH6], len);
		m_uRecvSize[WIZCHIP_NET_CH6] = 0;
		
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
static void write6(UCHAR *pdata, UINT16 len)
{
	UINT8 *buf = pdata;
	UINT16 size = len;
	UINT16 sentsize=0;
	INT32 ret;
	switch (m_uMode[WIZCHIP_NET_CH6])
	{
	case SOCK_TCP_SERVER:
		sentsize = 0;
		while(size != sentsize)
		{
		   ret = send(WIZCHIP_NET_CH6, buf+sentsize, size-sentsize);
		   if(ret < 0)
		   {
			  close(0);
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_UDP_SERVER:
		sentsize = 0;
		while(sentsize != size)
		{
		   ret = sendto(WIZCHIP_NET_CH6, buf+sentsize, size-sentsize, (uint8_t*)m_szDestIp[WIZCHIP_NET_CH6], m_uDestport);
		   if(ret < 0)
		   {
			  //DBG(TRACE("%d: sendto error. %ld\r\n", 0, ret));
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_TCP_CLIENT:
		sentsize = 0;
		while(size != sentsize)
		{
		   ret = send(WIZCHIP_NET_CH6, buf+sentsize, size-sentsize);
		   if(ret < 0)
		   {
			  close(0);
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_UDP_CLIENT:
		sentsize = 0;
		while(sentsize != size)
		{
		   ret = sendto(WIZCHIP_NET_CH6, buf+sentsize, size-sentsize, (uint8_t*)m_szDestIp[WIZCHIP_NET_CH6], m_uDestport);
		   if(ret < 0)
		   {
			  //DBG(TRACE("%d: sendto error. %ld\r\n", 0, ret));
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	default:
		break;
	}
}

static UINT16 read7(UCHAR *pdata, UINT16 len)
{
	if (m_uRecvSize[WIZCHIP_NET_CH7] > 0)
	{
		if (len > m_uRecvSize[WIZCHIP_NET_CH7])
			len = m_uRecvSize[WIZCHIP_NET_CH7];
		memcpy(pdata, m_aDataBuffer[WIZCHIP_NET_CH7], len);
		m_uRecvSize[WIZCHIP_NET_CH7] = 0;
		
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
static void write7(UCHAR *pdata, UINT16 len)
{
	UINT8 *buf = pdata;
	UINT16 size = len;
	UINT16 sentsize=0;
	INT32 ret;
	switch (m_uMode[WIZCHIP_NET_CH7])
	{
	case SOCK_TCP_SERVER:
		sentsize = 0;
		while(size != sentsize)
		{
		   ret = send(WIZCHIP_NET_CH7, buf+sentsize, size-sentsize);
		   if(ret < 0)
		   {
			  close(0);
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_UDP_SERVER:
		sentsize = 0;
		while(sentsize != size)
		{
		   ret = sendto(WIZCHIP_NET_CH7, buf+sentsize, size-sentsize, (uint8_t*)m_szDestIp[WIZCHIP_NET_CH7], m_uDestport);
		   if(ret < 0)
		   {
			  //DBG(TRACE("%d: sendto error. %ld\r\n", 0, ret));
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_TCP_CLIENT:
		sentsize = 0;
		while(size != sentsize)
		{
		   ret = send(WIZCHIP_NET_CH7, buf+sentsize, size-sentsize);
		   if(ret < 0)
		   {
			  close(0);
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	case SOCK_UDP_CLIENT:
		sentsize = 0;
		while(sentsize != size)
		{
		   ret = sendto(WIZCHIP_NET_CH7, buf+sentsize, size-sentsize, (uint8_t*)m_szDestIp[WIZCHIP_NET_CH7], m_uDestport);
		   if(ret < 0)
		   {
			  //DBG(TRACE("%d: sendto error. %ld\r\n", 0, ret));
			  return;
		   }
		   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
		}
		break;
	default:
		break;
	}
}
#endif // _WIZCHIP_ > 5100

/**
 * @brief: 创建端口串口通讯对象
 * @param sn: 串口端口号
 * @retval None
 */
static void New(UINT8 sn)
{
	if (sn >= WIZCHIP_NET_SIZE)
		return;
	
	if (NULL == pthis[sn])
	{
		switch (sn)
		{
		case WIZCHIP_NET_CH0:
			m_hInstance[sn].add_rx_obser = add_rx_obser0;
			m_hInstance[sn].init = init;
			m_hInstance[sn].deInit = deInit;
			m_hInstance[sn].set_baudrate = set_baudrate;
			m_hInstance[sn].set_databits = set_wordlength;
			m_hInstance[sn].set_stopbit = set_stopbit;
			m_hInstance[sn].set_parity = set_parity;
			m_hInstance[sn].read = read0;
			m_hInstance[sn].write = write0;
			pthis[sn] = &m_hInstance[sn];
			//pthis->init();
		break;
		case WIZCHIP_NET_CH1:
			m_hInstance[sn].add_rx_obser = add_rx_obser1;
			m_hInstance[sn].init = init;
			m_hInstance[sn].deInit = deInit;
			m_hInstance[sn].set_baudrate = set_baudrate;
			m_hInstance[sn].set_databits = set_wordlength;
			m_hInstance[sn].set_stopbit = set_stopbit;
			m_hInstance[sn].set_parity = set_parity;
			m_hInstance[sn].read = read1;
			m_hInstance[sn].write = write1;
			pthis[sn] = &m_hInstance[sn];
			//pthis->init();
		break;
		case WIZCHIP_NET_CH2:
			m_hInstance[sn].add_rx_obser = add_rx_obser2;
			m_hInstance[sn].init = init;
			m_hInstance[sn].deInit = deInit;
			m_hInstance[sn].set_baudrate = set_baudrate;
			m_hInstance[sn].set_databits = set_wordlength;
			m_hInstance[sn].set_stopbit = set_stopbit;
			m_hInstance[sn].set_parity = set_parity;
			m_hInstance[sn].read = read2;
			m_hInstance[sn].write = write2;
			pthis[sn] = &m_hInstance[sn];
			//pthis->init();
		break;
		case WIZCHIP_NET_CH3:
			m_hInstance[sn].add_rx_obser = add_rx_obser3;
			m_hInstance[sn].init = init;
			m_hInstance[sn].deInit = deInit;
			m_hInstance[sn].set_baudrate = set_baudrate;
			m_hInstance[sn].set_databits = set_wordlength;
			m_hInstance[sn].set_stopbit = set_stopbit;
			m_hInstance[sn].set_parity = set_parity;
			m_hInstance[sn].read = read3;
			m_hInstance[sn].write = write3;
			pthis[sn] = &m_hInstance[sn];
			//pthis->init();
		break;
#if _WIZCHIP_ > 5100
		case WIZCHIP_NET_CH4:
			m_hInstance[sn].add_rx_obser = add_rx_obser4;
			m_hInstance[sn].init = init;
			m_hInstance[sn].deInit = deInit;
			m_hInstance[sn].set_baudrate = set_baudrate;
			m_hInstance[sn].set_databits = set_wordlength;
			m_hInstance[sn].set_stopbit = set_stopbit;
			m_hInstance[sn].set_parity = set_parity;
			m_hInstance[sn].read = read4;
			m_hInstance[sn].write = write4;
			pthis[sn] = &m_hInstance[sn];
			//pthis->init();
		break;
		case WIZCHIP_NET_CH5:
			m_hInstance[sn].add_rx_obser = add_rx_obser5;
			m_hInstance[sn].init = init;
			m_hInstance[sn].deInit = deInit;
			m_hInstance[sn].set_baudrate = set_baudrate;
			m_hInstance[sn].set_databits = set_wordlength;
			m_hInstance[sn].set_stopbit = set_stopbit;
			m_hInstance[sn].set_parity = set_parity;
			m_hInstance[sn].read = read5;
			m_hInstance[sn].write = write5;
			pthis[sn] = &m_hInstance[sn];
			//pthis->init();
		break;
		case WIZCHIP_NET_CH6:
			m_hInstance[sn].add_rx_obser = add_rx_obser6;
			m_hInstance[sn].init = init;
			m_hInstance[sn].deInit = deInit;
			m_hInstance[sn].set_baudrate = set_baudrate;
			m_hInstance[sn].set_databits = set_wordlength;
			m_hInstance[sn].set_stopbit = set_stopbit;
			m_hInstance[sn].set_parity = set_parity;
			m_hInstance[sn].read = read6;
			m_hInstance[sn].write = write6;
			pthis[sn] = &m_hInstance[sn];
			//pthis->init();
		break;
		case WIZCHIP_NET_CH7:
			m_hInstance[sn].add_rx_obser = add_rx_obser7;
			m_hInstance[sn].init = init;
			m_hInstance[sn].deInit = deInit;
			m_hInstance[sn].set_baudrate = set_baudrate;
			m_hInstance[sn].set_databits = set_wordlength;
			m_hInstance[sn].set_stopbit = set_stopbit;
			m_hInstance[sn].set_parity = set_parity;
			m_hInstance[sn].read = read7;
			m_hInstance[sn].write = write7;
			pthis[sn] = &m_hInstance[sn];
			//pthis->init();
		break;
#endif
		}
	}
}

/**
 * @brief TCP Server
 */
static void taskForTcpServer0(void)
{
	UINT8 *buf = m_aDataBuffer[WIZCHIP_NET_CH0];
	UINT16 size = 0;
	//UINT16 sentsize=0;
	INT32 ret;
	UINT8 sn_sr = getSn_SR(WIZCHIP_NET_CH0);
   switch(sn_sr)
   {
      case SOCK_ESTABLISHED :
         if(getSn_IR(WIZCHIP_NET_CH0) & Sn_IR_CON)
         {
            DBG(TRACE("%d:Connected\r\n",WIZCHIP_NET_CH0));
            setSn_IR(WIZCHIP_NET_CH0,Sn_IR_CON);
         }
         if((size = getSn_RX_RSR(WIZCHIP_NET_CH0)) > 0)
         {
            if(size > SOCKET_DATA_BUF_SIZE)
				size = SOCKET_DATA_BUF_SIZE;
            ret = recv(WIZCHIP_NET_CH0,buf,size);
            if(ret <= 0)
				return;
			m_uRecvSize[WIZCHIP_NET_CH0] = ret;
			if (m_hrxobser[WIZCHIP_NET_CH0] != NULL)
				m_hrxobser[WIZCHIP_NET_CH0](1);
			////////////////////// 回环测试 ///////////////////////
            //sentsize = 0;
            //while(size != sentsize)
            //{
            //   ret = send(WIZCHIP_NET_CH0,buf+sentsize,size-sentsize);
            //   if(ret < 0)
            //   {
            //      close(WIZCHIP_NET_CH0);
            //      return;
            //   }
            //   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            //}
         }
         break;
      case SOCK_CLOSE_WAIT :
         //DBG(TRACE("%d:CloseWait\r\n",WIZCHIP_NET_CH0));
         if((ret=disconnect(WIZCHIP_NET_CH0)) != SOCK_OK)
		 {
			DBG(TRACE("%d:CloseWait Field\r\n",WIZCHIP_NET_CH0));
			 return;
		 }
         DBG(TRACE("%d:Closed\r\n",WIZCHIP_NET_CH0));
         break;
      case SOCK_INIT :
    	  //DBG(TRACE("%d:Listen, port [%d]\r\n",WIZCHIP_NET_CH0, m_uPort[WIZCHIP_NET_CH0]));
         if( (ret = listen(WIZCHIP_NET_CH0)) != SOCK_OK)
		 {
			 DBG(TRACE("%d:Listen, port [%d] Field\r\n",WIZCHIP_NET_CH0, m_uPort[WIZCHIP_NET_CH0]));
			 return;
		 }
         break;
      case SOCK_CLOSED:
         //DBG(TRACE("%d:LBTStart\r\n",WIZCHIP_NET_CH0));
         if((ret=socket(WIZCHIP_NET_CH0,Sn_MR_TCP,m_uPort[WIZCHIP_NET_CH0],0x00)) != WIZCHIP_NET_CH0)
		 {
			DBG(TRACE("%d:LBTStart Field\r\n",WIZCHIP_NET_CH0));
            return;
		 }
         //DBG(TRACE("%d:Opened\r\n",WIZCHIP_NET_CH0));
         break;
      default:
		  //DBG(TRACE("sn_sr:%d\r\n",sn_sr));
         break;
   }
}

/**
 * @brief UDP Server
 */
static void taskForUdpServer0(void)
{
	UINT8 *buf = m_aDataBuffer[WIZCHIP_NET_CH0];
	UINT16 size = 0;
	//UINT16 sentsize = 0;
	INT32  ret;
   //uint8_t  packinfo = 0;
	UINT8 sn_sr = getSn_SR(WIZCHIP_NET_CH0);
   switch(sn_sr)
   {
      case SOCK_UDP :
         if((size = getSn_RX_RSR(WIZCHIP_NET_CH0)) > 0)
         {
            if(size > SOCKET_DATA_BUF_SIZE)
				size = SOCKET_DATA_BUF_SIZE;
            ret = recvfrom(WIZCHIP_NET_CH0,buf,size,(uint8_t*)m_szDestIp[WIZCHIP_NET_CH0],(uint16_t*)&m_uDestport);
            if(ret <= 0)
            {
               DBG(TRACE("%d: recvfrom error. %ld\r\n",WIZCHIP_NET_CH0,ret));
               return;
            }
			m_uRecvSize[WIZCHIP_NET_CH0] = ret;
			if (m_hrxobser[WIZCHIP_NET_CH0] != NULL)
				m_hrxobser[WIZCHIP_NET_CH0](1);
			////////////////////// 回环测试 ///////////////////////
            //size = (uint16_t) ret;
            //sentsize = 0;
            //while(sentsize != size)
            //{
            //   ret = sendto(WIZCHIP_NET_CH0,buf+sentsize,size-sentsize,(uint8_t*)m_szDestIp[WIZCHIP_NET_CH0],m_uDestport);
            //   if(ret < 0)
            //   {
            //      DBG(TRACE("%d: sendto error. %ld\r\n",WIZCHIP_NET_CH0,ret));
            //      return;
            //   }
            //   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            //}
         }
         break;
      case SOCK_CLOSED:
         //DBG(TRACE("%d:LBUStart\r\n",WIZCHIP_NET_CH0));
         if((ret=socket(WIZCHIP_NET_CH0,Sn_MR_UDP,m_uPort[WIZCHIP_NET_CH0],0x00)) != WIZCHIP_NET_CH0)
		 {
			DBG(TRACE("%d:Opened, port [%d] Field\r\n",WIZCHIP_NET_CH0, m_uPort[WIZCHIP_NET_CH0]));
            return;
		 }
         //DBG(TRACE("%d:Opened, port [%d]\r\n",WIZCHIP_NET_CH0, m_uPort[WIZCHIP_NET_CH0]));
         break;
      default :
		  //DBG(TRACE("sn_sr:%d\r\n",sn_sr));
         break;
   }
}

/**
 * @brief TCP Server
 */
static void taskForTcpClient0(void)
{
	UINT16 anyport=20000;
	UINT8 *buf = m_aDataBuffer[WIZCHIP_NET_CH0];
	UINT16 size = 0;
	//UINT16 sentsize=0;
	INT32 ret;
	UINT8 sn_sr = getSn_SR(WIZCHIP_NET_CH0);
	
	if (m_szServer[WIZCHIP_NET_CH0] != NULL);
	{
	   switch(sn_sr)
	   {
		  case SOCK_ESTABLISHED :
			 if(getSn_IR(WIZCHIP_NET_CH0) & Sn_IR_CON)
			 {
				DBG(TRACE("%d:Connected\r\n",WIZCHIP_NET_CH0));
				setSn_IR(WIZCHIP_NET_CH0,Sn_IR_CON);
			 }
			 if((size = getSn_RX_RSR(WIZCHIP_NET_CH0)) > 0)
			 {
				if(size > SOCKET_DATA_BUF_SIZE)
					size = SOCKET_DATA_BUF_SIZE;
				ret = recv(WIZCHIP_NET_CH0,buf,size);
				if(ret <= 0)
					return;
				m_uRecvSize[WIZCHIP_NET_CH0] = ret;
				if (m_hrxobser[WIZCHIP_NET_CH0] != NULL)
					m_hrxobser[WIZCHIP_NET_CH0](1);
				////////////////////// 回环测试 ///////////////////////
				//sentsize = 0;
				//while(size != sentsize)
				//{
				//   ret = send(WIZCHIP_NET_CH0,buf+sentsize,size-sentsize);
				//   if(ret < 0)
				//   {
				//      close(WIZCHIP_NET_CH0);
				//      return;
				//   }
				//   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
				//}
			 }
			 break;
		  case SOCK_CLOSE_WAIT :
			 //DBG(TRACE("%d:CloseWait\r\n",WIZCHIP_NET_CH0));
			 if((ret=disconnect(WIZCHIP_NET_CH0)) != SOCK_OK)
			 {
				DBG(TRACE("%d:CloseWait Field\r\n",WIZCHIP_NET_CH0));
				 return;
			 }
			 //DBG(TRACE("%d:Closed\r\n",WIZCHIP_NET_CH0));
			 break;
		  case SOCK_INIT :
			 //DBG(TRACE("%d:connet[%d.%d.%d.%d], port [%d]\r\n",WIZCHIP_NET_CH0, m_szServer[WIZCHIP_NET_CH0][0], m_szServer[WIZCHIP_NET_CH0][1], m_szServer[WIZCHIP_NET_CH0][2], m_szServer[WIZCHIP_NET_CH0][3], m_uPort[WIZCHIP_NET_CH0]));
			 if( (ret = connect(WIZCHIP_NET_CH0, m_szServer[WIZCHIP_NET_CH0], m_uPort[WIZCHIP_NET_CH0])) != SOCK_OK)
			 {
				 DBG(TRACE("%d:connet[%d.%d.%d.%d], port [%d] Field\r\n",WIZCHIP_NET_CH0, m_szServer[WIZCHIP_NET_CH0][0], m_szServer[WIZCHIP_NET_CH0][1], m_szServer[WIZCHIP_NET_CH0][2], m_szServer[WIZCHIP_NET_CH0][3],  m_uPort));
				 return;
			 }
			 break;
		  case SOCK_CLOSED:
			 //DBG(TRACE("%d:LBTStart\r\n",WIZCHIP_NET_CH0));
			 if((ret=socket(WIZCHIP_NET_CH0,Sn_MR_TCP,anyport++,Sn_MR_ND)) != WIZCHIP_NET_CH0)
			 {
				DBG(TRACE("%d:LBTStart Field\r\n",WIZCHIP_NET_CH0));
				return;
			 }
			 //DBG(TRACE("%d:Opened\r\n",WIZCHIP_NET_CH0));
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
static void taskForUdpClient0(void)
{
}

/**
 * @brief TCP Server
 */
static void taskForTcpServer1(void)
{
	UINT8 *buf = m_aDataBuffer[WIZCHIP_NET_CH1];
	UINT16 size = 0;
	//UINT16 sentsize=0;
	INT32 ret;
	UINT8 sn_sr = getSn_SR(WIZCHIP_NET_CH1);
   switch(sn_sr)
   {
      case SOCK_ESTABLISHED :
         if(getSn_IR(WIZCHIP_NET_CH1) & Sn_IR_CON)
         {
            DBG(TRACE("%d:Connected\r\n",WIZCHIP_NET_CH1));
            setSn_IR(WIZCHIP_NET_CH1,Sn_IR_CON);
         }
         if((size = getSn_RX_RSR(WIZCHIP_NET_CH1)) > 0)
         {
            if(size > SOCKET_DATA_BUF_SIZE)
				size = SOCKET_DATA_BUF_SIZE;
            ret = recv(WIZCHIP_NET_CH1,buf,size);
            if(ret <= 0)
				return;
			m_uRecvSize[WIZCHIP_NET_CH1] = ret;
			if (m_hrxobser[WIZCHIP_NET_CH1] != NULL)
				m_hrxobser[WIZCHIP_NET_CH1](1);
			////////////////////// 回环测试 ///////////////////////
            //sentsize = 0;
            //while(size != sentsize)
            //{
            //   ret = send(WIZCHIP_NET_CH1,buf+sentsize,size-sentsize);
            //   if(ret < 0)
            //   {
            //      close(WIZCHIP_NET_CH1);
            //      return;
            //   }
            //   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            //}
         }
         break;
      case SOCK_CLOSE_WAIT :
         //DBG(TRACE("%d:CloseWait\r\n",WIZCHIP_NET_CH1));
         if((ret=disconnect(WIZCHIP_NET_CH1)) != SOCK_OK)
		 {
			DBG(TRACE("%d:CloseWait Field\r\n",WIZCHIP_NET_CH1));
			 return;
		 }
         DBG(TRACE("%d:Closed\r\n",WIZCHIP_NET_CH1));
         break;
      case SOCK_INIT :
    	  //DBG(TRACE("%d:Listen, port [%d]\r\n",WIZCHIP_NET_CH1, m_uPort[WIZCHIP_NET_CH1]));
         if( (ret = listen(WIZCHIP_NET_CH1)) != SOCK_OK)
		 {
			 DBG(TRACE("%d:Listen, port [%d] Field\r\n",WIZCHIP_NET_CH1, m_uPort[WIZCHIP_NET_CH1]));
			 return;
		 }
         break;
      case SOCK_CLOSED:
         //DBG(TRACE("%d:LBTStart\r\n",WIZCHIP_NET_CH1));
         if((ret=socket(WIZCHIP_NET_CH1,Sn_MR_TCP,m_uPort[WIZCHIP_NET_CH1],0x00)) != WIZCHIP_NET_CH1)
		 {
			DBG(TRACE("%d:LBTStart Field\r\n",WIZCHIP_NET_CH1));
            return;
		 }
         //DBG(TRACE("%d:Opened\r\n",WIZCHIP_NET_CH1));
         break;
      default:
		  //DBG(TRACE("sn_sr:%d\r\n",sn_sr));
         break;
   }
}

/**
 * @brief UDP Server
 */
static void taskForUdpServer1(void)
{
	UINT8 *buf = m_aDataBuffer[WIZCHIP_NET_CH1];
	UINT16 size = 0;
	//UINT16 sentsize = 0;
	INT32  ret;
   //uint8_t  packinfo = 0;
	UINT8 sn_sr = getSn_SR(WIZCHIP_NET_CH1);
   switch(sn_sr)
   {
      case SOCK_UDP :
         if((size = getSn_RX_RSR(WIZCHIP_NET_CH1)) > 0)
         {
            if(size > SOCKET_DATA_BUF_SIZE)
				size = SOCKET_DATA_BUF_SIZE;
            ret = recvfrom(WIZCHIP_NET_CH1,buf,size,(uint8_t*)m_szDestIp[WIZCHIP_NET_CH1],(uint16_t*)&m_uDestport);
            if(ret <= 0)
            {
               DBG(TRACE("%d: recvfrom error. %ld\r\n",WIZCHIP_NET_CH1,ret));
               return;
            }
			m_uRecvSize[WIZCHIP_NET_CH1] = ret;
			if (m_hrxobser[WIZCHIP_NET_CH1] != NULL)
				m_hrxobser[WIZCHIP_NET_CH1](1);
			////////////////////// 回环测试 ///////////////////////
            //size = (uint16_t) ret;
            //sentsize = 0;
            //while(sentsize != size)
            //{
            //   ret = sendto(WIZCHIP_NET_CH1,buf+sentsize,size-sentsize,(uint8_t*)m_szDestIp[WIZCHIP_NET_CH1],m_uDestport);
            //   if(ret < 0)
            //   {
            //      DBG(TRACE("%d: sendto error. %ld\r\n",WIZCHIP_NET_CH1,ret));
            //      return;
            //   }
            //   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            //}
         }
         break;
      case SOCK_CLOSED:
         //DBG(TRACE("%d:LBUStart\r\n",WIZCHIP_NET_CH1));
         if((ret=socket(WIZCHIP_NET_CH1,Sn_MR_UDP,m_uPort[WIZCHIP_NET_CH1],0x00)) != WIZCHIP_NET_CH1)
		 {
			DBG(TRACE("%d:Opened, port [%d] Field\r\n",WIZCHIP_NET_CH1, m_uPort[WIZCHIP_NET_CH1]));
            return;
		 }
         //DBG(TRACE("%d:Opened, port [%d]\r\n",WIZCHIP_NET_CH1, m_uPort[WIZCHIP_NET_CH1]));
         break;
      default :
		  //DBG(TRACE("sn_sr:%d\r\n",sn_sr));
         break;
   }
}

/**
 * @brief TCP Server
 */
static void taskForTcpClient1(void)
{
	UINT16 anyport=20000;
	UINT8 *buf = m_aDataBuffer[WIZCHIP_NET_CH1];
	UINT16 size = 0;
	//UINT16 sentsize=0;
	INT32 ret;
	UINT8 sn_sr = getSn_SR(WIZCHIP_NET_CH1);
	
	if (m_szServer[WIZCHIP_NET_CH1] != NULL);
	{
	   switch(sn_sr)
	   {
		  case SOCK_ESTABLISHED :
			 if(getSn_IR(WIZCHIP_NET_CH1) & Sn_IR_CON)
			 {
				DBG(TRACE("%d:Connected\r\n",WIZCHIP_NET_CH1));
				setSn_IR(WIZCHIP_NET_CH1,Sn_IR_CON);
			 }
			 if((size = getSn_RX_RSR(WIZCHIP_NET_CH1)) > 0)
			 {
				if(size > SOCKET_DATA_BUF_SIZE)
					size = SOCKET_DATA_BUF_SIZE;
				ret = recv(WIZCHIP_NET_CH1,buf,size);
				if(ret <= 0)
					return;
				m_uRecvSize[WIZCHIP_NET_CH1] = ret;
				if (m_hrxobser[WIZCHIP_NET_CH1] != NULL)
					m_hrxobser[WIZCHIP_NET_CH1](1);
				////////////////////// 回环测试 ///////////////////////
				//sentsize = 0;
				//while(size != sentsize)
				//{
				//   ret = send(WIZCHIP_NET_CH1,buf+sentsize,size-sentsize);
				//   if(ret < 0)
				//   {
				//      close(WIZCHIP_NET_CH1);
				//      return;
				//   }
				//   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
				//}
			 }
			 break;
		  case SOCK_CLOSE_WAIT :
			 //DBG(TRACE("%d:CloseWait\r\n",WIZCHIP_NET_CH1));
			 if((ret=disconnect(WIZCHIP_NET_CH1)) != SOCK_OK)
			 {
				DBG(TRACE("%d:CloseWait Field\r\n",WIZCHIP_NET_CH1));
				 return;
			 }
			 //DBG(TRACE("%d:Closed\r\n",WIZCHIP_NET_CH1));
			 break;
		  case SOCK_INIT :
			 //DBG(TRACE("%d:connet[%d.%d.%d.%d], port [%d]\r\n",WIZCHIP_NET_CH1, m_szServer[WIZCHIP_NET_CH1][0], m_szServer[WIZCHIP_NET_CH1][1], m_szServer[WIZCHIP_NET_CH1][2], m_szServer[WIZCHIP_NET_CH1][3], m_uPort[WIZCHIP_NET_CH1]));
			 if( (ret = connect(WIZCHIP_NET_CH1, m_szServer[WIZCHIP_NET_CH1], m_uPort[WIZCHIP_NET_CH1])) != SOCK_OK)
			 {
				 DBG(TRACE("%d:connet[%d.%d.%d.%d], port [%d] Field\r\n",WIZCHIP_NET_CH1, m_szServer[WIZCHIP_NET_CH1][0], m_szServer[WIZCHIP_NET_CH1][1], m_szServer[WIZCHIP_NET_CH1][2], m_szServer[WIZCHIP_NET_CH1][3],  m_uPort[WIZCHIP_NET_CH1]));
				 return;
			 }
			 break;
		  case SOCK_CLOSED:
			 //DBG(TRACE("%d:LBTStart\r\n",WIZCHIP_NET_CH1));
			 if((ret=socket(WIZCHIP_NET_CH1,Sn_MR_TCP,anyport++,Sn_MR_ND)) != WIZCHIP_NET_CH1)
			 {
				DBG(TRACE("%d:LBTStart Field\r\n",WIZCHIP_NET_CH1));
				return;
			 }
			 //DBG(TRACE("%d:Opened\r\n",WIZCHIP_NET_CH1));
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
static void taskForUdpClient1(void)
{
}

/**
 * @brief TCP Server
 */
static void taskForTcpServer2(void)
{
	UINT8 *buf = m_aDataBuffer[WIZCHIP_NET_CH2];
	UINT16 size = 0;
	//UINT16 sentsize=0;
	INT32 ret;
	UINT8 sn_sr = getSn_SR(WIZCHIP_NET_CH2);
   switch(sn_sr)
   {
      case SOCK_ESTABLISHED :
         if(getSn_IR(WIZCHIP_NET_CH2) & Sn_IR_CON)
         {
            DBG(TRACE("%d:Connected\r\n",WIZCHIP_NET_CH2));
            setSn_IR(WIZCHIP_NET_CH2,Sn_IR_CON);
         }
         if((size = getSn_RX_RSR(WIZCHIP_NET_CH2)) > 0)
         {
            if(size > SOCKET_DATA_BUF_SIZE)
				size = SOCKET_DATA_BUF_SIZE;
            ret = recv(WIZCHIP_NET_CH2,buf,size);
            if(ret <= 0)
				return;
			m_uRecvSize[WIZCHIP_NET_CH2] = ret;
			if (m_hrxobser[WIZCHIP_NET_CH2] != NULL)
				m_hrxobser[WIZCHIP_NET_CH2](1);
			////////////////////// 回环测试 ///////////////////////
            //sentsize = 0;
            //while(size != sentsize)
            //{
            //   ret = send(WIZCHIP_NET_CH2,buf+sentsize,size-sentsize);
            //   if(ret < 0)
            //   {
            //      close(WIZCHIP_NET_CH2);
            //      return;
            //   }
            //   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            //}
         }
         break;
      case SOCK_CLOSE_WAIT :
         //DBG(TRACE("%d:CloseWait\r\n",WIZCHIP_NET_CH2));
         if((ret=disconnect(WIZCHIP_NET_CH2)) != SOCK_OK)
		 {
			DBG(TRACE("%d:CloseWait Field\r\n",WIZCHIP_NET_CH2));
			 return;
		 }
         DBG(TRACE("%d:Closed\r\n",WIZCHIP_NET_CH2));
         break;
      case SOCK_INIT :
    	  //DBG(TRACE("%d:Listen, port [%d]\r\n",WIZCHIP_NET_CH2, m_uPort[WIZCHIP_NET_CH2]));
         if( (ret = listen(WIZCHIP_NET_CH2)) != SOCK_OK)
		 {
			 DBG(TRACE("%d:Listen, port [%d] Field\r\n",WIZCHIP_NET_CH2, m_uPort[WIZCHIP_NET_CH2]));
			 return;
		 }
         break;
      case SOCK_CLOSED:
         //DBG(TRACE("%d:LBTStart\r\n",WIZCHIP_NET_CH2));
         if((ret=socket(WIZCHIP_NET_CH2,Sn_MR_TCP,m_uPort[WIZCHIP_NET_CH2],0x00)) != WIZCHIP_NET_CH2)
		 {
			DBG(TRACE("%d:LBTStart Field\r\n",WIZCHIP_NET_CH2));
            return;
		 }
         //DBG(TRACE("%d:Opened\r\n",WIZCHIP_NET_CH2));
         break;
      default:
		  //DBG(TRACE("sn_sr:%d\r\n",sn_sr));
         break;
   }
}

/**
 * @brief UDP Server
 */
static void taskForUdpServer2(void)
{
	UINT8 *buf = m_aDataBuffer[WIZCHIP_NET_CH2];
	UINT16 size = 0;
	//UINT16 sentsize = 0;
	INT32  ret;
   //uint8_t  packinfo = 0;
	UINT8 sn_sr = getSn_SR(WIZCHIP_NET_CH2);
   switch(sn_sr)
   {
      case SOCK_UDP :
         if((size = getSn_RX_RSR(WIZCHIP_NET_CH2)) > 0)
         {
            if(size > SOCKET_DATA_BUF_SIZE)
				size = SOCKET_DATA_BUF_SIZE;
            ret = recvfrom(WIZCHIP_NET_CH2,buf,size,(uint8_t*)m_szDestIp[WIZCHIP_NET_CH2],(uint16_t*)&m_uDestport);
            if(ret <= 0)
            {
               DBG(TRACE("%d: recvfrom error. %ld\r\n",WIZCHIP_NET_CH2,ret));
               return;
            }
			m_uRecvSize[WIZCHIP_NET_CH2] = ret;
			if (m_hrxobser[WIZCHIP_NET_CH2] != NULL)
				m_hrxobser[WIZCHIP_NET_CH2](1);
			////////////////////// 回环测试 ///////////////////////
            //size = (uint16_t) ret;
            //sentsize = 0;
            //while(sentsize != size)
            //{
            //   ret = sendto(WIZCHIP_NET_CH2,buf+sentsize,size-sentsize,(uint8_t*)m_szDestIp[WIZCHIP_NET_CH2],m_uDestport);
            //   if(ret < 0)
            //   {
            //      DBG(TRACE("%d: sendto error. %ld\r\n",WIZCHIP_NET_CH2,ret));
            //      return;
            //   }
            //   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            //}
         }
         break;
      case SOCK_CLOSED:
         //DBG(TRACE("%d:LBUStart\r\n",WIZCHIP_NET_CH2));
         if((ret=socket(WIZCHIP_NET_CH2,Sn_MR_UDP,m_uPort[WIZCHIP_NET_CH2],0x00)) != WIZCHIP_NET_CH2)
		 {
			DBG(TRACE("%d:Opened, port [%d] Field\r\n",WIZCHIP_NET_CH2, m_uPort[WIZCHIP_NET_CH2]));
            return;
		 }
         //DBG(TRACE("%d:Opened, port [%d]\r\n",WIZCHIP_NET_CH2, m_uPort[WIZCHIP_NET_CH2]));
         break;
      default :
		  //DBG(TRACE("sn_sr:%d\r\n",sn_sr));
         break;
   }
}

/**
 * @brief TCP Server
 */
static void taskForTcpClient2(void)
{
	UINT16 anyport=20000;
	UINT8 *buf = m_aDataBuffer[WIZCHIP_NET_CH2];
	UINT16 size = 0;
	//UINT16 sentsize=0;
	INT32 ret;
	UINT8 sn_sr = getSn_SR(WIZCHIP_NET_CH2);
	
	if (m_szServer[WIZCHIP_NET_CH2] != NULL);
	{
	   switch(sn_sr)
	   {
		  case SOCK_ESTABLISHED :
			 if(getSn_IR(WIZCHIP_NET_CH2) & Sn_IR_CON)
			 {
				DBG(TRACE("%d:Connected\r\n",WIZCHIP_NET_CH2));
				setSn_IR(WIZCHIP_NET_CH2,Sn_IR_CON);
			 }
			 if((size = getSn_RX_RSR(WIZCHIP_NET_CH2)) > 0)
			 {
				if(size > SOCKET_DATA_BUF_SIZE)
					size = SOCKET_DATA_BUF_SIZE;
				ret = recv(WIZCHIP_NET_CH2,buf,size);
				if(ret <= 0)
					return;
				m_uRecvSize[WIZCHIP_NET_CH2] = ret;
				if (m_hrxobser[WIZCHIP_NET_CH2] != NULL)
					m_hrxobser[WIZCHIP_NET_CH2](1);
				////////////////////// 回环测试 ///////////////////////
				//sentsize = 0;
				//while(size != sentsize)
				//{
				//   ret = send(WIZCHIP_NET_CH2,buf+sentsize,size-sentsize);
				//   if(ret < 0)
				//   {
				//      close(WIZCHIP_NET_CH2);
				//      return;
				//   }
				//   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
				//}
			 }
			 break;
		  case SOCK_CLOSE_WAIT :
			 //DBG(TRACE("%d:CloseWait\r\n",WIZCHIP_NET_CH2));
			 if((ret=disconnect(WIZCHIP_NET_CH2)) != SOCK_OK)
			 {
				DBG(TRACE("%d:CloseWait Field\r\n",WIZCHIP_NET_CH2));
				 return;
			 }
			 //DBG(TRACE("%d:Closed\r\n",WIZCHIP_NET_CH2));
			 break;
		  case SOCK_INIT :
			 //DBG(TRACE("%d:connet[%d.%d.%d.%d], port [%d]\r\n",WIZCHIP_NET_CH2, m_szServer[WIZCHIP_NET_CH2][0], m_szServer[WIZCHIP_NET_CH2][1], m_szServer[WIZCHIP_NET_CH2][2], m_szServer[WIZCHIP_NET_CH2][3], m_uPort[WIZCHIP_NET_CH2]));
			 if( (ret = connect(WIZCHIP_NET_CH2, m_szServer[WIZCHIP_NET_CH2], m_uPort[WIZCHIP_NET_CH2])) != SOCK_OK)
			 {
				 DBG(TRACE("%d:connet[%d.%d.%d.%d], port [%d] Field\r\n",WIZCHIP_NET_CH2, m_szServer[WIZCHIP_NET_CH2][0], m_szServer[WIZCHIP_NET_CH2][1], m_szServer[WIZCHIP_NET_CH2][2], m_szServer[WIZCHIP_NET_CH2][3],  m_uPort[WIZCHIP_NET_CH2]));
				 return;
			 }
			 break;
		  case SOCK_CLOSED:
			 //DBG(TRACE("%d:LBTStart\r\n",WIZCHIP_NET_CH2));
			 if((ret=socket(WIZCHIP_NET_CH2,Sn_MR_TCP,anyport++,Sn_MR_ND)) != WIZCHIP_NET_CH2)
			 {
				DBG(TRACE("%d:LBTStart Field\r\n",WIZCHIP_NET_CH2));
				return;
			 }
			 //DBG(TRACE("%d:Opened\r\n",WIZCHIP_NET_CH2));
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
static void taskForUdpClient2(void)
{
}

/**
 * @brief TCP Server
 */
static void taskForTcpServer3(void)
{
	UINT8 *buf = m_aDataBuffer[WIZCHIP_NET_CH3];
	UINT16 size = 0;
	//UINT16 sentsize=0;
	INT32 ret;
	UINT8 sn_sr = getSn_SR(WIZCHIP_NET_CH3);
   switch(sn_sr)
   {
      case SOCK_ESTABLISHED :
         if(getSn_IR(WIZCHIP_NET_CH3) & Sn_IR_CON)
         {
            DBG(TRACE("%d:Connected\r\n",WIZCHIP_NET_CH3));
            setSn_IR(WIZCHIP_NET_CH3,Sn_IR_CON);
         }
         if((size = getSn_RX_RSR(WIZCHIP_NET_CH3)) > 0)
         {
            if(size > SOCKET_DATA_BUF_SIZE)
				size = SOCKET_DATA_BUF_SIZE;
            ret = recv(WIZCHIP_NET_CH3,buf,size);
            if(ret <= 0)
				return;
			m_uRecvSize[WIZCHIP_NET_CH3] = ret;
			if (m_hrxobser[WIZCHIP_NET_CH3] != NULL)
				m_hrxobser[WIZCHIP_NET_CH3](1);
			////////////////////// 回环测试 ///////////////////////
            //sentsize = 0;
            //while(size != sentsize)
            //{
            //   ret = send(WIZCHIP_NET_CH3,buf+sentsize,size-sentsize);
            //   if(ret < 0)
            //   {
            //      close(WIZCHIP_NET_CH3);
            //      return;
            //   }
            //   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            //}
         }
         break;
      case SOCK_CLOSE_WAIT :
         //DBG(TRACE("%d:CloseWait\r\n",WIZCHIP_NET_CH3));
         if((ret=disconnect(WIZCHIP_NET_CH3)) != SOCK_OK)
		 {
			DBG(TRACE("%d:CloseWait Field\r\n",WIZCHIP_NET_CH3));
			 return;
		 }
         DBG(TRACE("%d:Closed\r\n",WIZCHIP_NET_CH3));
         break;
      case SOCK_INIT :
    	  //DBG(TRACE("%d:Listen, port [%d]\r\n",WIZCHIP_NET_CH3, m_uPort[WIZCHIP_NET_CH3]));
         if( (ret = listen(WIZCHIP_NET_CH3)) != SOCK_OK)
		 {
			 DBG(TRACE("%d:Listen, port [%d] Field\r\n",WIZCHIP_NET_CH3, m_uPort[WIZCHIP_NET_CH3]));
			 return;
		 }
         break;
      case SOCK_CLOSED:
         //DBG(TRACE("%d:LBTStart\r\n",WIZCHIP_NET_CH3));
         if((ret=socket(WIZCHIP_NET_CH3,Sn_MR_TCP,m_uPort[WIZCHIP_NET_CH3],0x00)) != WIZCHIP_NET_CH3)
		 {
			DBG(TRACE("%d:LBTStart Field\r\n",WIZCHIP_NET_CH3));
            return;
		 }
         //DBG(TRACE("%d:Opened\r\n",WIZCHIP_NET_CH3));
         break;
      default:
		  //DBG(TRACE("sn_sr:%d\r\n",sn_sr));
         break;
   }
}

/**
 * @brief UDP Server
 */
static void taskForUdpServer3(void)
{
	UINT8 *buf = m_aDataBuffer[WIZCHIP_NET_CH3];
	UINT16 size = 0;
	//UINT16 sentsize = 0;
	INT32  ret;
   //uint8_t  packinfo = 0;
	UINT8 sn_sr = getSn_SR(WIZCHIP_NET_CH3);
   switch(sn_sr)
   {
      case SOCK_UDP :
         if((size = getSn_RX_RSR(WIZCHIP_NET_CH3)) > 0)
         {
            if(size > SOCKET_DATA_BUF_SIZE)
				size = SOCKET_DATA_BUF_SIZE;
            ret = recvfrom(WIZCHIP_NET_CH3,buf,size,(uint8_t*)m_szDestIp[WIZCHIP_NET_CH3],(uint16_t*)&m_uDestport);
            if(ret <= 0)
            {
               DBG(TRACE("%d: recvfrom error. %ld\r\n",WIZCHIP_NET_CH3,ret));
               return;
            }
			m_uRecvSize[WIZCHIP_NET_CH3] = ret;
			if (m_hrxobser[WIZCHIP_NET_CH3] != NULL)
				m_hrxobser[WIZCHIP_NET_CH3](1);
			////////////////////// 回环测试 ///////////////////////
            //size = (uint16_t) ret;
            //sentsize = 0;
            //while(sentsize != size)
            //{
            //   ret = sendto(WIZCHIP_NET_CH3,buf+sentsize,size-sentsize,(uint8_t*)m_szDestIp[WIZCHIP_NET_CH3],m_uDestport);
            //   if(ret < 0)
            //   {
            //      DBG(TRACE("%d: sendto error. %ld\r\n",WIZCHIP_NET_CH3,ret));
            //      return;
            //   }
            //   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            //}
         }
         break;
      case SOCK_CLOSED:
         //DBG(TRACE("%d:LBUStart\r\n",WIZCHIP_NET_CH3));
         if((ret=socket(WIZCHIP_NET_CH3,Sn_MR_UDP,m_uPort[WIZCHIP_NET_CH3],0x00)) != WIZCHIP_NET_CH3)
		 {
			DBG(TRACE("%d:Opened, port [%d] Field\r\n",WIZCHIP_NET_CH3, m_uPort[WIZCHIP_NET_CH3]));
            return;
		 }
         //DBG(TRACE("%d:Opened, port [%d]\r\n",WIZCHIP_NET_CH3, m_uPort[WIZCHIP_NET_CH3]));
         break;
      default :
		  //DBG(TRACE("sn_sr:%d\r\n",sn_sr));
         break;
   }
}

/**
 * @brief TCP Server
 */
static void taskForTcpClient3(void)
{
	UINT16 anyport=20000;
	UINT8 *buf = m_aDataBuffer[WIZCHIP_NET_CH3];
	UINT16 size = 0;
	//UINT16 sentsize=0;
	INT32 ret;
	UINT8 sn_sr = getSn_SR(WIZCHIP_NET_CH3);
	
	if (m_szServer[WIZCHIP_NET_CH3] != NULL);
	{
	   switch(sn_sr)
	   {
		  case SOCK_ESTABLISHED :
			 if(getSn_IR(WIZCHIP_NET_CH3) & Sn_IR_CON)
			 {
				DBG(TRACE("%d:Connected\r\n",WIZCHIP_NET_CH3));
				setSn_IR(WIZCHIP_NET_CH3,Sn_IR_CON);
			 }
			 if((size = getSn_RX_RSR(WIZCHIP_NET_CH3)) > 0)
			 {
				if(size > SOCKET_DATA_BUF_SIZE)
					size = SOCKET_DATA_BUF_SIZE;
				ret = recv(WIZCHIP_NET_CH3,buf,size);
				if(ret <= 0)
					return;
				m_uRecvSize[WIZCHIP_NET_CH3] = ret;
				if (m_hrxobser[WIZCHIP_NET_CH3] != NULL)
					m_hrxobser[WIZCHIP_NET_CH3](1);
				////////////////////// 回环测试 ///////////////////////
				//sentsize = 0;
				//while(size != sentsize)
				//{
				//   ret = send(WIZCHIP_NET_CH3,buf+sentsize,size-sentsize);
				//   if(ret < 0)
				//   {
				//      close(WIZCHIP_NET_CH3);
				//      return;
				//   }
				//   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
				//}
			 }
			 break;
		  case SOCK_CLOSE_WAIT :
			 //DBG(TRACE("%d:CloseWait\r\n",WIZCHIP_NET_CH3));
			 if((ret=disconnect(WIZCHIP_NET_CH3)) != SOCK_OK)
			 {
				DBG(TRACE("%d:CloseWait Field\r\n",WIZCHIP_NET_CH3));
				 return;
			 }
			 //DBG(TRACE("%d:Closed\r\n",WIZCHIP_NET_CH3));
			 break;
		  case SOCK_INIT :
			 //DBG(TRACE("%d:connet[%d.%d.%d.%d], port [%d]\r\n",WIZCHIP_NET_CH3, m_szServer[WIZCHIP_NET_CH3][0], m_szServer[WIZCHIP_NET_CH3][1], m_szServer[WIZCHIP_NET_CH3][2], m_szServer[WIZCHIP_NET_CH3][3], m_uPort[WIZCHIP_NET_CH3]));
			 if( (ret = connect(WIZCHIP_NET_CH3, m_szServer[WIZCHIP_NET_CH3], m_uPort[WIZCHIP_NET_CH3])) != SOCK_OK)
			 {
				 DBG(TRACE("%d:connet[%d.%d.%d.%d], port [%d] Field\r\n",WIZCHIP_NET_CH3, m_szServer[WIZCHIP_NET_CH3][0], m_szServer[WIZCHIP_NET_CH3][1], m_szServer[WIZCHIP_NET_CH3][2], m_szServer[WIZCHIP_NET_CH3][3],  m_uPort[WIZCHIP_NET_CH3]));
				 return;
			 }
			 break;
		  case SOCK_CLOSED:
			 //DBG(TRACE("%d:LBTStart\r\n",WIZCHIP_NET_CH3));
			 if((ret=socket(WIZCHIP_NET_CH3,Sn_MR_TCP,anyport++,Sn_MR_ND)) != WIZCHIP_NET_CH3)
			 {
				DBG(TRACE("%d:LBTStart Field\r\n",WIZCHIP_NET_CH3));
				return;
			 }
			 //DBG(TRACE("%d:Opened\r\n",WIZCHIP_NET_CH3));
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
static void taskForUdpClient3(void)
{
}

#if _WIZCHIP_ > 5100
/**
 * @brief TCP Server
 */
static void taskForTcpServer4(void)
{
	UINT8 *buf = m_aDataBuffer[WIZCHIP_NET_CH4];
	UINT16 size = 0;
	//UINT16 sentsize=0;
	INT32 ret;
	UINT8 sn_sr = getSn_SR(WIZCHIP_NET_CH4);
   switch(sn_sr)
   {
      case SOCK_ESTABLISHED :
         if(getSn_IR(WIZCHIP_NET_CH4) & Sn_IR_CON)
         {
            DBG(TRACE("%d:Connected\r\n",WIZCHIP_NET_CH4));
            setSn_IR(WIZCHIP_NET_CH4,Sn_IR_CON);
         }
         if((size = getSn_RX_RSR(WIZCHIP_NET_CH4)) > 0)
         {
            if(size > SOCKET_DATA_BUF_SIZE)
				size = SOCKET_DATA_BUF_SIZE;
            ret = recv(WIZCHIP_NET_CH4,buf,size);
            if(ret <= 0)
				return;
			m_uRecvSize[WIZCHIP_NET_CH4] = ret;
			if (m_hrxobser[WIZCHIP_NET_CH4] != NULL)
				m_hrxobser[WIZCHIP_NET_CH4](1);
			////////////////////// 回环测试 ///////////////////////
            //sentsize = 0;
            //while(size != sentsize)
            //{
            //   ret = send(WIZCHIP_NET_CH4,buf+sentsize,size-sentsize);
            //   if(ret < 0)
            //   {
            //      close(WIZCHIP_NET_CH4);
            //      return;
            //   }
            //   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            //}
         }
         break;
      case SOCK_CLOSE_WAIT :
         //DBG(TRACE("%d:CloseWait\r\n",WIZCHIP_NET_CH4));
         if((ret=disconnect(WIZCHIP_NET_CH4)) != SOCK_OK)
		 {
			DBG(TRACE("%d:CloseWait Field\r\n",WIZCHIP_NET_CH4));
			 return;
		 }
         DBG(TRACE("%d:Closed\r\n",WIZCHIP_NET_CH4));
         break;
      case SOCK_INIT :
    	  //DBG(TRACE("%d:Listen, port [%d]\r\n",WIZCHIP_NET_CH4, m_uPort[WIZCHIP_NET_CH4]));
         if( (ret = listen(WIZCHIP_NET_CH4)) != SOCK_OK)
		 {
			 DBG(TRACE("%d:Listen, port [%d] Field\r\n",WIZCHIP_NET_CH4, m_uPort[WIZCHIP_NET_CH4]));
			 return;
		 }
         break;
      case SOCK_CLOSED:
         //DBG(TRACE("%d:LBTStart\r\n",WIZCHIP_NET_CH4));
         if((ret=socket(WIZCHIP_NET_CH4,Sn_MR_TCP,m_uPort[WIZCHIP_NET_CH4],0x00)) != WIZCHIP_NET_CH4)
		 {
			DBG(TRACE("%d:LBTStart Field\r\n",WIZCHIP_NET_CH4));
            return;
		 }
         //DBG(TRACE("%d:Opened\r\n",WIZCHIP_NET_CH4));
         break;
      default:
		  //DBG(TRACE("sn_sr:%d\r\n",sn_sr));
         break;
   }
}

/**
 * @brief UDP Server
 */
static void taskForUdpServer4(void)
{
	UINT8 *buf = m_aDataBuffer[WIZCHIP_NET_CH4];
	UINT16 size = 0;
	//UINT16 sentsize = 0;
	INT32  ret;
   //uint8_t  packinfo = 0;
	UINT8 sn_sr = getSn_SR(WIZCHIP_NET_CH4);
   switch(sn_sr)
   {
      case SOCK_UDP :
         if((size = getSn_RX_RSR(WIZCHIP_NET_CH4)) > 0)
         {
            if(size > SOCKET_DATA_BUF_SIZE)
				size = SOCKET_DATA_BUF_SIZE;
            ret = recvfrom(WIZCHIP_NET_CH4,buf,size,(uint8_t*)m_szDestIp[WIZCHIP_NET_CH4],(uint16_t*)&m_uDestport);
            if(ret <= 0)
            {
               DBG(TRACE("%d: recvfrom error. %ld\r\n",WIZCHIP_NET_CH4,ret));
               return;
            }
			m_uRecvSize[WIZCHIP_NET_CH4] = ret;
			if (m_hrxobser[WIZCHIP_NET_CH4] != NULL)
				m_hrxobser[WIZCHIP_NET_CH4](1);
			////////////////////// 回环测试 ///////////////////////
            //size = (uint16_t) ret;
            //sentsize = 0;
            //while(sentsize != size)
            //{
            //   ret = sendto(WIZCHIP_NET_CH4,buf+sentsize,size-sentsize,(uint8_t*)m_szDestIp[WIZCHIP_NET_CH4],m_uDestport);
            //   if(ret < 0)
            //   {
            //      DBG(TRACE("%d: sendto error. %ld\r\n",WIZCHIP_NET_CH4,ret));
            //      return;
            //   }
            //   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            //}
         }
         break;
      case SOCK_CLOSED:
         //DBG(TRACE("%d:LBUStart\r\n",WIZCHIP_NET_CH4));
         if((ret=socket(WIZCHIP_NET_CH4,Sn_MR_UDP,m_uPort[WIZCHIP_NET_CH4],0x00)) != WIZCHIP_NET_CH4)
		 {
			DBG(TRACE("%d:Opened, port [%d] Field\r\n",WIZCHIP_NET_CH4, m_uPort[WIZCHIP_NET_CH4]));
            return;
		 }
         //DBG(TRACE("%d:Opened, port [%d]\r\n",WIZCHIP_NET_CH4, m_uPort[WIZCHIP_NET_CH4]));
         break;
      default :
		  //DBG(TRACE("sn_sr:%d\r\n",sn_sr));
         break;
   }
}

/**
 * @brief TCP Server
 */
static void taskForTcpClient4(void)
{
	UINT16 anyport=20000;
	UINT8 *buf = m_aDataBuffer[WIZCHIP_NET_CH4];
	UINT16 size = 0;
	//UINT16 sentsize=0;
	INT32 ret;
	UINT8 sn_sr = getSn_SR(WIZCHIP_NET_CH4);
	
	if (m_szServer[WIZCHIP_NET_CH4] != NULL);
	{
	   switch(sn_sr)
	   {
		  case SOCK_ESTABLISHED :
			 if(getSn_IR(WIZCHIP_NET_CH4) & Sn_IR_CON)
			 {
				DBG(TRACE("%d:Connected\r\n",WIZCHIP_NET_CH4));
				setSn_IR(WIZCHIP_NET_CH4,Sn_IR_CON);
			 }
			 if((size = getSn_RX_RSR(WIZCHIP_NET_CH4)) > 0)
			 {
				if(size > SOCKET_DATA_BUF_SIZE)
					size = SOCKET_DATA_BUF_SIZE;
				ret = recv(WIZCHIP_NET_CH4,buf,size);
				if(ret <= 0)
					return;
				m_uRecvSize[WIZCHIP_NET_CH4] = ret;
				if (m_hrxobser[WIZCHIP_NET_CH4] != NULL)
					m_hrxobser[WIZCHIP_NET_CH4](1);
				////////////////////// 回环测试 ///////////////////////
				//sentsize = 0;
				//while(size != sentsize)
				//{
				//   ret = send(WIZCHIP_NET_CH4,buf+sentsize,size-sentsize);
				//   if(ret < 0)
				//   {
				//      close(WIZCHIP_NET_CH4);
				//      return;
				//   }
				//   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
				//}
			 }
			 break;
		  case SOCK_CLOSE_WAIT :
			 //DBG(TRACE("%d:CloseWait\r\n",WIZCHIP_NET_CH4));
			 if((ret=disconnect(WIZCHIP_NET_CH4)) != SOCK_OK)
			 {
				DBG(TRACE("%d:CloseWait Field\r\n",WIZCHIP_NET_CH4));
				 return;
			 }
			 //DBG(TRACE("%d:Closed\r\n",WIZCHIP_NET_CH4));
			 break;
		  case SOCK_INIT :
			 //DBG(TRACE("%d:connet[%d.%d.%d.%d], port [%d]\r\n",WIZCHIP_NET_CH4, m_szServer[WIZCHIP_NET_CH4][0], m_szServer[WIZCHIP_NET_CH4][1], m_szServer[WIZCHIP_NET_CH4][2], m_szServer[WIZCHIP_NET_CH4][3], m_uPort[WIZCHIP_NET_CH4]));
			 if( (ret = connect(WIZCHIP_NET_CH4, m_szServer[WIZCHIP_NET_CH4], m_uPort[WIZCHIP_NET_CH4])) != SOCK_OK)
			 {
				 DBG(TRACE("%d:connet[%d.%d.%d.%d], port [%d] Field\r\n",WIZCHIP_NET_CH4, m_szServer[WIZCHIP_NET_CH4][0], m_szServer[WIZCHIP_NET_CH4][1], m_szServer[WIZCHIP_NET_CH4][2], m_szServer[WIZCHIP_NET_CH4][3],  m_uPort[WIZCHIP_NET_CH4]));
				 return;
			 }
			 break;
		  case SOCK_CLOSED:
			 //DBG(TRACE("%d:LBTStart\r\n",WIZCHIP_NET_CH4));
			 if((ret=socket(WIZCHIP_NET_CH4,Sn_MR_TCP,anyport++,Sn_MR_ND)) != WIZCHIP_NET_CH4)
			 {
				DBG(TRACE("%d:LBTStart Field\r\n",WIZCHIP_NET_CH4));
				return;
			 }
			 //DBG(TRACE("%d:Opened\r\n",WIZCHIP_NET_CH4));
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
static void taskForUdpClient4(void)
{
}

/**
 * @brief TCP Server
 */
static void taskForTcpServer5(void)
{
	UINT8 *buf = m_aDataBuffer[WIZCHIP_NET_CH5];
	UINT16 size = 0;
	//UINT16 sentsize=0;
	INT32 ret;
	UINT8 sn_sr = getSn_SR(WIZCHIP_NET_CH5);
   switch(sn_sr)
   {
      case SOCK_ESTABLISHED :
         if(getSn_IR(WIZCHIP_NET_CH5) & Sn_IR_CON)
         {
            DBG(TRACE("%d:Connected\r\n",WIZCHIP_NET_CH5));
            setSn_IR(WIZCHIP_NET_CH5,Sn_IR_CON);
         }
         if((size = getSn_RX_RSR(WIZCHIP_NET_CH5)) > 0)
         {
            if(size > SOCKET_DATA_BUF_SIZE)
				size = SOCKET_DATA_BUF_SIZE;
            ret = recv(WIZCHIP_NET_CH5,buf,size);
            if(ret <= 0)
				return;
			m_uRecvSize[WIZCHIP_NET_CH5] = ret;
			if (m_hrxobser[WIZCHIP_NET_CH5] != NULL)
				m_hrxobser[WIZCHIP_NET_CH5](1);
			////////////////////// 回环测试 ///////////////////////
            //sentsize = 0;
            //while(size != sentsize)
            //{
            //   ret = send(WIZCHIP_NET_CH5,buf+sentsize,size-sentsize);
            //   if(ret < 0)
            //   {
            //      close(WIZCHIP_NET_CH5);
            //      return;
            //   }
            //   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            //}
         }
         break;
      case SOCK_CLOSE_WAIT :
         //DBG(TRACE("%d:CloseWait\r\n",WIZCHIP_NET_CH5));
         if((ret=disconnect(WIZCHIP_NET_CH5)) != SOCK_OK)
		 {
			DBG(TRACE("%d:CloseWait Field\r\n",WIZCHIP_NET_CH5));
			 return;
		 }
         DBG(TRACE("%d:Closed\r\n",WIZCHIP_NET_CH5));
         break;
      case SOCK_INIT :
    	  //DBG(TRACE("%d:Listen, port [%d]\r\n",WIZCHIP_NET_CH5, m_uPort[WIZCHIP_NET_CH5]));
         if( (ret = listen(WIZCHIP_NET_CH5)) != SOCK_OK)
		 {
			 DBG(TRACE("%d:Listen, port [%d] Field\r\n",WIZCHIP_NET_CH5, m_uPort[WIZCHIP_NET_CH5]));
			 return;
		 }
         break;
      case SOCK_CLOSED:
         //DBG(TRACE("%d:LBTStart\r\n",WIZCHIP_NET_CH5));
         if((ret=socket(WIZCHIP_NET_CH5,Sn_MR_TCP,m_uPort[WIZCHIP_NET_CH5],0x00)) != WIZCHIP_NET_CH5)
		 {
			DBG(TRACE("%d:LBTStart Field\r\n",WIZCHIP_NET_CH5));
            return;
		 }
         //DBG(TRACE("%d:Opened\r\n",WIZCHIP_NET_CH5));
         break;
      default:
		  //DBG(TRACE("sn_sr:%d\r\n",sn_sr));
         break;
   }
}

/**
 * @brief UDP Server
 */
static void taskForUdpServer5(void)
{
	UINT8 *buf = m_aDataBuffer[WIZCHIP_NET_CH5];
	UINT16 size = 0;
	//UINT16 sentsize = 0;
	INT32  ret;
   //uint8_t  packinfo = 0;
	UINT8 sn_sr = getSn_SR(WIZCHIP_NET_CH5);
   switch(sn_sr)
   {
      case SOCK_UDP :
         if((size = getSn_RX_RSR(WIZCHIP_NET_CH5)) > 0)
         {
            if(size > SOCKET_DATA_BUF_SIZE)
				size = SOCKET_DATA_BUF_SIZE;
            ret = recvfrom(WIZCHIP_NET_CH5,buf,size,(uint8_t*)m_szDestIp[WIZCHIP_NET_CH5],(uint16_t*)&m_uDestport);
            if(ret <= 0)
            {
               DBG(TRACE("%d: recvfrom error. %ld\r\n",WIZCHIP_NET_CH5,ret));
               return;
            }
			m_uRecvSize[WIZCHIP_NET_CH5] = ret;
			if (m_hrxobser[WIZCHIP_NET_CH5] != NULL)
				m_hrxobser[WIZCHIP_NET_CH5](1);
			////////////////////// 回环测试 ///////////////////////
            //size = (uint16_t) ret;
            //sentsize = 0;
            //while(sentsize != size)
            //{
            //   ret = sendto(WIZCHIP_NET_CH5,buf+sentsize,size-sentsize,(uint8_t*)m_szDestIp[WIZCHIP_NET_CH5],m_uDestport);
            //   if(ret < 0)
            //   {
            //      DBG(TRACE("%d: sendto error. %ld\r\n",WIZCHIP_NET_CH5,ret));
            //      return;
            //   }
            //   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            //}
         }
         break;
      case SOCK_CLOSED:
         //DBG(TRACE("%d:LBUStart\r\n",WIZCHIP_NET_CH5));
         if((ret=socket(WIZCHIP_NET_CH5,Sn_MR_UDP,m_uPort[WIZCHIP_NET_CH5],0x00)) != WIZCHIP_NET_CH5)
		 {
			DBG(TRACE("%d:Opened, port [%d] Field\r\n",WIZCHIP_NET_CH5, m_uPort[WIZCHIP_NET_CH5]));
            return;
		 }
         //DBG(TRACE("%d:Opened, port [%d]\r\n",WIZCHIP_NET_CH5, m_uPort[WIZCHIP_NET_CH5]));
         break;
      default :
		  //DBG(TRACE("sn_sr:%d\r\n",sn_sr));
         break;
   }
}

/**
 * @brief TCP Server
 */
static void taskForTcpClient5(void)
{
	UINT16 anyport=20000;
	UINT8 *buf = m_aDataBuffer[WIZCHIP_NET_CH5];
	UINT16 size = 0;
	//UINT16 sentsize=0;
	INT32 ret;
	UINT8 sn_sr = getSn_SR(WIZCHIP_NET_CH5);
	
	if (m_szServer[WIZCHIP_NET_CH5] != NULL);
	{
	   switch(sn_sr)
	   {
		  case SOCK_ESTABLISHED :
			 if(getSn_IR(WIZCHIP_NET_CH5) & Sn_IR_CON)
			 {
				DBG(TRACE("%d:Connected\r\n",WIZCHIP_NET_CH5));
				setSn_IR(WIZCHIP_NET_CH5,Sn_IR_CON);
			 }
			 if((size = getSn_RX_RSR(WIZCHIP_NET_CH5)) > 0)
			 {
				if(size > SOCKET_DATA_BUF_SIZE)
					size = SOCKET_DATA_BUF_SIZE;
				ret = recv(WIZCHIP_NET_CH5,buf,size);
				if(ret <= 0)
					return;
				m_uRecvSize[WIZCHIP_NET_CH5] = ret;
				if (m_hrxobser[WIZCHIP_NET_CH5] != NULL)
					m_hrxobser[WIZCHIP_NET_CH5](1);
				////////////////////// 回环测试 ///////////////////////
				//sentsize = 0;
				//while(size != sentsize)
				//{
				//   ret = send(WIZCHIP_NET_CH5,buf+sentsize,size-sentsize);
				//   if(ret < 0)
				//   {
				//      close(WIZCHIP_NET_CH5);
				//      return;
				//   }
				//   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
				//}
			 }
			 break;
		  case SOCK_CLOSE_WAIT :
			 //DBG(TRACE("%d:CloseWait\r\n",WIZCHIP_NET_CH5));
			 if((ret=disconnect(WIZCHIP_NET_CH5)) != SOCK_OK)
			 {
				DBG(TRACE("%d:CloseWait Field\r\n",WIZCHIP_NET_CH5));
				 return;
			 }
			 //DBG(TRACE("%d:Closed\r\n",WIZCHIP_NET_CH5));
			 break;
		  case SOCK_INIT :
			 //DBG(TRACE("%d:connet[%d.%d.%d.%d], port [%d]\r\n",WIZCHIP_NET_CH5, m_szServer[WIZCHIP_NET_CH5][0], m_szServer[WIZCHIP_NET_CH5][1], m_szServer[WIZCHIP_NET_CH5][2], m_szServer[WIZCHIP_NET_CH5][3], m_uPort[WIZCHIP_NET_CH5]));
			 if( (ret = connect(WIZCHIP_NET_CH5, m_szServer[WIZCHIP_NET_CH5], m_uPort[WIZCHIP_NET_CH5])) != SOCK_OK)
			 {
				 DBG(TRACE("%d:connet[%d.%d.%d.%d], port [%d] Field\r\n",WIZCHIP_NET_CH5, m_szServer[WIZCHIP_NET_CH5][0], m_szServer[WIZCHIP_NET_CH5][1], m_szServer[WIZCHIP_NET_CH5][2], m_szServer[WIZCHIP_NET_CH5][3],  m_uPort[WIZCHIP_NET_CH5]));
				 return;
			 }
			 break;
		  case SOCK_CLOSED:
			 //DBG(TRACE("%d:LBTStart\r\n",WIZCHIP_NET_CH5));
			 if((ret=socket(WIZCHIP_NET_CH5,Sn_MR_TCP,anyport++,Sn_MR_ND)) != WIZCHIP_NET_CH5)
			 {
				DBG(TRACE("%d:LBTStart Field\r\n",WIZCHIP_NET_CH5));
				return;
			 }
			 //DBG(TRACE("%d:Opened\r\n",WIZCHIP_NET_CH5));
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
static void taskForUdpClient5(void)
{
}

/**
 * @brief TCP Server
 */
static void taskForTcpServer6(void)
{
	UINT8 *buf = m_aDataBuffer[WIZCHIP_NET_CH6];
	UINT16 size = 0;
	//UINT16 sentsize=0;
	INT32 ret;
	UINT8 sn_sr = getSn_SR(WIZCHIP_NET_CH6);
   switch(sn_sr)
   {
      case SOCK_ESTABLISHED :
         if(getSn_IR(WIZCHIP_NET_CH6) & Sn_IR_CON)
         {
            DBG(TRACE("%d:Connected\r\n",WIZCHIP_NET_CH6));
            setSn_IR(WIZCHIP_NET_CH6,Sn_IR_CON);
         }
         if((size = getSn_RX_RSR(WIZCHIP_NET_CH6)) > 0)
         {
            if(size > SOCKET_DATA_BUF_SIZE)
				size = SOCKET_DATA_BUF_SIZE;
            ret = recv(WIZCHIP_NET_CH6,buf,size);
            if(ret <= 0)
				return;
			m_uRecvSize[WIZCHIP_NET_CH6] = ret;
			if (m_hrxobser[WIZCHIP_NET_CH6] != NULL)
				m_hrxobser[WIZCHIP_NET_CH6](1);
			////////////////////// 回环测试 ///////////////////////
            //sentsize = 0;
            //while(size != sentsize)
            //{
            //   ret = send(WIZCHIP_NET_CH6,buf+sentsize,size-sentsize);
            //   if(ret < 0)
            //   {
            //      close(WIZCHIP_NET_CH6);
            //      return;
            //   }
            //   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            //}
         }
         break;
      case SOCK_CLOSE_WAIT :
         //DBG(TRACE("%d:CloseWait\r\n",WIZCHIP_NET_CH6));
         if((ret=disconnect(WIZCHIP_NET_CH6)) != SOCK_OK)
		 {
			DBG(TRACE("%d:CloseWait Field\r\n",WIZCHIP_NET_CH6));
			 return;
		 }
         DBG(TRACE("%d:Closed\r\n",WIZCHIP_NET_CH6));
         break;
      case SOCK_INIT :
    	  //DBG(TRACE("%d:Listen, port [%d]\r\n",WIZCHIP_NET_CH6, m_uPort[WIZCHIP_NET_CH6]));
         if( (ret = listen(WIZCHIP_NET_CH6)) != SOCK_OK)
		 {
			 DBG(TRACE("%d:Listen, port [%d] Field\r\n",WIZCHIP_NET_CH6, m_uPort[WIZCHIP_NET_CH6]));
			 return;
		 }
         break;
      case SOCK_CLOSED:
         //DBG(TRACE("%d:LBTStart\r\n",WIZCHIP_NET_CH6));
         if((ret=socket(WIZCHIP_NET_CH6,Sn_MR_TCP,m_uPort[WIZCHIP_NET_CH6],0x00)) != WIZCHIP_NET_CH6)
		 {
			DBG(TRACE("%d:LBTStart Field\r\n",WIZCHIP_NET_CH6));
            return;
		 }
         //DBG(TRACE("%d:Opened\r\n",WIZCHIP_NET_CH6));
         break;
      default:
		  //DBG(TRACE("sn_sr:%d\r\n",sn_sr));
         break;
   }
}

/**
 * @brief UDP Server
 */
static void taskForUdpServer6(void)
{
	UINT8 *buf = m_aDataBuffer[WIZCHIP_NET_CH6];
	UINT16 size = 0;
	//UINT16 sentsize = 0;
	INT32  ret;
   //uint8_t  packinfo = 0;
	UINT8 sn_sr = getSn_SR(WIZCHIP_NET_CH6);
   switch(sn_sr)
   {
      case SOCK_UDP :
         if((size = getSn_RX_RSR(WIZCHIP_NET_CH6)) > 0)
         {
            if(size > SOCKET_DATA_BUF_SIZE)
				size = SOCKET_DATA_BUF_SIZE;
            ret = recvfrom(WIZCHIP_NET_CH6,buf,size,(uint8_t*)m_szDestIp[WIZCHIP_NET_CH6],(uint16_t*)&m_uDestport);
            if(ret <= 0)
            {
               DBG(TRACE("%d: recvfrom error. %ld\r\n",WIZCHIP_NET_CH6,ret));
               return;
            }
			m_uRecvSize[WIZCHIP_NET_CH6] = ret;
			if (m_hrxobser[WIZCHIP_NET_CH6] != NULL)
				m_hrxobser[WIZCHIP_NET_CH6](1);
			////////////////////// 回环测试 ///////////////////////
            //size = (uint16_t) ret;
            //sentsize = 0;
            //while(sentsize != size)
            //{
            //   ret = sendto(WIZCHIP_NET_CH6,buf+sentsize,size-sentsize,(uint8_t*)m_szDestIp[WIZCHIP_NET_CH6],m_uDestport);
            //   if(ret < 0)
            //   {
            //      DBG(TRACE("%d: sendto error. %ld\r\n",WIZCHIP_NET_CH6,ret));
            //      return;
            //   }
            //   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            //}
         }
         break;
      case SOCK_CLOSED:
         //DBG(TRACE("%d:LBUStart\r\n",WIZCHIP_NET_CH6));
         if((ret=socket(WIZCHIP_NET_CH6,Sn_MR_UDP,m_uPort[WIZCHIP_NET_CH6],0x00)) != WIZCHIP_NET_CH6)
		 {
			DBG(TRACE("%d:Opened, port [%d] Field\r\n",WIZCHIP_NET_CH6, m_uPort[WIZCHIP_NET_CH6]));
            return;
		 }
         //DBG(TRACE("%d:Opened, port [%d]\r\n",WIZCHIP_NET_CH6, m_uPort[WIZCHIP_NET_CH6]));
         break;
      default :
		  //DBG(TRACE("sn_sr:%d\r\n",sn_sr));
         break;
   }
}

/**
 * @brief TCP Server
 */
static void taskForTcpClient6(void)
{
	UINT16 anyport=20000;
	UINT8 *buf = m_aDataBuffer[WIZCHIP_NET_CH6];
	UINT16 size = 0;
	//UINT16 sentsize=0;
	INT32 ret;
	UINT8 sn_sr = getSn_SR(WIZCHIP_NET_CH6);
	
	if (m_szServer[WIZCHIP_NET_CH6] != NULL);
	{
	   switch(sn_sr)
	   {
		  case SOCK_ESTABLISHED :
			 if(getSn_IR(WIZCHIP_NET_CH6) & Sn_IR_CON)
			 {
				DBG(TRACE("%d:Connected\r\n",WIZCHIP_NET_CH6));
				setSn_IR(WIZCHIP_NET_CH6,Sn_IR_CON);
			 }
			 if((size = getSn_RX_RSR(WIZCHIP_NET_CH6)) > 0)
			 {
				if(size > SOCKET_DATA_BUF_SIZE)
					size = SOCKET_DATA_BUF_SIZE;
				ret = recv(WIZCHIP_NET_CH6,buf,size);
				if(ret <= 0)
					return;
				m_uRecvSize[WIZCHIP_NET_CH6] = ret;
				if (m_hrxobser[WIZCHIP_NET_CH6] != NULL)
					m_hrxobser[WIZCHIP_NET_CH6](1);
				////////////////////// 回环测试 ///////////////////////
				//sentsize = 0;
				//while(size != sentsize)
				//{
				//   ret = send(WIZCHIP_NET_CH6,buf+sentsize,size-sentsize);
				//   if(ret < 0)
				//   {
				//      close(WIZCHIP_NET_CH6);
				//      return;
				//   }
				//   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
				//}
			 }
			 break;
		  case SOCK_CLOSE_WAIT :
			 //DBG(TRACE("%d:CloseWait\r\n",WIZCHIP_NET_CH6));
			 if((ret=disconnect(WIZCHIP_NET_CH6)) != SOCK_OK)
			 {
				DBG(TRACE("%d:CloseWait Field\r\n",WIZCHIP_NET_CH6));
				 return;
			 }
			 //DBG(TRACE("%d:Closed\r\n",WIZCHIP_NET_CH6));
			 break;
		  case SOCK_INIT :
			 //DBG(TRACE("%d:connet[%d.%d.%d.%d], port [%d]\r\n",WIZCHIP_NET_CH6, m_szServer[WIZCHIP_NET_CH6][0], m_szServer[WIZCHIP_NET_CH6][1], m_szServer[WIZCHIP_NET_CH6][2], m_szServer[WIZCHIP_NET_CH6][3], m_uPort[WIZCHIP_NET_CH6]));
			 if( (ret = connect(WIZCHIP_NET_CH6, m_szServer[WIZCHIP_NET_CH6], m_uPort[WIZCHIP_NET_CH6])) != SOCK_OK)
			 {
				 DBG(TRACE("%d:connet[%d.%d.%d.%d], port [%d] Field\r\n",WIZCHIP_NET_CH6, m_szServer[WIZCHIP_NET_CH6][0], m_szServer[WIZCHIP_NET_CH6][1], m_szServer[WIZCHIP_NET_CH6][2], m_szServer[WIZCHIP_NET_CH6][3],  m_uPort[WIZCHIP_NET_CH6]));
				 return;
			 }
			 break;
		  case SOCK_CLOSED:
			 //DBG(TRACE("%d:LBTStart\r\n",WIZCHIP_NET_CH6));
			 if((ret=socket(WIZCHIP_NET_CH6,Sn_MR_TCP,anyport++,Sn_MR_ND)) != WIZCHIP_NET_CH6)
			 {
				DBG(TRACE("%d:LBTStart Field\r\n",WIZCHIP_NET_CH6));
				return;
			 }
			 //DBG(TRACE("%d:Opened\r\n",WIZCHIP_NET_CH6));
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
static void taskForUdpClient6(void)
{
}

/**
 * @brief TCP Server
 */
static void taskForTcpServer7(void)
{
	UINT8 *buf = m_aDataBuffer[WIZCHIP_NET_CH7];
	UINT16 size = 0;
	//UINT16 sentsize=0;
	INT32 ret;
	UINT8 sn_sr = getSn_SR(WIZCHIP_NET_CH7);
   switch(sn_sr)
   {
      case SOCK_ESTABLISHED :
         if(getSn_IR(WIZCHIP_NET_CH7) & Sn_IR_CON)
         {
            DBG(TRACE("%d:Connected\r\n",WIZCHIP_NET_CH7));
            setSn_IR(WIZCHIP_NET_CH7,Sn_IR_CON);
         }
         if((size = getSn_RX_RSR(WIZCHIP_NET_CH7)) > 0)
         {
            if(size > SOCKET_DATA_BUF_SIZE)
				size = SOCKET_DATA_BUF_SIZE;
            ret = recv(WIZCHIP_NET_CH7,buf,size);
            if(ret <= 0)
				return;
			m_uRecvSize[WIZCHIP_NET_CH7] = ret;
			if (m_hrxobser[WIZCHIP_NET_CH7] != NULL)
				m_hrxobser[WIZCHIP_NET_CH7](1);
			////////////////////// 回环测试 ///////////////////////
            //sentsize = 0;
            //while(size != sentsize)
            //{
            //   ret = send(WIZCHIP_NET_CH7,buf+sentsize,size-sentsize);
            //   if(ret < 0)
            //   {
            //      close(WIZCHIP_NET_CH7);
            //      return;
            //   }
            //   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            //}
         }
         break;
      case SOCK_CLOSE_WAIT :
         //DBG(TRACE("%d:CloseWait\r\n",WIZCHIP_NET_CH7));
         if((ret=disconnect(WIZCHIP_NET_CH7)) != SOCK_OK)
		 {
			DBG(TRACE("%d:CloseWait Field\r\n",WIZCHIP_NET_CH7));
			 return;
		 }
         DBG(TRACE("%d:Closed\r\n",WIZCHIP_NET_CH7));
         break;
      case SOCK_INIT :
    	  //DBG(TRACE("%d:Listen, port [%d]\r\n",WIZCHIP_NET_CH7, m_uPort[WIZCHIP_NET_CH7]));
         if( (ret = listen(WIZCHIP_NET_CH7)) != SOCK_OK)
		 {
			 DBG(TRACE("%d:Listen, port [%d] Field\r\n",WIZCHIP_NET_CH7, m_uPort[WIZCHIP_NET_CH7]));
			 return;
		 }
         break;
      case SOCK_CLOSED:
         //DBG(TRACE("%d:LBTStart\r\n",WIZCHIP_NET_CH7));
         if((ret=socket(WIZCHIP_NET_CH7,Sn_MR_TCP,m_uPort[WIZCHIP_NET_CH7],0x00)) != WIZCHIP_NET_CH7)
		 {
			DBG(TRACE("%d:LBTStart Field\r\n",WIZCHIP_NET_CH7));
            return;
		 }
         //DBG(TRACE("%d:Opened\r\n",WIZCHIP_NET_CH7));
         break;
      default:
		  //DBG(TRACE("sn_sr:%d\r\n",sn_sr));
         break;
   }
}

/**
 * @brief UDP Server
 */
static void taskForUdpServer7(void)
{
	UINT8 *buf = m_aDataBuffer[WIZCHIP_NET_CH7];
	UINT16 size = 0;
	//UINT16 sentsize = 0;
	INT32  ret;
   //uint8_t  packinfo = 0;
	UINT8 sn_sr = getSn_SR(WIZCHIP_NET_CH7);
   switch(sn_sr)
   {
      case SOCK_UDP :
         if((size = getSn_RX_RSR(WIZCHIP_NET_CH7)) > 0)
         {
            if(size > SOCKET_DATA_BUF_SIZE)
				size = SOCKET_DATA_BUF_SIZE;
            ret = recvfrom(WIZCHIP_NET_CH7,buf,size,(uint8_t*)m_szDestIp[WIZCHIP_NET_CH7],(uint16_t*)&m_uDestport);
            if(ret <= 0)
            {
               DBG(TRACE("%d: recvfrom error. %ld\r\n",WIZCHIP_NET_CH7,ret));
               return;
            }
			m_uRecvSize[WIZCHIP_NET_CH7] = ret;
			if (m_hrxobser[WIZCHIP_NET_CH7] != NULL)
				m_hrxobser[WIZCHIP_NET_CH7](1);
			////////////////////// 回环测试 ///////////////////////
            //size = (uint16_t) ret;
            //sentsize = 0;
            //while(sentsize != size)
            //{
            //   ret = sendto(WIZCHIP_NET_CH7,buf+sentsize,size-sentsize,(uint8_t*)m_szDestIp[WIZCHIP_NET_CH7],m_uDestport);
            //   if(ret < 0)
            //   {
            //      DBG(TRACE("%d: sendto error. %ld\r\n",WIZCHIP_NET_CH7,ret));
            //      return;
            //   }
            //   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
            //}
         }
         break;
      case SOCK_CLOSED:
         //DBG(TRACE("%d:LBUStart\r\n",WIZCHIP_NET_CH7));
         if((ret=socket(WIZCHIP_NET_CH7,Sn_MR_UDP,m_uPort[WIZCHIP_NET_CH7],0x00)) != WIZCHIP_NET_CH7)
		 {
			DBG(TRACE("%d:Opened, port [%d] Field\r\n",WIZCHIP_NET_CH7, m_uPort[WIZCHIP_NET_CH7]));
            return;
		 }
         //DBG(TRACE("%d:Opened, port [%d]\r\n",WIZCHIP_NET_CH7, m_uPort[WIZCHIP_NET_CH7]));
         break;
      default :
		  //DBG(TRACE("sn_sr:%d\r\n",sn_sr));
         break;
   }
}

/**
 * @brief TCP Server
 */
static void taskForTcpClient7(void)
{
	UINT16 anyport=20000;
	UINT8 *buf = m_aDataBuffer[WIZCHIP_NET_CH7];
	UINT16 size = 0;
	//UINT16 sentsize=0;
	INT32 ret;
	UINT8 sn_sr = getSn_SR(WIZCHIP_NET_CH7);
	
	if (m_szServer[WIZCHIP_NET_CH7] != NULL);
	{
	   switch(sn_sr)
	   {
		  case SOCK_ESTABLISHED :
			 if(getSn_IR(WIZCHIP_NET_CH7) & Sn_IR_CON)
			 {
				DBG(TRACE("%d:Connected\r\n",WIZCHIP_NET_CH7));
				setSn_IR(WIZCHIP_NET_CH7,Sn_IR_CON);
			 }
			 if((size = getSn_RX_RSR(WIZCHIP_NET_CH7)) > 0)
			 {
				if(size > SOCKET_DATA_BUF_SIZE)
					size = SOCKET_DATA_BUF_SIZE;
				ret = recv(WIZCHIP_NET_CH7,buf,size);
				if(ret <= 0)
					return;
				m_uRecvSize[WIZCHIP_NET_CH7] = ret;
				if (m_hrxobser[WIZCHIP_NET_CH7] != NULL)
					m_hrxobser[WIZCHIP_NET_CH7](1);
				////////////////////// 回环测试 ///////////////////////
				//sentsize = 0;
				//while(size != sentsize)
				//{
				//   ret = send(WIZCHIP_NET_CH7,buf+sentsize,size-sentsize);
				//   if(ret < 0)
				//   {
				//      close(WIZCHIP_NET_CH7);
				//      return;
				//   }
				//   sentsize += ret; // Don't care SOCKERR_BUSY, because it is zero.
				//}
			 }
			 break;
		  case SOCK_CLOSE_WAIT :
			 //DBG(TRACE("%d:CloseWait\r\n",WIZCHIP_NET_CH7));
			 if((ret=disconnect(WIZCHIP_NET_CH7)) != SOCK_OK)
			 {
				DBG(TRACE("%d:CloseWait Field\r\n",WIZCHIP_NET_CH7));
				 return;
			 }
			 //DBG(TRACE("%d:Closed\r\n",WIZCHIP_NET_CH7));
			 break;
		  case SOCK_INIT :
			 //DBG(TRACE("%d:connet[%d.%d.%d.%d], port [%d]\r\n",WIZCHIP_NET_CH7, m_szServer[WIZCHIP_NET_CH7][0], m_szServer[WIZCHIP_NET_CH7][1], m_szServer[WIZCHIP_NET_CH7][2], m_szServer[WIZCHIP_NET_CH7][3], m_uPort[WIZCHIP_NET_CH7]));
			 if( (ret = connect(WIZCHIP_NET_CH7, m_szServer[WIZCHIP_NET_CH7], m_uPort[WIZCHIP_NET_CH7])) != SOCK_OK)
			 {
				 DBG(TRACE("%d:connet[%d.%d.%d.%d], port [%d] Field\r\n",WIZCHIP_NET_CH7, m_szServer[WIZCHIP_NET_CH7][0], m_szServer[WIZCHIP_NET_CH7][1], m_szServer[WIZCHIP_NET_CH7][2], m_szServer[WIZCHIP_NET_CH7][3],  m_uPort[WIZCHIP_NET_CH7]));
				 return;
			 }
			 break;
		  case SOCK_CLOSED:
			 //DBG(TRACE("%d:LBTStart\r\n",WIZCHIP_NET_CH7));
			 if((ret=socket(WIZCHIP_NET_CH7,Sn_MR_TCP,anyport++,Sn_MR_ND)) != WIZCHIP_NET_CH7)
			 {
				DBG(TRACE("%d:LBTStart Field\r\n",WIZCHIP_NET_CH7));
				return;
			 }
			 //DBG(TRACE("%d:Opened\r\n",WIZCHIP_NET_CH7));
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
static void taskForUdpClient7(void)
{
}

#endif //_WIZCHIP_ > 5100

/**
 * @brief W5500 初始化
 * @param hspi SPI接口操作句柄
 * @param netinfo 网络相关信息，如IP、Gateway、MAC ......
 * @param bSetNet 是否重新设置网络参数
 * @retval 初始化成功返回0, 否则返回-1
 */
int wizchip_net_Init(HALSpiTypeDef *hspi, wiz_NetInfo *netinfo, BOOL bSetNet)
{
	UINT8 tmpstr[16];
	uint8_t memsize[2][8] = {{2,2,2,2,2,2,2,2},{2,2,2,2,2,2,2,2}};	
		
	//New(0);
	HalGpioInit(WIZCHIP_CS_GPIO_TYPE, WIZCHIP_CS_GPIO_PIN, HAL_GPIOMode_Out_PP);
	HalGpioInit(WIZCHIP_RST_GPIO_TYPE, WIZCHIP_RST_GPIO_PIN, HAL_GPIOMode_Out_PP);
	HalGpioWriteBit(WIZCHIP_RST_GPIO_TYPE, WIZCHIP_RST_GPIO_PIN, 0);
	delay_ms(100);
	HalGpioWriteBit(WIZCHIP_RST_GPIO_TYPE, WIZCHIP_RST_GPIO_PIN, 1);
	
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
			 DBG(TRACE("WIZCHIP Initialized fail.\r\n"));
			 return -1;
		}
		
		if (bSetNet)
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
	
	for (int i=0; i<8; i++)
	{
		if (pthis[i] != NULL)
		{
			pthis[i]->deInit();
			pthis[i] = NULL;
			vizchip_net_stop(i);
		}
	}
}

static OsalTaskCback_t taskForTcpServer[WIZCHIP_NET_SIZE] = 
{
	taskForTcpServer0,
	taskForTcpServer1,
	taskForTcpServer2,
#if _WIZCHIP_ <= 5100
	taskForTcpServer3
#else
	taskForTcpServer3,
	taskForTcpServer4,
	taskForTcpServer5,
	taskForTcpServer6,
	taskForTcpServer7
#endif
};

static OsalTaskCback_t taskForUdpServer[WIZCHIP_NET_SIZE] = 
{
	taskForUdpServer0,
	taskForUdpServer1,
	taskForUdpServer2,
#if _WIZCHIP_ <= 5100
	taskForUdpServer3
#else
	taskForUdpServer3,
	taskForUdpServer4,
	taskForUdpServer5,
	taskForUdpServer6,
	taskForUdpServer7
#endif
};

static OsalTaskCback_t taskForTcpClient[WIZCHIP_NET_SIZE] = 
{
	taskForTcpClient0,
	taskForTcpClient1,
	taskForTcpClient2,
#if _WIZCHIP_ <= 5100
	taskForTcpClient3
#else
	taskForTcpClient3,
	taskForTcpClient4,
	taskForTcpClient5,
	taskForTcpClient6,
	taskForTcpClient7
#endif
};

static OsalTaskCback_t taskForUdpClient[WIZCHIP_NET_SIZE] = 
{
	taskForUdpClient0,
	taskForUdpClient1,
	taskForUdpClient2,
#if _WIZCHIP_ <= 5100
	taskForUdpClient3
#else
	taskForUdpClient3,
	taskForUdpClient4,
	taskForUdpClient5,
	taskForUdpClient6,
	taskForUdpClient7
#endif
};

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
	if ((NULL == m_hspi) || (sn >= WIZCHIP_NET_SIZE))
		return -1;
	
	m_uPort[sn] = (port > 0) ? port : 502;
	
	DBG(TRACE("start channel %d with port %d\r\n", sn, m_uPort[sn]));
#define TASK_NET_TIME 1
	
	switch (mode)
	{
	case SOCK_TCP_SERVER:
		m_uMode[sn] = mode;
		m_szServer[sn] = NULL;
		osal_task_create(taskForTcpServer[sn], TASK_NET_TIME);
		DBG(TRACE("start %d tcp service\r\n", sn));
		break;
	case SOCK_UDP_SERVER:
		m_uMode[sn] = mode;
		m_szServer[sn] = NULL;
		osal_task_create(taskForUdpServer[sn], TASK_NET_TIME);
		DBG(TRACE("start %d udp service\r\n", sn));
		break;
	case SOCK_TCP_CLIENT:
		m_uMode[sn] = mode;
		m_szServer[sn] = server;
		osal_task_create(taskForTcpClient[sn], TASK_NET_TIME);
		DBG(TRACE("start %d tcp client\r\n", sn));
		break;
	case SOCK_UDP_CLIENT:
		m_uMode[sn] = mode;
		m_szServer[sn] = server;
		//osal_task_create(taskForUdpClient[sn], TASK_NET_TIME);
		osal_task_create(taskForUdpServer[sn], TASK_NET_TIME);
		DBG(TRACE("start %d udp client\r\n", sn));
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
int vizchip_net_stop(UINT8 sn)
{
	switch (m_uMode[sn])
	{
	case SOCK_TCP_SERVER:
		osal_task_kill(taskForTcpServer[sn]);
		break;
	case SOCK_UDP_SERVER:
		osal_task_kill(taskForUdpServer[sn]);
		break;
	case SOCK_TCP_CLIENT:
		osal_task_kill(taskForTcpClient[sn]);
		break;
	case SOCK_UDP_CLIENT:
		osal_task_kill(taskForUdpClient[sn]);
		break;
	default:
		break;
	}
	return 0;
}

/**
 * @brief: 获取指定端口串口通讯句柄
 * @param sn 端口号
 * @retval: 指定端口串口通讯句柄
 */
HALUartTypeDef* vizchip_net_getinstance(UINT8 sn)
{
	if (NULL == pthis[sn])
	{
		New(sn);
	}
	
	return pthis[sn];
}

#endif //CFG_USE_NET
