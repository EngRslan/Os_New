#include <kernel/net/udp.h>
#include <kernel/net/addr.h>
#include <kernel/net/ip.h>
#include <kernel/mem/kheap.h>
#include <kernel/types.h>
#include <kernel/bits.h>
#include <string.h>
#include <logger.h>


void udp_send_packet(ipv4_addr_t * ip,uint16_t sport,uint16_t dport,ptr_t data,uint32_t len){
    udp_header_t * packet = (udp_header_t *)kmalloc(sizeof(udp_header_t));
    uint32_t total_size = sizeof(udp_header_t) + len;
    memset((ptr_t)packet,0,total_size);

    packet->dst_port = SWITCH_ENDIAN16(dport);
    packet->src_port = SWITCH_ENDIAN16(sport);
    packet->length = SWITCH_ENDIAN16(len);
    packet->checksum = 0;

    memcpy((ptr_t)packet+sizeof(udp_header_t),data,len);
    log_trace("UDP Packet Sent from port: %d to port: %d",(uint32_t)sport,(uint32_t)dport);
    ip_send_packet(ip,(ptr_t)packet,total_size);
}