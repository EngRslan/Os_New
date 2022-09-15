#ifndef DHCP_H
#define DHCP_H
#include <kernel/types.h>
#include <kernel/net/intf.h>
#include <kernel/net/addr.h>
#define DHCP_MAGIC_NUMBER 0x63825363
#define DHCP_DEFAULT_BOOT_SERVER_PORT 67
#define DHCP_DEFAULT_BOOT_CLIENT_PORT 68

typedef struct { 
    uint8_t options;
    uint8_t dataLength;

} __attribute__((packed)) DhcpOptionEntry;
typedef enum {
    DHCP_OP_REQUEST = 0x01,
    DHCP_OP_REPLAY  = 0x02,
} DhcpOp;

typedef enum {
    DHCP_MSG_DISCOVER   = 0x01,
    DHCP_MSG_OFFER      = 0x02,
    DHCP_MSG_REQUEST    = 0x03,
    DHCP_MSG_DECLINE    = 0x04,
    DHCP_MSG_ACK        = 0x05,
    DHCP_MSG_NCK        = 0x06,
    DHCP_MSG_RELEASE    = 0x07,
    DHCP_MSG_INFORM     = 0x08,
} DhcpMessageType;

typedef enum {
    DHCP_OPT_SUBNETMASK         = 1,
    DHCP_OPT_ROUTER             = 3,
    DHCP_OPT_DNS                = 6,
    DHCP_OPT_DOMAIN_NAME        = 15,
    DHCP_OPT_IPTTL              = 23,
    DHCP_OPT_TCPTTL             = 37,
    DHCP_OPT_ARP_CACHE_TIMEOUT  = 35,
    DHCP_OPT_NETBIOS_TCPIP_NS   = 44,
    DHCP_OPT_NETBIOS_TCPIP_NTYPE= 46,
    DHCP_OPT_NETBIOS_TCPIP_SCOPE= 47,
    DHCP_OPT_REQUESTEDIP        = 50,
    DHCP_OPT_IPLEASETIME        = 51,
    DHCP_OPT_MESSAGETYPE        = 53,
    DHCP_OPT_SERVER_IDENTIFIER  = 54,
    DHCP_OPT_PARAMETER_REQUEST_LIST  = 55,
    DHCP_OPT_MAX_MESSAGE_SIZE   = 57,
    DHCP_OPT_CLIENT_IDENTIFIER  = 61,
    DHCP_OPT_END                = 255
} DhcpOption;

typedef struct {
    uint8_t op;
    uint8_t hw_type;
    uint8_t hw_addr_len;
    uint8_t hops;
    uint32_t xid;
    uint16_t seconds;
    uint16_t flags;
    Ipv4Address client_ip;
    Ipv4Address yourip;
    Ipv4Address server_ip;
    Ipv4Address gateway_ip;
    uint8_t client_hw_addrp[16];
    uint8_t server_name[64];
    uint8_t file[128];
} __attribute__((packed)) DhcpHeader;

void DhcpDiscover(NetInterface *intf);
void DhcpReceive(NetBuffer *netbuffer);

#endif