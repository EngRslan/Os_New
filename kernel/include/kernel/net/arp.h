#ifndef ARP_H
#define ARP_H
#include <kernel/types.h>
#include <kernel/net/addr.h>
#include <stdbool.h>

typedef enum {
    Request = 1,
    Replay = 2
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

void arpSend(MacAddress *dstMacAddr,Ipv4Address *dstIpAddr,ArpOpCode opCode);
void arpReceive(NetBuffer *packet_buffer);
ArpEntry *arp_lookup(Ipv4Address *ip);

#endif