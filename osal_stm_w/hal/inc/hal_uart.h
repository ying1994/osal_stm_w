/**
 * @file    hal_uart.h
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
 * All rights Reserved
 *
 */
#ifndef _HAL_UART_H
#define _HAL_UART_H

#include "hal_cfg.h"
#include "hal_types.h"

#ifdef CFG_HAL_UART



/** @defgroup USART数据位
 * @{
 */ 
#define HAL_UART_WordLength_8b USART_WordLength_8b	/*!< 8位数据位 */
#define HAL_UART_WordLength_9b USART_WordLength_9b	/*!< 9位数据位 */
/**
 * @}
 */ 

/** @defgroup USART停止位 
 * @{
 */ 
#define HAL_UART_StopBits_1   USART_StopBits_1   	/*!< 1位停止位 */
#define HAL_UART_StopBits_0_5 USART_StopBits_0_5 	/*!< 0.5位停止位 */
#define HAL_UART_StopBits_2   USART_StopBits_2   	/*!< 2位停止位 */
#define HAL_UART_StopBits_1_5 USART_StopBits_1_5 	/*!< 1.5位停止位 */
/**
 * @}
 */

/** @defgroup USART校验位
 * @{
 */                   
#define HAL_UART_Parity_No    USART_Parity_No    	/*!< 无校验 */ 
#define HAL_UART_Parity_Even  USART_Parity_Even  	/*!< 偶校验 */ 
#define HAL_UART_Parity_Odd   USART_Parity_Odd   	/*!< 奇校验 */ 
/**
 * @}
 */


/**
 * @brief 串口端口号
 */
typedef enum 
{
	HAL_UART1 = 0,	/*!< UART1 */
	HAL_UART2,		/*!< UART2 */
	HAL_UART3,		/*!< UART3 */
	HAL_UART4,		/*!< UART4 */
	HAL_UART5,		/*!< UART5 */
	HAL_UART_SIZE	/*!< UART 端口总数 */
}HALUartNumer;


/**
 * @brief: UART接收观察者原型
 * @param data: 接收到的数据
 * @retval None
 */
typedef void (*HalUartCBack_t)(UCHAR data);

/**
 * @brief 串口操作结构定义
 */
typedef struct _HALUartTypeDef
{
	/**
	 * @brief: 初始化串口通讯端口
	 * @param: void
	 * @retval: void
	 */
	void (*init)(void);
	
	/**
	 * @brief 资源注销
	 * @param None
	 * @retval None
	 */
	void (*deInit)(void);
	
	/**
	 * @brief: 注册串口接收观察者
	 * @param hrxobser: 串口接收观察者句柄
	 * @retval: void
	 */
	void (*add_rx_obser)(HalUartCBack_t hrxobser);
	
	/**
	 * @brief: 设置串口通讯波特率
	 * @param baudrate: 串口通讯波特率
	 * @retval: void
	 */
	void (*set_baudrate)(UINT32 baudrate);
	
	/**
	 * @brief: 设置串口通讯字长
	 * @param wordlength: 串口通讯字长
	 * @retval: void
	 */
	void (*set_databits)(UINT16 databits);
	
	/**
	 * @brief: 设置串口通讯停止位
	 * @param stopbit: 串口通讯停止位
	 * @retval: void
	 */
	void (*set_stopbit)(UINT16 stopbit);
	
	/**
	 * @brief: 设置串口通讯校验位
	 * @param parity: 串口通讯校验位
	 * @retval: void
	 */
	void (*set_parity)(UINT16 parity);
	
	/**
	 * @brief: 向串口读取一组数据
	 * @param pdata: 数据存储地址指针
	 * @param len: 读取的数据长度
	 * @retval: 实际读取的数据长度
	 */
	UINT16 (*read)(UCHAR *pdata, UINT16 len);
	/**
	 * @brief: 向串口写入一组数据
	 * @param pdata: 写入数据存储地址指针
	 * @param len: 写入数据长度
	 * @retval: void
	 */
	void (*write)(UCHAR *pdata, UINT16 len);
}HALUartTypeDef;


/**
 * @brief: 获取指定端口串口通讯句柄
 * @param numer: 串口端口号
 * @retval: 指定端口串口通讯句柄
 */
HALUartTypeDef* hal_uart_getinstance(HALUartNumer numer);

#endif //CFG_HAL_UART
#endif
