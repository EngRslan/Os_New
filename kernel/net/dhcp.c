#include <kernel/net/dhcp.h>
#include <kernel/net/udp.h>
#include <kernel/net/addr.h>
#include <kernel/mem/kheap.h>
#include <kernel/types.h>



void dhcp_discover(){
    ipv4_addr_t sip;
    ipv4_addr_t dip;

    sip.bits = 0;
    dip.bits = 0xFFFFFFFF;

    

}