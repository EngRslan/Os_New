#include <kernel/net/ip.h>
#include <kernel/mem/kheap.h>
#include <string.h>

void ip_send_packet(uint8_t dst_ip[6], ptr_t * data,uint32_t len){
    ipv4_header_t * packet = kmalloc(sizeof(ipv4_header_t)+len);
    memset(packet,0,sizeof(ipv4_header_t));

    packet->version = 4;
    packet->ihl = 5;
    packet->tos = 0;
    packet->length = sizeof(ipv4_header_t)+len;
    packet->ttl = 64;
    packet->protocol = 17;
    packet->src_ip[0] = 192;
    packet->src_ip[1] = 168;
    packet->src_ip[2] = 76;
    packet->src_ip[3] = 10;
    
    packet->dst_ip[0] = 192;
    packet->dst_ip[1] = 168;
    packet->dst_ip[2] = 76;
    packet->dst_ip[3] = 2;
    
}