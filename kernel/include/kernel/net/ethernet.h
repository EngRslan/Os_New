#ifndef ETHERNET_H
#define ETHERNET_H
#include <kernel/types.h>
#include <kernel/net/addr.h>
#include <kernel/net/intf.h>

typedef enum {
    ETHERTYPE_PUP       = 0x0200,/* Xerox PUP */
    ETHERTYPE_SPRITE    = 0x0500,/* Sprite */
    ETHERTYPE_IP        = 0x0800,/* IP */
    ETHERTYPE_ARP       = 0x0806,/* Address resolution */
    ETHERTYPE_REVARP    = 0x8035,/* Reverse ARP */
    ETHERTYPE_AT        = 0x809B,/* AppleTalk protocol */
    ETHERTYPE_AARP      = 0x80F3,/* AppleTalk ARP */
    ETHERTYPE_VLAN      = 0x8100,/* IEEE 802.1Q VLAN tagging */
    ETHERTYPE_IPX       = 0x8137,/* IPX */
    ETHERTYPE_IPV6      = 0x86dd,/* IP protocol version 6 */
    ETHERTYPE_LOOPBACK  = 0x9000,/* used to test interfaces */
} EthernetType;

typedef void (*EthernetHandler)(NetBuffer *);

typedef struct{
    MacAddress destHost;
    MacAddress srcHost;
    uint16_t type;
} __attribute__ ((__packed__)) EthernetHeader;


void EthernetSend(NetBuffer *packet_buffer,MacAddress dstAddress,EthernetType et_type);
void EthernetReceive(NetBuffer *packet_buffer);
void EthernetRegisterProtocol(EthernetType etherType,EthernetHandler handler);
#endif