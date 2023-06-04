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
#define DEFAULT_WINDOW_SIZE 65495
#define DEFAULT_TCP_HEADER_SIZE 60
list_t *_activeConnections=NULL;

uint32_t baseIsn = 0;
NetPort _portSeq = 20003;
NetPort generateLocalPort(){
    return ++_portSeq;
}
uint16_t Checksum(uint16_t *tcph, int len)
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
    // hdr->checksum = SWITCH_ENDIAN16(hdr->checksum);
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
NetBuffer *tcpCreateBuffer(TcpConnection *conn,uint32_t datalen){
    // collect memory
    NetBuffer *netBuffer = (NetBuffer *)kmalloc(sizeof(NetBuffer));
    uint32_t packetSize = sizeof(TcpChecksum) + DEFAULT_TCP_HEADER_SIZE + datalen;
    uint8_t *packet = kmalloc(packetSize);
    memset(packet,0,packetSize);

    // create default tcp header
    TcpHeader *tcpHeader = (TcpHeader *)(packet + sizeof(TcpChecksum));
    tcpHeader->srcPort = conn->localPort;
    tcpHeader->dstPort = conn->remotePort;
    tcpHeader->windowSize = 8192;

    netBuffer->interface = conn->intf;
    netBuffer->packetData = packet + sizeof(TcpChecksum);
    return netBuffer;
}
uint8_t *tcpCreateSynOptions(TcpHeader *tcpHeader){

    uint8_t *optionsStart = (uint8_t *)tcpHeader + sizeof(TcpHeader);
    optionsStart[0] = 2;
    optionsStart[1] = 4;    
    *(uint16_t *)(optionsStart + 2) = SWITCH_ENDIAN16(1460);
    optionsStart += optionsStart[1];
    while ((uint32_t)(optionsStart - (uint8_t *)tcpHeader) & 3)
    {
        *optionsStart ++ = 0;
    }

    return optionsStart;
}
uint16_t tcpCalcChecksum(TcpConnection *conn, TcpHeader *tcpHeader,uint16_t packetSize){
    TcpChecksum *checksumHeader = (TcpChecksum *)((uint8_t *)tcpHeader - sizeof(TcpChecksum));
    uint16_t totalSumSize = packetSize + sizeof(TcpChecksum);
    CopyIpv4Address(conn->localAddr,checksumHeader->src);
    CopyIpv4Address(conn->remoteAddr,checksumHeader->dst);
    checksumHeader->res = 0;
    checksumHeader->protocol = IP_TCP;
    checksumHeader->len = SWITCH_ENDIAN16(packetSize);
    uint16_t chksum = Checksum((uint16_t *)checksumHeader,(int)totalSumSize);
    return chksum;
}
void tcpFreeBuffer(NetBuffer *netBuffer)
{
    kfree(netBuffer->packetData);
    kfree((void *)netBuffer);
}
void TcpSendPacket(TcpConnection *conn,TcpFlags flags, const void *data,uint32_t len)
{
    // collect default packet
    NetBuffer *netBuffer = tcpCreateBuffer(conn,len);
    
    TcpHeader *tcpHeader = (TcpHeader *)netBuffer->packetData;

    tcpHeader->flags = flags;
    tcpHeader->seqNum = conn->nxtSeq;

    // if this packet sent to ack then we need to send the ack number
    if(flags.isAck){
        tcpHeader->ackNum = conn->rNxtSeq;
    }


    // Swap header multibytes
    TcpSwap(tcpHeader);

    uint8_t *packetData = NULL;

    // add startup options if this packet for handshake
    if(flags.isSync)
    {
        packetData = tcpCreateSynOptions(tcpHeader);
    }
    else
    {
        packetData = (uint8_t *)tcpHeader + sizeof(TcpHeader);
    }

    // add data to packet if available
    if(len>0){
        memcpy(packetData,data,len);
    }


    uint16_t dataOffset = packetData - (uint8_t *)tcpHeader;
    tcpHeader->dataOffset = (packetData - (uint8_t *)tcpHeader) << 2;
    
    // Calculate check sum for packet
    uint16_t packetSize = dataOffset  + len;
    tcpHeader->checksum = tcpCalcChecksum(conn,tcpHeader,packetSize);
    


    // send packet
    netBuffer->length = packetSize;
    IpSend(netBuffer,conn->remoteAddr,IP_TCP);

    //update connection for the next request
    conn->nxtSeq += len;
    if(flags.isSync || flags.isFinish){
        conn->nxtSeq ++;
    }


    // free reserved memory
    tcpFreeBuffer(netBuffer);

}
// void TcpSendPacket(TcpConnection *conn, uint32_t seq,uint8_t flags, const void *data,uint32_t len){

