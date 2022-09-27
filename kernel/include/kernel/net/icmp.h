#ifndef ICMP_H
#define ICMP_H
#include <kernel/net/intf.h>
#include <kernel/types.h>
typedef enum IcmpEchoType{
    ICMP_TYPE_ECHO_REQUEST = 0x08,
    ICMP_TYPE_ECHO_REPLAY = 0x00
}IcmpEchoType;
typedef struct IcmpEchoReply
{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t id;
    uint16_t seq;
}__attribute__((packed))IcmpEchoReply;


void IcmpReceive(NetBuffer *netbuffer,Ipv4Address srcip);
#endif