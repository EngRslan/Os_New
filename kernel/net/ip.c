#include <kernel/net/ip.h>
#include <kernel/net/arp.h>
#include <kernel/net/addr.h>
#include <kernel/net/ethernet.h>
#include <kernel/mem/kheap.h>
#include <kernel/bits.h>
#include <string.h>
#include <logger.h>

uint16_t NetChecksumFinal(uint32_t sum)
{
    sum = (sum & 0xffff) + (sum >> 16);
    sum += (sum >> 16);

    uint16_t temp = ~sum;
    return ((temp & 0x00ff) << 8) | ((temp & 0xff00) >> 8); // TODO - shouldn't swap this twice
}
uint32_t NetChecksumAcc(const uint8_t *data, const uint8_t *end, uint32_t sum)
{
    uint32_t len = end - data;
    uint16_t *p = (uint16_t *)data;

    while (len > 1)
    {
        sum += *p++;
        len -= 2;
    }

    if (len)
    {
        sum += *(uint8_t *)p;
    }

    return sum;
}
uint16_t NetChecksum(const uint8_t *data, const uint8_t *end)
{
    uint32_t sum = NetChecksumAcc(data, end, 0);
    return NetChecksumFinal(sum);
}

// uint16_t ip_calculate_checksum(ipv4_header_t * packet){
//     int32_t array_size = sizeof(ipv4_header_t) / 2;
//     uint16_t * arr16 = (uint16_t *)packet;
//     uint32_t sum =0;
//     for (int32_t i = 0; i < array_size; i++)
//     {
//         sum += SWITCH_ENDIAN16(arr16[i]);
//     }

//     uint32_t carry = sum>>16;
//     sum = sum & 0x0000FFFF;
//     sum = sum + carry;
//     return ~sum;
    

// }
void ip_send_packet(ipv4_addr_t * dst_ip, ptr_t data,uint32_t len){
    uint32_t total_size = sizeof(ipv4_header_t)+len;
    ipv4_header_t * packet = kmalloc(total_size);
    memset(packet,0,sizeof(ipv4_header_t));

    packet->version = 4;
    packet->ihl = 5;
    packet->tos = 0;
    packet->length = total_size;
    packet->ttl = 64;
    packet->protocol = 17;
    packet->src_ip[0] = 0;
    packet->src_ip[1] = 0;
    packet->src_ip[2] = 0;
    packet->src_ip[3] = 0;
    
    packet->dst_ip[0] = dst_ip->n[0];
    packet->dst_ip[1] = dst_ip->n[1];
    packet->dst_ip[2] = dst_ip->n[2];
    packet->dst_ip[3] = dst_ip->n[3];

    ptr_t packet_data = (ptr_t)packet + (packet->ihl * 4);
    memcpy(packet_data,data,len);

    packet->ver_ihl = SWITCH_BITS(packet->ver_ihl,4);
    packet->flags_fragment = SWITCH_BITS((uint8_t)packet->flags_fragment,3);
    packet->length = SWITCH_ENDIAN16(packet->length);

    packet->checksum = 0;
    // packet->checksum = SWITCH_ENDIAN16(ip_calculate_checksum(packet));
    packet->checksum = SWITCH_ENDIAN16(NetChecksum((uint8_t *)packet,(uint8_t *)packet+sizeof(ipv4_header_t)));

    struct arp_entry * mac = arp_lookup(dst_ip);
    if(!mac){
        return;
    }

    char ip_str[50];
    char mac_str[50];
    ip2str(ip_str,dst_ip);
    mac2str(mac_str,&mac->mac);
    ethernet_send_packet(mac->mac.n ,(ptr_t)packet,total_size,0x0800);
    log_trace("New IP packet sent to dest.IP:%s dest.mac:%s",ip_str,mac_str);
}