//     NetBuffer *netBuffer = (NetBuffer *)kmalloc(sizeof(NetBuffer));
//     uint8_t *packet = kmalloc(sizeof(TcpChecksum) + sizeof(TcpHeader) + 4 + 1 + 1 + len);
//     TcpHeader *tcpHeader = (TcpHeader *)(packet + sizeof(TcpChecksum));

//     tcpHeader->srcPort = conn->localPort;
//     tcpHeader->dstPort = conn->remotePort;
//     tcpHeader->seqNum = seq;
//     tcpHeader->ackNum = flags & TCP_ACK ? conn->rcvNxt : 0;
//     tcpHeader->dataOffset = 0;
//     tcpHeader->flagsbits = flags;
//     tcpHeader->windowSize = 8192;
//     tcpHeader->checksum = 0;
//     tcpHeader->urgentPtr= 0;

//     TcpSwap(tcpHeader);

//     uint8_t *optionsStart = (uint8_t *)tcpHeader + sizeof(TcpHeader);

//     if(flags & TCP_SYN){
//         optionsStart[0] = 2;
//         optionsStart[1] = 4;    
//         *(uint16_t *)(optionsStart + 2) = SWITCH_ENDIAN16(1460);
//         optionsStart += optionsStart[1];
//     }

//     while ((uint32_t)(optionsStart - (uint8_t *)tcpHeader) & 3)
//     {
//         *optionsStart ++ = 0;
//     }

//     uint8_t doff = optionsStart - (uint8_t *)tcpHeader;
//     tcpHeader->dataOffset = doff << 2;
    
//     //moveData
//     memcpy(optionsStart, data, len);
//     uint8_t *packetEnd = (uint8_t *)optionsStart + len;

//     TcpChecksum *chksum = (TcpChecksum *)packet;
//     CopyIpv4Address(conn->localAddr,chksum->src);
//     CopyIpv4Address(conn->remoteAddr,chksum->dst);
//     chksum->res = 0;
//     chksum->protocol = IP_TCP;
//     chksum->len = SWITCH_ENDIAN16((uint16_t)(packetEnd - (uint8_t *)tcpHeader));

//     uint16_t chksumCalc = TcpChecksumCalc((uint16_t *)packet,packetEnd - packet);
//     tcpHeader->checksum = chksumCalc;

//     netBuffer->interface = conn->intf;
//     netBuffer->packetData = tcpHeader;
//     netBuffer->length = (uint32_t)(packetEnd - (uint8_t *)tcpHeader);

//     IpSend(netBuffer,conn->remoteAddr,IP_TCP);
//     conn->sndNxt += len;
//     if(flags & (TCP_SYN|TCP_FIN)){
//         ++conn->sndNxt;
//     }


//     kfree(packet);
//     kfree((ptr_t)netBuffer);

