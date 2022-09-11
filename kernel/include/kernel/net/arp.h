#ifndef ARP_H
#define ARP_H
#include <kernel/types.h>
#include <kernel/net/addr.h>

typedef struct arp_header
{
    uint16_t hw_type;
    uint16_t protocol;
    uint8_t hw_addr_len;
    uint8_t protocol_addr_len;
    uint16_t opcode;
    eth_addr_t src_hw_addr;
    ipv4_addr_t src_protocol_addr;
    eth_addr_t dst_hw_addr;
    ipv4_addr_t dst_protocol_addr;
} __attribute__((packed)) arp_header_t;

void arp_send_packet(eth_addr_t * dst_hw_addr,ipv4_addr_t * dst_protocol_addr);
void arp_handle_packet(arp_header_t * packet,uint32_t len);
struct arp_entry *arp_lookup(ipv4_addr_t * ip);

#endif