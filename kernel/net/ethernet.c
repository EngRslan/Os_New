#include <kernel/net/ethernet.h>
#include <kernel/net/arp.h>
#include <kernel/net/addr.h>
#include <kernel/net/intf.h>
#include <kernel/bits.h>
#include <kernel/mem/kheap.h>
#include <logger.h>
#include <string.h>
#define MAX_ETHERNET_PROTOCOLS_ENTRIES 10

typedef struct {
    bool isPresent;
    EthernetType type;
    EthernetHandler handler;
    
} EthernetProtocolTableEntry;

static EthernetProtocolTableEntry protocolTable[MAX_ETHERNET_PROTOCOLS_ENTRIES];
void EthernetRegisterProtocol(EthernetType etherType,EthernetHandler handler){
    EthernetProtocolTableEntry * entry = NULL;
    for (uint8_t i = 0; i < MAX_ETHERNET_PROTOCOLS_ENTRIES; i++)
    {
        if(protocolTable[i].isPresent && protocolTable[i].type == etherType){
            log_warning("[eth:protocol_override] Current Protocol entry override");
            protocolTable[i].handler = handler;
            return;
        }else if(entry == NULL && !protocolTable[i].isPresent){
            entry = &protocolTable[i];
        }
    }

    entry->isPresent = true;
    entry->type = etherType;
    entry->handler = handler;
}
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

    for (uint8_t i = 0; i < MAX_ETHERNET_PROTOCOLS_ENTRIES; i++)
    {
        if(protocolTable[i].isPresent && protocolTable[i].type == message_type){
            protocolTable[i].handler(packet_buffer);
            break;
        }

        log_warning("[eth] Unhandled protocol type 0x%x",(uint32_t)message_type);
    }
    kfree(eth_packet);
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