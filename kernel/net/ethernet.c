#include <kernel/net/ethernet.h>
#include <kernel/net/arp.h>
#include <kernel/bits.h>
#include <kernel/mem/kheap.h>
#include <kernel/drivers/rtl8139.h>
#include <logger.h>
#include <string.h>

void ethernet_handle_packet(struct ether_header * packet,int len){
    ptr_t data = (ptr_t) packet + sizeof(struct ether_header);
    uint32_t datalen = len-sizeof(struct ether_header);
    log_trace("eth: new packet type 0x%x from address: %x:%x:%x:%x:%x:%x to address: %x:%x:%x:%x:%x:%x:" ,(uint32_t)SWITCH_ENDIAN16(packet->type),(uint32_t)packet->ether_shost[0],
        (uint32_t)packet->ether_shost[1],(uint32_t)packet->ether_shost[2],(uint32_t)packet->ether_shost[3],(uint32_t)packet->ether_shost[4],(uint32_t)packet->ether_shost[5],
        (uint32_t)packet->ether_dhost[0],(uint32_t)packet->ether_dhost[1],(uint32_t)packet->ether_dhost[2],(uint32_t)packet->ether_dhost[3],(uint32_t)packet->ether_dhost[4],(uint32_t)packet->ether_dhost[5]
        );

    switch (SWITCH_ENDIAN16(packet->type))
    {
    case  ETHERTYPE_ARP:
        log_information("ARP Packet Recieved");
        arp_handle_packet((arp_header_t *) data,datalen);
        break;
    case  ETHERTYPE_IP:
        break;
    case  ETHERTYPE_IPV6:
        log_warning("eth: Protocol IPv6 Unsuported");
        break;
    default:
        log_warning("eth: Unhandled protocol type 0x%x",SWITCH_ENDIAN16(packet->type));
        break;
    }
}

void ethernet_send_packet(uint8_t ether_dhost[6],ptr_t data,uint32_t len,uint32_t protocol){
    struct ether_header * packet = (struct ether_header *)kmalloc(sizeof(struct ether_header) + len);
    read_mac_addr(packet->ether_shost);
    memcpy(packet->ether_dhost,ether_dhost,6);
    memcpy((uint8_t *)packet+sizeof(struct ether_header),data,len);
    packet->type = SWITCH_ENDIAN16(protocol);

    rtl8139_send_packet((ptr_t)packet,sizeof(struct ether_header) + len);
    kfree(packet);
}