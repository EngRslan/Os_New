#include <kernel/net/tcp.h>
#include <kernel/mem/kheap.h>
#include <kernel/net/manager.h>
#include <kernel/net/intf.h>
#include <kernel/net/addr.h>
#include <kernel/net/ip.h>
#include <kernel/net/dhcp.h>
#include <string.h>
#include <stdbool.h>
#include <ktime.h>
#include <kernel/datastruct/link.h>
#include <kernel/system.h>
#include <kernel/scheduler/scheduler.h>
#include <kernel/bits.h>
#include <kernel/datastruct/list.h>

list_t *_activeConnections=NULL;

uint32_t baseIsn = 0;
NetPort _portSeq = 20001;
NetPort generateLocalPort(){
    return ++_portSeq;
}
uint16_t TcpChecksumCalc(uint16_t *tcph, int len)
{
	
	uint16_t ret = 0;
	uint32_t sum = 0;
	uint16_t odd_byte;
	
	while (len > 1) {
		sum += *tcph++;
		len -= 2;
	}
	
	if (len == 1) {
		*(uint8_t*)(&odd_byte) = * (uint8_t*)tcph;
		sum += odd_byte;
	}
	
	sum =  (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	ret =  ~sum;
	
	return ret; 
}
TcpConnection *TcpCreate()
{
    TcpConnection *conn = (TcpConnection *)kmalloc(sizeof(TcpConnection));
    memset((void *)conn,0,sizeof(TcpConnection));

    return conn;
}
void TcpSwap(TcpHeader *hdr){
    hdr->srcPort = SWITCH_ENDIAN16(hdr->srcPort);
    hdr->dstPort = SWITCH_ENDIAN16(hdr->dstPort);
    hdr->seqNum = SWITCH_ENDIAN32(hdr->seqNum);
    hdr->ackNum = SWITCH_ENDIAN32(hdr->ackNum);
    hdr->checksum = SWITCH_ENDIAN16(hdr->checksum);
    hdr->windowSize = SWITCH_ENDIAN16(hdr->windowSize);
    hdr->urgentPtr = SWITCH_ENDIAN16(hdr->urgentPtr);
}
void TcpSetState(TcpConnection *conn,TcpState state){
    TcpState oldState = conn->state;
    conn->state = state;
    if(conn->onState){
        conn->onState(conn,oldState,conn->state);
    }
}
TcpConnection *TcpFind(Ipv4Address srcAddr,NetPort srcPort,Ipv4Address dstAddr,NetPort dstPort){
    foreach(item,_activeConnections){
        TcpConnection *conn = (TcpConnection *)item->value_ptr;
        if(srcPort == conn->remotePort && dstPort == conn->localPort
        && IsIpv4AddressEquals(srcAddr,conn->remoteAddr) 
        && IsIpv4AddressEquals(dstAddr,conn->localAddr)){
            return conn;
        }
    }

    return NULL;
    
}
void TcpSendPacket(TcpConnection *conn, uint32_t seq,uint8_t flags, const void *data,uint32_t len){

    NetBuffer *netBuffer = (NetBuffer *)kmalloc(sizeof(NetBuffer));
    uint8_t *packet = kmalloc(sizeof(TcpChecksum) + sizeof(TcpHeader) + 4 + 1 + 1 + len);
    TcpHeader *tcpHeader = (TcpHeader *)(packet + sizeof(TcpChecksum));

    tcpHeader->srcPort = conn->localPort;
    tcpHeader->dstPort = conn->remotePort;
    tcpHeader->seqNum = seq;
    tcpHeader->ackNum = flags & TCP_ACK ? conn->rcvNxt : 0;
    tcpHeader->dataOffset = 0;
    tcpHeader->flagsbits = flags;
    tcpHeader->windowSize = 8192;
    tcpHeader->checksum = 0;
    tcpHeader->urgentPtr= 0;

    TcpSwap(tcpHeader);

    uint8_t *optionsStart = (uint8_t *)tcpHeader + sizeof(TcpHeader);

    if(flags & TCP_SYN){
        optionsStart[0] = 2;
        optionsStart[1] = 4;    
        *(uint16_t *)(optionsStart + 2) = SWITCH_ENDIAN16(1460);
        optionsStart += optionsStart[1];
    }

    while ((uint32_t)(optionsStart - (uint8_t *)tcpHeader) & 3)
    {
        *optionsStart ++ = 0;
    }

    uint8_t doff = optionsStart - (uint8_t *)tcpHeader;
    tcpHeader->dataOffset = doff << 2;
    
    //moveData
    memcpy(optionsStart, data, len);
    uint8_t *packetEnd = (uint8_t *)optionsStart + len;

    TcpChecksum *chksum = (TcpChecksum *)packet;
    CopyIpv4Address(conn->localAddr,chksum->src);
    CopyIpv4Address(conn->remoteAddr,chksum->dst);
    chksum->res = 0;
    chksum->protocol = IP_TCP;
    chksum->len = SWITCH_ENDIAN16((uint16_t)(packetEnd - (uint8_t *)tcpHeader));

    uint16_t chksumCalc = TcpChecksumCalc((uint16_t *)packet,packetEnd - packet);
    tcpHeader->checksum = chksumCalc;

    netBuffer->interface = conn->intf;
    netBuffer->packetData = tcpHeader;
    netBuffer->length = (uint32_t)(packetEnd - (uint8_t *)tcpHeader);

    IpSend(netBuffer,conn->remoteAddr,IP_TCP);
    conn->sndNxt += len;
    if(flags & (TCP_SYN|TCP_FIN)){
        ++conn->sndNxt;
    }


    kfree(packet);
    kfree((ptr_t)netBuffer);

}
bool TcpConnect(TcpConnection *conn, Ipv4Address addr,uint16_t port)
{
    NetInterface *intf = GetDefaultInterface();
    conn->intf = intf;
    conn->localPort = generateLocalPort();
    CopyIpv4Address(intf->Ip,conn->localAddr);
    conn->remotePort = port;
    CopyIpv4Address(addr,conn->remoteAddr);

    uint32_t isn = baseIsn + Millis() * 250;

    conn->sndUna = isn;
    conn->sndNxt = isn;
    conn->sndWnd = 8192;
    conn->sndUP = 0;
    conn->sndWl1 = 0;
    conn->sndWl2 = 0;
    conn->iss = isn;

    conn->rcvNxt = 0;
    conn->rcvWnd = 8192;
    conn->rcvUP = 0;
    conn->irs = 0;

    // LinkBefore(&_activeConnections,&conn->link);
    list_add_item(_activeConnections,(ptr_t)conn);
    InterruptClear();
    TcpSendPacket(conn,conn->sndNxt,TCP_SYN,NULL,0);
    TcpSetState(conn,TCP_SYN_SENT);
    InterruptSet();
    return true;
}
void TcpReceiveSynSent(TcpConnection *conn,TcpHeader *tcpHeader){
    if(tcpHeader->isAck){
        if(SEQ_LE(tcpHeader->ackNum,conn->iss) || SEQ_GT(tcpHeader->ackNum,conn->sndNxt)){
            return;
        }
    }

    if(tcpHeader->isReset){
        return;
    }

    if(tcpHeader->isSync){
        conn->irs = tcpHeader->seqNum;
        conn->rcvNxt = tcpHeader->seqNum +1;

        if(tcpHeader->isAck){
            conn->sndUna = tcpHeader->ackNum;
            conn->sndWnd = tcpHeader->windowSize;
            conn->sndWl1 = tcpHeader->seqNum;
            conn->sndWl2 = tcpHeader->ackNum;
            
            TcpSendPacket(conn,conn->sndNxt,TCP_ACK,NULL,0);
            TcpSetState(conn,TCP_ESTABLISHED);
        }
    }else{

    }
}
void TcpReceive(NetBuffer *netbuffer,Ipv4Address ip){
    TcpHeader *tcpHeader = (TcpHeader *)netbuffer->packetData;
    TcpSwap(tcpHeader);
    TcpConnection *conn = TcpFind(ip,tcpHeader->srcPort,netbuffer->interface->Ip,tcpHeader->dstPort);
    if(!conn || conn->state == TCP_CLOSED){
        return;
    }

    if(conn->state == TCP_LISTEN){

    }
    else if(conn->state == TCP_SYN_SENT){
        TcpReceiveSynSent(conn,tcpHeader);
    }else{
        // TcpReceiveGeneral(conn,tcpHeader,)
    }
    
}
void TcpSend(TcpConnection *conn, const void *data, uint32_t count){
    TcpSendPacket(conn, conn->sndNxt, TCP_ACK | TCP_PSH, data, count);
}
void TcpInit(){
    Time tm;
    TimeNow(&tm);
    baseIsn = (tm * 1000 - Millis()) * 250;
    _activeConnections = list_create();
    IpRegisterProtocolHandler(IP_TCP,TcpReceive);
}