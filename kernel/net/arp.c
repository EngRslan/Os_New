#include <string.h>
#include <logger.h>
#include <kernel/bits.h>
#include <kernel/types.h>
#include <kernel/net/arp.h>
#include <kernel/net/addr.h>
#include <kernel/net/ethernet.h>
#include <kernel/net/intf.h>
#include <kernel/mem/kheap.h>

static ArpEntry arp_table[10]={
    {.isPresent=true,.ip=0xFFFFFFFF,.mac={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}}
};
eth_addr_t broadcast_mac_address = {.n={0,0,0,0,0,0}};

void arpReceive(NetBuffer *packet_buffer){
    // if(SWITCH_ENDIAN16(packet->opcode) == 1){
    //     //Request
    // }else if(SWITCH_ENDIAN16(packet->opcode) == 2){
    //     //replay
    //     arp_table_add(&packet->src_hw_addr,&packet->src_protocol_addr);
    // }
}
void arpSend(MacAddress *dstMacAddr,Ipv4Address *dstIpAddr,ArpOpCode opCode)
{
    NetBuffer *netbuffer = (NetBuffer *)kmalloc(sizeof(NetBuffer));
    uint32_t packet_size = sizeof(ArpHeader);
    ArpHeader *arp_header = (ArpHeader *)kmalloc(sizeof(ArpHeader));

    //find way to get default interface
    CopyMacAddress(&netbuffer->interface->macAddress,&arp_header->srcMacAddr);
    if(defaultAssignedIpAddress.assignMethod == None){
        log_information("[arp:no-ip] No ip addres to send arp packet");
        return;
    }
    CopyIpv4Address(&defaultAssignedIpAddress.Ip,&arp_header->srcIpAddr);

    CopyMacAddress(dstMacAddr, &arp_header->dstMacAddr);
    CopyIpv4Address(dstIpAddr,&arp_header->dstIpAddr);

    arp_header->opCode = SWITCH_ENDIAN16(opCode);

    arp_header->MacAddressLen = sizeof(MacAddress);
    arp_header->IpAddrLen = sizeof(Ipv4Address);

    arp_header->hwType = SWITCH_ENDIAN16(0x1);
    arp_header->protocol = SWITCH_ENDIAN16(ETHERTYPE_ARP);

    netbuffer->packetData = (void *)arp_header;
    netbuffer->length = packet_size;
    EthernetSend(netbuffer,&g__broadcastMacAddress,ETHERTYPE_ARP);
    // ethernet_send_packet(brdcast,(ptr_t)arp_pack,sizeof(arp_header_t),0x0806);
    kfree(netbuffer);
    kfree(arp_header);
}

void arpReceive(NetBuffer *packet_buffer){

}
ArpEntry *arp_lookup(Ipv4Address *ip){
    for (uint8_t i = 0; i < 10; i++)
    {
        if(IsIpv4AddressEquals(arp_table[i].ip) arp_table[i].ip.bits == uip && arp_table[i].present){
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