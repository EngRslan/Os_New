#include <string.h>
#include <logger.h>
#include <kernel/bits.h>
#include <kernel/types.h>
#include <kernel/net/arp.h>
#include <kernel/net/addr.h>
#include <kernel/net/ethernet.h>
#include <kernel/net/intf.h>
#include <kernel/mem/kheap.h>

static ArpEntry arp_table[10];

void arpTableAdd(MacAddress macAddress,Ipv4Address ipAddress){
    
    ArpEntry *arp_entry;
    for (uint8_t i = 0; i < 10; i++)
    {
        arp_entry = &arp_table[i];
        if(arp_entry->isPresent && IsIpv4AddressEquals(ipAddress,arp_entry->ip)){
            CopyMacAddress(macAddress,arp_entry->mac);
            return;
        }
    }
    
    for (uint8_t i = 0;i < 10;i++)
    {
        
        arp_entry = &arp_table[i];
        if(!arp_entry->isPresent){
            arp_entry->isPresent = true;
            CopyIpv4Address(ipAddress,arp_entry->ip);
            CopyMacAddress(macAddress,arp_entry->mac);
            return ;
        }
    }
}

void arpReceive(NetBuffer *packet_buffer){
    ArpHeader *arp_packet = (ArpHeader *)packet_buffer->packetData;
    if(SWITCH_ENDIAN16(arp_packet->opCode) == ARP_OP_REQUEST){
        arpSend(packet_buffer->interface,arp_packet->srcMacAddr,arp_packet->srcIpAddr,ARP_OP_REPLAY);
    }
    else if(SWITCH_ENDIAN16(arp_packet->opCode) == ARP_OP_REPLAY){
        arpTableAdd(arp_packet->srcMacAddr,arp_packet->srcIpAddr);
    }
}
void arpSend(NetInterface *intf, MacAddress dstMacAddr,Ipv4Address dstIpAddr,ArpOpCode opCode)
{
    NetBuffer *netbuffer = (NetBuffer *)kmalloc(sizeof(NetBuffer));
    uint32_t packet_size = sizeof(ArpHeader);
    ArpHeader *arp_header = (ArpHeader *)kmalloc(sizeof(ArpHeader));

    netbuffer->packetData = (void *)arp_header;
    netbuffer->length = packet_size;
    netbuffer->interface = intf;
    //find way to get default interface
    CopyMacAddress(netbuffer->interface->macAddress,arp_header->srcMacAddr);
    if(defaultAssignedIpAddress.assignMethod == IP_METHOD_NONE){
        log_information("[arp:no-ip] No ip address to send arp packet");
        return;
    }
    CopyIpv4Address(defaultAssignedIpAddress.Ip,arp_header->srcIpAddr);

    CopyMacAddress(dstMacAddr, arp_header->dstMacAddr);
    CopyIpv4Address(dstIpAddr,arp_header->dstIpAddr);

    arp_header->opCode = SWITCH_ENDIAN16(opCode);

    arp_header->MacAddressLen = sizeof(MacAddress);
    arp_header->IpAddrLen = sizeof(Ipv4Address);

    arp_header->hwType = SWITCH_ENDIAN16(ARP_ETH_TYPE);
    arp_header->protocol = SWITCH_ENDIAN16(ETHERTYPE_ARP);

    EthernetSend(netbuffer,dstMacAddr,ETHERTYPE_ARP);
    // ethernet_send_packet(brdcast,(ptr_t)arp_pack,sizeof(arp_header_t),0x0806);
    kfree(netbuffer);
    kfree(arp_header);
}

uint8_t *arp_lookup(NetInterface *intf, Ipv4Address ip){
    if(IsIpv4AddressEquals(ip,g__broadcastIpAddress)){
        return g__broadcastMacAddress;
    }
    for (uint8_t i = 0; i < 10; i++)
    {
        if(IsIpv4AddressEquals(arp_table[i].ip,ip) && arp_table[i].isPresent){
            return arp_table[i].mac;
        }
    }

    arpSend(intf, g__broadcastMacAddress,ip,ARP_OP_REQUEST);
    // arp_send_packet(&broadcast_mac_address,ip);
    
    while (1)
    {
        for (uint8_t i = 0; i < 10; i++)
        {
            if(IsIpv4AddressEquals(arp_table[i].ip,ip) && arp_table[i].isPresent){
                return arp_table[i].mac;
            }
        }
    }
    

    return NULL;
    
}
