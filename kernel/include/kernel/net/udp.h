#ifndef UDP_H
#define UDP_H
#include <kernel/types.h>

typedef struct udp_header
{
    uint16_t src_port;
    uint16_t dst_port;
    uint16_t length;
    uint16_t checksum;
} __attribute__((packed)) udp_header_t;


#endif