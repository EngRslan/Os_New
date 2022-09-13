#ifndef IP_H
#define IP_H
#include <kernel/types.h>
#include <kernel/net/addr.h>
#include <kernel/net/intf.h>

typedef struct
{
    union {
        uint8_t ver_ihl;
        struct
        {
            uint8_t version :4;
            uint8_t ihl:4;
        };
    };
    uint8_t tos;
    uint16_t length;
    uint16_t id;
    union 
    {
        uint8_t flags_fragment;
        struct
        {
            uint8_t flags:3;
            uint8_t fragment_offset_high:5;
        };
        
    };
    uint8_t fragment_offset_low;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    Ipv4Address src_ip;
    Ipv4Address dst_ip;
    
} __attribute__((packed)) Ipv4Header;

void IpSend(NetBuffer *netbuffer, Ipv4Address ip);
#endif