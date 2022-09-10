#ifndef IP_H
#define IP_H
#include <kernel/types.h>

typedef struct ipv4_header
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
        uint16_t flags_fragment;
        struct
        {
            uint8_t flags:3;
            uint8_t fragment_offset_high:5;
            uint8_t fragment_offset_low;
        };
        
    };
    uint8_t ttl;
    uint8_t protocol;
    uint16_t checksum;
    uint8_t src_ip[4];
    uint8_t dst_ip[4];
    
}__attribute__((packed)) ipv4_header_t;

#endif