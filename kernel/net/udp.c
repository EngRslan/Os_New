#include <kernel/net/udp.h>
#include <kernel/net/addr.h>
#include <kernel/net/intf.h>
#include <kernel/net/ip.h>
#include <kernel/mem/kheap.h>
#include <kernel/types.h>
#include <kernel/bits.h>
#include <string.h>
#include <logger.h>
#define MAX_UDP_TABLE_ENTRIES 50

typedef struct {
    bool isPresent;
    uint16_t port;
    UdpHandler handler;
} udpTableEntry;

static udpTableEntry udpTable[MAX_UDP_TABLE_ENTRIES];

void UdpRegisterHandler(uint16_t port,UdpHandler handler){
    if(!handler){
        return ;
    }

    udpTableEntry * entry = NULL;

    for (uint16_t i = 0; i < MAX_UDP_TABLE_ENTRIES; i++){
        if(udpTable[i].isPresent && udpTable[i].port == port){
            log_warning("[udp:handler-override] Override handler Registeration port %d",(uint32_t)port);
            udpTable[i].handler = handler;
            return;
        }
        else if(entry == NULL && !udpTable[i].isPresent){
            entry = &udpTable[i];
        }
    }

    entry->isPresent = true;
    entry->port = port;
    entry->handler = handler;
}
void UdpReceive(NetBuffer * netBuffer){
    UdpHeader *udpHeader = (UdpHeader *)netBuffer->packetData;
    for (uint16_t i = 0; i < MAX_UDP_TABLE_ENTRIES; i++)
    {
        if(udpTable[i].isPresent && udpTable[i].port == SWITCH_ENDIAN16(udpHeader->dst_port)){
            if(udpTable[i].handler){
                netBuffer->packetData = (uint8_t *)netBuffer->packetData+sizeof(UdpHeader);
                netBuffer->length -= sizeof(UdpHeader);
                udpTable[i].handler(netBuffer);
                return;
            }

            log_error("[udp:handler-error] Packet Dropped Handler Registeration Error");
        }
    }
    log_error("[udp:port-unmapped] Packet Dropped No Handler Registered port %d",(uint32_t)SWITCH_ENDIAN16(udpHeader->dst_port));
}
void UdpSend(NetBuffer *netbuffer, Ipv4Address ip,uint16_t src_port,uint16_t dst_port){
    uint32_t total_size = sizeof(UdpHeader) + netbuffer->length;
    UdpHeader * packet = (UdpHeader *)kmalloc(total_size);
    memset((ptr_t)packet,0,total_size);

    packet->src_port = SWITCH_ENDIAN16(src_port);
    packet->dst_port = SWITCH_ENDIAN16(dst_port);
    packet->length = SWITCH_ENDIAN16(total_size);
    packet->checksum = 0;

    memcpy((ptr_t)packet+sizeof(UdpHeader),netbuffer->packetData,netbuffer->length);
    log_trace("UDP Packet Sent from port: %d to port: %d",(uint32_t)src_port,(uint32_t)dst_port);
    netbuffer->packetData = packet;
    netbuffer->length = total_size;

    IpSend(netbuffer,ip,IP_UDP);
    kfree(packet);
}