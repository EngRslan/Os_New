#ifndef ARP_H
#define ARP_H
#include <kernel/types.h>
#include <kernel/net/addr.h>
#include <kernel/net/intf.h>
#include <stdbool.h>
#define ARP_ETH_TYPE 0x01

typedef enum {
    ARP_OP_REQUEST = 1,
    ARP_OP_REPLAY = 2
} ArpOpCode;

typedef struct{
    bool isPresent;
    Ipv4Address ip;
    MacAddress mac;
} ArpEntry;

typedef struct
{
    uint16_t hwType;
    uint16_t protocol;
    uint8_t MacAddressLen;
    uint8_t IpAddrLen;
    uint16_t opCode;
    MacAddress srcMacAddr;
    Ipv4Address srcIpAddr;
    MacAddress dstMacAddr;
    Ipv4Address dstIpAddr;
} __attribute__((packed)) ArpHeader;

void ArpSend(NetInterface *intf, MacAddress dstMacAddr,Ipv4Address dstIpAddr,ArpOpCode opCode);
void ArpReceive(NetBuffer *packet_buffer);
uint8_t *Arp_lookup(NetInterface *intf, Ipv4Address ip);

#endif