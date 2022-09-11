#ifndef UDP_H
#define UDP_H
#include <kernel/types.h>
#include <kernel/net/addr.h>

typedef struct udp_header
{
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
} __attribute__((packed)) udp_header_t;

void udp_send_packet(ipv4_addr_t * ip,uint16_t sport,uint16_t dport,ptr_t data,uint32_t len);
#endif