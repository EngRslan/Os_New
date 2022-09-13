#ifndef ETHERNET_H
#define ETHERNET_H
#include <kernel/types.h>
#include <kernel/net/addr.h>
#include <kernel/net/intf.h>

// /* Ethernet protocol ID's */
// #define	ETHERTYPE_PUP		0x0200          /* Xerox PUP */
// #define ETHERTYPE_SPRITE	0x0500		/* Sprite */
// #define	ETHERTYPE_IP		0x0800		/* IP */
// #define	ETHERTYPE_ARP		0x0806		/* Address resolution */
// #define	ETHERTYPE_REVARP	0x8035		/* Reverse ARP */
// #define ETHERTYPE_AT		0x809B		/* AppleTalk protocol */
// #define ETHERTYPE_AARP		0x80F3		/* AppleTalk ARP */
// #define	ETHERTYPE_VLAN		0x8100		/* IEEE 802.1Q VLAN tagging */
// #define ETHERTYPE_IPX		0x8137		/* IPX */
// #define	ETHERTYPE_IPV6		0x86dd		/* IP protocol version 6 */
// #define ETHERTYPE_LOOPBACK	0x9000		/* used to test interfaces */
typedef enum {
    ETHERTYPE_PUP       = 0x0200,
    ETHERTYPE_SPRITE    = 0x0500,
    ETHERTYPE_IP        = 0x0800,
    ETHERTYPE_ARP       = 0x0806,
    ETHERTYPE_REVARP    = 0x8035,
    ETHERTYPE_AT        = 0x809B,
    ETHERTYPE_AARP      = 0x80F3,
    ETHERTYPE_VLAN      = 0x8100,
    ETHERTYPE_IPX       = 0x8137,
    ETHERTYPE_IPV6      = 0x86dd,
    ETHERTYPE_LOOPBACK  = 0x9000,
} EthernetType;

// struct ether_header{
//     uint8_t ether_dhost[6];
//     uint8_t ether_shost[6];
//     uint16_t type;
// } __attribute__ ((__packed__));



typedef struct{
    MacAddress destHost;
    MacAddress srcHost;
    EthernetType type;
} __attribute__ ((__packed__)) EthernetHeader;


void EthernetSend(NetBuffer *packet_buffer,MacAddress dstAddress,EthernetType et_type);
void EthernetReceive(NetBuffer *packet_buffer);
// void ethernet_handle_packet(struct ether_header * packet,int len);
// void ethernet_send_packet(uint8_t ether_dhost[6],ptr_t data,uint32_t len,uint32_t protocol);
#endif