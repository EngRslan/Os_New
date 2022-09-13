#include <kernel/net/dhcp.h>
#include <kernel/net/udp.h>
#include <kernel/net/addr.h>
#include <kernel/net/intf.h>
#include <kernel/mem/kheap.h>
#include <kernel/types.h>
#include <kernel/bits.h>
#include <string.h>


void dhcp_gen(NetInterface *intf, DhcpHeader *packet, uint8_t msg_type,Ipv4Address ip){
    packet->op = 1;
    packet->hw_type = 1;
    packet->hw_addr_len = 6;
    packet->hops = 0;
    packet->xid = __builtin_bswap32(0x55555555);
    packet->flags = SWITCH_ENDIAN16(0x8000);
    CopyMacAddress(intf->macAddress, packet->client_hw_addrp);

    uint8_t *p = (uint8_t *)packet + sizeof(DhcpHeader);
    *(uint32_t *)p = __builtin_bswap32(0x63825363);
    p+=4;
    *p++ = 53;
    *p++ = 1;
    *p++ = msg_type;

    *p++ = 61;
    *p++ = 0x07;
    *p++ = 0x01;
    CopyMacAddress(intf->macAddress,(uint8_t *)p);
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
void DhcpDiscover(NetInterface *intf){
    NetBuffer *netbuffer = (NetBuffer *)kmalloc(sizeof(NetBuffer));

    Ipv4Address sip = {0,0,0,0};
    Ipv4Address dip = {0xFF,0xFF,0xFF,0xFF};

    uint16_t total_size=sizeof(DhcpHeader) + 64;
    DhcpHeader * packet = (DhcpHeader *)kmalloc(total_size);
    memset(packet,0,total_size);
    dhcp_gen(intf,packet,1,sip);
    netbuffer->interface = intf;
    netbuffer->packetData = packet;
    netbuffer->length = total_size;
    UdpSend(netbuffer,dip,68,67);
    kfree(netbuffer);
    kfree(packet);
}