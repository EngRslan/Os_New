#ifndef DHCP_H
#define DHCP_H
#include <kernel/types.h>

typedef struct dhcp_header {
    uint8_t op;
    uint8_t hw_type;
    uint8_t hw_addr_len;
    uint8_t hops;
    uint32_t xid;
    uint16_t seconds;
    uint16_t flags;
    uint32_t client_ip;
    uint32_t yourip;
    uint32_t server_ip;
    uint32_t gateway_ip;
    uint8_t client_hw_addrp[16];
    uint8_t server_name[64];
    uint8_t file[128];
} __attribute__((packed)) dhcp_header_t;


#endif