#ifndef INTF_H
#define INTF_H
#include <kernel/types.h>
#include <kernel/net/addr.h>

typedef struct NetBuffer NetBuffer;
typedef struct NetInterface NetInterface;

struct NetBuffer
{
    void    *packetData;
    uint32_t length;
    NetInterface *interface;
};

struct NetInterface{
    MacAddress macAddress;
    void(*send)(NetBuffer *);
    char name[50];
} ;

#endif