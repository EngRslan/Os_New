#include <kernel/net/udp.h>
#include <kernel/net/addr.h>
#include <kernel/net/intf.h>
#include <kernel/net/ip.h>
#include <kernel/mem/kheap.h>
#include <kernel/types.h>
#include <kernel/bits.h>
#include <string.h>
#include <logger.h>

void UdpSend(NetBuffer *netbuffer, Ipv4Address ip,uint16_t src_port,uint16_t dst_port){
    uint32_t total_size = sizeof(UdpHeader) + netbuffer->length;
    UdpHeader * packet = (UdpHeader *)kmalloc(total_size);
    memset((ptr_t)packet,0,total_size);

    packet->dst_port = SWITCH_ENDIAN16(src_port);
    packet->src_port = SWITCH_ENDIAN16(dst_port);
    packet->length = SWITCH_ENDIAN16(total_size);
    packet->checksum = 0;

    memcpy((ptr_t)packet+sizeof(UdpHeader),netbuffer->packetData,netbuffer->length);
    log_trace("UDP Packet Sent from port: %d to port: %d",(uint32_t)src_port,(uint32_t)dst_port);
    netbuffer->packetData = packet;
    netbuffer->length = total_size;

    IpSend(netbuffer,ip);
    kfree(packet);
}