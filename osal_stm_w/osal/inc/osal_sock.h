/**
 * @file    osal_sock.h
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

#ifndef OSAL_SOCK_H
#define OSAL_SOCK_H
#include "types.h"


UINT16 osal_sock_ntohs(UINT16 netshort);

UINT16 osal_sock_htons(UINT16 hostshort);

UINT32 osal_sock_ntohl(UINT32 netlong);

UINT32 osal_sock_htonl(UINT32 hostlong);


#endif
