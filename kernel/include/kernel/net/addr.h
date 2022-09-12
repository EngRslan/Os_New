#ifndef ADDR_H
#define ADDR_H
#include <kernel/types.h>
#include <stdbool.h>

typedef uint8_t MacAddress[6];
extern MacAddress g__broadcastMacAddress;
typedef struct
{
    union
    {
        uint8_t n[4];
        uint32_t bits;
    };
     
} __attribute__((packed)) ipv4_addr_t;

typedef struct
{
    uint8_t n[6];
}__attribute__((packed)) eth_addr_t;


void ip2str(string_t str,ipv4_addr_t * ip);
void mac2str(string_t str,eth_addr_t * mac);
void CopyMacAddress(MacAddress * src, MacAddress * dst);
bool IsMacAddressEquals(MacAddress * left, MacAddress * right);
void MacToStr(char *str,MacAddress mac);
#endif