#include <kernel/net/dhcp.h>
#include <kernel/net/udp.h>
#include <kernel/net/addr.h>
#include <kernel/mem/kheap.h>
#include <kernel/drivers/rtl8139.h>
#include <kernel/types.h>
#include <kernel/bits.h>
#include <string.h>


void dhcp_gen(dhcp_header_t * packet, uint8_t msg_type,ipv4_addr_t * ip){
    packet->op = 1;
    packet->hw_type = 1;
    packet->hw_addr_len = 6;
    packet->hops =0;
    packet->xid = __builtin_bswap32(0x55555555);
    packet->flags = SWITCH_ENDIAN16(0x8000);
    read_mac_addr(packet->client_hw_addrp);
    ipv4_addr_t dst_ip = {.n={0xff,0xff,0xff,0xff}};

    uint8_t *p = (uint8_t *)packet + sizeof(dhcp_header_t);
    *(uint32_t *)p = __builtin_bswap32(0x63825363);
    p+=4;
    *p++ = 53;
    *p++ = 1;
    *p++ = msg_type;

    *p++ = 61;
    *p++ = 0x07;
    *p++ = 0x01;
    read_mac_addr(p);
    p += 6;

    // *p++ = 55;
    // *p++ = 3;
    // *p++ = 1;
    // *p++ = 3;
    // *p++ = 6;

    // Parameter request list
    *(p++) = 55;
    *(p++) = 8;
    *(p++) = 0x1;
    *(p++) = 0x3;
    *(p++) = 0x6;
    *(p++) = 0xf;
    *(p++) = 0x2c;
    *(p++) = 0x2e;
    *(p++) = 0x2f;
    *(p++) = 0x39;

    *p++ = 0xFF;

    


}
void dhcp_discover(){
    ipv4_addr_t sip;
    ipv4_addr_t dip;

    sip.bits = 0;
    dip.bits = 0xFFFFFFFF;

    uint16_t total_size=sizeof(dhcp_header_t) + 64;
    dhcp_header_t * packet = (dhcp_header_t *)kmalloc(total_size);
    memset(packet,0,total_size);
    dhcp_gen(packet,1,&sip);
    udp_send_packet(&dip,68,67,packet,sizeof(dhcp_header_t) + 64);
}