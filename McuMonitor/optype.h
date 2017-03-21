/**
 * @file    bd_optype.h
 * @author  WSF
 * @version V1.0.0
 * @date    2014.09.11
 * @brief   实现数据操作的操作类型和操作状态定义，包括数据操作的功能ID定义和
 *          操作方法定义
 ******************************************************************************
 * @attention
 *
 */
#ifndef BD_OPTYPE_H
#define BD_OPTYPE_H

/**
 * @brief 数据操作类型定义
 */
typedef enum _OperationType
{
    OP_STATUS	= 0x00,		/*!< 数据操作状态 */
    OP_SET		= 0x01,		/*!< 参数设置操作 */
    OP_GET		= 0x02,		/*!< 参数数据读取操作 */
    OP_SETGET	= 0x03,		/*!< 参数设置及读取操作 */
    OP_ERROR	= 0xff		/*!< 操作执行出错 */
}OperationType;



#endif // BD_OPTYPE_H

