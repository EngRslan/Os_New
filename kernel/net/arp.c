#include <kernel/net/arp.h>
#include <kernel/net/addr.h>
#include <kernel/drivers/rtl8139.h>
#include <kernel/mem/kheap.h>
#include <string.h>
#include <kernel/bits.h>
#include <kernel/types.h>
#include <kernel/net/ethernet.h>
#include <kernel/net/addr.h>

static struct arp_entry arp_table[10]={
    {.present=1,.ip.bits=0xFFFFFFFF,.mac.n={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}}
};
eth_addr_t broadcast_mac_address = {.n={0,0,0,0,0,0}};

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

struct arp_entry *arp_lookup(ipv4_addr_t * ip){
    uint32_t uip = ip->bits;
    for (uint8_t i = 0; i < 10; i++)
    {
        if(arp_table[i].ip.bits == uip && arp_table[i].present){
            return &arp_table[i];
        }
    }

    arp_send_packet(&broadcast_mac_address,ip);
    
    while (1)
    {
        for (uint8_t i = 0; i < 10; i++)
        {
            if(arp_table[i].ip.bits == uip && arp_table[i].present){
                return &arp_table[i];
            }
        }
    }
    

    return NULL;
    
}
struct arp_entry * arp_table_add(eth_addr_t * dst_hw_addr,ipv4_addr_t * dst_protocol_addr){
    struct arp_entry *arp_entry;
    if((arp_entry = arp_lookup(dst_protocol_addr))){
        memcpy(arp_entry->mac.n,dst_hw_addr->n,6);
        return arp_entry;
    }else{
        for (uint8_t i = 0;i < 10;i++)
        {
            arp_entry = &arp_table[i];
            if(!arp_entry->present){
                arp_entry->present = 1;
                arp_entry->ip.bits = dst_protocol_addr->bits;
                memcpy(&arp_entry->mac,dst_hw_addr,6);
                return arp_entry;
            }
        }
    }
    return NULL;
}
void arp_handle_packet(arp_header_t * packet,uint32_t len)
{
    if(SWITCH_ENDIAN16(packet->opcode) == 1){
        //Request
    }else if(SWITCH_ENDIAN16(packet->opcode) == 2){
        //replay
        arp_table_add(&packet->src_hw_addr,&packet->src_protocol_addr);
    }

}