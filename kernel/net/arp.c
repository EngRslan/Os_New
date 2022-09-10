#include <kernel/net/arp.h>
#include <kernel/net/addr.h>
#include <kernel/drivers/rtl8139.h>
#include <kernel/mem/kheap.h>
#include <string.h>
#include <kernel/bits.h>
#include <kernel/types.h>
#include <kernel/net/ethernet.h>

void arp_send_packet(eth_addr_t * dst_hw_addr,ipv4_addr_t * dst_protocol_addr){
    arp_header_t * arp_pack = kmalloc(sizeof(arp_header_t));

    read_mac_addr(arp_pack->src_hw_addr.n);
    arp_pack->src_protocol_addr.n[0] = 192;
    arp_pack->src_protocol_addr.n[1] = 168;
    arp_pack->src_protocol_addr.n[2] = 76;
    arp_pack->src_protocol_addr.n[3] = 10;

    memcpy(arp_pack->dst_hw_addr.n,dst_hw_addr->n,sizeof(eth_addr_t));
    memcpy(arp_pack->dst_protocol_addr.n,dst_protocol_addr->n,sizeof(ipv4_addr_t));

    arp_pack->opcode = SWITCH_ENDIAN16(0x1);

    arp_pack->hw_addr_len = 6;
    arp_pack->protocol_addr_len = 4;

    arp_pack->hw_type = SWITCH_ENDIAN16(0x1);
    arp_pack->protocol = SWITCH_ENDIAN16(0x0800);

    uint8_t brdcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    ethernet_send_packet(brdcast,(ptr_t)arp_pack,sizeof(arp_header_t),0x0806);
}


void arp_handle_packet(arp_header_t * packet,uint32_t len)
{

}