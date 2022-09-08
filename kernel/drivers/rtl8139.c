#include <kernel/drivers/pci.h>
#include <kernel/types.h>
#include <kernel/drivers/rtl8139.h>
#include <kernel/system.h>
#include <kernel/bits.h>
#include <kernel/mem/vmm.h>
#include <kernel/isr.h>
#include <logger.h>
#include <stddef.h>
#include <string.h>
#include <kernel/system.h>

#define ETH_BUFFER_ADDRESS GET_VIRTUAL_ADDRESS(0x302,0)

ptr_t rx_buffer = (ptr_t *)ETH_BUFFER_ADDRESS;
uint32_t io_addr = 0;
uint8_t get_hwverid(uint32_t io_addr,string_t * s){
    uint32_t hwverid = ((inportl(io_addr + 0x40))&0x7E000000)>>25;
    if(s){
        const char * vers[]={ "RTL8139", "RTL8139A", "RTL8139A-G", "RTL8139B", "RTL8130", "RTL8139C", "Unknown" };
        switch (hwverid)
        {
            case 0x30://0b00110000:
                *s = (string_t)vers[0];
                break;
            case 0x38://0b00111000:
                *s = (string_t)vers[1];
                break;
            case 0x39://0b00111001:
                *s = (string_t)vers[2];
                break;
            case 0x3C://0b00111100:
                *s = (string_t)vers[3];
                break;
            case 0x3E://0b00111110:
                *s = (string_t)vers[4];
                break;
            case 0x3A://0b00111010:
                *s = (string_t)vers[5];
                break;
        default:
            *s = (string_t)vers[6];
            break;
        }
    }
    return hwverid;
}
void read_mac_addr(uint32_t io_addr,uint8_t mac_addr[]){
    mac_addr[0] = inportb(io_addr + 0);
    mac_addr[1] = inportb(io_addr + 1);
    mac_addr[2] = inportb(io_addr + 2);
    mac_addr[3] = inportb(io_addr + 3);
    mac_addr[4] = inportb(io_addr + 4);
    mac_addr[5] = inportb(io_addr + 5);
}
void reset_device(uint32_t io_addr){
    outportb(io_addr+0x37,0x10);
    while (BITREAD(inportb(io_addr+0x37),4) != 0) { }
}

void eth_irq_handler(register_t * reg){
    uint16_t status = inports(io_addr + 0x3e);

    if(status & (1<<2)){
        log_trace("eth: IRQ Fired (Packet Sent)");
    }

    if(status & (1<<0)){
        log_trace("eth: IRQ Fired (Packet Received)");
    }

    outports(io_addr + 0x3E,0x5);
}
void rtl8139_install(pci_config_t * _device){
    pci_device_config_t * dev = (pci_device_config_t *)_device->config;
    io_addr = dev->BAR0;
    io_addr = io_addr & (~0x3);

    outportb(io_addr+0x52,0);
    reset_device(io_addr);

    uint8_t mac_addr[6];
    read_mac_addr(io_addr,mac_addr);

    string_t hwvers = NULL;
    uint32_t hwverid = get_hwverid(io_addr,&hwvers);
    
    log_trace("eth: %s Detected MAC:%01x:%01x:%01x:%01x:%01x:%01x",hwvers,(uint32_t)mac_addr[0],(uint32_t)mac_addr[1],(uint32_t)mac_addr[2],(uint32_t)mac_addr[3],(uint32_t)mac_addr[4],(uint32_t)mac_addr[5]);

    callocate_region(kernel_directory,(v_addr_t)rx_buffer,3,0,1);
    memset((void *)rx_buffer,0,3*PAGE_SIZE);
    
    uint16_t pci_command = dev->common.command;
    pci_command |= (1<<2);
    pci_writew(_device->bus,_device->device,_device->function,PCI_COMMAND,pci_command);

    ptr_t rx_ph_buffer = (ptr_t)virtual2physical((v_addr_t)rx_buffer);
    outportl(io_addr + 0x30,(uint32_t)rx_ph_buffer);

    outports(io_addr + 0x3c,0x0005);
    outportl(io_addr + 0x44,0xf|(1<<7));
    outportb(io_addr + 0x37,0x0c);

    uint8_t irq = 32 + dev->interrupt_line;
    register_interrupt_handler(irq,eth_irq_handler);


}