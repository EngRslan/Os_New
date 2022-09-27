#include <kernel/net/icmp.h>
#include <kernel/bits.h>
#include <kernel/net/intf.h>
#include <kernel/net/ip.h>
#include <kernel/mem/kheap.h>
#include <logger.h>
#include <string.h>
uint16_t IcmpChecksum(uint16_t *icmph, int len)
{
	
	uint16_t ret = 0;
	uint32_t sum = 0;
	uint16_t odd_byte;
	
	while (len > 1) {
		sum += *icmph++;
		len -= 2;
	}
	
	if (len == 1) {
		*(uint8_t*)(&odd_byte) = * (uint8_t*)icmph;
		sum += odd_byte;
	}
	
	sum =  (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	ret =  ~sum;
	
	return ret; 
}
void IcmpSendEchoReplay(NetInterface *intf, Ipv4Address ip,uint16_t id,uint16_t seq,void *data,uint16_t len){
    uint32_t totalsize = sizeof(IcmpEchoReply) + len;
    NetBuffer *buffer = (NetBuffer *)kmalloc(sizeof(NetBuffer));
    IcmpEchoReply *packet = (IcmpEchoReply *)kmalloc(totalsize);
    buffer->length = totalsize;
    buffer->interface = intf;
    buffer->packetData = (void *)packet;

    packet->type = ICMP_TYPE_ECHO_REPLAY,
    packet->code = 0;
    packet->id = id;
    packet->seq = seq;
    packet->checksum = 0;
    memcpy(buffer->packetData+sizeof(IcmpEchoReply),data,len);
    packet->checksum = IcmpChecksum((uint16_t *)buffer->packetData,totalsize);
    IpSend(buffer,ip,IP_ICMP);
    kfree(packet);
    kfree(buffer);
}
void IcmpReceive(NetBuffer *netbuffer,Ipv4Address srcip){
    IcmpEchoReply *packet = (IcmpEchoReply *)netbuffer->packetData;
    if(packet->type == ICMP_TYPE_ECHO_REQUEST){
        log_debug("Icmp Echo Request Received");
        IcmpSendEchoReplay(netbuffer->interface, srcip,packet->id,packet->seq,(void *)packet+sizeof(IcmpEchoReply),netbuffer->length-sizeof(IcmpEchoReply));
    }else{
        log_warning("Icmp type not handled");
    }
}