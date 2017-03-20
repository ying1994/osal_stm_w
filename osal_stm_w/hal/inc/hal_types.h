/**
  @file:    hal_types.h
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
#ifndef _HAL_TYPES_H
#define _HAL_TYPES_H

/* Texas Instruments CC2540 */

/* ------------------------------------------------------------------------------------------------
 *                                               Types
 * ------------------------------------------------------------------------------------------------
 */
/** @defgroup HAL_TYPES HAL Types
 * @{
 */
typedef signed   char   int8;     //!< Signed 8 bit integer
typedef unsigned char   uint8;    //!< Unsigned 8 bit integer

typedef signed   short  int16;    //!< Signed 16 bit integer
typedef unsigned short  uint16;   //!< Unsigned 16 bit integer

typedef signed   long   int32;    //!< Signed 32 bit integer
typedef unsigned long   uint32;   //!< Unsigned 32 bit integer

typedef unsigned char   bool;     //!< Boolean data type

typedef uint8           halDataAlign_t; //!< Used for byte alignment

typedef      void VOID;

typedef unsigned char     BYTE;		//!< BYTE
typedef signed   char     CHAR;		//!< Signed 8 bit integer
typedef unsigned char     UCHAR;	//!< Unsigned 8 bit integer
typedef signed   char     INT8;		//!< Signed 8 bit integer
typedef unsigned char     UINT8;	//!< Unsigned 8 bit integer
typedef signed   short    INT16;	//!< Signed 16 bit integer
typedef unsigned short    UINT16;	//!< Unsigned 16 bit integer
typedef signed   int      INT;		//!< Signed 16 bit integer
typedef unsigned int      UINT;		//!< Unsigned 16 bit integer
typedef signed   long     INT32;	//!< Signed 32 bit integer
typedef unsigned long     UINT32;	//!< Unsigned 32 bit integer
typedef signed   long     LONG;		//!< Signed 32 bit integer
typedef unsigned long     ULONG;	//!< Unsigned 32 bit integer
typedef unsigned __int64  UINT64;	//!< Unsigned 64 bit integer
typedef unsigned char	  BOOL;

typedef void* 			  HANDLE;

typedef void (*HAL_BASE_FUNC)(void);


/** @} End HAL_TYPES */

/* ------------------------------------------------------------------------------------------------
 *                               Memory Attributes and Compiler Macros
 * ------------------------------------------------------------------------------------------------
 */

/* ----------- IAR Compiler ----------- */
#ifdef __IAR_SYSTEMS_ICC__
#define ASM_NOP    asm("NOP")

/* ----------- KEIL Compiler ----------- */
#elif defined __KEIL__
#define ASM_NOP   __nop()

/* ----------- GNU Compiler ----------- */
#elif defined __GNUC__
#define ASM_NOP __asm__ __volatile__ ("nop")

/* ---------- MSVC compiler ---------- */
#elif _MSC_VER
#define ASM_NOP __asm NOP

/* ----------- CCS Compiler ----------- */
#elif defined __TI_COMPILER_VERSION
#define ASM_NOP    asm(" NOP")

/* ----------- Unrecognized Compiler ----------- */
#else
#error "ERROR: Unknown compiler."
#endif


/* ------------------------------------------------------------------------------------------------
 *                                        Standard Defines
 * ------------------------------------------------------------------------------------------------
 */
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef NULL
#define NULL (VOID*)0
#endif


#ifndef BIT
#define BIT(x) (1 << (x))
#endif

#ifndef CONST
#define CONST const
#endif

/**************************************************************************************************
 */
#endif
