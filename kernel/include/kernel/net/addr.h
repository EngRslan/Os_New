#ifndef ADDR_H
#define ADDR_H
#include <kernel/types.h>
#include <stdbool.h>

typedef uint8_t MacAddress[6];
typedef uint8_t Ipv4Address[4];

extern MacAddress g__broadcastMacAddress;
extern Ipv4Address g__broadcastIpAddress;

typedef enum {
    IP_METHOD_NONE      = 0,
    IP_METHOD_STATIC    = 1,
    IP_METHOD_DHCP      = 2,
} IpAssignMethod;

struct AssignedIpAddress {
    IpAssignMethod assignMethod;
    Ipv4Address Ip;
    uint8_t subnet;
    Ipv4Address gateway;
};

extern struct AssignedIpAddress defaultAssignedIpAddress;
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


void CopyMacAddress(MacAddress src, MacAddress dst);
bool IsMacAddressEquals(MacAddress left, MacAddress right);
void MacToStr(char *str,MacAddress mac);
void Ipv4ToStr(char *str,Ipv4Address ip);
bool IsIpv4AddressEquals(Ipv4Address left, Ipv4Address right);
void CopyIpv4Address(Ipv4Address src, Ipv4Address dst);

#endif