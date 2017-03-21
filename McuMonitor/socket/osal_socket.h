#ifndef BD_SOCK_H
#define BD_SOCK_H
#include "types.h"


UINT16 bd_sock_ntohs(UINT16 netshort);

UINT16 bd_sock_htons(UINT16 hostshort);

UINT32 bd_sock_ntohl(UINT32 netlong);

UINT32 bd_sock_htonl(UINT32 hostlong);


#endif // BD_SOCK_H