// }
bool TcpConnect(TcpConnection *conn, Ipv4Address addr,uint16_t port)
{
    NetInterface *intf = GetDefaultInterface();
    conn->intf = intf;
    conn->localPort = generateLocalPort();
    CopyIpv4Address(intf->Ip,conn->localAddr);
    conn->remotePort = port;
    CopyIpv4Address(addr,conn->remoteAddr);

    uint32_t isn = baseIsn + Millis() * 250;

    conn->nxtSeq = isn;
    // conn->sndNxt = isn;
    // conn->sndWnd = 8192;
    // conn->sndUP = 0;
    // conn->sndWl1 = 0;
    // conn->sndWl2 = 0;
    conn->iss = isn;

    conn->rNxtSeq = 0;
    // conn->rcvWnd = 8192;
    // conn->rcvUP = 0;
    conn->riss = 0;

    // LinkBefore(&_activeConnections,&conn->link);
    list_add_item(_activeConnections,(ptr_t)conn);
    InterruptClear();
    TcpFlags flags = { .bits = 0 };
    flags.isSync = 1;
    TcpSendPacket(conn,flags,NULL,0);
    TcpSetState(conn,TCP_SYN_SENT);
    InterruptSet();
    return true;
}
void TcpReceiveSynSent(TcpConnection *conn,TcpHeader *tcpHeader){
    // if(tcpHeader->isAck){
    //     if(SEQ_LE(tcpHeader->ackNum,conn->iss) || SEQ_GT(tcpHeader->ackNum,conn->sndNxt)){
    //         return;
    //     }
    // }

    // if(tcpHeader->isReset){
    //     return;
    // }

    // if(tcpHeader->isSync){
    //     conn->irs = tcpHeader->seqNum;
    //     conn->rcvNxt = tcpHeader->seqNum +1;

    //     if(tcpHeader->isAck){
    //         conn->sndUna = tcpHeader->ackNum;
    //         conn->sndWnd = tcpHeader->windowSize;
    //         conn->sndWl1 = tcpHeader->seqNum;
    //         conn->sndWl2 = tcpHeader->ackNum;
            
    //         TcpSendPacket(conn,conn->sndNxt,TCP_ACK,NULL,0);
    //         TcpSetState(conn,TCP_ESTABLISHED);
    //     }
    // }else{

    // }
}
void TcpReceive(NetBuffer *netbuffer,Ipv4Address ip){
    TcpHeader *tcpHeader = (TcpHeader *)netbuffer->packetData;
    TcpSwap(tcpHeader);
    TcpConnection *conn = TcpFind(ip,tcpHeader->srcPort,netbuffer->interface->Ip,tcpHeader->dstPort);
    TcpFlags flags = {.bits = 0};

    // if(!conn || conn->state == TCP_CLOSED){
    //     flags.isReset = 1;
    //     TcpSendPacket(conn,conn->nxtSeq,flags,NULL,0);
    //     return;
    // }

    // if(tcpHeader->isReset && conn->state == TCP_SYN_SENT)
    // {
    //     conn->state = TCP_CLOSED;
    //     if(conn->onError)   {
    //         conn->onError(conn,TCP_ERROR_REFUSED);
    //         return;
    //     }
    // }

    if(conn->state == TCP_SYN_SENT){
        if(!tcpHeader->flags.isSync){
            flags.isReset = 1;
            TcpSendPacket(conn, flags , NULL, 0);
            return;
        }

        conn->riss = tcpHeader->seqNum;
        conn->rNxtSeq = conn->riss + 1;

        if(tcpHeader->flags.isAck){
            flags.isAck = 1;
            TcpSendPacket(conn, flags , NULL, 0);
            TcpSetState(conn,TCP_ESTABLISHED);
        }
        
        return;
    }

    if(tcpHeader->flags.isAck)
    {
        conn->unAck = tcpHeader->ackNum;
    }

    uint32_t hlen = tcpHeader->dataOffset >> 2;
    uint32_t len = netbuffer->length - (tcpHeader->dataOffset >> 2);
    if(len > 0){
        flags.isAck = 1;
        conn->rNxtSeq += len;
        TcpSendPacket(conn, flags , NULL, 0);
    }
    if(tcpHeader->flags.isPush){
        flags.bits = 0;
        flags.isFinish = 1;
        // flags.isAck = 1;
        conn->state = TCP_CLOSING;
        TcpSendPacket(conn, flags , NULL, 0);
        return;
    }

    if(conn->state == TCP_CLOSING){

    }

    if(tcpHeader->flags.isFinish){
        flags.bits = 0;
        flags.isAck = 1;
        TcpSendPacket(conn, flags , NULL, 0);
    }

    // if(tcpHeader->isPush){
    //     TcpSendPacket(conn, conn->sndNxt, TCP_ACK , NULL, 0);
    //     if(conn->onData){
    //         conn->onData(conn,(uint8_t *)tcpHeader + (tcpHeader->dataOffset >> 2),)
    //     }
    // }

    // if(conn->state == TCP_LISTEN){

    // }
    // else if(conn->state == TCP_SYN_SENT){
    //     TcpReceiveSynSent(conn,tcpHeader);
    // }else{
    //     // TcpReceiveGeneral(conn,tcpHeader,)
    // }
    
}
void TcpSend(TcpConnection *conn, const void *data, uint32_t count){
    TcpFlags flags = { .bits = 0 };
    flags.isAck = 1;
    flags.isPush = 1;
    TcpSendPacket(conn, flags , data, count);
}
void TcpInit(){
    Time tm;
    TimeNow(&tm);
    baseIsn = (tm * 1000 - Millis()) * 250;
    _activeConnections = list_create();
    IpRegisterProtocolHandler(IP_TCP,TcpReceive);
}