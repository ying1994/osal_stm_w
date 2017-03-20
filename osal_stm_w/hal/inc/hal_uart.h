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
 * All rights res
 *
 */
#ifndef _HAL_UART_H
#define _HAL_UART_H

#include "hal_board.h"
#include "hal_types.h"

#ifdef CFG_HAL_UART



/** @defgroup USART����λ
 * @{
 */ 
#define HAL_UART_WordLength_8b USART_WordLength_8b	/*!< 8λ����λ */
#define HAL_UART_WordLength_9b USART_WordLength_9b	/*!< 9λ����λ */
/**
 * @}
 */ 

/** @defgroup USARTֹͣλ 
 * @{
 */ 
#define HAL_UART_StopBits_1   USART_StopBits_1   	/*!< 1λֹͣλ */
#define HAL_UART_StopBits_0_5 USART_StopBits_0_5 	/*!< 0.5λֹͣλ */
#define HAL_UART_StopBits_2   USART_StopBits_2   	/*!< 2λֹͣλ */
#define HAL_UART_StopBits_1_5 USART_StopBits_1_5 	/*!< 1.5λֹͣλ */
/**
 * @}
 */

/** @defgroup USARTУ��λ
 * @{
 */                   
#define HAL_UART_Parity_No    USART_Parity_No    	/*!< ��У�� */ 
#define HAL_UART_Parity_Even  USART_Parity_Even  	/*!< żУ�� */ 
#define HAL_UART_Parity_Odd   USART_Parity_Odd   	/*!< ��У�� */ 
/**
 * @}
 */


/**
 * @brief ���ڶ˿ں�
 */
typedef enum 
{
	HAL_UART1 = 0,	/*!< UART1 */
	HAL_UART2,		/*!< UART2 */
	HAL_UART3,		/*!< UART3 */
	HAL_UART4,		/*!< UART4 */
	HAL_UART5,		/*!< UART5 */
	HAL_UART_SIZE	/*!< UART �˿����� */
}HALUartNumer;


/**
 * @brief: UART���չ۲���ԭ��
 * @param data: ���յ�������
 * @retval None
 */
typedef void (*HalUartCBack_t)(UCHAR data);

/**
 * @brief ���ڲ����ṹ����
 */
typedef struct _HALUartTypeDef
{
	/**
	 * @brief: ��ʼ������ͨѶ�˿�
	 * @param: void
	 * @retval: void
	 */
	void (*init)(void);
	
	/**
	 * @brief ��Դע��
	 * @param None
	 * @retval None
	 */
	void (*deInit)(void);
	
	/**
	 * @brief: ע�ᴮ�ڽ��չ۲���
	 * @param hrxobser: ���ڽ��չ۲��߾��
	 * @retval: void
	 */
	void (*add_rx_obser)(HalUartCBack_t hrxobser);
	
	/**
	 * @brief: ���ô���ͨѶ������
	 * @param baudrate: ����ͨѶ������
	 * @retval: void
	 */
	void (*set_baudrate)(UINT32 baudrate);
	
	/**
	 * @brief: ���ô���ͨѶ�ֳ�
	 * @param wordlength: ����ͨѶ�ֳ�
	 * @retval: void
	 */
	void (*set_wordlength)(UINT16 wordlength);
	
	/**
	 * @brief: ���ô���ͨѶֹͣλ
	 * @param stopbit: ����ͨѶֹͣλ
	 * @retval: void
	 */
	void (*set_stopbit)(UINT16 stopbit);
	
	/**
	 * @brief: ���ô���ͨѶУ��λ
	 * @param parity: ����ͨѶУ��λ
	 * @retval: void
	 */
	void (*set_parity)(UINT16 parity);
	
	/**
	 * @brief: �򴮿�д��һ������
	 * @param *pdata: д�����ݴ洢��ַָ��
	 * @param len: д�����ݳ���
	 * @retval: void
	 */
	void (*write)(UCHAR *pdata, UINT16 len);
}HALUartTypeDef;


/**
 * @brief: ��ȡָ���˿ڴ���ͨѶ���
 * @param numer: ���ڶ˿ں�
 * @retval: ָ���˿ڴ���ͨѶ���
 */
HALUartTypeDef* hal_uart_getinstance(HALUartNumer numer);

#endif //CFG_HAL_UART
#endif
