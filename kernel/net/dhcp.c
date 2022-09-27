#include <kernel/net/dhcp.h>
#include <kernel/net/udp.h>
#include <kernel/net/addr.h>
#include <kernel/net/intf.h>
#include <kernel/mem/kheap.h>
#include <kernel/types.h>
#include <kernel/bits.h>
#include <string.h>
#include <logger.h>
#include <stdio.h>

void dhcpRequest(NetInterface *intf, DhcpHeader *packet);

typedef enum {
    DHCP_STAGE_NONE = 0x00,
    DHCP_STAGE_DESCOVERING ,
    DHCP_STAGE_REQUESTED,
    DHCP_STAGE_ACKNOWLEDGE
}DhcpStage;
static struct DhcpOptions {

    Ipv4Address subnetMask;
    Ipv4Address routerIp;
    Ipv4Address dhcpServerIp;
    Ipv4Address nextServerIp;
    Ipv4Address requestedIp;
    Ipv4Address dnsIps[4];
    uint8_t messageType;
    uint32_t xid;
    uint32_t leaseTime;
    uint8_t ipTtl;
    uint8_t tcpTtl;
    uint32_t arpCachTimeout;
} dhcpOptions;



uint8_t * dhcpGenerateHeader(NetInterface *intf, DhcpHeader *packet, DhcpMessageType msg_type,Ipv4Address ip){
    packet->op = DHCP_OP_REQUEST;
    packet->hw_type = 1;
    packet->hw_addr_len = sizeof(MacAddress);
    packet->hops = 0;
    dhcpOptions.xid = 0x55555555;
    packet->xid = SWITCH_ENDIAN32(dhcpOptions.xid);
    packet->flags = SWITCH_ENDIAN16(0x8000);
    CopyMacAddress(intf->macAddress, packet->client_hw_addrp);

    uint8_t *p = (uint8_t *)packet + sizeof(DhcpHeader);
    *(uint32_t *)p = SWITCH_ENDIAN32(DHCP_MAGIC_NUMBER);
     p+=4;
    *p++ = DHCP_OPT_MESSAGETYPE;
    *p++ = 1;
    *p++ = msg_type;

    *p++ = DHCP_OPT_CLIENT_IDENTIFIER;
    *p++ = 0x07;
    *p++ = 0x01; // HW TYPE (ETHERNET)
    CopyMacAddress(intf->macAddress,(uint8_t *)p);
    p += 6;

    // Parameter request list
    *(p++) = DHCP_OPT_PARAMETER_REQUEST_LIST;
    *(p++) = 8;
    *(p++) = DHCP_OPT_SUBNETMASK;
    *(p++) = DHCP_OPT_ROUTER;
    *(p++) = DHCP_OPT_DNS;
    *(p++) = DHCP_OPT_DOMAIN_NAME;
    *(p++) = DHCP_OPT_NETBIOS_TCPIP_NS;
    *(p++) = DHCP_OPT_NETBIOS_TCPIP_NTYPE;
    *(p++) = DHCP_OPT_NETBIOS_TCPIP_SCOPE;
    *(p++) = DHCP_OPT_MAX_MESSAGE_SIZE;

    return p;

}

