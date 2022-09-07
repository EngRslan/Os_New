#include <kernel/drivers/pci.h>
#include <kernel/types.h>
#include <kernel/drivers/rtl8139.h>
#include <kernel/system.h>
#include <kernel/bits.h>
#include <logger.h>
#include <stddef.h>

void rtl8139_install(pci_config_t * _device){
    pci_device_config_t * dev = (pci_device_config_t *)_device->config;
    uint32_t io_addr = dev->BAR0;
    io_addr = io_addr & (~0x3);

    outportb(io_addr+0x52,0);
    outportb(io_addr+0x37,0x10);
    while (BITREAD(inportb(io_addr+0x37),4) != 0) { }

    uint8_t mac_addr[6];
    mac_addr[0] = inportb(io_addr + 0);
    mac_addr[1] = inportb(io_addr + 1);
    mac_addr[2] = inportb(io_addr + 2);
    mac_addr[3] = inportb(io_addr + 3);
    mac_addr[4] = inportb(io_addr + 4);
    mac_addr[5] = inportb(io_addr + 5);

    
    uint32_t hwverid = ((inportl(io_addr + 0x40))&0x7E000000)>>25;
    const char * vers[]={ "RTL8139", "RTL8139A", "RTL8139A-G", "RTL8139B", "RTL8130", "RTL8139C", "Unknown" };
    string_t hwvers = NULL;
    switch (hwverid)
    {
        case 0b00110000:
            hwvers = vers[0];
            break;
        case 0b00111000:
            hwvers = vers[1];
            break;
        case 0b00111001:
            hwvers = vers[2];
            break;
        case 0b00111100:
            hwvers = vers[3];
            break;
        case 0b00111110:
            hwvers = vers[4];
            break;
        case 0b00111010:
            hwvers = vers[5];
            break;
    default:
        hwvers = vers[6];
        break;
    }
    log_trace("Eth: %s Detected MAC:%01x:%01x:%01x:%01x:%01x:%01x",hwvers,(uint32_t)mac_addr[0],(uint32_t)mac_addr[1],(uint32_t)mac_addr[2],(uint32_t)mac_addr[3],(uint32_t)mac_addr[4],(uint32_t)mac_addr[5]);

    


}