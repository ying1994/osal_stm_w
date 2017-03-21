#include "osal_socket.h"

//大小端模式判断
static union { char c[4]; unsigned long l; } endian_test = { { 'l', '?', '?', 'b' } };
#define ENDIANNESS ((char)endian_test.l)

UINT16 bd_sock_ntohs(UINT16 netshort)
{
    if (ENDIANNESS == 'l')//little endian
    {
        UINT16 hostshort = 0;
        hostshort = (netshort) & 0xff;
        hostshort = (hostshort << 8) | ((netshort >> 8) & 0xff);
        return hostshort;
    }
    else				//big endian
    {
        return netshort;
    }
}

UINT16 bd_sock_htons(UINT16 hostshort)
{
    if (ENDIANNESS == 'l')//little endian
    {
        UINT16 netshort = 0;
        netshort = (hostshort) & 0xff;
        netshort = (netshort << 8) | ((hostshort >> 8) & 0xff);
        return netshort;
    }
    else				//big endian
    {
        return hostshort;
    }
}

UINT32 bd_sock_ntohl(UINT32 netlong)
{
    if (ENDIANNESS == 'l')//little endian
    {
        UINT32 hostlong = 0;
        hostlong = (netlong) & 0xff;
        hostlong = (hostlong << 8) | ((netlong >> 8) & 0xff);
        hostlong = (hostlong << 8) | ((netlong >> 16) & 0xff);
        hostlong = (hostlong << 8) | ((netlong >> 24) & 0xff);
        return hostlong;
    }
    else				//big endian
    {
        return netlong;
    }
}

UINT32 bd_sock_htonl(UINT32 hostlong)
{
    if (ENDIANNESS == 'l')//little endian
    {
        UINT32 netlong = 0;
        netlong = (hostlong) & 0xff;
        netlong = (netlong << 8) | ((hostlong >> 8) & 0xff);
        netlong = (netlong << 8) | ((hostlong >> 16) & 0xff);
        netlong = (netlong << 8) | ((hostlong >> 24) & 0xff);
        return netlong;
    }
    else				//big endian
    {
        return hostlong;
    }
}