void DhcpReceive(NetBuffer *netbuffer){
    DhcpHeader *packet = (DhcpHeader *)netbuffer->packetData;
    if(packet->op != DHCP_OP_REPLAY){
        log_information("[Dhcp:unexpected-request] packet dropped unexpected packet recevied");
        return;
    }
    //Switch ednian
    if(SWITCH_ENDIAN32(packet->xid) != dhcpOptions.xid){
        log_warning("[Dhcp:xid-mismatch] Packet Dropped XID Changed");
        return;
    }

    uint8_t * optionsPtr = (uint8_t *)netbuffer->packetData + sizeof(DhcpHeader);

    if(SWITCH_ENDIAN32(*(uint32_t *)optionsPtr) != DHCP_MAGIC_NUMBER){
        log_warning("[dhcp:magic-mismatch] Packet Dropped Magic Number Mismatch");
        return;
    }

    optionsPtr += 4;
    
    CopyIpv4Address(packet->server_ip,dhcpOptions.nextServerIp);
    CopyIpv4Address(packet->yourip,dhcpOptions.requestedIp);

    DhcpOptionEntry *opts = (DhcpOptionEntry *)optionsPtr;
    uint8_t *dataPtr = NULL;
    bool endLoop = false;
    while (!endLoop)
    {
        dataPtr = (uint8_t *)(opts + 1);
        switch(opts->options){
            case DHCP_OPT_SUBNETMASK:
            {
                CopyIpv4Address(dataPtr,dhcpOptions.subnetMask);
                break;
            }
            case DHCP_OPT_ROUTER:
            {
                CopyIpv4Address(dataPtr,dhcpOptions.routerIp);
                break;
            }
            case DHCP_OPT_DNS:
            {
                uint8_t listedIpsCount = opts->dataLength / 4;
                listedIpsCount = listedIpsCount > 4 ? 4 : listedIpsCount;
                for (uint8_t i = 0; i < listedIpsCount; i++)
                {
                    CopyIpv4Address(dataPtr,dhcpOptions.dnsIps[i]);
                    dataPtr += sizeof(Ipv4Address);
                }
                
                break;
            }
            case DHCP_OPT_IPTTL:
            {
                dhcpOptions.ipTtl = *dataPtr;
                break;
            }
            case DHCP_OPT_ARP_CACHE_TIMEOUT:
            {
                dhcpOptions.arpCachTimeout = SWITCH_ENDIAN32(*(uint32_t *)dataPtr);
                break;
            }
            case DHCP_OPT_TCPTTL:
            {
                dhcpOptions.tcpTtl = *dataPtr;
                break;
            }
            case DHCP_OPT_REQUESTEDIP:
            {
                CopyIpv4Address(dataPtr,dhcpOptions.requestedIp);
                break;
            }
            case DHCP_OPT_IPLEASETIME:
            {
                dhcpOptions.leaseTime = SWITCH_ENDIAN32(*(uint32_t *)dataPtr);
                break;
            }
            case DHCP_OPT_MESSAGETYPE:
            {
                dhcpOptions.messageType = *dataPtr;
                break;
            }
            case DHCP_OPT_SERVER_IDENTIFIER:
            {
                CopyIpv4Address(dataPtr,dhcpOptions.dhcpServerIp);
                break;
            }
            case DHCP_OPT_END:
                endLoop = true;
            break;
            default:
                log_warning("[eth:unhandled-option] Option (%d) Need to implement",(uint32_t)opts->options);
            break;
        }

        opts = (DhcpOptionEntry *)((uint8_t *)opts + sizeof(DhcpOptionEntry) + opts->dataLength);
    }
    
    if(dhcpOptions.messageType == DHCP_MSG_OFFER){
        uint32_t msgSize = sizeof(DhcpHeader)+64;
        DhcpHeader *dhcp_packet = (DhcpHeader *)kmalloc(msgSize);
        memset(dhcp_packet,0,msgSize);
        dhcpRequest(netbuffer->interface,dhcp_packet);
        kfree(dhcp_packet);
    }
    else if(dhcpOptions.messageType == DHCP_MSG_ACK){
        char ipstrBuffer[20];
        Ipv4ToStr(ipstrBuffer,dhcpOptions.requestedIp);
        
        defaultAssignedIpAddress.assignMethod = IP_METHOD_DHCP;
        CopyIpv4Address(dhcpOptions.requestedIp,defaultAssignedIpAddress.Ip);
        CopyIpv4Address(dhcpOptions.nextServerIp,defaultAssignedIpAddress.gateway);
        CopyIpv4Address(dhcpOptions.subnetMask,defaultAssignedIpAddress.subnet);
        log_information("[dhcp:ack] system has fetch new ip %s",ipstrBuffer);
        printf("\033[20,50HIp:      %d.%d.%d.%d",dhcpOptions.requestedIp[0],dhcpOptions.requestedIp[1],dhcpOptions.requestedIp[2],dhcpOptions.requestedIp[3]);
        printf("\033[21,50HNetmask: %d.%d.%d.%d",dhcpOptions.subnetMask[0],dhcpOptions.subnetMask[1],dhcpOptions.subnetMask[2],dhcpOptions.subnetMask[3]);
        printf("\033[22,50HGateway: %d.%d.%d.%d",dhcpOptions.nextServerIp[0],dhcpOptions.nextServerIp[1],dhcpOptions.nextServerIp[2],dhcpOptions.nextServerIp[3]);
    }


}

void DhcpDiscover(NetInterface *intf){
    NetBuffer *netbuffer = (NetBuffer *)kmalloc(sizeof(NetBuffer));
    Ipv4Address sip = {0,0,0,0};
    uint16_t total_size = sizeof(DhcpHeader) + 64;
    DhcpHeader * packet = (DhcpHeader *)kmalloc(total_size);
    memset(packet,0,total_size);
    uint8_t * p= dhcpGenerateHeader(intf,packet,DHCP_MSG_DISCOVER,sip);
    *p++ = DHCP_OPT_END;
    netbuffer->interface = intf;
    netbuffer->packetData = packet;
    netbuffer->length = total_size;
    UdpSend(netbuffer, g__broadcastIpAddress, DHCP_DEFAULT_BOOT_CLIENT_PORT, DHCP_DEFAULT_BOOT_SERVER_PORT);
    kfree(netbuffer);
    kfree(packet);
}


void dhcpRequest(NetInterface *intf, DhcpHeader *packet){
    NetBuffer *netbuffer = (NetBuffer *)kmalloc(sizeof(NetBuffer));
    Ipv4Address sip = {0,0,0,0};
    uint16_t total_size = sizeof(DhcpHeader) + 64;
    
    uint8_t * p = dhcpGenerateHeader(intf,packet,DHCP_MSG_REQUEST,sip);
    
    *p++ = DHCP_OPT_REQUESTEDIP;
    *p++ = 4;
    CopyIpv4Address(dhcpOptions.requestedIp,p);
    p+=4;

    *p++ = DHCP_OPT_SERVER_IDENTIFIER;
    *p++ = 4;
    CopyIpv4Address(dhcpOptions.dhcpServerIp,p);
    p+=4;

    *p = DHCP_OPT_END;


    netbuffer->interface = intf;
    netbuffer->packetData = packet;
    netbuffer->length = total_size;
    UdpSend(netbuffer,g__broadcastIpAddress,DHCP_DEFAULT_BOOT_CLIENT_PORT,DHCP_DEFAULT_BOOT_SERVER_PORT);
    kfree(netbuffer);
    kfree(packet);
}