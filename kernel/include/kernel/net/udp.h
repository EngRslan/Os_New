#ifndef UDP_H
#define UDP_H
#include <kernel/types.h>
#include <kernel/net/addr.h>
#include <kernel/net/intf.h>

typedef struct
{
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
} __attribute__((packed)) UdpHeader;

typedef void (*UdpHandler)(NetBuffer *);
void UdpSend(NetBuffer *netbuffer, Ipv4Address ip,uint16_t src_port,uint16_t dst_port);
void UdpReceive(NetBuffer * netBuffer);
void UdpRegisterHandler(uint16_t port,UdpHandler handler);
#endif