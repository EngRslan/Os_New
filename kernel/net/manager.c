#include <kernel/net/manager.h>
#include <kernel/drivers/pci.h>
#include <kernel/drivers/rtl8139.h>
#include <kernel/mem/kheap.h>
#include <kernel/net/intf.h>
#include <kernel/net/ethernet.h>
#include <kernel/net/ip.h>
#include <kernel/net/arp.h>
#include <kernel/net/udp.h>
#include <kernel/net/dhcp.h>
#include <kernel/net/icmp.h>

typedef struct
{
    void(*install)(void);
    void(*send)(NetBuffer *);
}InterfaceDriver;

struct{
    bool isPresent;
    NetInterface *interface;
    InterfaceDriver driver;
    
} netDevices[5];


void NetworkInstall(){
    pci_config_t *eth_controller = pci_get_device(0x2,0x0);
    NetInterface *netf = (NetInterface *)kmalloc(sizeof(NetInterface));
    rtl8139_install(netf,eth_controller);    

    netDevices[0].isPresent = true;
    netDevices[0].interface = netf;


    //Register DATALINK LAYER;
    EthernetRegisterProtocol(ETHERTYPE_ARP,ArpReceive);
    EthernetRegisterProtocol(ETHERTYPE_IP,IpReceive);

    IpRegisterProtocolHandler(IP_UDP,UdpReceive);
    IpRegisterProtocolHandler(IP_ICMP,IcmpReceive);
    //REGISTER APPLICATION LAYER;
    UdpRegisterHandler(DHCP_DEFAULT_BOOT_CLIENT_PORT,DhcpReceive);


    DhcpDiscover(netf);

}

NetInterface *GetDefaultInterface()
{
    return netDevices[0].interface;
}