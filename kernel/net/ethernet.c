#include <kernel/net/ethernet.h>
#include <kernel/net/arp.h>
#include <kernel/net/addr.h>
#include <kernel/net/intf.h>
#include <kernel/bits.h>
#include <kernel/mem/kheap.h>
#include <logger.h>
#include <string.h>

// void ethernet_handle_packet(struct ether_header * packet,int len){
//     ptr_t data = (ptr_t) packet + sizeof(struct ether_header);
//     uint32_t datalen = len-sizeof(struct ether_header);

//     char from_address_str[(3*6)+1];
//     mac2str(from_address_str,(eth_addr_t *)packet->ether_shost);
//     char to_address_str[(3*6)+1];
//     mac2str(to_address_str,(eth_addr_t *)packet->ether_dhost);
//     log_trace("eth: new packet type 0x%x from address: %s to address: %s" ,(uint32_t)SWITCH_ENDIAN16(packet->type),from_address_str,to_address_str);

//     switch (SWITCH_ENDIAN16(packet->type))
//     {
//     case  ETHERTYPE_ARP:
//         log_information("ARP Packet Recieved");
//         arp_handle_packet((arp_header_t *) data,datalen);
//         break;
//     case  ETHERTYPE_IP:
//         log_information("eth: Ip packet type arrived 0x%x",(uint32_t)SWITCH_ENDIAN16(packet->type));
//         break;
//     case  ETHERTYPE_IPV6:
//         log_warning("eth: Protocol IPv6 Unsuported");
//         break;
//     default:
//         log_warning("eth: Unhandled protocol type 0x%x",SWITCH_ENDIAN16(packet->type));
//         break;
//     }
// }

// void ethernet_send_packet(uint8_t ether_dhost[6],ptr_t data,uint32_t len,uint32_t protocol){
//     struct ether_header * packet = (struct ether_header *)kmalloc(sizeof(struct ether_header) + len);
//     read_mac_addr(packet->ether_shost);
//     memcpy(packet->ether_dhost,ether_dhost,6);
//     memcpy((uint8_t *)packet+sizeof(struct ether_header),data,len);
//     packet->type = SWITCH_ENDIAN16(protocol);
//     rtl8139_send_packet((ptr_t)packet,sizeof(struct ether_header) + len);
//     kfree(packet);
// }

void EthernetReceive(NetBuffer *packet_buffer){
    EthernetHeader * eth_packet = (EthernetHeader *)packet_buffer->packetData;
    if(!IsMacAddressEquals(packet_buffer->interface->macAddress,eth_packet->destHost)
        && !IsMacAddressEquals(g__broadcastMacAddress,eth_packet->destHost)){
        char mac_str[19];
        MacToStr(mac_str,eth_packet->destHost);
        log_trace("[eth:hwid-mismatch] packet dropped destination host address %s",mac_str);
        return;
    }
    EthernetType message_type = SWITCH_ENDIAN16(eth_packet->type);
    packet_buffer->packetData = packet_buffer->packetData + sizeof(EthernetHeader);
    packet_buffer->length -= sizeof(EthernetHeader);

    switch (message_type)
    {
    case ETHERTYPE_ARP:
        log_information("ARP Packet Recieved");
        arpReceive(packet_buffer);
        break;
    case ETHERTYPE_IP:
        log_information("eth: Ip packet type arrived 0x%x",(uint32_t)message_type);
        break;
    default:
        log_warning("eth: Unhandled protocol type 0x%x",(uint32_t)message_type);
        break;
    }
}
void EthernetSend(NetBuffer *packet_buffer,MacAddress dstAddress,EthernetType et_type){
    uint32_t packet_size = sizeof(EthernetHeader) + packet_buffer->length;
    EthernetHeader * eth_packet = (EthernetHeader *)kmalloc(packet_size);
    CopyMacAddress(packet_buffer->interface->macAddress, eth_packet->srcHost);
    CopyMacAddress(dstAddress,eth_packet->destHost);
    eth_packet->type = SWITCH_ENDIAN16(et_type);
    
    memcpy((char *)eth_packet + sizeof(EthernetHeader),packet_buffer->packetData,packet_buffer->length);
    packet_buffer->packetData = (void *)eth_packet;
    packet_buffer->length = packet_size;
    if(!packet_buffer->interface){
        log_warning("[eth:if-undefined] No interface defined to send message packet ignored");
    }
    packet_buffer->interface->send(packet_buffer);
    kfree(eth_packet);
}