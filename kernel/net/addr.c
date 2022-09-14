#include <kernel/net/addr.h>
#include <kernel/net/ethernet.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

struct AssignedIpAddress defaultAssignedIpAddress;
MacAddress g__broadcastMacAddress = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
Ipv4Address g__broadcastIpAddress = { 0xFF, 0xFF, 0xFF, 0xFF};
// void ip2str(string_t str,ipv4_addr_t * ip){
//     sprintf(str,"%d.%d.%d.%d",(uint32_t)ip->n[0],(uint32_t)ip->n[1],(uint32_t)ip->n[2],(uint32_t)ip->n[3]);
// }
// void mac2str(string_t str,eth_addr_t * mac){
//     sprintf(str,"%x:%x:%x:%x:%x:%x",(uint32_t)mac->n[0],(uint32_t)mac->n[1],(uint32_t)mac->n[2],(uint32_t)mac->n[3],(uint32_t)mac->n[4],(uint32_t)mac->n[5]);
// }

void MacToStr(char *str,MacAddress mac){
    sprintf(str,"%x:%x:%x:%x:%x:%x",(uint32_t)(mac[0]),(uint32_t)(mac[1]),(uint32_t)(mac[2]),(uint32_t)(mac[3]),(uint32_t)(mac[4]),(uint32_t)(mac[5]));
}

void CopyMacAddress(MacAddress src, MacAddress dst){
    memcpy((void *)dst,(void *)src,sizeof(MacAddress));
}

bool IsMacAddressEquals(MacAddress left, MacAddress right){
    return memcmp((void *)left,(void *)right,sizeof(MacAddress)) == 0;
}

void CopyIpv4Address(Ipv4Address src, Ipv4Address dst){
    memcpy((void *)dst,(void *)src,sizeof(Ipv4Address));
}

bool IsIpv4AddressEquals(Ipv4Address left, Ipv4Address right){
    return memcmp((void *)left,(void *)right,sizeof(Ipv4Address)) == 0;
}
void Ipv4ToStr(char *str,Ipv4Address ip)
{
    sprintf(str,"%d.%d.%d.%d",(uint32_t)ip[0],(uint32_t)ip[1],(uint32_t)ip[2],(uint32_t)ip[3]);
}